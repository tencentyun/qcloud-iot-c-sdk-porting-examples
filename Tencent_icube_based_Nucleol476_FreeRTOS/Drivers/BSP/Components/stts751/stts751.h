/**
 ******************************************************************************
 * @file    stts751.h
 * @author  MEMS Software Solutions Team
 * @brief   STTS751 header driver file
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STTS751_H
#define STTS751_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "stts751_reg.h"
#include <string.h>

/** @addtogroup BSP BSP
 * @{
 */

/** @addtogroup Component Component
 * @{
 */

/** @addtogroup STTS751 STTS751
 * @{
 */

/** @defgroup STTS751_Exported_Types STTS751 Exported Types
 * @{
 */

typedef int32_t (*STTS751_Init_Func)(void);
typedef int32_t (*STTS751_DeInit_Func)(void);
typedef int32_t (*STTS751_GetTick_Func)(void);
typedef int32_t (*STTS751_WriteReg_Func)(uint16_t, uint16_t, uint8_t *, uint16_t);
typedef int32_t (*STTS751_ReadReg_Func)(uint16_t, uint16_t, uint8_t *, uint16_t);

typedef struct
{
  STTS751_Init_Func          Init;
  STTS751_DeInit_Func        DeInit;
  uint32_t                   BusType; /*0 means I2C */
  uint8_t                    Address;
  STTS751_WriteReg_Func      WriteReg;
  STTS751_ReadReg_Func       ReadReg;
  STTS751_GetTick_Func       GetTick;
} STTS751_IO_t;

typedef struct
{
  STTS751_IO_t       IO;
  stts751_ctx_t      Ctx;
  uint8_t            is_initialized;
  uint8_t            temp_is_enabled;
  float              temp_odr;
} STTS751_Object_t;

typedef struct
{
  uint8_t Temperature;
  uint8_t Pressure;
  uint8_t Humidity;
  uint8_t LowPower;
  float   HumMaxOdr;
  float   TempMaxOdr;
  float   PressMaxOdr;
} STTS751_Capabilities_t;

typedef struct
{
  int32_t (*Init)(STTS751_Object_t *);
  int32_t (*DeInit)(STTS751_Object_t *);
  int32_t (*ReadID)(STTS751_Object_t *, uint8_t *);
  int32_t (*GetCapabilities)(STTS751_Object_t *, STTS751_Capabilities_t *);
} STTS751_CommonDrv_t;

typedef struct
{
  int32_t (*Enable)(STTS751_Object_t *);
  int32_t (*Disable)(STTS751_Object_t *);
  int32_t (*GetOutputDataRate)(STTS751_Object_t *, float *);
  int32_t (*SetOutputDataRate)(STTS751_Object_t *, float);
  int32_t (*GetTemperature)(STTS751_Object_t *, float *);
} STTS751_TEMP_Drv_t;

/**
 * @}
 */

/** @defgroup STTS751_Exported_Constants STTS751 Exported Constants
 * @{
 */
#define STTS751_I2C_BUS           0U

/** STTS751 error codes  **/
#define STTS751_OK                 0
#define STTS751_ERROR             -1

/**
 * @}
 */

/** @addtogroup STTS751_Exported_Functions STTS751 Exported Functions
 * @{
 */

int32_t STTS751_RegisterBusIO(STTS751_Object_t *pObj, STTS751_IO_t *pIO);
int32_t STTS751_Init(STTS751_Object_t *pObj);
int32_t STTS751_DeInit(STTS751_Object_t *pObj);
int32_t STTS751_ReadID(STTS751_Object_t *pObj, uint8_t *Id);
int32_t STTS751_GetCapabilities(STTS751_Object_t *pObj, STTS751_Capabilities_t *Capabilities);
int32_t STTS751_Get_Init_Status(STTS751_Object_t *pObj, uint8_t *Status);

int32_t STTS751_TEMP_Enable(STTS751_Object_t *pObj);
int32_t STTS751_TEMP_Disable(STTS751_Object_t *pObj);
int32_t STTS751_TEMP_GetOutputDataRate(STTS751_Object_t *pObj, float *Odr);
int32_t STTS751_TEMP_SetOutputDataRate(STTS751_Object_t *pObj, float Odr);
int32_t STTS751_TEMP_GetTemperature(STTS751_Object_t *pObj, float *Value);
int32_t STTS751_TEMP_Get_DRDY_Status(STTS751_Object_t *pObj, uint8_t *Status);
int32_t STTS751_TEMP_SetLowTemperatureThreshold(STTS751_Object_t *pObj, float Value);
int32_t STTS751_TEMP_SetHighTemperatureThreshold(STTS751_Object_t *pObj, float Value);
int32_t STTS751_TEMP_GetTemperatureLimitStatus(STTS751_Object_t *pObj, uint8_t *HighLimit, uint8_t *LowLimit, uint8_t *ThermLimit);
int32_t STTS751_TEMP_SetEventPin(STTS751_Object_t *pObj, uint8_t Enable);

int32_t STTS751_Read_Reg(STTS751_Object_t *pObj, uint8_t Reg, uint8_t *Data);
int32_t STTS751_Write_Reg(STTS751_Object_t *pObj, uint8_t Reg, uint8_t Data);

int32_t STTS751_Set_One_Shot(STTS751_Object_t *pObj);
int32_t STTS751_Get_One_Shot_Status(STTS751_Object_t *pObj, uint8_t *Status);

/**
 * @}
 */

/** @addtogroup STTS751_Exported_Variables STTS751 Exported Variables
 * @{
 */

extern STTS751_CommonDrv_t STTS751_COMMON_Driver;
extern STTS751_TEMP_Drv_t STTS751_TEMP_Driver;

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
