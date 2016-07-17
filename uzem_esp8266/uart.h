#ifndef _UART_H
#define _UART_H

#include "stdint.h"

class Serial{
public:
	uint8_t	tx_fifo[1024*1024];
	uint32_t	tx_in_pos;
	uint32_t	tx_out_pos;
	uint8_t	rx_fifo[1024*1024];
	uint32_t	rx_in_pos;
	uint32_t	rx_out_pos;
	//uint32_t	tx_unread_bytes;

	uint8_t DoubleSpeed;
	uint8_t rx_enabled;
	uint8_t tx_enabled;
	uint8_t bad_baud;


	uint32_t 	ReadWait,WriteWait;
	uint32_t 	ReadCycles,WriteCycles;
	uint8_t 	baud_bits0,baud_bits1;

	uint8_t tx_byte;
	uint8_t rx_byte;

	uint8_t UCSR0A;
	uint8_t UCSR0B;
	uint8_t UCSR0C;
	void Update();
	void ScrambeByte();
	
};

inline void UART_UCSR0A_Write(uint8_t value){
	UART.UCSR0A = value;
}

inline void UART_UCSR0B_Write(uint8_t value){
	UART.UCSR0B = value;
}

inline void UART_UCSR0C_Write(uint8_t value){
	UART.UCSR0C = value;
}
/*
inline uint8_t UART_UCSR0A_Read(){
	return UART.UCSR0A
}

inline uint8_t UART_UCSR0B_Read(){
	return UART.UCSR0B
}

inline uint8_t UART_UCSR0C_Read(){
	return UART.UCSR0C
}
*/


inline uint8_t uart::ScrambleByte(uint8_t b){
	static uint8_t pattern = 0b01011001;
	if(pattern & 1)
		b ^= pattern;
	else
		b &= pattern;
	pattern >>= 1;
	if(!(pattern & 1))
		pattern |= 128;
	return b;
}

inline void uart::Update(uint32_t cycles){
if(UART_UCSR0B & 16)
	printf("tx enabled................\n");
if(UART_UCSR0B & 8)
	printf("rx enabled................\n");

	if(ESP8266.RxReady && (UART_tx_out_pos != UART_tx_in_pos)){//8266 processed last byte, and we have new data to send, safe to send data over UART	
		ESP8266.RxByte = UART_tx_fifo[UART_tx_out_pos];
		if(UART_bad_baud)
			UART_ScrambleByte(ESP8266.RxByte);
		if(++UART_tx_out_pos == sizeof(UART_tx_fifo))
			UART_tx_out_pos = 0;

		ESP8266.RxReady = 0;//thread safety, 8266 knows it can read if we set this to 0
	}

	if(ESP8266.TxReady){//8266 has data ready to send
		UART_rx_fifo[UART_rx_in_pos] = ESP8266.TxByte;
		if(++UART_rx_in_pos == sizeof(UART_rx_fifo))
			UART_rx_in_pos = 0;

		ESP8266.TxReady = 0;//thread safety, 8266 knows it can write if we set this to 0
	}

	//update UART clocks
	//this is done after any current UART data is sent to decouple the thread timing.
	if(UART_rx_in_pos != UART_rx_out_pos){//we have new data available
		if(UART_ReadCycles <= cycles)
			UART_ReadCycles = 0;
		else
			UART_ReadCycles -= cycles;
	}
	if(UART_WriteCycles <= cycles)
		UART_WriteCycles = 0;
	else
		UART_WriteCycles -= cycles;

		
}

void uart::RecalculateBaud(uint16_t regs){

	regs &= 0xFF;
	UART_ReadWait = (regs*16)*8;
	UART_WriteWait = (regs*16)*8;

}

inline uint8_t UART_WriteReady(){
	if(UART_WriteCycles)
		return 0;
	return 1;
}

inline uint8_t UART_ReadReady(){
	if(UART_ReadCycles)
		return 0;
	return 1;
}


void UART_WriteByte(uint8_t b){
	
	if(UART_UCSR0B & 8){//TXENabled
		//while(UART_tx_in_pos == UART_tx_out_pos-1)//8266 thread is locked up and we are out of buffer space, we must pause or the UART will be corrupted
			//SDL_Delay(1);

		UART_tx_fifo[UART_tx_in_pos] = b;
		if(++UART_tx_in_pos == sizeof(UART_tx_fifo))
			UART_tx_in_pos = 0;
	}
	UART_WriteCycles = UART_WriteWait;

}

uint8_t UART_ReadByte(){
	uint8_t t = 0;
	if(UART_UCSR0B & 16){//RXENabled
		t = UART_rx_fifo[UART_rx_out_pos];
		if(++UART_rx_out_pos == sizeof(UART_rx_fifo))
			UART_rx_out_pos = 0;
		UART_ReadCycles = UART_ReadWait;
	}
	return t;

}

#endif/*_UART_H*/


