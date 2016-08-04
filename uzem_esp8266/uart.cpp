#include <stdint.h>
#include "esp8266.h"
#include "uart.h"


void UART_Update(uint32_t cycles){

	if(ESP8266.RxReady && (UART_tx_out_pos != UART_tx_in_pos)){//8266 has processed the last byte, safe to send data over UART
		ESP8266.RxByte = UART_tx_fifo[UART_tx_out_pos];
		if(++UART_tx_out_pos == sizeof(UART_tx_fifo))
			UART_tx_out_pos = 0;

		ESP8266.RxReady = 0;//thread safety, 8266 knows it can read if we set this to 0
	}
	if(ESP8266.TxReady && UART_tx_unread_bytes < sizeof(UART_tx_fifo)-1){//8266 has data ready to send and we have room to buffer
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

void UART_RecalculateBaud(){

	UART_ReadWait = 60000;
	UART_WriteWait = 60000;

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

	UART_tx_fifo[UART_tx_in_pos] = b;
	if(++UART_tx_in_pos == sizeof(UART_tx_fifo))
		UART_tx_in_pos = 0;
	UART_WriteCycles = UART_WriteWait;

}

uint8_t UART_ReadByte(){

	uint8_t t = UART_rx_fifo[UART_rx_out_pos];
	if(++UART_rx_out_pos == sizeof(UART_rx_fifo))
		UART_rx_out_pos = 0;
	UART_ReadCycles = UART_ReadWait;

	return t;

}

