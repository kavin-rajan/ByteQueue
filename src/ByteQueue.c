//**************************** ByteQueue **************************************
//  Copyright (c) 2026 Trenser 
//  All Rights Reserved 
//***************************************************************************** 
// 
// File    : ByteQueue.c 
// Summary : Contains definition for ByteQueue API.
// Note    : 
// Author  : Kavin M
// Date    : 16.02.2026
// 
//***************************************************************************** 
 
//******************************* Include Files ******************************* 
#include "ByteQueue.h"
//******************************* Local Types ********************************* 
 
//***************************** Local Constants ******************************* 

//***************************** Local Variables ******************************* 
 
//****************************** Local Functions ******************************
//******************************.FUNCTION_HEADER.****************************** 
//Purpose : Wrapper for Mutex lock, display error and abort in case of an error
//Inputs  : mutex: Address of Mutex lock
//Outputs : 
//Return  : None
//Notes   : 
 
//***************************************************************************** 
static inline void BqMutexLock(pthread_mutex_t *mutex) {
    int rc = pthread_mutex_lock(mutex);
    if (rc != 0) {
        printf("pthread_mutex_lock failed, error id: %d\n", rc);
        // abort the test if lock fails.
        abort(); 
    }
}

//******************************.FUNCTION_HEADER.****************************** 
//Purpose : Wrapper for mutex Unlock, display error and abort
//Inputs  : mutex: Address of Mutex lock
//Outputs : 
//Return  : None
//Notes   :
 
//***************************************************************************** 
static inline void BqMutexUnlock(pthread_mutex_t *mutex) {
    int rc = pthread_mutex_unlock(mutex);
    if (rc != 0) {
        printf("pthread_mutex_unlock failed, error id: %d\n", rc);
        abort();
    }
}
//******************************.FUNCTION_HEADER.****************************** 
//Purpose : Initializes the byte queue with default values
//Inputs  : *psByteQueue: pointer to Byte Queue structure
//          *pucBuffer  : Address of the buffer memory location
//           unSize     : Size of the buffer
//Outputs : 
//Return  : FALSE: NULL check error or unSize invalid or mutex init error
//          TRUE : if initialization is successful
//Notes   :
 
//***************************************************************************** 
BOOL BqInitByteQueue(_sByteQueue *psByteQueue, uint8 *pucBuffer, uint8 unSize)
{
    // NULL check
    if ((NULL == psByteQueue) || (NULL == pucBuffer))
    {
        printf("pointer is NULL!\n");
        return FALSE;
    }

    // Size validtity check
    if (0 == unSize)
    {
        printf("invalid size\n");
        return FALSE;
    }

    psByteQueue->pucBuffer = pucBuffer;
    psByteQueue->unCapacity = unSize;
    psByteQueue->unHead = 0;
    psByteQueue->unTail = 0;
    psByteQueue->unSize = 0;

    if (0 != pthread_mutex_init(&psByteQueue->lock, NULL))
    {
        // Handle error if initialization fails
        printf("Mutex init failed!\n");
        return FALSE;
    }
    return TRUE;
}

//******************************.FUNCTION_HEADER.****************************** 
//Purpose : Pushes the given input data to Byte Queue
//Inputs  : *psByteQueue: Pointer to Byte Queue
//          ucData      : Data to be pushed to Queue
//Outputs : 
//Return  : FALSE :NULL check error or if queue is full
//          TRUE : if data is pushed to Queue successful
//Notes   :
 
//***************************************************************************** 
BOOL BqPush(_sByteQueue *psByteQueue, uint8 ucData)
{
    // NULL check
    if ((NULL == psByteQueue))
    {
        printf("pointer is NULL!\n");
        return FALSE;
    }

    BqMutexLock(&psByteQueue->lock); // Wait and lock
    // if size and capacity are equal queue is full
    if (psByteQueue->unSize == psByteQueue->unCapacity)
    {
        printf("ByteQueue is full!");
        BqMutexUnlock(&psByteQueue->lock); // Unlock
        return FALSE;
    }
    // Push the data where tail is pointed
    psByteQueue->pucBuffer[psByteQueue->unTail] = ucData;
    // Increase the index of tail 
    // reset tail to zero if it is greater than capacity 
    psByteQueue->unTail = (psByteQueue->unTail + 1) % psByteQueue->unCapacity;
    psByteQueue->unSize ++;

    BqMutexUnlock(&psByteQueue->lock); // Unlock
    return TRUE;
}
//******************************.FUNCTION_HEADER.****************************** 
//Purpose : Writes the given input data stream to Queue
//Inputs  : *psByteQueue: Pointer to ByteQueue
//          *pucBuffer  : Pointer to data stream to be pushed
//          unSize      : Number of data to be pushed
//Outputs : 
//Return  : ERROR: NULL check error
//          nNoOfDataWritten: Number of data written to the Queue
//Notes   :
 
//***************************************************************************** 
int16 BqWrite(_sByteQueue *psByteQueue, uint8 *pucBuffer, uint8 unSize)
{
    // NULL check
    if ((NULL == psByteQueue) || (NULL == pucBuffer))
    {
        printf("pointer is NULL!\n");
        return ERROR;
    }

    // Local variables
    uint8 i = 0;
    int16 nNoOfDataWritten = 0;
    BqMutexLock(&psByteQueue->lock); // Wait and lock
    for (i=0; i < unSize; i++)
    {
        // if size and capacity are equal queue is full
        if ((psByteQueue->unSize) == psByteQueue->unCapacity)
        {
            printf("ByteQueue is full!");
            break;
        }
        // Push the data where tail is pointed
        psByteQueue->pucBuffer[psByteQueue->unTail] = pucBuffer[i];
        // Increase the index of tail 
        // reset tail to zero if it is greater than capacity 
        psByteQueue->unTail = (psByteQueue->unTail + 1) % psByteQueue->unCapacity;
        psByteQueue->unSize ++;

        nNoOfDataWritten ++;
    }
    BqMutexUnlock(&psByteQueue->lock); // Unlock
    return nNoOfDataWritten;
}

//******************************.FUNCTION_HEADER.******************************
//Purpose : Pop the data from Byte Queue
//Inputs  : *psByteQueue: Pointer to ByteQueue
//Outputs : *ucDataRead : data that is read by pop operation
//Return  : FALSE: NULL check error or Queue is empty
//          TRUE :if pop is successful
//Notes   :
 
//*****************************************************************************
BOOL BqPop(_sByteQueue *psByteQueue, uint8 *ucDataRead)
{
    // NULL check
    if ((NULL == psByteQueue) || (NULL == ucDataRead))
    {
        printf("pointer is NULL!\n");
        return FALSE;
    }

    BqMutexLock(&psByteQueue->lock); // Wait and lock
    // check if queue is empty
    if (0 == (psByteQueue->unSize))
    {
        printf("ByteQueue is empty!\n");
        BqMutexUnlock(&psByteQueue->lock); // Unlock
        return FALSE;
    }

    // Local variables
    *ucDataRead = 0;

    // pop the data where head is pointed
    *ucDataRead = psByteQueue->pucBuffer[psByteQueue->unHead];
    // Increase the index of head 
    // reset tail to head if it is greater than capacity 
    psByteQueue->unHead = (psByteQueue->unHead + 1) % psByteQueue->unCapacity;
    psByteQueue->unSize --;

    BqMutexUnlock(&psByteQueue->lock); // Unlock
    return TRUE;

}

//******************************.FUNCTION_HEADER.******************************
//Purpose : Read a stream of data from Byte Queue
//Inputs  : *psByteQueue : Pointer to ByteQueue
//          unSize       : number of data to be Read
//Outputs : *pucBuffer   : Address where the read data is stored
//Return  : ERROR        : NULL check error
//          nNoOfDataRead: Number of data that is read successful
//Notes   :
 
//*****************************************************************************
int16 BqRead(_sByteQueue *psByteQueue, uint8 *pucBuffer, uint8 unSize)
{
    // NULL check
    if ((NULL == psByteQueue) || (NULL == pucBuffer))
    {
        printf("pointer is NULL!\n");
        return ERROR;
    }
    
    // Local variables
    uint8 i = 0;
    int16 nNoOfDataRead = 0;

    BqMutexLock(&psByteQueue->lock); // Wait and lock
    for (i=0; i < unSize; i++)
    {
        // check if queue is empty
        if (0 == (psByteQueue->unSize))
        {
            printf("ByteQueue is empty!");
            break;
        }

        // pop the data where head is pointed
        pucBuffer[i] = psByteQueue->pucBuffer[psByteQueue->unHead];
        // Increase the index of head 
        // reset tail to head if it is greater than capacity 
        psByteQueue->unHead = (psByteQueue->unHead + 1) % psByteQueue->unCapacity;
        psByteQueue->unSize --;

        nNoOfDataRead ++;

    }
    BqMutexUnlock(&psByteQueue->lock); // Unlock
    return nNoOfDataRead;
}

//******************************.FUNCTION_HEADER.******************************
//Purpose : To get the free available space in Byte Queue
//Inputs  : *psByteQueue  : Pointer to ByteQueue
//Outputs : 
//Return  : ERROR         : NULL check error
//          lNoOfFreeSpace: Free space available
//Notes   :
 
//*****************************************************************************
int32 BqGetFreeSpace(_sByteQueue *psByteQueue)
{
    // NULL check
    if ((NULL == psByteQueue))
    {
        printf("pointer is NULL!\n");
        return ERROR;
    }

    int32 lNoOfFreeSpace = 0;

    BqMutexLock(&psByteQueue->lock); // Wait and lock
    lNoOfFreeSpace = (int32)(psByteQueue->unCapacity) - (int32)(psByteQueue->unSize);
    BqMutexUnlock(&psByteQueue->lock); // Unlock

    return lNoOfFreeSpace;
}

//******************************.FUNCTION_HEADER.******************************
//Purpose : To get the filled space in Byte Queue
//Inputs  : *psByteQueue    : Pointer to ByteQueue
//Outputs : 
//Return  : ERROR           : NULL check error
//          lNoOfFilledSpace: Number of space filled in Byte Queue
//Notes   :
 
//*****************************************************************************
int32 BqGetFilledSpace(_sByteQueue *psByteQueue)
{
    // NULL check
    if ((NULL == psByteQueue))
    {
        printf("pointer is NULL!\n");
        return ERROR;
    }
    int32 lNoOfFilledSpace = 0;

    BqMutexLock(&psByteQueue->lock); // Wait and lock
    lNoOfFilledSpace = (int32)(psByteQueue->unSize);
    BqMutexUnlock(&psByteQueue->lock); // Unlock

    return lNoOfFilledSpace; 
}
//******************************.FUNCTION_HEADER.******************************
//Purpose : Removes the memory allocated and clears the byte queue
//Inputs  : *psByteQueue : Pointer to ByteQueue
//Outputs : 
//Return  : FALSE: NULL check error
//          TRUE : if BqFlush successful
//Notes   :

//*****************************************************************************
BOOL BqFlush(_sByteQueue *psByteQueue)
{
    // NULL check
    if ((NULL == psByteQueue))
    {
        printf("pointer is NULL!\n");
        return FALSE;
    }
    psByteQueue->pucBuffer = NULL;
    psByteQueue->unCapacity = 0;
    psByteQueue->unHead = 0;
    psByteQueue->unTail = 0;
    psByteQueue->unSize = 0;  

    return TRUE;
}
// EOF 
