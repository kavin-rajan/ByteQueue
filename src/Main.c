//******************************* Main ****************************************
//  Copyright (c) 2026 Trenser 
//  All Rights Reserved 
//***************************************************************************** 
// 
// File    : Main.c 
// Summary : Contains the main function of ByteQueue.
// Note    : 
// Author  : Kavin M
// Date    : 16.02.2026
// 
//***************************************************************************** 
 
//******************************* Include Files ******************************* 
#include <stdio.h> 
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h> // for sleep()
#include "ByteQueue.h"

//******************************* Local Types ********************************* 
// Structure contains the data to be passed into a Thread
typedef struct threadData
{
    _sByteQueue *sByteQueue;
    uint8 threadId;
}_thereadData;

//***************************** Local Constants ******************************* 

//***************************** Local Variables ******************************* 
 
//****************************** Local Functions ******************************

//******************************.FUNCTION_HEADER.****************************** 
//Purpose : Thread that adds data to the Byte Queue
//Inputs  : arg : pointer to ByteQueue struct
//Outputs : 
//Return  : NULL
//Notes   :
//***************************************************************************** 
void *AddThread(void *arg)
{
    _thereadData *psLocThreadData = (_thereadData*)arg;
    _sByteQueue *sByteQueue = psLocThreadData->sByteQueue;
    
    // local variables;
    uint8 ucMessage[]="Hello";
    uint8 i = 0;

    for (i = 0; i < 5; i++)
    {
        BqPush(sByteQueue, (uint8)ucMessage[i]);
        printf("[AddThread%d] Sent: %c\n", psLocThreadData->threadId, ucMessage[i]);
    }
    return NULL;
}

//******************************.FUNCTION_HEADER.****************************** 
//Purpose : Thread that removes data to the Byte Queue
//Inputs  : arg : pointer to ByteQueue struct
//Outputs : 
//Return  : NULL
//Notes   :
//*****************************************************************************
void *RemoveThread(void *arg)
{
    _sByteQueue *sByteQueue = (_sByteQueue*)arg;
    uint8 ucRemovedByte = 0;
    uint8 i = 0;

    for (i = 0; i < 10; i++)
    {
        BqPop(sByteQueue, &ucRemovedByte);
        printf("[RemoveThread] Received: %c\n", (char)ucRemovedByte);
    }
    return NULL;
}

//******************************.FUNCTION_HEADER.****************************** 
//Purpose : Main function test ByteQueue API
//Inputs  : argc : Number of Arguments, argv[] : Array of arguments
//Outputs : 
//Return  : EXIT_SUCCESS: on Successful execution
//Notes   :
//***************************************************************************** 
int main(int argc, char *argv[])
{
    uint8 ucBuffer[3];
    _sByteQueue sByteQueue;

    _thereadData ThreadA;
    _thereadData ThreadB;

    ThreadA.sByteQueue = &sByteQueue;
    ThreadB.sByteQueue = &sByteQueue;
    ThreadA.threadId = 1;
    ThreadB.threadId = 2;

    pthread_t ThreadAdd;
    pthread_t ThreadRemove;
    pthread_t ThreadAdd1;

    BqInitByteQueue(&sByteQueue, &ucBuffer[0], 3, Bq_Wait);

    // create thread
    pthread_create(&ThreadAdd, NULL, AddThread, (void*)&ThreadA);
    pthread_create(&ThreadRemove, NULL, RemoveThread, (void*)&sByteQueue);
    pthread_create(&ThreadAdd1, NULL, AddThread, (void*)&ThreadB);    
    
    // Join thread
    pthread_join(ThreadAdd, NULL);
    pthread_join(ThreadRemove, NULL);
    pthread_join(ThreadAdd1, NULL);

    // Cleanup
    printf("\nTransmission done... Clean up...\n");
    BqFlush(&sByteQueue);
    return EXIT_SUCCESS;
}
  
// EOF 
