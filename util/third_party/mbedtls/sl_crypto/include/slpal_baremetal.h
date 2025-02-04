/*
 *  Platform Abstraction Layer interface for bare-metal applications.
 *
 *  Copyright (C) 2016, Silicon Labs, http://www.silabs.com
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef MBEDTLS_SLPAL_BAREMETAL_H
#define MBEDTLS_SLPAL_BAREMETAL_H

#if !defined(MBEDTLS_CONFIG_FILE)
#include "config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

/* Need to include emlib configuration file, if present,
   in order to determine CORE_ATOMIC_BASE_PRIORITY_LEVEL. */
#if defined(EMLIB_USER_CONFIG)
#include "emlib_config.h"
#endif

#include "slpal_common.h"
#include "em_assert.h"
#include "em_core.h"
#include <stdbool.h>

/*******************************************************************************
 ******************************   Defines     **********************************
 ******************************************************************************/

/* In order to wait forever in blocking functions the user can pass the
   following value.  */
#define SLPAL_WAIT_FOREVER  (-1)
/* In order to return immediately in blocking functions the user can pass the
   following value.  */
#define SLPAL_NON_BLOCKING  (0)

#if !defined(CORE_ATOMIC_BASE_PRIORITY_LEVEL)
#define CORE_ATOMIC_BASE_PRIORITY_LEVEL  (3)  /* Same as in emlib/em_core.c */
#endif

/* Priority to use for CRYPTO IRQ */
#if defined(MBEDTLS_CRYPTO_IRQ_PRIORITY)
#define SLPAL_CRYPTO_IRQ_PRIORITY MBEDTLS_CRYPTO_IRQ_PRIORITY
#else
#define SLPAL_CRYPTO_IRQ_PRIORITY (0x0)
#endif

/*******************************************************************************
 ******************************   TYPEDEFS    **********************************
 ******************************************************************************/

/* Completion type used to wait for and signal end of operation. */
typedef volatile bool SLPAL_Completion_t;
/* Mutex object used for mutual exclusion, e.g. locking resources. */
typedef volatile unsigned int SLPAL_Mutex_t;

/*******************************************************************************
 ******************************    GLOBALS    **********************************
 ******************************************************************************/
#if defined(SLPAL_TEST)
/* Global variable to keep track of ticks in bare metal test apps. */
extern unsigned int gTicks;
#endif

/*******************************************************************************
 ******************************   FUNCTIONS   **********************************
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *  Enter a critical region.
 *
 * @details
 *  This function enters a critical region using the CORE_EnterAtomic method
 *  implemented in emlib/em_core.c. SLPAL_CriticalEnter returns the current IRQ
 *  state which subsequently should be passed to SLPAL_CriticalExit in order to
 *  restore the IRQ state.
 *
 * @return
 *   IRQ state.
 ******************************************************************************/
__STATIC_INLINE SLPAL_irqState_t SLPAL_CriticalEnter(void)
{
  return (SLPAL_irqState_t) CORE_EnterAtomic();
}

/***************************************************************************//**
 * @brief
 *  Exit a critical region. 
 *
 * @details
 *  This function exits a critical section using the CORE_ExitAtomic method
 *  implemented in emlib/em_core.c. SLPAL_CriticalEnter returns the current IRQ
 *  state which subsequently should be passed to SLPAL_CriticalExit in order to
 *  restore the IRQ state.
 *
 * @param irqState
 *  IRQ state.
 ******************************************************************************/
__STATIC_INLINE void SLPAL_CriticalExit(SLPAL_irqState_t irqState)
{
  CORE_ExitAtomic((CORE_irqState_t) irqState);
}

/***************************************************************************//**
 * @brief
 *  Enter an ISR.
 *
 * @details
 *  This function informs the OS that we have entered an ISR. This function
 *  should be called first thing in all ISRs.
 ******************************************************************************/
__STATIC_INLINE void SLPAL_IsrEnter(void)
{
}

/***************************************************************************//**
 * @brief
 *  Exit an ISR.
 *
 * @details
 *  This function informs the OS that we are about to exit an ISR. This function
 *  should be called last in all ISRs.
 ******************************************************************************/
__STATIC_INLINE void SLPAL_IsrExit(void)
{
}

/***************************************************************************//**
 * @brief
 *  Get thread identifier of calling thread.
 *
 * @details
 *  This function always returns 0 because there is only one single thread
 *  executing in the bare-metal environment.
 *
 * @return
 *   Always 0.
 ******************************************************************************/
__STATIC_INLINE void* SLPAL_ThreadIdGet(void)
{
  return 0;
}

/***************************************************************************//**
 * @brief
 *  Get thread priority of calling thread.
 *
 * @details
 *  This function returns the thread priority of the calling thread. For
 *  bare-metal we return 1 when running at interrupt level, and 0 when running
 *  at non-interrupt level.
 *
 * @return
 *   1 for interrupt level or 0 for non-interrupt level.
 ******************************************************************************/
__STATIC_INLINE unsigned long SLPAL_ThreadPriorityGet(void)
{
  return  (unsigned long) RUNNING_AT_INTERRUPT_LEVEL ? 1 : 0;
}

/***************************************************************************//**
 * @brief
 *  Resume a thread which may have been suspended.
 *
 * @details
 *  This function is empty because suspend/resume threads is not available when
 *  running bare-metal without a multi-thread library.
 *
 * @param threadId
 *  Thread identifier. Ignored.
 ******************************************************************************/
__STATIC_INLINE void SLPAL_ThreadResume(void* threadId)
{
  (void)threadId;
}

/***************************************************************************//**
 * @brief
 *  Suspend a thread
 *
 * @details
 *  This function is empty because suspend/resume threads is not available when
 *  running bare-metal without a multi-thread library.
 *
 * @param threadId
 *  Thread identifier.
 ******************************************************************************/
__STATIC_INLINE void SLPAL_ThreadSuspend(void* threadId)
{
  (void)threadId;
}

/*******************************************************************************
 * @brief
 *  Initialize a completion object.
 *
 * @param pComp
 *  Pointer to an SLPAL_Completion_t object allocated by the user.
 *
 * @return
 *  Always 0 for success
*******************************************************************************/
__STATIC_INLINE int SLPAL_InitCompletion(SLPAL_Completion_t *pComp)
{
  *pComp = false;
  return (0);
}

/*******************************************************************************
 * @brief
 *  Free a completion object.
 *
 * @param pComp
 *  Pointer to an SLPAL_Completion_t object.
 *
 * @return
 *  Always 0 for success
*******************************************************************************/
__STATIC_INLINE int SLPAL_FreeCompletion(SLPAL_Completion_t *pComp)
{
  *pComp = false;
  return (0);
}

/*******************************************************************************
 * @brief
 *  Wait for completion event.
 *
 * @param[in]  pComp
 *  Pointer to completion object which must be initialized by calling
 *  SLPAL_CompletionInit before calling this function.
 * 
 * @param[in]  ticks
 *  Ticks to wait for the completion.
 *  Pass a value of SLPAL_WAIT_FOREVER in order to wait forever. 
 *  Pass a value of SLPAL_NON_BLOCKING in order to return immediately.
 *  
 * @return
 *  0 if success, and SLPAL_ERROR_TIMEOUT if the completion was not completed
 *  within the timeout.
********************************************************************************/
__STATIC_INLINE int SLPAL_WaitForCompletion(SLPAL_Completion_t *pComp, int ticks)
{
  int ret;
  if (ticks == SLPAL_WAIT_FOREVER)
  {
    while( *pComp == false )
    {
#if defined(SLPAL_TEST)
      gTicks++;
#endif
    }
    *pComp = false;
    ret = 0;
  }
  else
  {
    while ((*pComp == false) && (ticks>0))
    {
      ticks--;
#if defined(SLPAL_TEST)
      gTicks++;
#endif
    }
    if (*pComp == true)
    {
      *pComp = false;
      ret = 0;
    }
    else
    {
      ret = SLPAL_ERROR_TIMEOUT;
    }
  }
  
  return( ret );
}

/*******************************************************************************
 * @brief
 *  Signal completion.
 *
 * @param[in]  pComp
 *  Pointer to completion object which must be initialized by calling
 *  SLPAL_CompletionInit before calling this function.
 *  
 * @return
 *   0 for success
********************************************************************************/
__STATIC_INLINE int SLPAL_Complete(SLPAL_Completion_t* pComp)
{
  *pComp = true;
  return ( 0 );
}

/*******************************************************************************
 * @brief
 *  Initialize a mutex object.
 *
 * @param pMutex
 *  Pointer to an SLPAL_Mutex_t object allocated by the user.
 *
 * @return
 *  0 for success, SLPAL_ERROR_OS_SPECIFIC for error.
*******************************************************************************/
__STATIC_INLINE int SLPAL_InitMutex( SLPAL_Mutex_t *pMutex )
{
  *pMutex = 1;
  return( 0 );
}

/*******************************************************************************
 * @brief
 *  Free a mutex object.
 *
 * @param pMutex
 *  Pointer to an SLPAL_Mutex_t object.
 *
 * @return
 *  0 for success, SLPAL_ERROR_OS_SPECIFIC for error.
*******************************************************************************/
__STATIC_INLINE int SLPAL_FreeMutex(SLPAL_Mutex_t *pMutex)
{
  *pMutex = 0;
  return( 0 );
}

/*******************************************************************************
 * @brief
 *  Take (and optionally wait for) a mutex to be given.
 *
 * @param[in]  pMutex
 *  Pointer to mutex object which must be initialized by calling
 *  SLPAL_MutexInit before calling this function.
 * 
 * @param[in]  ticks
 *  Ticks to wait for the mutex.
 *  Pass a value of SLPAL_WAIT_FOREVER in order to wait forever. 
 *  Pass a value of SLPAL_NON_BLOCKING in order to return immediately.
 *  
 * @return
 *  0 if success, and SLPAL_ERROR_TIMEOUT if the mutex was not given
 *  within the timeout.
********************************************************************************/
__STATIC_INLINE int SLPAL_TakeMutex(SLPAL_Mutex_t *pMutex, int ticks)
{
  int ret;
  if (ticks == SLPAL_WAIT_FOREVER)
  {
    while( *pMutex == 0 )
    {
#if defined(SLPAL_TEST)
      gTicks++;
#endif
    }
    *pMutex = 0;
    ret = 0;
  }
  else
  {
    while ( (*pMutex == 0) && (ticks>0) )
    {
      ticks--;
#if defined(SLPAL_TEST)
      gTicks++;
#endif
    }
    if (*pMutex > 0)
    {
      *pMutex = 0;
      ret = 0;
    }
    else
    {
      ret = SLPAL_ERROR_TIMEOUT;
    }
  }
  
  return( ret );
}

/*******************************************************************************
 * @brief
 *  Give mutex.
 *
 * @param[in]  pMutex
 *  Pointer to mutex object which must be initialized by calling
 *  SLPAL_MutexInit before calling this function.
 *  
 * @return
 *   0 for success, SLPAL_ERROR_OS_SPECIFIC for error.
********************************************************************************/
__STATIC_INLINE int SLPAL_GiveMutex(SLPAL_Mutex_t* pMutex)
{
  *pMutex = 1;
  return ( 0 );
}

#endif /* MBEDTLS_SLPAL_BAREMETAL_H */
