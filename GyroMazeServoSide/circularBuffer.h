#ifndef JAH_CIRCULAR_BUFFER_H
#define JAH_CIRCULAR_BUFFER_H

#include "stm32L4xx.h"
#include "USART.h"

#define MAXCOMMBUFFER 2048
typedef struct _commBuffer_t {
	uint32_t head;
	uint32_t tail;
	char buffer[MAXCOMMBUFFER + 1];
	uint8_t type;
	uint32_t MessageCount;
	uint32_t bufferSize;
} commBuffer_t;

#define CIRCULAR_RX 1<<0
#define CIRCULAR_TX 1<<1

//Initializes the struct commBuffer_t to zero, Type = Rx or Tx
void initBuffer(commBuffer_t* comm, uint8_t type);
//Test if a complete message is in buffer delimiter is \n
uint8_t haveMessage(commBuffer_t* comm);
//Put character in buffer and update head
void putChar(commBuffer_t* comm, char ch);
//Get character from buffer and update tail;
char getChar(commBuffer_t* comm);
//put C string into buffer
void putMessage(commBuffer_t* comm, char* str, uint8_t length);
//get C string from buffer
void getMessage(commBuffer_t* comm, char* str);
//get Size of Buffer
int getBufferSize(commBuffer_t* comm);
#endif

