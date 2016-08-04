/*
(The MIT License)

Copyright (c) 2016 Lee Weber

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/



#include <stdio.h>
#include "esp8266.h"

#ifdef _WIN32
	#include <winsock2.h>
#else//POSIX system(Linux, OSX, etc)
	#include <sys/socket.h>
	#include <sys/types.h>
	#include <netinet/in.h>
	#include <netinet/tcp.h>
	#include <arpa/inet.h>
#endif



////https://cdn.sparkfun.com/assets/learn_tutorials/4/0/3/4A-ESP8266__AT_Instruction_Set__EN_v0.30.pdf//////



const char *start_up_string = "ets Jan 8 2014,rst cause 4, boot mode(3,7)\n\nwdt reset\nload 0x401000000,len 24444,room 16\nchksum 0xe0 ho 0 tail 12 room 4\nready\r\n";
const char *at_gmr_string = "AT Version:Uzem ESP8266 AT Driver 0.5\nSDK version:Uzem ESP8266 SDK 1.2.0\nUzebox Emulator uzebox.org\nBuild:1.0\r\nOK\r\n";


const char *fake_ap_name[] = {
"WHY DO THESE 2 GET IGNORED?",
"WHY DO THESE 2 GET IGNORED?",
"Linksys01958",
"The Bottle Imp",
"Uzem Emulated AP",
"The Promised LAN!!",
"NSA Surveilance Van",
"Fast Fiber W Slow Wifi!",
"you kids get off my LAN",
"Put Your Best Port Forward",
"Zelda a Linksys to the Past",
"HiddenVariablesCollapseWaves",
"Cats Against Schrodinger",
"Pyongyang Free Wifi >:)",
"Pretty Fly For A Wifi",
"TellMyWifiLoveHer",
"CenturyLink22840",
"LANDownUnder",
"NetGear009",
};

const char *bin_to_at[] = {//binary mode to AT conversion, allows to interface with less UART data/faster
"AT",
"ATE",
"ATE0",
"AT+RST",
"AT+GMR",
"AT+GSLP",

};

#define ESP_NUM_FAKE_APS	19

const char *ESP_OK_STR  = "OK\r\n";
const char *ESP8266_CORE_VERSION = "v0.3";

ESPModule ESP;

int ESP8266_Container(void *data){//thread container
	ESP.Tick();
	return 0;
}



uint16_t ESPModule::GetRandom(uint16_t seed){
	static uint16_t prng_state;
  	
	if(seed!=0) prng_state=seed;
	
	uint16_t bit  = ((prng_state >> 0) ^ (prng_state >> 2) ^ (prng_state >> 3) ^ (prng_state >> 5) ) & 1;
	prng_state =  (prng_state >> 1) | (bit << 15);
	return prng_state;
}

void ESPModule::FirstTick(){

	//load up preferences
	LoadConfig();
	UartBaudDelayCycles = 2967;
	DecoherenceLimit = 750;//1200
	AT_CIOBAUD("9600");//forces BaudDivisor to 9600
	WifiTimer = 1;
	WifiDelay = 0;//on emulator start wifi is connected, as if Uzebox just switched from a game setup rom and did not reset module
	DebugLevel = 1;
	State |= (ESP_READY|ESP_DID_FIRST_TICK|ESP_AP_CONNECTED);//Uzebox thread waits until we set first tick

	InitializeSocketSystem();
	//while(!(State & ESP_UZEBOX_ACKNOWLEDGED_THREAD));

}


void ESPModule::Tick(){//must be this format to be called as a thread

	while(1){


		if(!(State & ESP_DID_FIRST_TICK))	//first tick since thread started
			FirstTick();
	
		while(HandleReset());

		TimeStall(0);//synchronizes UART buffers, do not run again until Uzebox does
		if(!ResetPinState)
			continue;

//printf("[");
		if(RxAwaitingTime){//unvarnished mode is active

//TODO CAN THIS CAUSE BUFFER OVERFLOW IF UZEBOX THREAD STALLS??
			if(RxAwaitingTime > (ESP_UNVARNISHED_DELAY)){
//printf("*");
				if(RxBufferBytes == 3 && !strncmp((char *)&CommandBuffer,"+++",3)){//end unvarnished mode
					printf("End unvarnished transmission mode\n");
					RxAwaitingTime = 0;//disable unvarnished mode when we receive a packet consisting of only "+++"
					RxBufferBytes = 0;
					return;
				}else
					RxAwaitingTime -= ESP_UNVARNISHED_DELAY;

				if(RxBufferBytes){
					//printf("sent unvarnished:%d,%s\n",RxBufferBytes,RxBuffer);
					NetSend(SendToSocket,(const char *)RxBuffer,RxBufferBytes,0);
					RxAwaitingBytes = 0;
					RxBufferBytes = 0;
				}
			}

		}else if(RxBufferNewData && !UzeboxRxBufferBytes){//Uzebox is still behind on UART time, wait
			ProcessAT();//big heavy function
			RxBufferNewData = 0;
			continue;
		}
		ProcessIPD();

		if(FlashDirty)
			SaveConfig();
	}

}


void ESPModule::ClearATCommand(){

	//Remove 1 AT command from the RX buffer, put any following bytes at the buffer start
	//we know there was a command and it was terminated, it must, or crash.
	uint32_t i=0,j=0;

	//while(RxBuffer[i++] != '\r');
	//while(RxBuffer[i++] != '\n');
	while(RxBuffer[i++] != '\r' && RxBuffer[i] != '\n');
	i++;

	for(j=0;i<RxBufferBytes;i++){
		RxBuffer[j++] = RxBuffer[i];
	}
	RxBufferBytes = j;
	RxBufferPos = 0;
	RxBuffer[RxBufferBytes] = '\0';//terminate string for strncmp(), etc.
}



void ESPModule::TxP(const char *s){//write const string to tx buffer(eventually makes it to Uzebox rx)

	uint32_t off = 0;
	while(s[off] != '\0'){
		while(TxBufferPos >= sizeof(TxBuffer)-4)//wait until we have enough space if necessary
			TimeStall(0);

		TxBuffer[TxBufferPos++] = s[off++];
	}

	TxBuffer[TxBufferPos] = '\0';

}


void ESPModule::TxI(int32_t i){
	char buf[8];
	sprintf(buf,"%i",i);
	TxP(buf);
}


int32_t ESPModule::InitializeSocketSystem(){

	if(DebugLevel)	Debug("ESP8266_InitializeSocketSystem() Start",0);
//#if WINDOWS
	WSADATA WSAData;
	if(WSAStartup(MAKEWORD(2,2),&WSAData)){
		printf("Failed for Winsock 2.2 reverting to 1.1\n");
		if(WSAStartup(MAKEWORD(1,1),&WSAData)){
			Debug("Failed on WSAStartup()",WSAGetLastError());
			return SOCKET_ERROR;
		}
	}
//#elif LINUX

//#endif
	int i;
	for(i=0;i<5;i++)
		Socks[i] = INVALID_SOCKET;

/*
   const int val=255;
    int i, sd;
    struct packet pckt;
    struct sockaddr_in r_addr;
    int loop;
    struct hostent *hname;
    struct sockaddr_in addr_ping,*addr;

	pid = getpid();


	hname = gethostbyname(adress);
	bzero(&addr_ping, sizeof(addr_ping));
    addr_ping.sin_family = hname->h_addrtype;
    addr_ping.sin_port = 0;
    addr_ping.sin_addr.s_addr = *(long*)hname->h_addr;

    addr = &addr_ping;

	PingSock = socket(PF_INET,SOCK_RAW,IPPROTO_ICMP);
	if(PingSock == INVALID_SOCKET){
		printf("Failed to create RAW socket, ping cannot succeed\n");
		return 0;
	}
	

    if(setsockopt(PingSock,SOL_IP,IP_TTL,&val, sizeof(val)) != 0)
    {
        perror("Set TTL option");
        return 1;
    }
    if ( fcntl(sd, F_SETFL, O_NONBLOCK) != 0 )
    {
        perror("Request nonblocking I/O");
        return 1;
    }

*/
	return 0;
}

uint16_t ESPModule::CheckSum(void *b, int32_t len){
/*
//standard 1s complement checksum
	int16_t *buf = b;
	uint32_t sum=0;
	uint16_t result;

	for(sum=0;len>1;len-=2)
		sum += *buf++;

	if (len==1)
		sum += *(unsigned char*)buf;

	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	result = ~sum;
	return result;
*/
}

void ESPModule::AT_CWSAP(){//get or set wifi credentials for softAP

	if(!strncmp((char*)&CommandBuffer[3+5],"?\r\n",3) || !strncmp((char*)&CommandBuffer[3+5],"_CUR?\r\n",7) || !strncmp((char*)&CommandBuffer[3+5],"_DEF?\r\n",7)){//query only
		if(DebugLevel)	Debug("CWSAP QUERY ONLY",0);
		char sapbuffer[256];
		if(!strncmp((char*)&CommandBuffer[3+5],"_DEF?\r\n",7)){//must read default saved version in case current version is different
		//TODO TODO
		
		}else
			sprintf(sapbuffer,"\"%s\",\"%s\",%d,%d\r\n",SoftAPName,SoftAPPass,SoftAPChannel,SoftAPEncryption);
printf("\nCWSAP:%s\n",sapbuffer);
		TxP(sapbuffer);
		TxP_OK();
		return;
	}

	//if(DebugLevel)	Debug("CWSAP ",0);

	int32_t at_error = 0;
	int32_t i;

	//TODO SUPPORT _CUR AND _DEF
	if(CommandBuffer[3+6] != '"'){//must start with '"'
			at_error = 1;
		}else{
			for(i=(3+7);i<(3+7)+64;i++){
				if(CommandBuffer[i] == '"'){//got terminating '"'
					continue;
				}
				if(i == (3+7)+64-1){//too long or wasn't properly terminated
					at_error = 2;
					break;
				}
			}
		
		}
		if(at_error){
			if(at_error == 1){
				if(DebugLevel > 0)	Debug("Didn't get first \" for argument",0);
			}else{
				if(DebugLevel > 0)	Debug("Too long or didn't get last \" for argument",0);	
			}
			TxP_ERROR();
			return;
		}
		sprintf(SoftAPName,"%s",(char *)&CommandBuffer[3+8]);
		sprintf(SoftAPPass,"%s",(char *)&CommandBuffer[3+8]);
		//don't care, we will just let it work for now
		TxP_OK();
}


void ESPModule::AT_CIPMODE(){//set the packet send mode

	//command does not support query in any firmware?

	if(CommandBuffer[11] < '0' || CommandBuffer[11] > '1')
		TxP_ERROR();
	else
		CipMode = CommandBuffer[11] - '0';
	
	TxP_OK();
}


void ESPModule::AT_CWLAP(){//list all available wifi access points
	if(DebugLevel > 0)	Debug("CWLAP list access points",0);
	
	State |= ESP_LIST_APS;//TODO WHY ???
	char ap_buffer[128];
	uint32_t ap_pos = 0;
	int32_t i;

	TimeStall(ESP_AT_CWLAP_DELAY);
	for(i=0;i<ESP_NUM_FAKE_APS;i++){
//	printf("%s\n",fake_ap_name);
		sprintf(ap_buffer,"+CWLAP:(1,\"%s\",-%i,\"%02x:%02x:%02x:%02x:%02x:%02x\")\r\n",fake_ap_name[i],rand()%100,rand()%256,rand()%256,rand()%256,rand()%256,rand()%256,rand()%256);
		TxP(ap_buffer);
		TimeStall(ESP_AT_CWLAP_INTER_DELAY);
	}
asm volatile("": : :"memory");//memory fence
	TxP_OK();
}


void ESPModule::AT_CIPSEND(){//send a packet

printf("AT+CIPSEND\n");

	if(!strncmp((char*)&CommandBuffer[3+7],"?\r\n",3)){//query only
		if(DebugLevel > 0)	Debug("CIPSEND QUERY ONLY",0);
			
		if((State & ESP_AP_CONNECTED) && (Socks[0] != INVALID_SOCKET))
			TxP_OK();//sends should succeed
		else{//no way we could send a packet
			if(DebugLevel > 0)	Debug("Not connected to AP can't send",0);
               	TxP_ERROR();
		}

		return;

	}else if(CommandBuffer[3+7] == '\r' && CommandBuffer[3+8] == '\n'){//enter unvarnished transmission mode
printf("Enter unvarnished transmission mode\n");
		if(CipMode == 1)
			TxP_ERROR();
		else{
			RxAwaitingTime = 1;//start 20ms packet window timer
			TxP((const char *)">");
		}
		return;

	}else{//send

		int32_t connection_num = 0;//when not MUX, the only active socket is 0

		if(State & ESP_MUX){//multiple connections mode, must specify connection number

printf("send MUX\n");
			connection_num = CommandBuffer[3+8]-'0';
			if(connection_num < 0 || connection_num > 3){
				/*if(DebugLevel > 0)*/	Debug("CIPSEND MUX connection must be >0 && <4, got:",connection_num);
				TxP_ERROR();
				
				return;
			}

		}else
			printf("send not MUX\n");

		SendToSocket = connection_num;//store this so we know what socket to set the payload to later

		if(!(State & ESP_AP_CONNECTED) || (Socks[connection_num] == INVALID_SOCKET)){//can't send if not connected.(What if we are actings as AP?)
				
			if(!(State & ESP_AP_CONNECTED)){
				/*if(DebugLevel > 0)*/	Debug("Can't SEND, not connected to the AP",0);
			}else{
				/*if(DebugLevel > 0)*/	Debug("Can't SEND, no connection on socket:",connection_num);
			}
			TxP_ERROR();
			
			return;

		}

		if(DebugLevel > 0)	Debug("CIPSEND=",0);
		char i = CommandBuffer[3+8];
		if((State & ESP_AP_CONNECTED)){//must be connected to AP? or what if AP??
				
			if(i >= '0' && i < '4' && CommandBuffer[3+9] == ','){
					
				int32_t s = CommandBuffer[3+8] - '0';//get the socket we will use
				if(!(State & ESP_MUX) && s > 0){
					if(DebugLevel > 0)	Debug("CIPSEND= cant use connection > 0 without MUX=1",0);
					
				}else{
					if(DebugLevel > 0)	Debug("CIPSEND good connection num",s);
				}

				if(Socks[s&3] == INVALID_SOCKET){//error, not connected on this socket
					if(DebugLevel > 0)	Debug("Cant send, not connected on socket",s);

				}else{//socket is good, continue checking format

					int32_t bad_format = 0;
					for(i=3+10;i<3+10+4;i++){//maximum 4 digit number of bytes to send, in base 10
						if(CommandBuffer[i] >= '0' && CommandBuffer[i] <= '9'){

						}else if(i == 3+10 || CommandBuffer[i] != '\r'){//must be >= 1 byte, if not a base 10 number, then must be start of "\r\n"
							bad_format = 1;
						}else{//it's '\r', replace it with a '\0' so Atoi() will work, we will have to replace it with \r later so clean up works
							CommandBuffer[i] = '\0';//must replace with \r later, we keep the value of i after the for loop for this
							break;
						}
					}
					if(bad_format){//either terminated with no length specified, or got garbage characters before "\r\n" termination
						if(DebugLevel > 0)	Debug("CIPSEND=Bad format",0);

					}else{//format passed, get ready to receive the specified number of bytes(must be > 0)

						RxAwaitingBytes = Atoi((char *)&CommandBuffer[3+10]);//we terminated it with '\0' above
						if(DebugLevel > 0)	Debug("CIPSEND format good, waiting for bytes:",RxAwaitingBytes);
			CommandBuffer[i] = '\r';//now we replace the '\0' with '\r' so this command can be removed properly

//TODO DO WE HAVE TO EAT THE BYTES OR JUST UPDATE THE BUFFER POS TO READ FROM?!?!?!


						if(RxAwaitingBytes){

                                	SendToSocket = s;//keep track of which socket we will be sending on after waiting for input
							State |= ESP_AWAITING_SEND;//next time around, we will receive the bytes
							TxP((const char*)">");
						}else{//Atoi says the string == 0

						}
					}//format passed
				}//socket is good
			}else{//bad format
				if(DebugLevel > 0)	Debug("CIPSEND bad connection num",0);

			}
		}else{//not connected to AP
			if(DebugLevel > 0)	Debug("not connected to AP can't send ,",0);
			if(Socks[0] == INVALID_SOCKET){//not connected

			}
		}
	}//send mode(not query only mode)
}

void ESPModule::AT_CWJAP(){//join a wifi access point, fake
//netsh wlan connect ssid=YOURSSID name=PROFILENAME //anyone want to tackle this and make it real??
		
	uint8_t at_error = 0;
	int32_t i;
	if(strncmp("?\r\n",(const char *)&CommandBuffer[8],3)){//query only
		if(DebugLevel > 0)	Debug("CWJAP QUERY",0);
		if(State & ESP_AP_CONNECTED){
			char ap_buffer[64+64];
			sprintf(ap_buffer,"+CWJAP:%s\r\nOK\r\n",WifiName);
			TxP(ap_buffer);
		}else{
			TxP((const char *)"ERROR\r\n");
printf("jap failed?!?");
		}

	}else{//check the credentials against the fake APs
//HACK
TimeStall(ESP_AT_CWJAP_DELAY);
TxP_OK();
return;
		if(CommandBuffer[3+6] != '"'){//must start with "
			at_error = 1;
		}else{
			for(i=(3+7);i;i++){
				if(CommandBuffer[i] == '"'){//got terminating "
					break;
				}
				if(i == (3+6)+32){//SSID too long or wasn't properly terminated
					at_error = 2;
					break;
				}
			}
			if(!at_error && CommandBuffer[++i] != ',')
				at_error = 3;
			else if(!at_error && CommandBuffer[++i] != '"')
				at_error = 4;
			else{//check for valid password
				for(;i;i++){
					if(CommandBuffer[i] == '"'){//got terminating "
						break;
					}
					if(i == (3+6)+32){//password too long or wasn't properly terminated
						at_error = 5;
						break;
					}
				}
			}

		
		}
		if(at_error){
			if(DebugLevel > 0){
				if(at_error == 1){
					Debug("Didn't get first '\"' for SSID",at_error);
					//printf("got '%c' instead\n",(char *)CommandBuffer[3+5]);
				}else if(at_error == 2){
					Debug("SSID too long or not '\"' terminated",at_error);
				}else if(at_error == 3){
					Debug("missing comma between SSID and password",at_error);
				}else if(at_error == 4){
					Debug("missing first '\"' for password",at_error);
				}else if(at_error == 5){
					Debug("password too long or not '\"' terminated",at_error);
				}else{
					Debug("bad AT format for CWJAP",at_error);
				}
			}
			TxP_ERROR();
			return;
		}
		sprintf(WifiName,"%s",(char *)&CommandBuffer[3+8]);
		sprintf(WifiPass,"%s",(char *)&CommandBuffer[3+8]);
		FlashDirty = 1;//make sure this gets saved
		if(DebugLevel > 0)	Debug("Connected to wifi AP, adding fake wait period",0);
		WifiTimer = 1;
		WifiDelay = ESP_AT_CWJAP_DELAY+((rand()%1000)*ESP_AT_MS_DELAY);

		TxP_OK();
		}

}


void ESPModule::AT_CWMODE(){

	if(!strncmp((char*)&CommandBuffer[9],"?\r\n",3)){//ask which mode we are in
		if(DebugLevel > 0)	Debug("CWMODE QUERY ONLY",0);
		
		if(UartATMode == 1)//STA
			TxP((const char*)"CWMODE:1\r\nOK\r\n");
		else if(UartATMode == 2)//AP
			TxP((const char*)"CWMODE:2\r\nOK\r\n");
		else/*if(UartATMode == 3)*///STA+AP
			TxP((const char*)"CWMODE:3\r\nOK\r\n");

	}else if(CommandBuffer[9] == '=' && !strncmp((char*)&CommandBuffer[11],"\r\n",2)){//set the mode we are in

		int32_t mt = CommandBuffer[3+7];
		if(DebugLevel > 0)	DebugC("CWMODE= set mode",mt);

		if(mt < '1' || mt > '3'){//bad format

			if(DebugLevel > 0)	Debug("bad CWMODE format must be 1,2, or 3",0);
			TxP_ERROR();
			
			return;

		}else{
//TODO SAVE THIS TO FLASH?!?
			TxP_OK();
			mt -= '1';
			UartATMode = mt;

		}

	}else if(!strncmp((char*)&CommandBuffer[9],"=?\r\n",4)){//ask what modes are possible(dubious but some firmwares support this??)
		TxP((const char*)"+CWMODE:(1-3)\r\nOK\r\n"); 

	}else if(!strncmp((char*)&CommandBuffer[9],"_CUR=",5 && CommandBuffer[15] == '\r' && CommandBuffer[16] == '\n')){//set the mode but do not save as default to flash

		int32_t mt = CommandBuffer[3+11];
		if(DebugLevel > 0)	DebugC("CWMODE_CUR= set mode",mt);

		if(mt < '1' || mt > '3'){//bad format

			TxP_ERROR();
			if(DebugLevel > 0)	Debug("bad CWMODE_CUR format must be 1,2, or 3",0);

			
			return;

		}else{

			TxP_OK();
			mt -= '1';
			UartATMode = mt;
		}

	}else{
		if(DebugLevel > 0)	Debug("unknown/bad CWMODE format",0);
		TxP_ERROR();
	}
}


void ESPModule::AT_AT(){
	TxP_OK();
}


void ESPModule::AT_ATE(){
	if(!strncmp("0\r\n",(const char*)&CommandBuffer[3],3)){//turn echo off
//TimeStall(ESP_AT_MS_DELAY*2000);
		TxP_OK();
		if(DebugLevel > 0)	Debug("Echo Off",0);
		State &= ~ESP_ECHO;

	}else if(!strncmp("1\r\n",(const char*)&CommandBuffer[3],3)){//turn echo on
		TxP_OK();
		if(DebugLevel > 0)	Debug("Echo On",1);
		State |= ESP_ECHO;

	}else if(!strncmp("?\r\n",(const char*)&CommandBuffer[3],3)){//query only(is this supported?)
		TxP_OK();
		if(DebugLevel > 0)	Debug("Echo Query",1);


	}else{//bad format
		if(DebugLevel > 0)	Debug("Echo missing argument, must specify 0 or 1 ie. \"ATE0\\r\\n\"",0);
		TxP_ERROR();

	}

	return;

}


void ESPModule::AT_CIPSTART(){

//HACK HACK HACK
State |= ESP_MUX;

printf("START-%s\n",CommandBuffer);
	int32_t at_error = 0;
	if(!strncmp((char*)&CommandBuffer[3+9],"?\r\n",3)){//query only(supported on any firmware??)
		/*if(DebugLevel > 0)*/	Debug("CIPSTART QUERY ONLY",0);//TODO check that we could send
		//if(Socks[0] != INVALID_SOCKET)
		//	TxP_OK();
		
		if(State & ESP_AP_CONNECTED)//possible to start a connection to the internet
			TxP_OK();
		else
			TxP_ERROR();
		return;

	}


//start connection, check AT format
	if(DebugLevel > 0)	Debug("CIPSTART= connect to remote host",0);
	
	char s = CommandBuffer[12];//AT+CIPSTART=x
	uint32_t proto;
	uint32_t off = 0;

	if(State & ESP_MUX){//get the connection number and check that it is valid

		if(s < '0' || s > '4' || CommandBuffer[13] != ','){//bad connection number or missing comma
			if(DebugLevel > 0)	Debug("Bad CIPSTART connection number",0);
			TxP_ERROR();
			return;
		}
		
		s -= '0';
		if(Socks[s] != INVALID_SOCKET){//socket is already in use
			if(DebugLevel > 0)	Debug("CIPSTART ON SOCKET ALREADY CONNECTED, must close it first",0);
			TxP_ERROR();
			return;
		}
		off = 2;
	}else if(CommandBuffer[12] != '"'){//when not MUX, string should be "AT+CIPSTART="TCP".." or "AT+CIPSTART="UDP",.."
		if(DebugLevel > 0)	Debug("Connection number specified when not MUX",0);
		TxP_ERROR();
		return;
	}

	//we are clear to use this connection number, or if !MUX none was given, continue checking format
	
	int32_t type = 0;

	if(!strncmp((char*)&CommandBuffer[3+9+off],"\"TCP\",",6)){//open a TCP connection

		type = ESP_PROTO_TCP;
		if(DebugLevel > 0)	Debug("CIPSTART Good protocol name",0);

	}else if(!strncmp((char*)&CommandBuffer[3+9+off],"\"UDP\",",6)){//open a UDP "connection"

		type = ESP_PROTO_UDP;
		if(DebugLevel > 0)	Debug("CIPSTART Good protocol name",0);

	}else{

		if(DebugLevel > 0)	Debug("CIPSTART Bad protocol name, remember quotes? eg \"TCP\"",0);
		TxP_ERROR();
		return;				
	}
	
	Protocol[s] = type;
	//check the hostname given
	char hostname[128+1];
	int32_t hlen = 0;
	int32_t i;
	for(i=16+off;i<(sizeof(hostname)-1)+16+off;i++){
		if(CommandBuffer[3+i] == ','){
			if(DebugLevel > 0)	Debug("host name terminated",i);
				hostname[i] = '\0';//TODO------------------------------
				break;
			}
			hostname[hlen++] = CommandBuffer[3+i];
		}
	if(!hlen){
		if(DebugLevel > 0)	Debug("bad hostname with length of 0",0);//TODO THIS ISNT WORKING
		TxP_ERROR();
		return;
	}
			
	hostname[hlen-1] = '\0';
	if(DebugLevel > 0)	Debug("CIPSTART hostname format accepted",0);
	int32_t port = Atoi((char *)&CommandBuffer[3+i+1]);//50697
					 
	// hostname[i-1] = '\0';//remove trailing "
	i = Connect(hostname,s,port,type);
	
	if(!i){//connection success
	
	if(DebugLevel > 0)	Debug("CIPSTART= connection success",0);
		TxP((const char*)"OK\r\nLinked\r\n");
		return;

	}else{//connection failed(doesn't exist, server down, etc)
		if(DebugLevel > 0)	Debug("CIPSTART= good format, but failed to connect",0);
		TxP_ERROR();
		return;
	}

				

}

void ESPModule::AT_IPR(){
	//THIS IS DEPRECIATED, remove? For now just move the data over 4 and process as "AT+CIOBAUD"
	uint32_t i=3+4;
	do{
		CommandBuffer[i+4] = CommandBuffer[4];
	}while(CommandBuffer[i] != '\n');

	AT_CIOBAUD(0);
}


void ESPModule::AT_CIPCLOSE(){
	
	if(DebugLevel > 0)	Debug("CIPCLOSE=",0);
	
	if(!(State & ESP_MUX)){//single connection mode? then there is only 1 possible command format
	
		if(CommandBuffer[11] != '\r' || CommandBuffer[12] != '\n')
			TxP_ERROR();
		else
			TxP((const char*)"CLOSED\r\n");
		return;

	}else if(CommandBuffer[11] != '='){//multiple connection mode
		TxP_ERROR();
	}


	char i = CommandBuffer[12];

	if(i >= '0' && i < '4'){
		i -= '0';
		if(!(State & ESP_MUX) && i > 0){
			if(DebugLevel > 0)	Debug("CIPCLOSE= cant close connection > 0 when MUX=0",0);//TODO does it return error or just OK(since it would already be closed)
			TxP_ERROR();
		}else{
			CloseSocket(i-'0',1);
			TxP((const char*)"CLOSED\r\n");
		}
	}else if(i == '5' && (State & ESP_MUX)){//close all connections
		for(uint32_t i=0;i<5;i++)
			CloseSocket(i,1);
		TxP((const char*)"CLOSED\r\n");
		
	}else{
		if(DebugLevel > 0)	Debug("CIPCLSE=bad connection num must be >=0 <4",0);//TODO what about listen()????????
	}

}


void ESPModule::AT_CIPSTATUS(){
	if(DebugLevel > 0)	Debug("CIPSTATUS",0);
	
	if(!strncmp((const char *)&CommandBuffer[12],"=?\r\n",4)){//this command does nothing? TODO check against other documentation
		TxP_OK();
		return;
	}
	
	if(CommandBuffer[12] != '\r' && CommandBuffer[13] != '\n'){
		TxP_ERROR();
		return;
	}

	TxP((const char *)"status:");

	for(uint32_t i=0;i<4;i++){
		if(Socks[i] == INVALID_SOCKET)
			continue;
	
		TxP((const char *)"+CIPSTATUS:");
		TxI(i);
	
		if(Protocol[i] == ESP_PROTO_UDP)
			TxP((const char *)",\"UDP\",");
		else
			TxP((const char *)",\"TCP\",");
	
		//TODO PRINT REAL IP ADDRESS FOR CONNECTION
		TxP((const char *)"127.0.0.1,");

		//TODO PRINT REAL PORT FOR CONNECTION
		TxP((const char *)"1000");

		//TODO PRINT REAL ROLE(SERVER OR CLIENT) FOR THIS CONNECTION
		TxP((const char *)"0");
	
		TxP((const char *)"\r\n");
	}
	TxP_OK();
}

void ESPModule::AT_CIPSERVER(){
//Note that this command is TCP only, to listen for UDP packets you just need to "AT+CIPSTART=0,"UDP",999" and have something send data to the port(connectionless)

/*
1. Server can only be created when AT+CIPMUX=1
2. Server monitor will automatically be created when Server is created.
3. When a client is connected to the server, it will take up one connection?and be provided an ID
TODO*/

	if(DebugLevel > 0)	Debug("CIPSERVER\n",0);

	char c = CommandBuffer[3+11];

	if(CommandBuffer[3+10] != '=' || c < '0' || c > '3' || CommandBuffer[3+12] != ',' || CommandBuffer[3+13] < '1' || CommandBuffer[3+13] > '9'){//bad format
		TxP_ERROR();
		return;
	}
	
	int32_t p = Atoi((char *)&CommandBuffer[3+13]);
	
	if(!p || p > 65535){
		TxP_ERROR();
		return;
	}

	uint32_t i=0;
	for(;i<5;i++){//look for a free socket

	}
	if(i > 3){//no free socket
		TxP_ERROR();
		return;
	}
/*
		unsigned long block_mode = 1;
#ifdef _WIN32
		if(ioctlsocket(Socks[sock],FIONBIO,&block_mode) != NO_ERROR){
#else
		if(ioctl(Socks[sock],FIONBIO,&block_mode) != NO_ERROR){
#endif
			if(DebugLevel > 0)	Debug("Ioctl() failed to set non-blocking mode, error",GetLastError());
		return INVALID_SOCKET;
    }

	Listen(i);
*/
	CloseSocket(i,1);
/*
	i = CreateSocket(i,ESP_PROTO_TCP,p);
	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_port = htons(27015);

    if (bind(i,
             (SOCKADDR *) & service, sizeof (service)) == SOCKET_ERROR) {
        wprintf(L"bind failed with error: %ld\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    //----------------------
    // Listen for incoming connection requests.
    // on the created socket
    if (listen(ListenSocket, 1) == SOCKET_ERROR) {
        wprintf(L"listen failed with error: %ld\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
*/
	ServerTimer = 1;//this will start counting up until specified time limit("AT+CIPSTO")
	TxP_OK();
}


void ESPModule::AT_CIPMUX(){
//This mode can only be changed after all connections are disconnected. If server is started, reboot is required TODO

int32_t at_error = 0;
		if(!strncmp((char*)&CommandBuffer[9],"?\r\n",3)){//query only
			if(DebugLevel > 0)	Debug("MUX query only",0);
			if(State & ESP_MUX)
				TxP((const char*)"MUX is 1\r\n");
			else
				TxP((const char*)"MUX is 0\r\n");
			return;

		}else if(CommandBuffer[3+6] == '='){//setting mode
			if(!strncmp((char*)&CommandBuffer[3+8],"\r\n",2)){//properly terminated?

				if(CommandBuffer[3+7] == '0'){//turn MUX off

					State &= ~ESP_MUX;
					if(DebugLevel > 0)	Debug("MUX set 0",0);
					int32_t s;
					for(s=1;s<4;s++)//only connection 0 is usable without MUX
						CloseSocket(s,1);
					TxP_OK();

				}else if(CommandBuffer[3+7] == '1'){//turn MUX on, 4 connections max
					
					if(DebugLevel > 0)	Debug("MUX set 1",0);
					State |= ESP_MUX;
					TxP_OK();
				
				}else{//error
					if(DebugLevel > 0)	Debug("MUX set not \'0\' or \'1\'",0);
					TxP_ERROR();
				}
			}else
				at_error = 1;
		}else
			at_error = 1;

	if(at_error){
		
	}
}


void ESPModule::AT_BAD_COMMAND(){

	CommandBuffer[sizeof(CommandBuffer)-1] = 0;
	printf("Bad command:%s\n",CommandBuffer);

	if(DebugLevel > 0)	Debug("Bad AT command",0);
	TxP((const char*)"this no fun\r\n");//it really says that..
	//we need to eliminate this entire command now, including the /r/n which do exist if we got here

}


void ESPModule::AT_CWQAP(){

	if(!strncmp((char*)&CommandBuffer[8],"?\r\n",3)){//do something special in this case for some firmwares?

	}else if(!strncmp((char*)&CommandBuffer[8],"\r\n",2)){
		CloseSocket(ESP_ALL_CONNECTIONS,0);
		State ^= ESP_AP_CONNECTED;
		if(DebugLevel > 0)	Debug("CWQAP disconnected from wifi",0);

		TxP_OK();
	}else//bad format
		TxP_ERROR();
}


void ESPModule::AT_GSLP(){

	if(CommandBuffer[8] < '1' || CommandBuffer[8] > '9'){
		if(DebugLevel > 0)	DebugC("GSLP bad format, must use digits 0-9, cannot start with 0. Got:",(char)CommandBuffer[8]);
		TxP_ERROR();
		return;
	}
	int32_t i;
	for(i=0;i<sizeof(CommandBuffer);i++){//terminate the string so Atoi works as expected
		if(CommandBuffer[i] == '\r'){
			CommandBuffer[i] = 0;
			break;
		}
	}
	int32_t usec = Atoi((char *)&CommandBuffer);
	//if(usec > ?)usec = ?;//TODO - find out the limits on the real hardware
	if(DebugLevel > 0)	Debug("8266 going into deep sleep, ms long:",usec);
	TimeStall(ESP_AT_MS_DELAY*usec);
	if(DebugLevel > 0)	Debug("8266 woke up from deep sleep",0);

}


void ESPModule::AT_CWLIF(){

	//https://github.com/espressif/ESP8266_AT/wiki/CWLIF
	if(DebugLevel > 0)	Debug("AT+CWLIF-command does nothing\n",0);
	TimeStall(ESP_AT_OK_DELAY);
	TxP_OK();

}


void ESPModule::AT_CWDHCP(){//really not useful for us

	//https://github.com/espressif/ESP8266_AT/wiki/CWDHCP
	if(DebugLevel > 0)	Debug("AT+CWDHCP-command does nothing\n",0);
	TimeStall(ESP_AT_OK_DELAY);
	TxP_OK();
}


uint32_t ESPModule::VerifyMACString(uint32_t off){//pass the offset to the first " at the start of the command string
	if(CommandBuffer[off] != '"' || CommandBuffer[off+12+5] != '"')
		return 1;
	
	for(uint32_t i=off+1;i<off+12+5;i+=3){
		char c = CommandBuffer[i];
		if(c < '0' || (c > '9' && c < 'A') || (c < 'a' && c > 'F') || c > 'F')
			return 0;

		c = CommandBuffer[i+1];
		if(c < '0' || (c > '9' && c < 'A') || (c < 'a' && c > 'F') || c > 'F')
			return 0;

		if(i <= off+12+3 && CommandBuffer[i+2] != ':')
			return 0;
	}

}

void ESPModule::SaveStationMAC(){


}

void ESPModule::AT_CIPSTAMAC(){//set MAC address of station, also "AT+CIPSTAMAC_CUR" and "AT+CIPSTAMAC_DEF"
	printf("CIPSTAMAC\n");
	//https://github.com/espressif/ESP8266_AT/wiki/CIPSTAMAC

	if(!strncmp((const char*)&CommandBuffer[12],"?\r\n",3)){//query only

		if(DebugLevel > 0)	Debug("AT+CIPSTAMAC query only\n",0);
		TimeStall(ESP_AT_OK_DELAY);
		TxP("+CIPSTAMAC:");
		TxP(StationMAC);
		TxP("\r\n");
		TxP_OK();

	}else{

		if(!VerifyMACString(13)){
			TxP_ERROR();
			return;
		}
		CommandBuffer[14+12+5] = '\0';//overwrite last " and make it into a proper string
		strcpy((char *)&CommandBuffer[14],StationMAC);
		
	}
}


void ESPModule::AT_CIPAPMAC(){//set MAC address of SoftAP, also "AT+CIPAPMAC_CUR" and "AT+CIPAPMAC_DEF"

	//https://github.com/espressif/ESP8266_AT/wiki/CIPAPMAC
	printf("!!!!!!!!!!CIPAPMAC!\n");

	if(!strncmp((const char*)&CommandBuffer[11],"?\r\n",3)){//query only

		if(DebugLevel > 0)	Debug("AT+CIPAPMAC query only\n",0);
		//TimeStall(ESP_AT_OK_DELAY);
		TxP("+CIPAPMAC:\"");
		TxP(SoftAPMAC);
		TxP("\"\r\n");
		TxP_OK();

	}else{

		if(!VerifyMACString(12)){
			TxP_ERROR();
			return;
		}
		CommandBuffer[13+12+5] = '\0';//overwrite last " and make it into a proper string
		strcpy((char *)&CommandBuffer[13],SoftAPMAC);

	}


}


void ESPModule::AT_CIPSTA(){//set IP address of station, "AT+CIPSTA_CUR" and "AT+CIPSTA_DEF"

	//https://github.com/espressif/ESP8266_AT/wiki/CIPSTA
	if(!strncmp((const char*)&CommandBuffer[9],"?\r\n",3)){//query only

		if(DebugLevel > 0)	Debug("AT+CIPSTA query only\n",0);
		TimeStall(ESP_AT_OK_DELAY);
		TxP("+CIPSTA:");
		TxP(StationIP);
		TxP("\r\n");
		TxP_OK();

	}else{

		if(!VerifyMACString(10)){
			TxP_ERROR();
			return;
		}
		CommandBuffer[11+12+5] = '\0';//overwrite last " and make it into a proper string
		strcpy((char *)&CommandBuffer[11],StationIP);

	}


}


void ESPModule::AT_CIPAP(){//set IP address of SoftAP, also "AT+CIPAP_CUR" and "AT+CIPAP_DEF"

	//https://github.com/espressif/ESP8266_AT/wiki/CIPAP

	if(!strncmp((const char*)&CommandBuffer[8],"?\r\n",3)){//query only

		if(DebugLevel > 0)	Debug("AT+CIPAP query only\n",0);
		TimeStall(ESP_AT_OK_DELAY);
		TxP("+CIPAP:");
		TxP(SoftAPIP);
		TxP("\r\n");
		TxP_OK();

	}else{

		if(!VerifyMACString(10)){
			TxP_ERROR();
			return;
		}
		CommandBuffer[10+12+5] = '\0';//overwrite last " and make it into a proper string
		strcpy((char *)&CommandBuffer[10],SoftAPIP);


	}

}


void ESPModule::AT_CIPSTO(){

	//https://github.com/espressif/ESP8266_AT/wiki/CIPSTO
	//server timeout, range 0~7200 seconds
	//if 0, will never timeout

	if(!strncmp((const char*)&CommandBuffer[9],"?\r\n",3)){//query only

		if(DebugLevel > 0)	Debug("AT+CIPSTO QUERY ONLY\n",0);
		TxP("+CIPSTO:");
		TxI(ServerTimeout);
		TxP("\r\n");

	}else{
		uint32_t val = Atoi((char *)&CommandBuffer[10]);
		if(val > 7200)
			TxP_ERROR();
		else{
			ServerTimeout = val;
			TxP_OK();
		}
	}
}


void ESPModule::AT_CIFSR(){

	//https://github.com/espressif/ESP8266_AT/wiki/CIFSR

	if(DebugLevel > 0)	Debug("CIFSR request",0);
	//TODO DO SOFTAP, OR NO IP IF NOT CONNECTED
	TxP("+CIFSR:");
	TxP(WifiIP);
	TxP_OK();

}


void ESPModule::AT_UART(){//also "AT+UART_CUR" and "AT+UART_DEF"

	uint32_t b = Atoi((char *)&CommandBuffer[8]);
	int i=0;
	for(i=0;i<10;i++){
		if(CommandBuffer[8+i] < '0' || CommandBuffer[8+i] > '9')
			break;
	}
	if(i == 10 || CommandBuffer[8+i] != ','){
		TxP_ERROR();
		return;
	}

//	for(i=0;i<sizeof(SupportedBaudRates);i++){
		
//	}

}

void ESPModule::AT_RFPOWER(){
	//range 0 ~ 82, unit:0.25dBm
	if(!strncmp((char*)&CommandBuffer[3+7],"?\r\n",3)){//query only...not supported on real hardware?
		TxP("+RFPOWER:\"");
		TxI(RFPower);
		TxP("\"\r\n");

	}else{//set
		char c1 = CommandBuffer[3+8];
		char c2 = CommandBuffer[3+9];
		char c3 = CommandBuffer[3+10];

		if(CommandBuffer[3+7] != '=' || c1 < '0' || c1 > '9' || (c2 != '\r' && (c2 < '0' || c2 > '9')) || (c2 == '\r' && c3 != '\n')){ 
			TxP_ERROR();
			return;
		}

		int p = CommandBuffer[3+8] - '0';
		if(CommandBuffer[3+9] != '\r')
			p += CommandBuffer[3+9]-'0';
		
		if(p > 82){//out of range
			TxP_ERROR();
			return;
		}

		RFPower = p;
	}
	
	TxP_OK();
}

void ESPModule::AT_RFVDD(){


}

void ESPModule::SetFactoryState(){
	DefaultBaudDivisor = 185;//9600 baud

	memset(SoftAPName,'\0',sizeof(SoftAPName));
	sprintf(SoftAPName,"Uzem SoftAP");
		
	memset(SoftAPPass,'\0',sizeof(SoftAPPass));
	sprintf(SoftAPPass,"password");
		
	memset(SoftAPMAC,'\0',sizeof(SoftAPMAC));
	sprintf(SoftAPMAC,"ec:44:4a:67:cb:d8");
		
	memset(SoftAPIP,'\0',sizeof(SoftAPIP));
	sprintf(SoftAPIP,"10.0.0.1");

	memset(WifiName,'\0',sizeof(WifiName));
	sprintf(WifiName,"Uzem Wifi");

	memset(WifiPass,'\0',sizeof(WifiPass));
	sprintf(WifiPass,"password");

	memset(WifiMAC,'\0',sizeof(WifiMAC));
	sprintf(WifiMAC,"ec:44:4a:67:cb:d8");

	memset(WifiIP,'\0',sizeof(WifiIP));
	sprintf(WifiIP,"10.0.0.1");
}


void ESPModule::AT_RESTORE(){
	
	SetFactoryState();
	SaveConfig();
	TxP_OK();
	AT_RST();
}


void ESPModule::AT_CIPDINFO(){
	if(!strncmp((char*)&CommandBuffer[3+8],"?\r\n",3)){//query only...not supported on real hardware?
		TxP("+CIPDINFO:");
		TxI((State & ESP_CIPDINFO)?1:0);
		TxP("\r\n");
	}else if(CommandBuffer[3+8] == '=' && (CommandBuffer[3+9] == '0' || CommandBuffer[3+9] == '1') && CommandBuffer[3+10] == '\r' && CommandBuffer[3+11] == '\n'){
		
		if(CommandBuffer[3+9] == '0')
			State &= ~ESP_CIPDINFO;
		else
			State |= ESP_CIPDINFO;
		TxP_OK();
	}else
		TxP_ERROR();

}

void ESPModule::AT_PING(){

/*
    for (loop=0;loop < 10; loop++)
    {

        int len=sizeof(r_addr);

        if ( recvfrom(sd, &pckt, sizeof(pckt), 0, (struct sockaddr*)&r_addr, &len) > 0 )
        {
            return 0;
        }

        bzero(&pckt, sizeof(pckt));
        pckt.hdr.type = ICMP_ECHO;
        pckt.hdr.un.echo.id = pid;
        for ( i = 0; i < sizeof(pckt.msg)-1; i++ )
            pckt.msg[i] = i+'0';
        pckt.msg[i] = 0;
        pckt.hdr.un.echo.sequence = cnt++;
        pckt.hdr.checksum = checksum(&pckt, sizeof(pckt));
        if ( sendto(sd, &pckt, sizeof(pckt), 0, (struct sockaddr*)addr, sizeof(*addr)) <= 0 )
            perror("sendto");

        usleep(300000);

    }

    return 1;
*/
}


void ESPModule::AT_CWAUTOCONN(){
	if(!strncmp((char*)&CommandBuffer[3+10],"?\r\n",3)){//query only...not supported on real hardware?
		TxP("+CWAUTOCONN:");
		TxI((State & ESP_AUTOCONNECT)?1:0);
		TxP("\r\n");
	}else if(CommandBuffer[3+10] == '=' && (CommandBuffer[3+11] == '0' || CommandBuffer[3+11] == '1') && CommandBuffer[3+12] == '\r' && CommandBuffer[3+13] == '\n'){
		if(CommandBuffer[3+11] == '0')
			State &= ~ESP_AUTOCONNECT;
		else
			State |= ESP_AUTOCONNECT;
		TxP_OK();
	}else
		TxP_ERROR();
}


void ESPModule::SaveTransLink(){

}

void ESPModule::AT_SAVETRANSLINK(){

	char host[160];
	int i,j;
	for(i=20;i<20+128;i++){
		if(CommandBuffer[i] == '"')
			break;
		host[i-20] = CommandBuffer[i];
		host[(i-20)+1] = '\0';
	}

	char c1 = CommandBuffer[17];//1 or 0, start unvarnished or not
	char c2 = CommandBuffer[i];
	char c3 = CommandBuffer[i+1];

	//"AT+SAVETRANSLINK=1,"uzebox.net",5800,"TCP"\r\n

	int port = Atoi((char *)&CommandBuffer[i+3]);//get the port number
	
	if(c2 != '"' || c3 != ',' || (c1 != '0' && c1 != '1') || CommandBuffer[18] != ',' || CommandBuffer[19] != '"' || port < 1 || port > 65535){
		TxP_ERROR();
		return;
	}

	for(j=i+3;j<i+3+6;j++){//make sure the port number is followed by ,"
		if(CommandBuffer[j] == ',' && CommandBuffer[j+1] == '"')
			break;
	}
	
	if(j == i+3+6 || CommandBuffer[j+5] != '"' || CommandBuffer[j+6] != '\r' || CommandBuffer[j+7] != '\n'){
		TxP_ERROR();;
		return;
	}

	int proto = 0;

	if(!strncmp((const char *)&CommandBuffer[j+2],"TCP",3)){
		proto = 0;		
	}else if(!strncmp((const char *)&CommandBuffer[j+2],"UDP",3)){
		proto = 1;
	}else{
		TxP_ERROR();
		return;
	}

	TransLinkProto = (proto == 0)?ESP_PROTO_TCP:ESP_PROTO_UDP;
	strcpy(TransLinkHost,host);
	TransLinkPort = port;
	SaveTransLink();
		
	TxP_OK();
	
}

void ESPModule::AT_CIPBUFRESET(){
	//fails if not all segments are sent yet or no connection		
	
	int connection = 255;

	if(!(State & ESP_MUX)){//single mode
		if(!strncmp((const char *)&CommandBuffer[14],"\r\n",2))
			connection = 0;

	}else{//multiple connection mode
		if(!strncmp((const char *)&CommandBuffer[16],"\r\n",2)){
			if(CommandBuffer[15] > '0' && CommandBuffer[13] < '9')
				connection = (CommandBuffer[13]-'0')+1;
		}
	}
	
	if(connection == 255)
		TxP_ERROR();
	else{
		TCPSegID[connection] = 0;
		TxP_OK();
	}
}

void ESPModule::AT_CIPCHECKSEQ(){
	AT_BAD_COMMAND();

}

void ESPModule::AT_CIPBUFSTATUS(){
	AT_BAD_COMMAND();

}

void ESPModule::AT_CIPSENDBUF(){
	//TCPSendBuf[] = ...

}

void ESPModule::AT_SENDEX(){
	AT_BAD_COMMAND();

}



void ESPModule::AT_CWSTARTSMART(){
	State |= ~ESP_SMARTCONFIG_ACTIVE;

}

void ESPModule::AT_CWSTOPSMART(){
	State &= ~ESP_SMARTCONFIG_ACTIVE;
}

void ESPModule::AT_CIUPDATE(){

	//https://github.com/espressif/ESP8266_AT/wiki/CIUPDATE
	if(DebugLevel > 0)	Debug("Emulated command does not update Uzem code",0);
	TimeStall(ESP_UZEBOX_CORE_FREQUENCY/4);
	TxP("1: found server\n");
	TimeStall(ESP_UZEBOX_CORE_FREQUENCY/10);
	TxP("2: connect server\n");
	TimeStall(ESP_UZEBOX_CORE_FREQUENCY/8);
	TxP("3: got edition\n");
	TimeStall(ESP_UZEBOX_CORE_FREQUENCY/10);
	TxP("4: start update\n");
	TimeStall(ESP_UZEBOX_CORE_FREQUENCY*5);//fake writing new version to flash...emulator is obviously still the same after reset.
	AT_RST();
}


void ESPModule::AT_RST(){

	State &= ~ESP_DID_FIRST_TICK;//module will reset as soon as the main tick iterates
	WifiTimer = 1;//now the module will take a bit to reconnect to the Wifi AP
	WifiDelay = ESP_AT_CWJAP_DELAY+((rand()%1000)*ESP_AT_MS_DELAY);
}


void ESPModule::AT_GMR(){

	if(DebugLevel > 0)	Debug("GMR query firmware version",0);
	TimeStall(ESP_AT_OK_DELAY);
	TxP(at_gmr_string);
//HACK FOR UZENET CONSOLE
//TimeStall(50000);
//TxP((char *)"WIFI GOT IP\r\n");

//while(1){printf("*");}


}


void ESPModule::AT_CIOBAUD(const char *OverrideString){

	if(DebugLevel > 0)	Debug("CIOBAUD baudrate change",0);

	uint32_t NewDivisor;
	if(OverrideString == NULL)
		NewDivisor = atoi((char*)&CommandBuffer[11]);
	else
		NewDivisor = atoi(OverrideString);
//http://wormfood.net/avrbaudcalc.php?bitrate=600%2C1200%2C2400%2C4800%2C9600%2C14.4k%2C19.2k%2C28.8k%2C38.4k%2C57.6k%2C76.8k%2C115.2k&clock=28.63636&databits=8
//185,//9600
//124,//14400
//92,//19200
//61,//28800
//46,//38400
//30,//57600
//22,//76800
//15,//115200
//7,//230400
//6,//250000

	switch(NewDivisor){
		case 9600:
		BaudDivisor = 185;
		break;
		
		case 14400:
		BaudDivisor = 124;
		break;

		case 19200:
		BaudDivisor = 92;
		break;

		case 28800:
		BaudDivisor = 61;
		break;

		case 38400:
		BaudDivisor = 46;
		break;

		case 57600:
		BaudDivisor = 30;
		break;

		case 76800:
		BaudDivisor = 22;
		break;

		case	115200:
		BaudDivisor = 15;
		break;

		case 230400:
		BaudDivisor = 7;
		break;

		default:
	//	TimeStall(ESP_AT_OK_DELAY);
		TxP_ERROR();
	//	return;
		break;
	};

//	TimeStall(ESP_AT_OK_DELAY);
	TxP_OK();
	BaudDivisor = NewDivisor;
//while(1){printf("*");}

}


void ESPModule::AT_BINARY(){

	if(DebugLevel > 0)	Debug("Switched to binary mode",0);
	TimeStall(ESP_AT_OK_DELAY);
	UartATMode= ESP_MODE_BINARY;
	TxP_OK();

}


void ESPModule::AT_DEBUG(){//Does not exist on any known firmware(yet), a pseudo command for developers
	
	if(CommandBuffer[9] == '='){//set
		if(CommandBuffer[10] >= '0' && CommandBuffer[10] <= '9')
			DebugLevel = CommandBuffer[10]-'0';
		else
			Debug("bad format for psuedo command \"AT+DEBUG\", use \"AT+DEBUG=1\" or \"AT+DEBUG=0\"",0);
	}//no query mode
	TimeStall(ESP_AT_OK_DELAY);
	TxP_OK();
}


inline void ESPModule::ProcessAT(){//look at the byte stream Uzebox has sent, search for completed AT commands, handle sending packets out

	//TODO PROCESS BINARY COMMANDS INSTEAD OF AT?!

	if(false && (UartATState & ESP_MODE_BINARY) && !RxAwaitingTime){
		printf("Binary mode\n");
		if(!RxAwaitingBytes)
			return;
		ProcessBinary();
		return;
	}

	//AT+CIPSEND=X.. single send mode

	if(RxBufferBytes<2 || UzeboxTxRequested)//no data we can touch yet
		return;

//asm volatile("": : :"memory");//memory fence, not needed likely?

	for(uint32_t i=0;i<RxBufferBytes;i++)
		CommandBuffer[i] = RxBuffer[i];


	if(RxAwaitingBytes){//previous command was AT+CIPSEND=, wait until the whole payload has arrived

		if(RxBufferBytes >= RxAwaitingBytes){//payload complete, send it

			NetSend(SendToSocket,(const char *)CommandBuffer,RxAwaitingBytes,0);
			TxP((const char*)"SEND OK\r\n");
			if(DebugLevel > 0){
				CommandBuffer[RxAwaitingBytes] = '\0';
				printf("Payload sent:%s\n",CommandBuffer);
			}
			if(RxBufferBytes > RxAwaitingBytes){//program sent too much data(or followed up too fast)
				TxP((const char*)"busy..\r\n");//TODO, RESEARCH REAL BEHAVIOR TIMING
				Debug("GOT BUSY",0);
			}
			RxBufferBytes = 0;
			RxAwaitingBytes = 0;//get us out of wait for payload mode
		}
		return;
	}

	if(RxBufferBytes == 2 && CommandBuffer[0] == '\r' && CommandBuffer[1] == '\n'){//handle special case
		RxBufferBytes = 0;
		RxBufferPos = 0;
		CommandBuffer[0] = '\0';
		RxBuffer[0] = '\0';
		TxP_ERROR();
		return;
	}


	uint32_t commandlen = 0;

	for(uint32_t i=0;i<RxBufferBytes-1;i++){
		if(CommandBuffer[i] == '\r' && CommandBuffer[i+1] == '\n'){
			commandlen = i+1;
			break;
		}
		if(i >= RxBufferBytes-2)
			return;//no command has been terminated yet
	}

	ClearATCommand();//a command has been completed and transferred to non-volatile buffer, delete it from the Rx stream
	//must remember that now RxBufferBytes is unrelated to how many bytes are in CommandBuffer!!!!!
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	//CommandBuffer[sizeof(CommandBuffer)-1] = 0;//TEMP HACK
	//printf("***Command:%s\n",CommandBuffer);//TEMP HACK


	if(strncmp((char*)CommandBuffer, "AT+", 3)){//no way an AT+X command has been formed
		if(!strncmp((char*)CommandBuffer,"ATE",3)){//check special case for echo on/off

			AT_ATE();
			return;

		}else if(!strncmp((char*)CommandBuffer,"AT\r\n",4)){//check special case for AT

			AT_AT();
			return;

		}else{//command is terminated but it doesn't start with AT!
			if(DebugLevel > 0)	Debug("no preceding AT+",0);
			printf("BAD:%s\n\n",CommandBuffer);

			TxP_ERROR();
			return;
		}
	}

	//to get here we know the buffer starts with "AT+"

	if(CommandBuffer[3] == 'C'){//"C.."
		
		if(CommandBuffer[4] == 'I'){//"CI.."

			//if(commandlen == ...
			
			if(!strncmp((char*)&CommandBuffer[5],"PSEND",5))//send a packet(most common command during gameplay)
				AT_CIPSEND();
			else if(!strncmp((char*)&CommandBuffer[5],"PSTART=",7))//starting a connection
				AT_CIPSTART();
			else if(!strncmp((char*)&CommandBuffer[5],"PCLOSE",6))//close a connection, different format depending on CIPMUX?
				AT_CIPCLOSE();
			else if(!strncmp((char*)&CommandBuffer[5],"PMODE=",6))//set packet sending mode
				AT_CIPMODE();
			else if(!strncmp((char*)&CommandBuffer[5],"FSR",3))//list ip addresse(s)
				AT_CIFSR();
			else if(!strncmp((char*)&CommandBuffer[5],"PSTATUS",7))//get the status of connections
				AT_CIPSTATUS();
			else if(!strncmp((char*)&CommandBuffer[5],"PSERVER=",8))//turn listen mode on or off
				AT_CIPSERVER();
			else if(!strncmp((char*)&CommandBuffer[5],"PMUX",4))//turn multiple connections mode on or off
				AT_CIPMUX();
			else if(!strncmp((char*)&CommandBuffer[5],"OBAUD=",6))//change the baud rate(certain firmwares)
				AT_CIOBAUD(NULL);
			else if(!strncmp((char*)&CommandBuffer[5],"PSTAMAC",7))//set station mac address
				AT_CIPSTAMAC();
			else if(!strncmp((char*)&CommandBuffer[5],"PSTA",4))//set ip address of station
				AT_CIPSTA();
			else if(!strncmp((char*)&CommandBuffer[5],"PAPMAC",6))
				AT_CIPAPMAC();
			else if(!strncmp((char*)&CommandBuffer[5],"PAP",3))//set ip address of softAP(access point32_t emulated by 8266)
				AT_CIPAP();
			else if(!strncmp((char*)&CommandBuffer[5],"PSTO",4))//set server time out(0~7200 seconds)
				AT_CIPSTO();
			else if(!strncmp((char*)&CommandBuffer[5],"UPDATE",6))//get firmware upgrade OTA(fake)
				AT_CIUPDATE();
			else
				AT_BAD_COMMAND();
		
		}else if(CommandBuffer[4] == 'W'){//"CW.."

			if(!strncmp((char*)&CommandBuffer[5],"QAP",3))//quit the AP
				AT_CWQAP();
			else if(!strncmp((char*)&CommandBuffer[5],"LAP\r\n",5))//list AP, fake
				AT_CWLAP();
			else if(!strncmp((char*)&CommandBuffer[5],"SAP",3))//set wifi logon credentials for the HOSTED ap
				AT_CWSAP();
			else if(!strncmp((char*)&CommandBuffer[5],"JAP",3))//this will always pass
				AT_CWJAP();
			else if(!strncmp((char*)&CommandBuffer[5],"MODE",4))//we are not emulating AP mode, but our OS is
				AT_CWMODE();
			else
				AT_BAD_COMMAND();

		}
	
	}else{//!"C.."

		if(!strncmp((char*)&CommandBuffer[3],"GMR\r\n",5))//get firmware version, no arguments possible
			AT_GMR();
		else if(!strncmp((char*)&CommandBuffer[3],"BINARY",6))//switch to non-AT binary mode(doesn't yet exist on the real thing)
			AT_BINARY();
		else if(!strncmp((char*)&CommandBuffer[3],"IPR=",4))//change baud rate(latest standard?)
			AT_IPR();
		else if(!strncmp((char*)&CommandBuffer[3],"GSLP=",5))//go to deep sleep(milliseconds)
			AT_GSLP();
		else if(!strncmp((char*)&CommandBuffer[3],"DEBUG",5))//turn debug console output on/off
			AT_DEBUG();
		else if(!strncmp((char*)&CommandBuffer[3],"RST\r\n",5))//reset module, no arguments allowed
			AT_RST();
		else{//if we get here, we did receive an "AT+" and the ending "\r\n", but the command doesn't fit anything
			printf("BAD COMMAND:%s",CommandBuffer);
			AT_BAD_COMMAND();//literally says:"this no fun.."
		}
	}

}


void ESPModule::ProcessIPD(){//get internet data, put it to Tx
//printf("I");
	int32_t i,num_bytes;
	for(i=0;i<4;i++){

		if(Socks[i] == INVALID_SOCKET)//MUX is automatically handled by this mechanism
			continue;

		//Recv() automatically handles UDP or TCP...TODO
		if((num_bytes = NetRecv(i,(char *)&RxPacket,sizeof(RxPacket),0)) == ESP_SOCKET_ERROR){//no data is available or socket error
			if(GetLastError() != ESP_WOULD_BLOCK){//error, disconnected?
				/*if(DebugLevel > 0)*/	Debug("recv socket error",GetLastError());
				if(false && (UartATState & ESP_MODE_BINARY)){
					TxP((const char*)"0");//closed command
					TxI(i);//connection number, always sent regardless of MUX in binary mode
				}else{
					if(State & ESP_MUX){
						TxP((const char*)"CLOSED ");
						TxI(i);//connection number
						TxP((const char*)"\r\n");
					}else
						TxP((const char*)"CLOSED\r\n");
				}
				CloseSocket(i,1);
			}
			continue;
		}
printf("Got packet,");
		RxPacket[num_bytes] = '\0';
		if(RxAwaitingTime){//unvarnished mode, sent it right away
			printf("Got unvarnished:%s\n",RxPacket);
			int j;
			for(j=0;j<num_bytes;j++)
				TxBuffer[TxBufferPos++] = RxPacket[j];
			//TODO avoid buffer overflow ;)
			return;
		}
		

		//got a packet, with num_bytes length, buffer it and send it when possible
				
		/*if(DebugLevel > 0)*/	Debug("Got packet, num bytes:",num_bytes);
		char header_buffer[32];
		uint32_t t;
		uint32_t poff = 0;	

		while(num_bytes){//send all the data in UART frames of 256 bytes or less
			
			//printf(".");
			t = num_bytes;
			if(t > 256)
			t = 256;

			while(t+TxBufferPos >= sizeof(TxBuffer)-18){//wait for space if required
				printf("-");
				TimeStall(0);
			}


			//TODO CHECK ALL BUFFER SIZES ARE LARGE ENOUGH FOR LARGEST PACKET(THEY AREN'T...)
			if(false && (UartATState & ESP_MODE_BINARY)){
				TxP((const char*)"1");//recv command
				TxI(i);//connection num always sent regardless of MUX
				TxI(num_bytes%256);
				uint32_t k;
				for(k=0;k<num_bytes%256;k++)
					TxBuffer[TxBufferPos++] = RxPacket[poff++];
				if(num_bytes > 256)
					num_bytes = 256;
				else
					num_bytes = 0;

			}else{//normal text based mode
				if(State & ESP_MUX)
					sprintf(header_buffer,"+IPD,%d,%d:",i,t);
				else//single connection mode
					sprintf(header_buffer,"+IPD,%d:",t);
				TxP(header_buffer);
			
				uint32_t k;
				for(k=0;k<t;k++)
					TxBuffer[TxBufferPos++] = RxPacket[poff++];

				if(num_bytes >= 256)
					num_bytes -= 256;
				else{
					//TxP((const char *)"\r\n");
					break;
				}
			}
		}
	}//for


	//if server mode is active, listen for an incoming connection
	if(ServerTimer){
/*
		i = accept(Socks[4]);
		if(i == -1){//some error, most likely nothing is trying to connect on a non-blocking socket
			if(GetLastError() != ESP_WOULD_BLOCK){//it is an actual error
				CloseSocket(4,1);
				//TODO what do do, silent error?
				return;
			}
		}
*/
	}

	
}

void ESPModule::Debug(const char *s, int32_t arg){

		printf("UART DEBUG:%s:%d\n\n",s,arg);
}


void ESPModule::Debug2(const char *s1, const char *s2, int32_t arg1, int32_t arg2){

	printf("\t-ESP8266_debug:%s:%u, %s:%u\n",s1,arg1,s2,arg2);

}

void ESPModule::DebugC(const char *s, char c){
	if(c == '\r')
		printf("\t-debug:%s:\\r\n");
	else if(c == '\n')
		printf("\t-debug:%s:\\n\n");
	else
		printf("\t-debug:%s:%c\n",s,c);
}


void ESPModule::LoadConfig(){

	FlashDirty = 0;
//printf("c");
	FILE *f = fopen("ESP8266.ini","r");
	if(f == NULL){
		Debug("\nESP8266.ini settings file does not exist",0);
		f = fopen("ESP8266.ini","w");
		if(f == NULL)//can't create it
			Debug("Failed to create ESP8266.ini, settings will not be saved",0);
		else
			Debug("Created ESP8266.ini settings file",0);
		fclose(f);

		SetFactoryState();
		SaveConfig();

	}else{// if(f != NULL){//file exists, load wifi credentials and mac address	

		fread(SoftAPName,1,sizeof(SoftAPName),f);
		fread(SoftAPPass,1,sizeof(SoftAPPass),f);
		fread(SoftAPMAC,1,sizeof(SoftAPMAC),f);
		fread(SoftAPIP,1,sizeof(SoftAPIP),f);

		fread(WifiName,1,sizeof(WifiName),f);
		fread(WifiPass,1,sizeof(WifiPass),f);
		fread(WifiMAC,1,sizeof(WifiMAC),f);
		fread(WifiIP,1,sizeof(WifiIP),f);
		DefaultBaudDivisor = fgetc(f);

		if(State & ESP_DID_FIRST_TICK){
			printf("\nESP8266.ini configuration loaded\n");
			printf("Wifi SSID:\"%s\"\n",WifiName);
			printf("WifiPass:\"%s\"\n",WifiPass);
			printf("WifiMAC:\"%s\"\n",WifiMAC);
			printf("WifiIP:\"%s\"\n",WifiIP);

			printf("SoftAP SSID:\"%s\"\n",SoftAPName);
			printf("SoftAPPass:\"%s\"\n",SoftAPPass);
			printf("SoftAPMAC:\"%s\"\n",SoftAPMAC);
			printf("SoftAPIP:\"%s\"\n",SoftAPIP);
			printf("Default Baud Divisor:%d",DefaultBaudDivisor);

		}
		fclose(f);
	}


}


void ESPModule::SaveConfig(){

	FlashDirty = 0;

	FILE *f = fopen("ESP8266.ini","w");
	if(f == NULL){
	//	Debug("Can't open ESP8266.ini to save settings",0);
		return;
	}

		fwrite(SoftAPName,1,sizeof(SoftAPName),f);
		fwrite(SoftAPPass,1,sizeof(SoftAPPass),f);
		fwrite(SoftAPMAC,1,sizeof(SoftAPMAC),f);
		fwrite(SoftAPIP,1,sizeof(SoftAPIP),f);


		fwrite(WifiName,1,sizeof(WifiName),f);
		fwrite(WifiPass,1,sizeof(WifiPass),f);
		fwrite(WifiMAC,1,sizeof(WifiMAC),f);
		fwrite(WifiIP,1,sizeof(WifiIP),f);

		fputc(DefaultBaudDivisor,f);
	fclose(f);

}


int32_t ESPModule::Connect(char const *hostname, uint32_t sock, int32_t port, int32_t type){

	if(DebugLevel > 0)	Debug("Connect():",sock);

	LPHOSTENT hostEntry;
	hostEntry = gethostbyname(hostname);
	if(!hostEntry){
		if(DebugLevel > 0){
			Debug("gethostbyname() failed",0);
			Debug(hostname,port);
			Debug2("socket system returns error",NULL,GetLastError(),0);
		}
		return INVALID_SOCKET;
	}

	Socks[sock] = socket(AF_INET,(type & ESP_PROTO_TCP)?SOCK_STREAM:SOCK_DGRAM,(type & ESP_PROTO_TCP)?IPPROTO_TCP:0);

	if(Socks[sock] == INVALID_SOCKET){
		if(DebugLevel > 0)	Debug("Failed to create socket",0);
		return INVALID_SOCKET;
	}
	if(DebugLevel > 0)	Debug("Found host address",0);

	SockInfo[sock].sin_family = AF_INET;
	SockInfo[sock].sin_addr = *((LPIN_ADDR)*hostEntry->h_addr_list);
	SockInfo[sock].sin_port = htons(port);

	if((connect(Socks[sock],(LPSOCKADDR)&SockInfo[sock],sizeof(struct sockaddr))) == SOCKET_ERROR){
		Socks[sock] = INVALID_SOCKET;

		if(GetLastError() == WSAECONNREFUSED){
			if(DebugLevel > 0)	Debug("Remote actively refused connection, wrong port?",0);
			return INVALID_SOCKET;
		}else{
			if(DebugLevel > 0)	Debug("Failed to connect, timeout or socket error",0);
			return INVALID_SOCKET;
			}
		}

		unsigned long block_mode = 1;
#ifdef _WIN32
		if(ioctlsocket(Socks[sock],FIONBIO,&block_mode) != NO_ERROR){
#else
		if(ioctl(Socks[sock],FIONBIO,&block_mode) != NO_ERROR){
#endif
			if(DebugLevel > 0)	Debug("Ioctl() failed to set non-blocking mode, error",GetLastError());
		return INVALID_SOCKET;
	}

	bool optval = true;
	int optlen = sizeof(bool);
	setsockopt(Socks[sock],IPPROTO_TCP,TCP_NODELAY,(char *)&optval,optlen);

    return 0;
//    return connect(socket,name,name_len);
}


int32_t ESPModule::Listen(uint32_t port){

	if(DebugLevel > 0)	Debug("ESP8266_Listen()\n",port);
	return 0;

}


int32_t ESPModule::CreateSocket(uint32_t s, uint32_t proto, uint32_t port){

	if(proto == ESP_PROTO_TCP)
		Socks[s] = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	else//UDP
		Socks[s] = socket(AF_INET, SOCK_DGRAM, 0);

	return Socks[s];
}


void ESPModule::CloseSocket(uint32_t socket, bool showdebug){
	if(DebugLevel)	Debug("CloseSocket():",socket);

	if(socket != ESP_ALL_CONNECTIONS){

#ifdef _WIN32
	closesocket(Socks[socket]);
#else
	close(Socks[socket]);
#endif
	Socks[socket] = INVALID_SOCKET;

	}else{//close all connections

		for(uint32_t i=0;i<5;i++){
#ifdef _WIN32
			closesocket(Socks[i]);
#else
			close(Socks[i]);
#endif
			Socks[i] = INVALID_SOCKET;

		}
	}
}

int32_t ESPModule::GetLastError(){
#ifdef _WIN32
	return WSAGetLastError();
#else
	return errno;
#endif
}


int32_t ESPModule::NetSend(uint32_t s, const char *buf, int32_t len, int32_t flags){
if(len == 0)
printf("nolen!");
	//if(DebugLevel)	Debug("ESP8266 send()",s);
	return send(Socks[s],buf,len,flags);
}


int32_t ESPModule::NetRecv(uint32_t s, char *buf, int32_t len, int32_t flags){
	if(Protocol[s] == ESP_PROTO_TCP)
		return recv(Socks[s],buf,len,flags);
//	else//UDP
//		return recvfrom(
}


int32_t ESPModule::StrLen(char *str){
	int32_t len = 0;
	
	while(*str){
		str++;
		len++;
	}
	return len;
}

void ESPModule::SetBootState(){
	CloseSocket(ESP_ALL_CONNECTIONS,0);
	State = (State & ESP_DID_FIRST_TICK)?ESP_DID_FIRST_TICK:0;
	
	State |= (ESP_START_UP|ESP_AP_CONNECTED|ESP_AS_STA|ESP_AP_CONNECTED);
	BusyCycles = 0;
	UartATMode = ESP_MODE_TEXT;
	Ready = 1;
	ResetUart();

}

void ESPModule::ResetUart(){
	UzeboxTxBufferPos =
	UzeboxRxBufferPos =
	UzeboxRxBufferBytes = 
	//UzeboxTxBufferBytes =
	TxBufferPos =
	RxBufferPos =
	RxBufferBytes
	//TxBufferBytes
	= 0;
	
	for(uint32_t i=0;i<sizeof(UzeboxTxBuffer);i++)
		UzeboxTxBuffer[i] = 0;
	for(uint32_t i=0;i<sizeof(UzeboxRxBuffer);i++)
		UzeboxRxBuffer[i] = 0;
	for(uint32_t i=0;i<sizeof(CommandBuffer);i++)
		CommandBuffer[i] = 0;

	
}


int ESPModule::HandleReset(){

	if(ResetPinState && PrevResetPinState)//we are operating
		return 0;

	if(!ResetPinState){//can't operate when RST pin grounded

		if(PrevResetPinState)//wasn't grounded last tick
			if(DebugLevel)	Debug("Reset pin grounded",0);

		ResetUart();
		TimeStall(0);
		ResetUart();//we are not listening
		PrevResetPinState = 0;
		return 1;

	}else if(!PrevResetPinState){//RST pin just got released, boot

		if(DebugLevel > 0)	Debug("Reset pin released",0);
		PrevResetPinState = 1;
		ResetUart();
		TimeStall(ESP_RESET_BOOT_DELAY);
		ResetUart();//we are not listening, we wont get to read any data during this process
	}
	
		/*if(DebugLevel > 0)*/	printf((const char*)"\nESP8266 Emulation Core %s Start Up\n",ESP8266_CORE_VERSION);
		//State &= ~ESP_START_UP;
		LoadConfig();
		ResetUart();
		if(!ResetPinState)//program reset again before boot was complete, do not send startup messages
			return 1;

		TxP(start_up_string);//includes "ready\r\n"
		TxP_OK();

		if(InitializeSocketSystem() == SOCKET_ERROR){

			State &= ~ESP_INTERNET_ACCESS;
               /* if(DebugLevel > 0)*/	Debug("ESP8266_InitializeSocketSystem() FAILED",0);

		}else
			/*if(DebugLevel > 0)*/	Debug("ESP8266_InitializeSocketSystem() SUCCESS",0);
		
		return 0;//ready to operate
	//}
}





void ESPModule::ProcessBinary(){
	
	//TODO 1 byte = 1 command and handle arguments

	uint32_t removebytes = 0;

	if(BinaryState == 0)//need a new command
		BinaryState = RxBuffer[0];

	if(BinaryState == 1){//"AT\r\n"
		removebytes = 1;
		sprintf((char *)&CommandBuffer,"AT\r\n");
		BinaryState = 0;
	
	}else if(BinaryState == 2){//"ATE0\r\n" 
		removebytes = 1;
		sprintf((char *)&CommandBuffer,"ATE0\r\n");
		BinaryState = 0;
	
	}else if(BinaryState == 3){//"ATE1\r\n"
		removebytes = 1;
		sprintf((char *)&CommandBuffer,"ATE1\r\n");
		BinaryState = 0;
		//AT_

	}else if(BinaryState == 4){//"AT+CIPSEND.."
		removebytes = (State & ESP_MUX)?4:3;//send length is 2 bytes
		if(removebytes > RxBufferBytes)//not enough data yet, leave the buffer alone and try again later
			return;

		int n,l;
		if(removebytes == 4){//MUX
			n = RxBuffer[1];//connection number
			l = RxBuffer[2];//length of data
			sprintf((char *)&CommandBuffer,"AT+CIPSEND=%d,%d\r\n",n,l);
		}else{
			l = RxBuffer[2];//length of data
			sprintf((char *)&CommandBuffer,"AT+CIPSEND=%d\r\n",l);
		}
		BinaryState = 0;
		AT_CIPMUX();

	}else if(BinaryState == 5){//"AT+CIPSTART=..."
		removebytes = (State & ESP_MUX)?6:5;//5(1),connection_num(1),proto(1),port(2),hostname_len(1),host_name...
		if(RxBufferBytes <= removebytes || removebytes+RxBuffer[5] > RxBufferBytes)
			return;
		
		int p;

		if(State & ESP_MUX){
			p = (int)RxBuffer[3];
			sprintf((char *)&CommandBuffer,"AT+CIPSTART=%d,\"%d\",\"%s\",%d\r\n",RxBuffer[1],(RxBuffer[2] ? "TCP":"UDP"),RxBuffer[6],p);		
		}else{
			p = (int)RxBuffer[2];
			sprintf((char *)&CommandBuffer,"AT+CIPSTART=\"%d\",\"%s\",%d\r\n",(RxBuffer[2] ? "TCP":"UDP"),RxBuffer[6],p);	
		}
		AT_CIPSTART();
		BinaryState = 0;
	}
}


inline int32_t ESPModule::Atoi(char *s){

	return atoi(s);

}


void ESPModule::TimeStall(uint32_t cycles){
	//Allow UART and clock synchronization with Uzebox. If Uzebox thread detects module thread is not updating the UART it locks in case the thread is stalled.
	//BusyCycles = cycles;

//totalcycles += ModuleClock;
//printf("%d\n",totalcycles);


//////////////////
//////Observation: it appears uzebox thread is moving along much faster....


	do{
//ModuleClock++;//hack
		//UART data synchronization(thread/race safe)

		if(TxRequested){	//Uzebox has requested we sync our Tx, it will not read from this buffer until we clear the flag
//asm volatile("": : :"memory");//memory fence
	
			//to get this request, we already know Uzebox has exhausted all previous bytes
			for(uint32_t i=0;i<TxBufferPos;i++){
				//UzeboxRxBufferBytes++;
				UzeboxRxBuffer[i]= TxBuffer[i];
			}
			UzeboxRxBufferBytes = TxBufferPos;
			TxBufferPos = 0;

asm volatile("": : :"memory");//memory fence
			TxRequested = 0;//let Uzebox know we are done and it can report new data, if any
		}



		if(UzeboxTxBufferPos){//Uzebox has data ready
 			if(!UzeboxTxRequested)//no request in progress?
				UzeboxTxRequested = 1;//tell Uzebox to send it's data
			while(UzeboxTxRequested){//wait until it is done
				Decoherence = 0;
			}
/*		else{//request already in progress, Uzebox always resets this to indicate it is not stalled
//TODO DO WE NEED THIS FOR TIMING?!?!?!?!?!?!?!?!?!
				while(UzeboxTxRequested){//stall until Uzebox thread continues on
					//Sleep(0);
				}
			}
*/
		}

		//UART synchronization end

	//	Sleep(0);//try to yield to other thread, important for speed!?

		//a couple cycles here from race conditions should not matter much, no other issues with this?!
		uint64_t Time = ModuleClock;
		uint64_t Elapsed = Time-OldModuleClock;
		OldModuleClock = Time;

		if(cycles <= Elapsed)
			cycles = 0;
		else
			cycles -= Elapsed;

		asm volatile("": : :"memory");//memory fence
		Decoherence = 0;//let Uzebox thread know we are not hung up too long

		if(RxAwaitingTime){
//printf("%d,",cycles);
		//if(RxAwaitingTime >= ESP_UNVARNISHED_DELAY)
		//	printf("d");
			RxAwaitingTime += Elapsed;
		}
		if(ServerTimer){
			ServerTimer += Elapsed;
			if(ServerTimer > ServerTimeout){//end of listening period
				CloseSocket(5,1);
			}
				
		}

		if(WifiTimer){
			WifiTimer += Elapsed;
			if(WifiTimer > WifiDelay){
				WifiTimer = 0;//turn off counter, do not spawn the message again
				//TODO MAKE IT CONNECT
				TxP((const char*)"WIFI GOT IP\r\n");
			}
		}

asm volatile("": : :"memory");//memory fence
//printf("%d,",ModuleClock);

//TODO KEEP 64 BIT TIMER AND NEVER RESET, SIMPLY COMPARE OLD AND NEW VALUE, 64 BIT WILL NOT ROLL OVER FOR THOUSANDS OF DAYS

		//ModuleClock = 0;

//RACE CONDITION ON SETTING 0 LOSES CYCLES? MANY MANY CYCLES?!?!?!?!?!?!
////////!!!!!!!!!!!!!!!!!!!!!!!!!!
////////!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!



//	printf(".");
	}while(cycles);//delay for the specified cycle count based off Uzebox clock	
}


void ESPModule::TxP_ERROR(){
	TxP((const char*)"ERROR\r\n");
}


void ESPModule::TxP_OK(){
	TxP((const char*)"OK\r\n");
}

