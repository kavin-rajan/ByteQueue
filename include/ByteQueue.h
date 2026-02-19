//**************************** ByteQueue.h ************************************
//  Copyright (c) 2026 Trenser 
//  All Rights Reserved 
//***************************************************************************** 
// 
// Summary    : Contains the declaration of supporting function of ByteQueue
// Note       : 
// Author     : Kavin M
// Date       : 16.02.2026
// 
//***************************************************************************** 
 
#ifndef _BYTEQUEUE_H_ 
#define _BYTEQUEUE_H_ 

//******************************* Include Files ******************************* 
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdlib.h>
#include "Types.h"
//******************************* Global Types ******************************** 

#define BQDEFAULT_SIZE 10
#define ERROR -1

typedef struct __sByteQueue
{
    uint8* pucBuffer;        // Address where data stored
    uint16 unCapacity;       // Maximum size of the buffer
    uint16 unHead;           // Head index of the queue
    uint16 unTail;           // Tail index of the queue
    uint16 unSize;           // current No of elements in queue
    pthread_mutex_t lock;    // Mutex lock
}_sByteQueue;

//***************************** Global Constants ****************************** 
 
//***************************** Global Variables ****************************** 
 
//**************************** Forward Declarations *************************** 
BOOL BqInitByteQueue(_sByteQueue* sByteQueue, uint8* pucBuffer, uint8 unSize);
BOOL BqPush(_sByteQueue* sByteQueue, uint8 ucData);
int16 BqWrite(_sByteQueue* sByteQueue, uint8* pucBuffer, uint8 unSize);
BOOL BqPop(_sByteQueue* sByteQueue, uint8* ucDataRead);
int16 BqRead(_sByteQueue* sByteQueue,  uint8* pucBuffer, uint8 unSize);
int32 BqGetFreeSpace(_sByteQueue* sByteQueue);
int32 BqGetFilledSpace(_sByteQueue* sByteQueue);
BOOL BqFlush(_sByteQueue* sByteQueue);

#endif // _BYTEQUEUE_H_ 
// EOF  