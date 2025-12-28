#ifndef I2C_H_
#define I2C_H_

/*includes*/
#include "stm32f4xx.h"
#include "lib_stm32.h"

/*defaines*/

//#define MAX_DELAY      0xFFFFFFFF
#define assert_param(expr) ((void)0U)

#define BUFFER_SIZE_I2C 10
#define I2C_ADDRESSINGMODE_7BIT 0x00004000U
#define CLOCK_SPEED 100000

#define I2C_FLAG_BUSY 0x00100002U
#define I2C_FLAG_BTF 0x00010004U
#define I2C_FLAG_MASK 0x0000FFFFU

/*задержки на ошибку*/
#define I2C_TIMEOUT_FLAG          35U         /*!< Timeout 35 ms             */
#define I2C_TIMEOUT_BUSY_FLAG     25U         /*!< Timeout 25 ms             */
#define I2C_TIMEOUT_STOP_FLAG     5U          /*!< Timeout 5 ms              */

/*флаги состояний*/
#define I2C_FLAG_OVR                    0x00010800U
#define I2C_FLAG_AF                     0x00010400U
#define I2C_FLAG_ARLO                   0x00010200U
#define I2C_FLAG_BERR                   0x00010100U
#define I2C_FLAG_TXE                    0x00010080U
#define I2C_FLAG_RXNE                   0x00010040U
#define I2C_FLAG_STOPF                  0x00010010U
#define I2C_FLAG_ADD10                  0x00010008U
#define I2C_FLAG_BTF                    0x00010004U
#define I2C_FLAG_ADDR                   0x00010002U
#define I2C_FLAG_SB                     0x00010001U
#define I2C_FLAG_DUALF                  0x00100080U
#define I2C_FLAG_GENCALL                0x00100010U
#define I2C_FLAG_TRA                    0x00100004U
#define I2C_FLAG_BUSY                   0x00100002U
#define I2C_FLAG_MSL                    0x00100001U

#define I2C_MEMADD_SIZE_8BIT            0x00000001U
#define I2C_MEMADD_SIZE_16BIT           0x00000010U

#define I2C_MEM_ADD_MSB(__ADDRESS__)                       ((uint8_t)((uint16_t)(((uint16_t)((__ADDRESS__) & (uint16_t)0xFF00)) >> 8)))
#define I2C_MEM_ADD_LSB(__ADDRESS__)                       ((uint8_t)((uint16_t)((__ADDRESS__) & (uint16_t)0x00FF)))

#define IS_I2C_MEMADD_SIZE(SIZE) (((SIZE) == I2C_MEMADD_SIZE_8BIT) || \
                                  ((SIZE) == I2C_MEMADD_SIZE_16BIT))
																	
/*var*/

/*structures*/
typedef struct
{
	I2C_TypeDef* I2Cx;  /*Указывает настраиваемые I2C*/
	uint8_t fill_Factor; /*значение Коэффициента заполнения*/
	
} I2C_Structure;
/*func*/

/*Функции запуска*/
void Init_I2C(I2C_Structure* i2c_str);
void Enable_RCC_I2C(I2C_Structure* i2c_str);
void Config_GPIO_I2C(I2C_Structure* i2c_str);
void Config_I2C(I2C_TypeDef *I2Cx);

uint32_t RCC_GetPCLK1Freq(void);
uint32_t I2C_Speed(uint32_t pclk, uint32_t speed, uint32_t dutyCycle);
uint32_t I2C_Rise_Time(uint32_t freqrange, uint32_t clockSpeed);
/*--------------------------*/
/*Внешние функции*/
uint8_t I2C_Master_Transmit(I2C_TypeDef *I2Cx,uint16_t SlaveAddress, uint8_t *pData, uint16_t Size,uint32_t Timeout);
uint8_t I2C_Master_Receive(I2C_TypeDef *I2Cx,uint16_t SlaveAddress, uint8_t *pData, uint16_t Size,uint32_t Timeout);
uint8_t I2C_Mem_Write(I2C_TypeDef *I2Cx, uint16_t SlaveAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);
uint8_t I2C_Mem_Read(I2C_TypeDef *I2Cx,uint16_t SlaveAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size,uint32_t Timeout);
/*--------------------------*/
/*Служебные функции*/
FlagStatus I2C_Get_Flag(I2C_TypeDef *I2Cx, uint32_t flag);
uint8_t I2C_WaitOnFlagUntilTimeout(I2C_TypeDef *I2Cx, uint32_t Flag, FlagStatus Status, uint32_t Timeout);
uint8_t I2C_WaitOnBTFFlagUntilTimeout(I2C_TypeDef *I2Cx, uint32_t Timeout);
void I2C_WaitOnMasterAddressFlagUntilTimeout(I2C_TypeDef *I2Cx, uint32_t Flag);
void I2C_WaitOnTXEFlagUntilTimeout(I2C_TypeDef *I2Cx);
void I2C_WaitOnRXNEFlagUntilTimeout(I2C_TypeDef *I2Cx);

uint8_t I2C_MasterRequestWrite(I2C_TypeDef *I2Cx, uint16_t SlaveAddress,uint32_t Timeout);
uint8_t I2C_MasterRequestRead(I2C_TypeDef *I2Cx,uint16_t SlaveAddress,uint32_t Timeout);
uint8_t I2C_RequestMemoryRead(I2C_TypeDef *I2Cx,uint16_t SlaveAddress, uint16_t MemAddress, uint16_t MemAddSize, uint32_t Timeout);
uint8_t I2C_RequestMemoryWrite(I2C_TypeDef *I2Cx, uint16_t SlaveAddress, uint16_t MemAddress, uint16_t MemAddSize, uint32_t Timeout);

void I2C_Clear_Status_Reg(I2C_TypeDef *I2Cx);
void I2C_Clear_Flag(I2C_TypeDef *I2Cx, uint32_t flag);
uint8_t I2C_IsAcknowledgeFailed(I2C_TypeDef *I2Cx);
uint8_t I2C_7BIT_ADD_Write(uint8_t DevAddress);
uint8_t I2C_IsDeviceReady(I2C_TypeDef *I2Cx, uint16_t DevAddress, uint32_t Trials, uint32_t Timeout);

void Error_Handler(void);
/*--------------------------*/
#endif
