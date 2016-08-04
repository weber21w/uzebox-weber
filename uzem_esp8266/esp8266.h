#ifndef _ESP8266_H
#define _ESP8266_H

#include <stdint.h>
#include "SDL.h"

#ifdef _WIN32
	#include <winsock2.h>
#else
	#include <sys/socket.h>
	#include <sys/types.h>
	#include <netinet/in.h>
	#include <netinet/tcp.h>
	#include <arpa/inet.h>
#endif


int ESP8266_Container(void *data);//must be C style function signature...should have made the whole thing totally C.


class ESPModule{

public:
	ESPModule(){
		ESP8266_Thread = SDL_CreateThread(ESP8266_Container, NULL);
	};
	
	~ESPModule(){
		SDL_KillThread(ESP8266_Thread);
	};


	SDL_Thread *ESP8266_Thread;

	volatile	uint8_t		TxBuffer[2048];//data we will send to Uzebox
	volatile	uint32_t		TxBufferPos;//position to write next byte
	volatile	bool	TxRequested;//Uzebox requests we put Tx data into Uzebox Rx buffer(it will not touch it until we clear this flag)

	volatile	uint8_t	RxBuffer[2048];//data we receive from Uzebox
	volatile	uint32_t	RxBufferPos;//position to read from
	volatile	uint32_t	RxBufferBytes;//number of unread bytes
			uint32_t	RxAwaitingBytes;//number of bytes we are waiting for, as a "AT+CIPSEND..." payload
	volatile	uint32_t	RxAwaitingTime;//cycles elapsed counting towards "unvarnished mode" packet interval
	volatile	uint32_t	RxBufferNewData;//new data since last iteration?
	volatile	uint64_t	ModuleClock;//TODO...WHAT IF ROLLOVER ON BYTE WITH NON-ATOMIC 64BIT LOAD?!?!
			uint64_t	OldModuleClock;
			uint8_t	CommandBuffer[2048];//non-volatile buffer that all commands are brought to before the many string compares

	volatile	uint8_t		UzeboxTxBuffer[2048];//data sent to ESP module
	volatile	uint32_t		UzeboxTxBufferPos;//position for Uzebox to write next byte	
	volatile	bool 	UzeboxTxRequested;//ESP module requests we put Tx data into ESP Rx buffer(it will not touch it until we clear this flag)
	
	volatile	uint8_t	UzeboxRxBuffer[4096];//data we receive from ESP module
	volatile	uint32_t	UzeboxRxBufferPos;//position to read from
	volatile	uint32_t	UzeboxRxBufferBytes;//number of unread bytes
	volatile	uint32_t	Decoherence;
	/*volatile*/	uint32_t	DecoherenceLimit;//number of cycles until last forced sync

	uint32_t			ServerTimer;//the amount of time we have waited for an accept()
	uint32_t			ServerTimeout;//the time to wait on accepting a connection(real thing is not indefinite, limit is ~7200 seconds)
	uint32_t			WifiTimer;//the time from reset(previously connected) or a "AT+JAP" to actually finish connecting and send connect message
	uint32_t			WifiDelay;//the time it takes to finish connecting to wifi 
	volatile	uint32_t	BaudDivisor;
	volatile	uint32_t	UzeboxBaudDivisor;
	uint32_t			DefaultBaudDivisor;//baud rate to use upon reset

	uint32_t		ActiveSocket;//the current socket for send operations, stored after send commands while we await the payload
	
	uint8_t		RxPacket[2048];//buffer for received network packets
	uint32_t		RxPacketBytes;//how many bytes are left in the packet
	uint8_t		DebugLevel;//enable various debug console output(very slow)
	uint8_t		ResetPinState;
	uint8_t		PrevResetPinState;
	uint8_t		pad;
	uint8_t		RFPower;

	/*volatile*/ uint32_t		State;
	bool			FlashDirty;
	bool			Ready;
	uint64_t		totalcycles;//temp test hack
	
	uint8_t UCSR0A;	//
	uint8_t UCSR0B;
	uint8_t UCSR0C;
	
	uint8_t		UartScramblerPos;
	uint32_t		UartTxDelayCycles;//Uzebox just sent, how many cycles until we can send again(based on baud rate, module never touches this)
	uint32_t 		UartRxDelayCycles;//Uzebox just got a new byte, how many cycles until we could get a new byte(based on baud rate, module never touches?)
	uint32_t		UartBaudDelayCycles;//the amount of cycles to wait, updated every time baud rate or UART settings are changed(mmodule never touches this)
	uint8_t		UartDataBits;
	uint8_t		UartStopBits;
	uint8_t		UartParity;
	uint8_t		UartFlowControl;

	uint32_t		UartATMode;//waiting for an AT command, waiting for bytes for an AT+CIPSEND, or waiting some milliseconds to send packet for AT+CIPSENDEX?
	uint32_t		UartATState;//0 = AT, 1 = binary mode
	uint8_t 		baud_bits0,baud_bits1;

	uint32_t		SendToSocket;	//the current socket an "AT+CIPSEND/X" is dealing with, that is, where to send the payload when received
	uint32_t		CipMode;
	uint32_t		BinaryState;
	int32_t		Socks[5];
	uint8_t		TCPSendBuf[4][2048];
	uint32_t		TCPSegID[4];//reset with "AT+CIPBUFRESET"
	int32_t		PingSock;
	uint32_t		Protocol[5];
	SOCKADDR_IN	SockInfo[5];

	volatile uint32_t	BusyCycles;

	char			WifiName[64];
	char			WifiPass[64];
	char			WifiMAC[16+1];
	char			WifiIP[16+1];

	char			SoftAPName[64];
	char			SoftAPPass[64];
	char			SoftAPMAC[32];
	char			SoftAPIP[24];
	uint32_t		SoftAPChannel;
	uint32_t		SoftAPEncryption;

	char			TransLinkHost[128];
	uint8_t		TransLinkProto;
	uint16_t		TransLinkPort;

	char			StationMAC[32];//todo
	char			StationIP[24];
	FILE*			ConfigFile;

	void	Tick();

	void SaveConfig();
	void LoadConfig();

	int32_t Atoi(char *s);
	int32_t StrLen(char *s);

	int32_t CheckRxTermination();
	void ProcessCommands();


	void ProcessAT();
	void ProcessIPD();
	void TxP(const char *s);//transmit a string over UART
	void TxI(int32_t i);//transmit an int over UART
	void TxP_ERROR();
	void TxP_OK();
	void UpdateUart();
	void PutByte();
	void GetByte();
	void RecalculateBaud();
	void ResetUart();
	int HandleReset();
	uint16_t GetRandom(uint16_t seed);
	uint32_t VerifyMACString(uint32_t off);

	//wrappers to make sockets call cross platform
	int NetSend(uint32_t s, const char *buf, int32_t len, int32_t flags);
	int32_t NetRecv(uint32_t s, char *buf, int32_t len, int32_t flags);
	int32_t CreateSocket(uint32_t s, uint32_t proto, uint32_t port);
	void CloseSocket(uint32_t s, bool showdebug);
	int32_t Listen(uint32_t s);
	int32_t Connect(char const *hostname, uint32_t sock, int32_t port, int32_t type);
	int32_t GetLastError();//cross platform socket error code wrapper

	void Debug(const char *s, int32_t arg);
	void Debug2(const char *s1, const char *s2, int32_t arg1, int32_t arg2);
	void DebugC(const char *s, char c);
	
	void SetBootState();

	void UCSR0A_Write(uint8_t b);
	void UCSR0B_Write(uint8_t b);
	void UCSR0C_Write(uint8_t b);
	void UCSR0D_Write(uint8_t b);
	void ClearATCommand();
	void TimeStall(uint32_t cycles);

	//AT handlers
	void AT_AT();
	void AT_RST();
	void AT_GMR();
	void AT_GSLP();
	void AT_ATE();
	
	void AT_CWMODE();
	void AT_CWJAP();
	void AT_CWLAP();
	void AT_CWQAP();
	void AT_CWSAP();
	void AT_CWLIF();
	void AT_CWDHCP();
	void AT_CIPSTAMAC();
	void AT_CIPAPMAC();
	void AT_CIPSTA();
	void AT_CIPAP();
	
	void AT_CIPSTATUS();
	void AT_CIPSTART();
	void AT_CIPSEND();
	void AT_CIPCLOSE();
	void AT_CIFSR();
	void AT_CIPMUX();
	void AT_CIPSERVER();
	void AT_CIPMODE();
	void AT_CIPSTO();
	void AT_CIUPDATE();
	void AT_IPR();

	void AT_UART();
	void AT_RFPOWER();
	void AT_RFVDD();
	void AT_RESTORE();
	void AT_CIPDINFO();
	void AT_PING();
	void AT_CWAUTOCONN();
	void AT_SAVETRANSLINK();
	void AT_CIPBUFRESET();
	void AT_CIPCHECKSEQ();
	void AT_CIPBUFSTATUS();
	void AT_CIPSENDBUF();
	void AT_SENDEX();
	void AT_CWSTARTSMART();
	void AT_CWSTOPSMART();
	
	void AT_CIOBAUD(const char *OverrideString);
	void AT_BAD_COMMAND();
	void AT_BINARY();
	void AT_DEBUG();
	void SetFactoryState();
	void SaveTransLink();
	void SaveUART();
	void SaveStationMAC();
	void SaveStationIP();
	void SaveSoftAPMAC();
	void SaveSoftAPIP();
	void SaveSoftAPCredentials();
	void SaveDHCP();
	void SaveMode();
	void SaveWifiCredentials();
	void SaveAutoConn();

	void ProcessBinary();
	int32_t InitializeSocketSystem();
	void FirstTick();

	uint16_t CheckSum(void *b, int32_t len);

	inline void UzeboxUpdateUart(uint32_t cycles);
};



#define ESP_AP_CONNECTED		1	//connected to wifi so AT+CIPSEND is valid
#define ESP_AWAITING_SEND		2	//
#define ESP_START_UP			8	//rebooting
#define ESP_READY			16	//module is ready for commands
#define ESP_MUX				32	//multiple connections mode is active(changes some syntax and adds restrictions)
#define ESP_ECHO			64	//echo back received UART data
#define ESP_PROTO_TCP			128	//standard TCP protocol
#define ESP_PROTO_UDP			256	//standard UDP protocol
#define ESP_ALL_CONNECTIONS	512	//
#define ESP_INTERNET_ACCESS	1024	//not used?
#define ESP_LIST_APS			2048	//currently waiting to spit out simulated AP list
#define ESP_CIPDINFO			4096 //show ip and port information with +IPD messages
#define ESP_SMARTCONFIG_ACTIVE	4096*2
#define ESP_AUTOCONNECT		4096*2*2

#define ESP_AS_AP			1	//softAP only mode(cannot be connected to wifi, some restrictions)
#define ESP_AS_STA			2	//station only mode(cannot be softAP)
#define ESP_AS_AP_STA			3	//softAP and station mode(some restrictions)

#define ESP_MODE_TEXT			0	//normal AT command set
#define ESP_MODE_BINARY		1	//future experiments for binary command set

#define ESP_BIN_ECHO			1//responds back with 0b10101011
#define ESP_BIN_CONNECT		2
#define ESP_BIN_DISCONNECT		3
#define ESP_BIN_SEND			4
#define ESP_BIN_SEND_BIG		5//16 bit payload length specifer


#define ESP_UART_AWAITING_COMMAND		4096
#define ESP_UART_AWAITING_TIME			8192
#define ESP_UART_AWAITING_PAYLOAD		16384

#define ESP_DID_FIRST_TICK				32768//thread has started and did first time load stuff
#define ESP_UZEBOX_ACKNOWLEDGED_THREAD	65536//uzebox waited until we set first tick, then sets this to let us know it is continuing emulation

//Timing(some of this is just guesses from memory with no research, TODO)

#define 	ESP_UZEBOX_CORE_FREQUENCY		28636363UL//(8*3.579545)*1000000//cycles per second
#define	ESP_RESET_BOOT_DELAY			ESP_UZEBOX_CORE_FREQUENCY+(ESP_UZEBOX_CORE_FREQUENCY>>2)//how long after a reset it takes to become ready
#define	ESP_AT_MS_DELAY				ESP_UZEBOX_CORE_FREQUENCY/1000UL//1 millisecond
#define	ESP_AT_OK_DELAY				ESP_AT_MS_DELAY//how quickly a response to a simply command like "AT\r\n" takes to send "OK\r\n"
#define	ESP_AT_CWLAP_DELAY			4000UL*ESP_AT_MS_DELAY
#define	ESP_AT_CWLAP_INTER_DELAY		6UL*ESP_AT_MS_DELAY
#define	ESP_AT_CWJAP_DELAY			1800UL*ESP_AT_MS_DELAY
#define	ESP_UNVARNISHED_DELAY			20UL*ESP_AT_MS_DELAY
#define	ESP_AT_RST_DELAY			ESP_AT_OK_DELAY


#ifdef _WIN32
#define ESP_WOULD_BLOCK		WSAEWOULDBLOCK//for our purposes, just means the reason the function failed is there is no data available(or connection requests)
#else
#define ESP_WOULD_BLOCK		EWOULDBLOCK
#endif

const unsigned char UartScrambler[] = {//random values to add to UART data when baud divisors do not match, not realistic, but functionally it works
44,119,83,4,172,183,28,169,191,3,54,158,224,161,94,228,82,76,214,123,209,139,160,31,216,186,20,5,117,225,90,51,32,89,173,246,95,34,81,198,61,70,231,73,100,140,88,245,35,27,166,200,235,128,193,189,11,30,43,2,126,201,164,85,253,8,109,250,16,84,67,239,49,174,175,53,157,39,136,33,152,241,112,242,40,10,101,18,130,50,103,132,113,171,255,71,108,116,162,22,254,142,151,55,86,182,222,170,69,45,48,65,247,24,60,141,17,13,93,56,15,25,177,167,77,21,122,74,29,97,149,63,62,222,134,176,238,120,217,115,234,125,66,23,114,99,212,131,184,213,194,252,79,190,197,244,104,155,233,137,168,6,46,251,150,127,129,121,146,72,37,223,98,26,106,124,138,96,220,1,180,159,102,203,156,110,179,230,145,143,80,12,147,0,133,42,58,78,206,148,135,105,195,36,144,9,38,19,92,165,14,68,91,236,57,249,87,59,7,163,208,211,202,227,153,118,154,47,111,107,187,205,219,178,192,41,215,221,248,229,226,240,237,232,64,204,243,52,181,210,185,188,199,218,75,207
};




#endif
