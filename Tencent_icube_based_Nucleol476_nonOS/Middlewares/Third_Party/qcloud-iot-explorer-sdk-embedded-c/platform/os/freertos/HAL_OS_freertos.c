/*
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.

 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "cmsis_os.h"
#include "qcloud_iot_import.h"
#include "stm32l4xx_hal.h"


void HAL_Printf(_IN_ const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    fflush(stdout);
}

int HAL_Snprintf(_IN_ char *str, const int len, const char *fmt, ...)
{
    va_list args;
    int rc;

    va_start(args, fmt);
    rc = vsnprintf(str, len, fmt, args);
    va_end(args);

    return rc;
}

int HAL_Vsnprintf(_IN_ char *str, _IN_ const int len, _IN_ const char *format, va_list ap)
{
    return vsnprintf(str, len, format, ap);
}


void *HAL_Malloc(_IN_ uint32_t size)
{
    return pvPortMalloc( size);
}

void HAL_Free(_IN_ void *ptr)
{
    vPortFree(ptr);
}

void* HAL_Calloc(uint32_t nelements, uint32_t elementSize)
{
    uint32_t size;
    void *ptr = NULL;

    size = nelements * elementSize;
    ptr = pvPortMalloc(size);

    if (ptr) {
        memset(ptr, 0, size);
    }

    return ptr;
}



void HAL_SleepMs(_IN_ uint32_t ms)
{
    (void)osDelay(ms);
}

void HAL_DelayMs(_IN_ uint32_t ms)
{
    (void)HAL_Delay(ms);
}

void *HAL_MutexCreate(void)
{

    return (void *)osMutexCreate (NULL);
}

void HAL_MutexDestroy(_IN_ void * mutex)
{
    osStatus ret;

    if (osOK != (ret = osMutexDelete((osMutexId)mutex))) {
        HAL_Printf("HAL_MutexDestroy err, err:%d\n\r", ret);
    }
}

void HAL_MutexLock(_IN_ void * mutex)
{
    osStatus ret;

    if (osOK != (ret = osMutexWait((osMutexId)mutex, osWaitForever))) {
        HAL_Printf("HAL_MutexLock err, err:%d\n\r", ret);
    }
}

void HAL_MutexUnlock(_IN_  void   *   mutex)
{
    osStatus ret;

    if (osOK != (ret = osMutexRelease((osMutexId)mutex))) {
        HAL_Printf("HAL_MutexUnlock err, err:%d\n\r", ret);
    }
}

/*
* return void* threadId
*/
void * HAL_ThreadCreate(uint16_t stack_size, int priority, char * taskname, void *(*fn)(void*), void* arg)
{
#define DEFAULT_STACK_SIZE 1024

    osThreadId thread_t;
    osThreadDef_t thread_def;

    if (NULL == fn) {
        return NULL;
    }

    thread_def.name = taskname;
    thread_def.pthread = (os_pthread)fn;
    thread_def.tpriority = (osPriority)priority;
    thread_def.instances = 0;
    thread_def.stacksize = (stack_size == 0) ? DEFAULT_STACK_SIZE : stack_size;

    thread_t = osThreadCreate(&thread_def, arg);
    if (NULL == thread_t) {
        HAL_Printf("create thread fail\n\r");
    }

    return (void *)thread_t;

#undef  DEFAULT_STACK_SIZE
}


int HAL_ThreadDestroy(void* threadId)
{
    return osThreadTerminate(threadId);
}

void *HAL_SemaphoreCreate(void)
{
    return (void *)osSemaphoreCreate(NULL, 1);
}

void HAL_SemaphoreDestroy(void *sem)
{
    osStatus ret;

    ret = osSemaphoreDelete ((osSemaphoreId)sem);
    if (osOK != ret) {
        HAL_Printf("HAL_SemaphoreDestroy err, err:%d\n\r", ret);
    }
}

void HAL_SemaphorePost(void *sem)
{
    osStatus ret;

    ret = osSemaphoreRelease ((osSemaphoreId) sem);

    if (osOK != ret) {
        HAL_Printf("HAL_SemaphorePost err, err:%d\n\r", ret);
    }
}

int HAL_SemaphoreWait(void *sem, uint32_t timeout_ms)
{
    return osSemaphoreWait ((osSemaphoreId)sem, timeout_ms);

}
