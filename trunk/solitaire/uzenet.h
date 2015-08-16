	extern void UartGoBack(unsigned char count);
	extern u8 UartUnreadCount();
	extern s16 UartReadChar();
	extern s8 UartSendChar(u8 data);		
	extern bool IsUartTxBufferEmpty();
	extern bool IsUartTxBufferFull();
	extern void InitUartTxBuffer();
	extern void InitUartRxBuffer();

uint8_t uzenet_substate,uzenet_iteration,uzenet_step,uzenet_stall_ticks,uzenet_wait_ticks, uzenet_wait2;
uint8_t uzenet_got_records;
inline void Uzenet_SendByte(uint8_t b){

	UartSendChar(b);

}


void Uzenet_HardResetModule(){

	UBRR0H=0;

	//UBRR0L=185;	//9600 bauds	960 bytes/s	16 bytes/field
	UBRR0L=92;	//19200 bauds	1920 bytes/s	32 bytes/field
	//UBRR0L=92;	//38400 bauds	3840 bytes/s	64 bytes/field
	//UBRR0L=30;	//57600 bauds	5760 bytes/s	96 bytes/field

	UCSR0A=(1<<U2X0); // double speed mode
	UCSR0C=(1<<UCSZ01)+(1<<UCSZ00)+(0<<USBS0); //8-bit frame, no parity, 1 stop bit
	UCSR0B=(1<<RXEN0)+(1<<TXEN0); //Enable UART TX & RX

	DDRD|=(1<<PD3);
	PORTD&=~(1<<PD3);
	WaitVsync(1);
	PORTD|=(1<<PD3);
//WaitVsync(120);
	InitUartTxBuffer();
	InitUartRxBuffer();

}


void Uzenet_SendStringP(const char* str){
//return;


	char c;
	while(str!=NULL){
		c=pgm_read_byte(str);
		if(c==0)break;				
		while(UartSendChar(c)==-1); //block if buffer full	
		str++;
	};	
}

void UartSendStringP(const char* str){

	char c;
	while(str!=NULL){
		c=pgm_read_byte(str);
		if(c==0)break;				
		while(UartSendChar(c)==-1); //block if buffer full	
		str++;
	};
		
}

void Uzenet_SendData(uint8_t *dat, uint8_t len){

	uint8_t off = 0;
	while(1){
		UartSendChar(dat[off++]);
		if(off >= len)
			return;
	}
}

void Uzenet_AutoBaud(){
return;
	for(uint8_t i=0;i<60;i++){
	Uzenet_SendStringP(PSTR("AT\r"));
	WaitVsync(1);
	}

}


/*
const char UzenetMinimalScript[] PROGMEM = "AT+"
*/
void Uzenet_MinimalHighScoreUpdate(){
	//pick a baud rate, and run a script all the way through with no error checking
	//this will be extremely slow because we give everything a huge timeout value
	//and don't check if we can move on early. We don't even check if it suceeds
	//so this will send every EEPROM entry every time the game is run. This gives
	//a huge advantage in that it is so small it should fit into any existing game
	//resource wise and not require reworking EEPROM formats much
/*
	static unsigned char uzenet_min_step = 0;
	static unsigned char uzenet_min_iteration = 0;

	if(uzenet_min_wait)
		if(--uzenet_min_wait == 0)
			uzenet_min_step++;

	if(uzenet_min_step == 0){//lower reset pin

		Uzenet_Reset();

	}else if(uzenet_min_step == 1){//raise reset pin and wait for start up text to end(take a guess at baud rate)
	
		PORTD |= (1<<PD3);
		UBRR0L = pgm_read_byte(BaudLookUpTable[uzenet_min_iteration]);//try a new baud rate without checking
		uzenet_min_wait = 240;

	}else if(uzenet_min_step == 2){//join the AP 1 of 2

		Uzenet_SendStringP(PSTR("AT+C"));

	}else if(uzenet_min_step == 3){//finish UART command, hope it worked..

		Uzenet_SendStringP(PSTR("WJAP"));

	}else if(uzenet_min_step == 4){//finish command

		Uzenet_SendStringP(PSTR("\r\n"));
		uzenet_min_wait = 240;	

	}else if(uzenet_min_step == 5){//turn off echo
		
		Uzenet_SendStringP(PSTR("ATE0"));	

	}else if(uzenet_min_step == 6){

		Uzenet_SendStringP(PSTR("\r\n"));	

	}else if(uzenet_min_step == 7){//set baud rate to something we can keep up with!!
		
		Uzenet_SendStringP(PSTR("AT+C"));//send first part of baud string	

	}else if(uzenet_min_step == 6){

		Uzenet_SendStringP("IOBA");	

	}else if(uzenet_min_step == 7){

		Uzenet_SendStringP("UD=4");	

	}else if(uzenet_min_step == 6){

		Uzenet_SendStringP(PSTR("800"));//second part of baud string

	}else if(uzenet_min_step == 7){
	
		Uzenet_SendStringP(PSTR("\r\n"));//complete baud command

	}else if(uzenet_min_step == 8){//set single connection mode
		
		Uzenet_SendStringP(PSTR("AT+CIPMU"));
		
	}else if(uzenet_min_step == 9){
		
		Uzenet_SendStringP(PSTR("X=0\r\n"));

	}

	if(!uzenet_min_wait)
		uzenet_min_step++;


	*/
}

void Uzenet_Reset(){

	UBRR0H=0;

	//UBRR0L=185;	//9600 bauds	960 bytes/s	16 bytes/field
	UBRR0L=92;	//19200 bauds	1920 bytes/s	32 bytes/field
	//UBRR0L=92;	//38400 bauds	3840 bytes/s	64 bytes/field
	//UBRR0L=30;	//57600 bauds	5760 bytes/s	96 bytes/field

	UCSR0A=(1<<U2X0); // double speed mode
	UCSR0C=(1<<UCSZ01)+(1<<UCSZ00)+(0<<USBS0); //8-bit frame, no parity, 1 stop bit
	UCSR0B=(1<<RXEN0)+(1<<TXEN0); //Enable UART TX & RX

	DDRD|=(1<<PD3);
	PORTD&=~(1<<PD3);
//	WaitVsync(1);
//	PORTD|=(1<<PD3);
//WaitVsync(120);
	InitUartTxBuffer();
	InitUartRxBuffer();

	uzenet_wait_ticks = 0;
	uzenet_step = 0;
}


void UzenetCopyPayload(){
	for(uint8_t i=30;i<80;i++)
		eeprom_data[i] = UartReadChar();
}

const char BaudLookUpTable[] PROGMEM = {
186,//9600
124,//14400
92,//19200
61,//28800
46,//38400
30,//57600
22,//76800
15,//115200
//7,//230400
//6,//250000
};

const char packet_string[] PROGMEM = {
0xC7,//magic number
1,
0x00,
SOLITAIRE_UZENET_ID>>8,SOLITAIRE_UZENET_ID&0xFF,
'\r','\n'
};

void UpdateUzenet(){
//return;
//SPrintNum(3,0,uzenet_step,1);
//SPrintNum(3,1,uzenet_wait_ticks,1);
//SPrintNum(6,3,TCNT1,1);
	if(uzenet_wait_ticks)
		if(--uzenet_wait_ticks)
			return;//still waiting for something, try again next tick

//SPrintNum(5,20,uzenet_step,0);
//SPrintNum(5,21,uzenet_wait_ticks,1);

	if(uzenet_step == 0){//RESET MODULE//////////////////////////////////////////////////////////////////////////////////////////////

		UBRR0H=0;
		UCSR0A=(1<<U2X0); // double speed mode
		UCSR0C=(1<<UCSZ01)+(1<<UCSZ00)+(0<<USBS0); //8-bit frame, no parity, 1 stop bit
		UCSR0B=(1<<RXEN0)+(1<<TXEN0); //Enable UART TX & RX

		DDRD|=(1<<PD3);
		PORTD&=~(1<<PD3);//need to turn this pin back on next frame

		InitUartTxBuffer();
		InitUartRxBuffer();

		uzenet_substate = 0;
		uzenet_wait_ticks = 1;
		uzenet_wait2 = 0;
		uzenet_iteration = 0;
		uzenet_step++;

	}else if(uzenet_step == 1){//TURN MODULE BACK ON(RESET PIN)//////////////////////////////////////////////////////////////////////
	
		PORTD |= (1<<PD3);
		uzenet_wait_ticks = 180;//give it a chance to spit out its bootup stuff
		uzenet_step++;
		uzenet_iteration = sizeof(BaudLookUpTable)+1;//counter for number of attempts(each at different baud)
	
	}else if(uzenet_step == 2){//FIND THE BAUD RATE//////////////////////////////////////////////////////////////////////////////////

uzenet_step++;return;//EMULATOR HACK////////////////////////////////////	

		if(uzenet_substate == 0){//attempt to reach ESP8266 on UART

			if(uzenet_wait2 >= uzenet_iteration)//tried every baud rate, something is wrong
				uzenet_step = 0;
			else{

				InitUartRxBuffer();//clear any garbage(from startup or bad baud rates comms)
				UBRR0L = pgm_read_byte(BaudLookUpTable[uzenet_wait2++]);//try a new baud rate
				uzenet_wait_ticks = 2;//make sure a response could be here in time(next tick)
				Uzenet_SendStringP(PSTR("AT\r\n"));//we might have sent garbage at the last baud rate, clear it(could get "ERROR\r\n")
				uzenet_substate++;//move on to next sub-step
			}

		}else if(uzenet_substate == 1){//CLEAR POSSIBLE BAD DATA ON MODULE SIDE(MAY HAVE SENT AT WRONG RATE BEFORE)/////////////////
		
			Uzenet_SendStringP(PSTR("AT\r\n"));
			uzenet_substate++;
			uzenet_wait_ticks = 8;
			uzenet_wait2 = 5;
			
		}else if(uzenet_substate == 2){//CHECK FOR "OK\r\n" TO VERIFY CORRECT BAUD//////////////////////////////////////////////////
			
			//we cleared our buffer last step, so if we don't get "OK\r\n" there was a comms problem
			if(UartUnreadCount() < 4 || UartReadChar() != 'O' || UartReadChar() != 'K'){//wrong baud or other comms problem, keep trying

				if(--uzenet_wait2 == 0)//took too long, start from the top
					uzenet_step = 0;
				else
					uzenet_wait_ticks = 1;
					
			}else{//WE FOUND THE BAUD RATE!!
				uzenet_step++;//uzenet_wait_ticks is 0 so will automatically run next step on the next frame
				uzenet_wait2 = 0;
				uzenet_iteration = 0;
			}
		}


	}else if(uzenet_step == 3){//turn off echo

		Uzenet_SendStringP("ATE0\r\n");
		uzenet_wait_ticks = 1;
		uzenet_step++;
	
	}else if(uzenet_step == 4){//CHANGE 8266 BAUD RATE TO THE ONE WE WANT////////////////////////////////////////////////////////////
		
		Uzenet_SendStringP(PSTR("AT+CIOBAUD=57600\r\n"));
		UBRR0L = 30;	//57600 bauds	5760 bytes/s	96 bytes/field
		uzenet_step++;//uzenet_wait_ticks is 0 so will automatically run next step on the next frame

	}else if(uzenet_step == 5){//VERIFY WIFI CONNECTION//////////////////////////////////////////////////////////////////////////////
		
			//the response from baud rate change should already be in Rx buffer
			InitUartRxBuffer();//we will ignore it and check errors later(should have worked though)
			Uzenet_SendStringP(PSTR("AT+CWJAP\r\n"));
			uzenet_wait2 = 255;
			uzenet_step++;
	
	}else if(uzenet_step == 6){//WAIT FOR WIFI CONNECTION OK MESSAGE/////////////////////////////////////////////////////////////////
		
		if(UartUnreadCount() > 1 && UartReadChar() == 'O' && UartReadChar() == 'K'){//connected
			uzenet_step++;
	//		uzenet_iteration = 0;	
		}else if(--uzenet_wait2 == 0)//timed out, reset and try from the top
			uzenet_step = 0;
		else
			uzenet_wait_ticks = 1;
	
	}else if(uzenet_step == 7){//SET SINGLE CONNECTION MODE//////////////////////////////////////////////////////////////////////////
		
		InitUartRxBuffer();//burn any bytes left from connecting to wifi
		Uzenet_SendStringP(PSTR("AT+CIPMUX=0\r\n"));//this should always work if we got this far
		uzenet_wait_ticks = 1;
		uzenet_step++;
		uzenet_iteration = 0;	

	}else if(uzenet_step == 8){//CONNECT TO UZENET///////////////////////////////////////////////////////////////////////////////////

		InitUartRxBuffer();//burn response from previous command(which should always work)	
		//Uzenet_SendStringP(PSTR("AT+CIPSTART=\"TCP\",\"uzebox.net\""));//we need to split this message to support Tx buffer sized as low as 8
		Uzenet_SendStringP(PSTR("AT+CIPSTART=0,\"TCP\",\"uzebox.net\",51697\r\n"));//EMULATOR HACK////////////////////////////////////	
		uzenet_wait_ticks = 2;//wait for '>'
		uzenet_wait2 = 255;//wait for response in next step
		uzenet_step++;

	}else if(uzenet_step == 9){//VERIFY CONNECTION IS GOOD(check response)///////////////////////////////////////////////////////////
		
		if(UartUnreadCount() > 1 && UartReadChar() == 'O' && UartReadChar() == 'K')//connection good, ready to send packets
			uzenet_step++;
		else if(--uzenet_wait2 == 0)//count down to try again
				uzenet_step = 0;//something went wrong, try again from the start

	}else if(uzenet_step == 10){//PREPARE PACKET SEND///////////////////////////////////////////////////////////////////////////////

			Uzenet_SendStringP("AT+CIPSEND=0,7,\r\n");//EMULATOR HACK////////////////////////////////////	
			uzenet_iteration = 0;
			uzenet_wait_ticks = 2;//give time to receive '>'
			uzenet_wait2 = 4;
			uzenet_substate = 0;
			uzenet_step++;

	}else if(uzenet_step == 11){//BUILD A REQUEST PACKET///////////////////////////////////////////////////////////////////////////
		
		Uzenet_SendByte(0xC7);//magic number
		Uzenet_SendByte(1);//set request params(so we can ask for things later with less bytes)
		Uzenet_SendByte(0x00);//flow control flags
		Uzenet_SendByte(SOLITAIRE_UZENET_ID>>8);//rom ID
		Uzenet_SendByte(SOLITAIRE_UZENET_ID&0xFF);
		Uzenet_SendStringP(PSTR("\r\n"));
		uzenet_wait_ticks = 2;


	}else if(uzenet_step == 12){
		
		//should already have got '>', otherwise we will detect problems later anyway(would be hardware problem)
		if(uzenet_substate == 0){//check if this score should be sent using wait2 as sub-state to save ram
		
			if(eeprom_data[uzenet_iteration*10] & 128){//already sent this score before
			
				if(++uzenet_iteration > 2){//we have cycled through all the entries
		
					uzenet_iteration = 0;
					uzenet_step++;//now retrieve updated high scores from the server
					uzenet_substate = 1;
					uzenet_wait_ticks = 2;
		
				}

			}else//move on to next sub-step
				uzenet_substate++;

		}else if(uzenet_substate == 1){

			Uzenet_SendStringP(PSTR("AT+CIPSEND=0,12\r\n"));//EMULATOR HACK////////////////////////////////////	
			uzenet_wait_ticks = 2;//wait for '>'
			uzenet_substate++;

		}else if(uzenet_substate == 2){//BUILD A REQUEST PACKET

			Uzenet_SendByte(0xC7);//magic number so server knows this isn't run away code
			Uzenet_SendByte(2);//write command
			
			Uzenet_SendByte(eeprom_data[(uzenet_iteration*10)+0]);//send score part including MSB
			Uzenet_SendByte(eeprom_data[(uzenet_iteration*10)+1]);//server sorts everything by MSB so names are irrelevent(older score precedence)
			uzenet_substate++;

		}else if(uzenet_substate == 3){

			for(uint8_t i=2;i<10;i++)//send name bytes(after score because server sorts everything by MSB)
				Uzenet_SendByte(eeprom_data[(uzenet_iteration*10)+i]&127);//don't send the MSB which is used as a flag for different things
			
			Uzenet_SendStringP(PSTR("\r\n"));
			uzenet_wait_ticks = 2;
			uzenet_wait2 = 250;
			uzenet_substate++;

		}else{//make sure the send worked
			if(UartUnreadCount() > 1 && UartReadChar() == 'O' && UartReadChar() == 'K'){//it worked

				uzenet_step++;
				uzenet_wait2 = 0;
				uzenet_iteration = 0;
				eeprom_data[uzenet_iteration*10] |= 128;//make sure we don't try to send this again

			}else if(--uzenet_wait2 == 0){//timed out start from the top

			}

		}

	}else if(uzenet_step == 13){//REQUEST UPDATED LIST
		
		if(uzenet_substate == 1){

			Uzenet_SendStringP(PSTR("AT+CIPSEND=7\r\n"));//EMULATOR HACK////////////////////////////////////	
			uzenet_wait_ticks = 2;//wait for '>'
			uzenet_substate++;			

		}else{

			Uzenet_SendByte(0xC7);//magic number
			Uzenet_SendByte(0x03);//read command
			Uzenet_SendByte(0);//start at top of list
			Uzenet_SendByte(10);//number of bytes we will be reading 2 score bytes + 8 name bytes
			Uzenet_SendByte(5);//5 entries total = 50 bytes
			Uzenet_SendStringP(PSTR("\r\n"));
			uzenet_wait2 = 255;
			uzenet_substate = 1;
			uzenet_step++;
		}

	}else if(uzenet_step == 14){//GET UPDATED LIST
		if(uzenet_iteration == 1){//eat "+IPD,X:"
			if(UartUnreadCount() && UartReadChar() == ':')
				uzenet_iteration++;
			else if(--uzenet_wait2 == 0)//timed out, try it all again from the top
				uzenet_step = 0;		
			
		}else if(uzenet_iteration == 2){
			if(UartUnreadCount() >= 25){
				uzenet_wait2 = 255;
				for(uint8_t i=(3*10);i<(3*10)+25;i++){
					eeprom_data[i] = UartReadChar();
				}
				uzenet_iteration++;
			}else if(--uzenet_wait2 == 0)//timed out
				uzenet_step = 0;

		}else if(uzenet_iteration == 3){

			if(UartUnreadCount() >= 25){
				uzenet_wait2 = 255;
				for(uint8_t i=(3*10)+25;i<(3*10)+25+25;i++)
					eeprom_data[i] = UartReadChar();
				uzenet_iteration++;
			}else if(--uzenet_wait2 == 0)//timed out
				uzenet_step = 0;

		}else{//got the full update, repeatedly attempt to send our scores and get updated list
			uzenet_got_records = 1;//make sure high screen shows this now
			uzenet_step = 12;//go back to send any updated data
			uzenet_iteration = 0;
			uzenet_wait_ticks = 1;
			uzenet_wait2 = 0;
		}

	}

}
