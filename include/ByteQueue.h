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
#include <stdlib.h>
#include "Types.h"
#include "pThreadHelper.h"

//******************************* Global Types ******************************** 
// Enum for type of the ByteQueue
typedef enum __eQueueType
{
    Bq_Overwrite,  // Overwrite if Queue is Full
    Bq_Wait        // Wait if Queue is Full
}_eQueueType;

// This structure implements a FIFO ByteQueue with Head/Tail counters.
// Type of ByteQueue is specified by eByType
// Access to this structure must be synchronized via sLock Mutex 
typedef struct __sByteQueue
{
    uint8 *pucBuffer;        // Address where data stored
    uint32 ulCapacity;       // Maximum size of the buffer
    volatile uint32 ulHead;           // Head index of the queue
    volatile uint32 ulTail;           // Tail index of the queue
    _eQueueType eBqType;     // Type of Byte queue 
    pthread_mutex_t sLock;   // Mutex lock
}_sByteQueue;

//***************************** Global Constants ****************************** 
 
//***************************** Global Variables ****************************** 
 
//**************************** Forward Declarations *************************** 
BOOL BqInitByteQueue(_sByteQueue *sByteQueue, 
                    uint8 *pucBuffer, 
                    uint32 ulSize, 
                    _eQueueType eBqType);
BOOL BqPush(_sByteQueue *sByteQueue, uint8 ucData);
int16 BqWrite(_sByteQueue *sByteQueue, uint8 *pucBuffer, uint32 ulSize);
BOOL BqPop(_sByteQueue *sByteQueue, uint8 *ucDataRead);
int16 BqRead(_sByteQueue *sByteQueue,  uint8 *pucBuffer, uint32 ulSize);
int32 BqGetFreeSpace(_sByteQueue *sByteQueue);
int32 BqGetFilledSpace(_sByteQueue *sByteQueue);
BOOL BqFlush(_sByteQueue *sByteQueue);

#endif // _BYTEQUEUE_H_ 
// EOF  