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
#include <sched.h> // for sched_yield

//******************************* Local Types ********************************* 
 
//***************************** Local Constants ******************************* 

//***************************** Local Variables ******************************* 
 
//****************************** Local Functions ******************************

//******************************.FUNCTION_HEADER.****************************** 
//Purpose : Initializes the byte queue with default values
//Inputs  : pucBuffer   : Address of the buffer memory location
//          ulSize      : Size of the buffer
//          eBqType     : Type of ByteQueue
//Outputs : psByteQueue : pointer to Byte Queue structure
//Return  : FALSE : if initialization fails due to PointerNULL check error or 
//                 mutex init error
//          TRUE : if initialization is successful
//Notes   :
//***************************************************************************** 
BOOL BqInitByteQueue(_sByteQueue *psByteQueue, 
                    uint8 *pucBuffer, 
                    uint32 ulSize, 
                    _eQueueType eBqType)
{
    do
    {
        // Pointer NULL check
        if ((NULL == psByteQueue) || (NULL == pucBuffer))
        {
            printf("pointer is NULL!\n");
            break;
        }

        // Byte Queue initialization and Type configuration
        psByteQueue->pucBuffer = pucBuffer;
        psByteQueue->ulCapacity = ulSize;
        psByteQueue->ulHead = 0;
        psByteQueue->ulTail = 0;
        psByteQueue->eBqType = eBqType;

        // Mutex lock initialization
        if (0 != pthread_mutex_init(&psByteQueue->sLock, NULL))
        {
            // Handle error if initialization fails
            printf("Mutex init failed!\n");
            break;
        }
        return TRUE;
    }
    while (0);
    return FALSE;
}

//******************************.FUNCTION_HEADER.****************************** 
//Purpose : Pushes the given input data to Byte Queue
//Inputs  : ucData      : Data to be pushed to Queue
//Outputs : psByteQueue : Pointer to Byte Queue
//Return  : FALSE : if Push fails due to pointer NULL check Or
//                  ByteQueue type is invalid
//          TRUE : if data is pushed to Queue successful
//Notes   :
//***************************************************************************** 
BOOL BqPush(_sByteQueue *psByteQueue, uint8 ucData)
{
    // Pointer NULL check
    if (NULL == psByteQueue)
    {
        printf("pointer is NULL!\n");
        return FALSE;
    }

    BqMutexLock(&psByteQueue->sLock); // lock mutex

    // ByteQueue Type Handling during Push
    if (Bq_Wait == psByteQueue->eBqType)
    {
        // Wait until an empty space in queue
        do
        {
            // if size and capacity are equal queue is full
            if (BqGetFreeSpace(psByteQueue) <= 1)
            {
                // if queue is full yield
                BqMutexUnlock(&psByteQueue->sLock); // Unlock
                sched_yield();
                BqMutexLock(&psByteQueue->sLock); // lock
            }
            else
            {
                break;
            }
        }
        while (1);
    }
    else if (Bq_Overwrite == psByteQueue->eBqType)
    {
        // Skip Wait for Bq_Overwrite type ByteQueue 
    }
    else
    {
        printf("Invalid ByteQueue Type!!\n");
        BqMutexUnlock(&psByteQueue->sLock); // Unlock
        return FALSE;
    }

    // Push the data where tail is pointed
    psByteQueue->pucBuffer[psByteQueue->ulTail] = ucData;

    // Increase the index of tail 
    // reset tail to zero if it is greater than capacity 
    psByteQueue->ulTail = (psByteQueue->ulTail + 1) % psByteQueue->ulCapacity;

    BqMutexUnlock(&psByteQueue->sLock); // Unlock
    return TRUE;
}
//******************************.FUNCTION_HEADER.****************************** 
//Purpose : Writes the given input array of data to Queue
//Inputs  : pucBuffer   : Pointer to array of data to be pushed
//          ulSize      : Number of data to be pushed
//Outputs : psByteQueue : Pointer to ByteQueue
//Return  : ERROR : if Write fails due to pointer NULL check or
//                  if ulSize is equal to zero
//          nNoOfDataWritten : Number of data written to the Queue
//Notes   :
//***************************************************************************** 
int16 BqWrite(_sByteQueue *psByteQueue, uint8 *pucBuffer, uint32 ulSize)
{
    // Local variables
    uint8 i = 0;
    int16 nNoOfDataWritten = 0;

    do
    {
        // Pointer NULL check
        if ((NULL == psByteQueue) || (NULL == pucBuffer))
        {
            printf("pointer is NULL!\n");
            break;
        }
        // if Size Equal to zero 
        if (0 == ulSize)
        {
            printf("Mininmum size should be atleast 1 !!\n");
            break;
        }

        // iterate through the array of data
        for (i=0; i < ulSize; i++)
        {
            // Push each data into ByteQueue
            if (TRUE == BqPush(psByteQueue, pucBuffer[i]))
            {
                nNoOfDataWritten++;
            }
            else
            {
                printf("Error occured during Push!!\n");
            }
        }

        return nNoOfDataWritten;
    } 
    while (0);
    return ERROR;
}

//******************************.FUNCTION_HEADER.******************************
//Purpose : Pop the data from Byte Queue
//Inputs  : 
//Outputs : ucDataRead  : data that is read by pop operation
//          psByteQueue : Pointer to ByteQueue
//Return  : FALSE : if Pop fails due to pointer NULL check
//          TRUE  : if pop is successful
//Notes   :
//*****************************************************************************
BOOL BqPop(_sByteQueue *psByteQueue, uint8 *ucDataRead)
{
    // Pointer NULL check
    if ((NULL == psByteQueue) || (NULL == ucDataRead))
    {
        printf("pointer is NULL!\n");
        return FALSE;
    }

    BqMutexLock(&psByteQueue->sLock); // Wait and lock

    // Wait if the ByteQueue is Empty
    do
    {
        // check if queue is empty
        if (0 == BqGetFilledSpace(psByteQueue))
        {
            // if queue is Empty yield
            BqMutexUnlock(&psByteQueue->sLock); // Unlock
            sched_yield();
            BqMutexLock(&psByteQueue->sLock); // lock
        }
        else
        {
            break;
        }
    }
    while (1);

    // pop the data where head is pointed
    *ucDataRead = psByteQueue->pucBuffer[psByteQueue->ulHead];

    // Increase the index of head 
    // reset tail to head if it is greater than capacity 
    psByteQueue->ulHead = (psByteQueue->ulHead + 1) % psByteQueue->ulCapacity;
        
    BqMutexUnlock(&psByteQueue->sLock); // Unlock

    return TRUE;
}

//******************************.FUNCTION_HEADER.******************************
//Purpose : Read a stream of data from Byte Queue
//Inputs  : ulSize      : number of data to be Read
//Outputs : pucBuffer   : Address where the read data is stored
//          psByteQueue : Pointer to ByteQueue
//Return  : ERROR       : if Read fails due to pointer NULL check or 
//                        if ulSize is equal to zero
//          nNoOfDataRead : Number of data that is read successful
//Notes   :
//*****************************************************************************
int16 BqRead(_sByteQueue *psByteQueue, uint8 *pucBuffer, uint32 ulSize)
{
    // Local variables
    uint8 i = 0;
    int16 nNoOfDataRead = 0;

    do
    {
        // Pointer NULL check
        if ((NULL == psByteQueue) || (NULL == pucBuffer))
        {
            printf("pointer is NULL!\n");
            break;
        }
        // if Size Equal to zero 
        if (0 == ulSize)
        {
            printf("Mininmum size should be atleast 1 !!\n");
            break;
        }

        // iterate for given ulSize
        for (i=0; i < ulSize; i++)
        {
            // Pop the data into buffer
            if (TRUE == BqPop(psByteQueue, &pucBuffer[i]))
            {
                nNoOfDataRead++;
            }
            else
            {
                printf("Error occured during POP!!\n");
            }
        }
        return nNoOfDataRead;
    } 
    while (0);
    return ERROR;
}

//******************************.FUNCTION_HEADER.******************************
//Purpose : To get the free available space in Byte Queue
//Inputs  : psByteQueue    : Pointer to ByteQueue
//Outputs : 
//Return  : ERROR          : when passed pointer is NULL
//          lNoOfFreeSpace : Free space available
//Notes   : FreeSpace = (Head - Tail - 1 + Capacity) % Capacity
//*****************************************************************************
int32 BqGetFreeSpace(_sByteQueue *psByteQueue)
{
    // Pointer NULL check
    if (NULL == psByteQueue)
    {
        printf("pointer is NULL!\n");
        return ERROR;
    }

    return (((int32)psByteQueue->ulHead - (int32)psByteQueue->ulTail - 1 + 
            (int32)psByteQueue->ulCapacity) % (int32)psByteQueue->ulCapacity);
}

//******************************.FUNCTION_HEADER.******************************
//Purpose : To get the filled space in Byte Queue
//Inputs  : psByteQueue      : Pointer to ByteQueue
//Outputs : 
//Return  : ERROR            : when passed pointer is NULL
//          lNoOfFilledSpace : Number of space filled in Byte Queue
//Notes   : FilledSpace = (Tail - Head + Capacity) % Capacity
//*****************************************************************************
int32 BqGetFilledSpace(_sByteQueue *psByteQueue)
{
    // Pointer NULL check
    if (NULL == psByteQueue)
    {
        printf("pointer is NULL!\n");
        return ERROR;
    }

    return (((int32)psByteQueue->ulTail - (int32)psByteQueue->ulHead + 
            (int32)psByteQueue->ulCapacity) % (int32)psByteQueue->ulCapacity); 
}
//******************************.FUNCTION_HEADER.******************************
//Purpose : Removes the memory allocated and clears the byte queue
//Inputs  : psByteQueue : Pointer to ByteQueue
//Outputs : 
//Return  : FALSE : When passed pointer is NULL
//          TRUE  : if BqFlush successful
//Notes   :
//*****************************************************************************
BOOL BqFlush(_sByteQueue *psByteQueue)
{
    // Pointer NULL check
    if (NULL == psByteQueue)
    {
        printf("pointer is NULL!\n");
        return FALSE;
    }
    BqMutexLock(&psByteQueue->sLock); // Wait and lock
    psByteQueue->ulHead = 0;
    psByteQueue->ulTail = 0; 
    BqMutexUnlock(&psByteQueue->sLock); // Unlock
    return TRUE;
}
// EOF 
