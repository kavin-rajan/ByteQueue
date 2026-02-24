//**************************** pThreadHelper.h ************************************
//  Copyright (c) 2026 Trenser 
//  All Rights Reserved 
//***************************************************************************** 
// 
// Summary    : Contains the declaration thread helper function
// Note       : 
// Author     : Kavin M
// Date       : 24.02.2026
// 
//***************************************************************************** 
 
#ifndef _PTHREADHELPER_H_ 
#define _PTHREADHELPER_H_ 
 
//******************************* Include Files ******************************* 
#include <pthread.h>

//******************************* Global Types ******************************** 

//***************************** Global Constants ****************************** 
 
//***************************** Global Variables ****************************** 
 
//**************************** Forward Declarations *************************** 

//******************************.FUNCTION_HEADER.****************************** 
//Purpose : Wrapper for Mutex lock, display error and abort in case of an error
//Inputs  : mutex : Address of Mutex lock
//Outputs : 
//Return  : None
//Notes   : 
//***************************************************************************** 
static inline void BqMutexLock(pthread_mutex_t *sMutex) 
{
    int16 nLockStatus = (int16)pthread_mutex_lock(sMutex);
    if (nLockStatus != 0) 
    {
        printf("pthread_mutex_lock failed, error id: %d\n", nLockStatus);
        // abort the test if lock fails.
        abort(); 
    }
}

//******************************.FUNCTION_HEADER.****************************** 
//Purpose : Wrapper for mutex Unlock, display error and abort
//Inputs  : mutex : Address of Mutex lock
//Outputs : 
//Return  : None
//Notes   :
//***************************************************************************** 
static inline void BqMutexUnlock(pthread_mutex_t *sMutex) 
{
    int16 nLockStatus = (int16)pthread_mutex_unlock(sMutex);
    if (nLockStatus != 0) 
    {
        printf("pthread_mutex_unlock failed, error id: %d\n", nLockStatus);
        abort();
    }
}
#endif // _PTHREADHELPER_H_ 
// EOF 