/*
 * circularBuffer.c
 *
 *  Created on: Mar 29, 2019
 *      Author: Mike Devoe
 */

#include "circularBuffer.h"
#include "stm32L4xx.h"
#include "string.h"

void initBuffer(commBuffer_t* comm, uint8_t type){
	comm->head = 0;
	comm->tail = 0;
	comm->type = type;
	comm->MessageCount= 0;
	comm->bufferSize = 0;
}

uint8_t haveMessage(commBuffer_t* comm){
	if (comm->MessageCount > 0){
		return 1;
	}else{
		return 0;
	}
}

void putChar(commBuffer_t* comm, char ch){

		if (comm->bufferSize == MAXCOMMBUFFER - 1 && ch != '\n') {
			return;
		}
		comm->bufferSize += 1;
		//if head hasn't looped and isn't right behind tail
		if(ch == '\n'){
			comm->MessageCount += 1;
		}else if(ch == '\n'){

			comm->MessageCount += 1;
		}

		comm->buffer[comm->head] = ch;
		comm->head = (comm->head+1)%MAXCOMMBUFFER;
	/*comm->bufferSize += 1;
	if (comm->bufferSize >= MAXCOMMBUFFER) {
		ch = '\n';
	}

	//if head hasn't looped and isn't right behind tail
	if(ch == '\n'){
		comm->MessageCount += 1;
	}else if(ch == '\0'){
		ch = '\n';
		comm->MessageCount += 1;
	}

	comm->buffer[comm->head] = ch;
	comm->head = (comm->head+1)%MAXCOMMBUFFER;*/
}

char getChar(commBuffer_t* comm){

    if (comm->bufferSize == 0) return '\0';
    char c = comm->buffer[comm->tail];
    if (c == '\n'){
    	comm->MessageCount -= 1;
    }
	comm->tail = (comm->tail+1)%MAXCOMMBUFFER;
	comm->bufferSize -= 1;
	return c;
}


void putMessage(commBuffer_t* comm, char* str, uint8_t length){
	for(int i = 0; i < length; i++){
		putChar(comm, str[i]);
	}
	/*int messCount = 0;
	while(messCount <= length){
		if(comm->bufferSize == (MAXCOMMBUFFER - 1)){
			putChar(comm, '\n');
			comm->MessageCount += 1;
			break;
		}else if(comm->bufferSize < (MAXCOMMBUFFER - 1)){
			if(str[messCount] == '\0'){
				putChar(comm, '\n');
			}else{
				putChar(comm, str[messCount]);
			}
			messCount++;
		}
	}*/
}

//get C string from buffer
void getMessage(commBuffer_t* comm, char* str){
	int i = 0;
	while(comm->MessageCount > 0){
		str[i] = getChar(comm);
		if( str[i] == '\n'){
			str[i] = '\n';
			break;
		}
		i++;
	}
}

//get Size of Buffer
int getBufferSize(commBuffer_t* comm){
	return comm->bufferSize;
}
