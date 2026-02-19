//******************************* Main ****************************************
//  Copyright (c) 2026 Trenser 
//  All Rights Reserved 
//***************************************************************************** 
// 
// File    : Main.c 
// Summary : Contains the main function of hexdump.
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
 
//***************************** Local Constants ******************************* 

//***************************** Local Variables ******************************* 
 
//****************************** Local Functions ******************************
//******************************.FUNCTION_HEADER.****************************** 
//Purpose : Thread that adds data to the Byte Queue
//Inputs  : *arg: pointer to ByteQueue struct
//Outputs : 
//Return  : NULL
//Notes   :
 
//***************************************************************************** 

void *AddThread(void *arg)
{
    _sByteQueue *sByteQueue = (_sByteQueue*)arg;
    // local variables;
    uint8 ucMessage[]="Hello";
    uint8 i = 0;

    for (i = 0; i < 5; i++)
    {
        while (!BqPush(sByteQueue, (uint8)ucMessage[i]))
        {
            // If queue is full, wait a bit and try again
            usleep(100000); 
        }
        printf("[AddThread] Sent: %c\n", ucMessage[i]);
        usleep(50000); // Simulate data arriving over time
    }
    return NULL;

}
//******************************.FUNCTION_HEADER.****************************** 
//Purpose : Thread that removes data to the Byte Queue
//Inputs  : *arg: pointer to ByteQueue struct
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
        // Try to dequeue data
        while (!BqPop(sByteQueue, &ucRemovedByte))
        {
            // If queue is empty, wait for Producer to catch up
            usleep(100000); 
        }
        printf("[RemoveThread] Received: %c\n", (char)ucRemovedByte);
        usleep(150000); // Simulate slow processing
    }
    return NULL;

}
//******************************.FUNCTION_HEADER.****************************** 
//Purpose : Main function test ByteQueue API
//Inputs  : argc: Number of Arguments, *argv[]: Array of arguments
//Outputs : 
//Return  : EXIT_SUCCESS: on Successful execution
//Notes   :
 
//***************************************************************************** 
int main(int argc, char *argv[])
{
    uint8 ucBuffer[BQDEFAULT_SIZE];
    _sByteQueue sByteQueue;
    pthread_t ThreadAdd;
    pthread_t ThreadRemove;
    pthread_t ThreadAdd1;

    BqInitByteQueue(&sByteQueue, &ucBuffer[0], BQDEFAULT_SIZE);

    // create thread
    pthread_create(&ThreadAdd, NULL, AddThread, (void*)&sByteQueue);
    pthread_create(&ThreadRemove, NULL, RemoveThread, (void*)&sByteQueue);
    pthread_create(&ThreadAdd1, NULL, AddThread, (void*)&sByteQueue);    
    
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
