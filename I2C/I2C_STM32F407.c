#include "I2C_STM32F407.h"
int tt;
uint32_t dataBufRx[1];

/*-----------------------------Функции запуска-----------------------------*/
void Init_I2C(I2C_Structure* i2c_str)
{
    Enable_RCC_I2C(i2c_str);
    Config_GPIO_I2C(i2c_str);
    Config_I2C(i2c_str->I2Cx);
}

void Enable_RCC_I2C(I2C_Structure* i2c_str)
{
    /*RM0090 Reference manual на STM32F407/417*/

    if((i2c_str->I2Cx) == I2C1)
    {
        Enable_BIT(RCC->APB1ENR,RCC_APB1ENR_I2C1EN); /*PB6 — I2C1 SCL, PB7 — I2C1 SDA*/
        Enable_BIT(RCC->AHB1ENR,RCC_AHB1ENR_GPIOBEN);
    }

    if((i2c_str->I2Cx) == I2C2)
    {
        Enable_BIT(RCC->APB1ENR,RCC_APB1ENR_I2C2EN); /*PB10 — I2C2 SCL, PB11 — I2C2 SDA*/
        Enable_BIT(RCC->AHB1ENR,RCC_AHB1ENR_GPIOBEN);
    }

    if((i2c_str->I2Cx) == I2C3)
    {
        Enable_BIT(RCC->APB1ENR,RCC_APB1ENR_I2C3EN); /*PA8  — I2C3 SCL, PC9  — I2C3 SDA*/
        Enable_BIT(RCC->AHB1ENR,RCC_AHB1ENR_GPIOCEN);
        Enable_BIT(RCC->AHB1ENR,RCC_AHB1ENR_GPIOAEN);
    }

    //Enable_BIT(RCC->AHB1ENR,RCC_AHB1ENR_DMA1EN); /*aaa*/
}

void Config_GPIO_I2C(I2C_Structure* i2c_str)
{
    GPIO_Structure PIN_SCL;
    GPIO_Structure PIN_SDA;

    if((i2c_str->I2Cx) == I2C1)
    {
        /* PB6 (SCL), PB7 (SDA), AF4 для I2C,GPIO_SPEED_LOW, GPIO_OTYPER_OPENDRAIN*/
        PIN_SCL.GPIOx = GPIOB;
        PIN_SCL.Pin = PIN6;

        PIN_SDA.GPIOx = GPIOB;
        PIN_SDA.Pin = PIN7;
    }

    if((i2c_str->I2Cx) == I2C2)
    {
        /* PB10 (SCL), PB11 (SDA), AF4 для I2C, GPIO_SPEED_LOW, GPIO_OTYPER_OPENDRAIN*/
        PIN_SCL.GPIOx = GPIOB;
        PIN_SCL.Pin = PIN10;

        PIN_SDA.GPIOx = GPIOB;
        PIN_SDA.Pin = PIN11;
    }

    if((i2c_str->I2Cx) == I2C3)
    {
        /* PA8 (SCL), PC9 (SDA), AF4 для I2C, GPIO_SPEED_LOW, GPIO_OTYPER_OPENDRAIN*/
        PIN_SCL.GPIOx = GPIOA;
        PIN_SCL.Pin = PIN8;

        PIN_SDA.GPIOx = GPIOC;
        PIN_SDA.Pin = PIN9;
    }

    PIN_SCL.Mode = GPIO_MODE_AF;
    PIN_SCL.Otyper = GPIO_OTYPER_OPENDRAIN;
    PIN_SCL.Speed = GPIO_SPEED_LOW;
    PIN_SCL.Alternate = GPIO_AF4;

    PIN_SDA.Mode = GPIO_MODE_AF;
    PIN_SDA.Otyper = GPIO_OTYPER_OPENDRAIN;
    PIN_SDA.Speed = GPIO_SPEED_LOW;
    PIN_SDA.Alternate = GPIO_AF4;

    GPIO_Init(&PIN_SCL);
    GPIO_Init(&PIN_SDA);
}

void Config_I2C(I2C_TypeDef *I2Cx)
{
    uint32_t freqrange;
    uint32_t pclk1;

    /*Сброс I2C*/
    Enable_BIT(I2Cx->CR1, I2C_CR1_SWRST);
    Disable_BIT(I2Cx->CR1, I2C_CR1_SWRST);

    /* Получить частоту PCLK1 */
    pclk1 = RCC_GetPCLK1Freq();
    freqrange = pclk1 / 1000000;

    /* Настройка I2Cx: диапазон частот */
    Enable_BIT(I2Cx->CR2, (freqrange << I2C_CR2_FREQ_Pos));

    /* Настройка I2Cx: время нарастания */
    Enable_BIT(I2Cx->TRISE, (I2C_Rise_Time(freqrange, CLOCK_SPEED) << I2C_TRISE_TRISE_Pos));

    /* Настройка I2Cx: скорость */
    Enable_BIT(I2Cx->CCR, I2C_Speed(pclk1, CLOCK_SPEED, 0));

    /* Настройка I2Cx: режим Generalcall и NoStretch */
    Enable_BIT(I2Cx->CR1, 0 << I2C_CR1_ENGC_Pos);
    Enable_BIT(I2Cx->CR1, 0 << I2C_CR1_NOSTRETCH_Pos);

    /* Настройте I2Cx: собственный адрес1 и режим адресации */
    Enable_BIT(I2Cx->OAR1, (I2C_ADDRESSINGMODE_7BIT << I2C_OAR1_ADDMODE_Pos));
    Enable_BIT(I2Cx->OAR1, 0 << I2C_OAR1_ADD0_Pos);

    /* Настройка I2Cx: двойной режим и собственный адрес2 */
    Enable_BIT(I2Cx->OAR2, 0 << I2C_OAR2_ENDUAL_Pos);
    Enable_BIT(I2Cx->OAR2, 0 << I2C_OAR2_ADD2_Pos);

    /* Включить выбранное периферийное устройство I2C */
    Enable_BIT(I2Cx->CR1,I2C_CR1_PE);

    NVIC_EnableIRQ(I2C1_EV_IRQn);
}

uint32_t RCC_GetPCLK1Freq(void)
{
    uint32_t temp = SystemCoreClock >> APBPrescTable[(RCC->CFGR & RCC_CFGR_PPRE1) >> RCC_CFGR_PPRE1_Pos]; // 10U=RCC_CFGR_PPRE1_Pos
    return temp;
}

uint32_t I2C_Speed(uint32_t pclk, uint32_t speed, uint32_t dutyCycle)
{
    uint32_t timing = 0;

    if (dutyCycle == I2C_CCR_DUTY)
    {
        /*Формула для коэффициента заполнения 16/9*/
        timing = (pclk / (speed * 9)) - 1;
    }
    else
    {
        /*Формула для коэффициента заполнения 1/2*/
        timing = (pclk / (speed * 2)) - 1;
    }

    return timing;
}

uint32_t I2C_Rise_Time(uint32_t freqrange, uint32_t clockSpeed)
{
    if (clockSpeed <= 100000)
    {
        freqrange = freqrange + 1;
    }
    else
    {
        freqrange = ((freqrange * 300) / 1000) + 1;
    }
    return freqrange;
}
/*-------------------------------------------------------------------------*/

/*-----------------------------Внешние функции-----------------------------*/

uint8_t I2C_Master_Transmit(I2C_TypeDef *I2Cx, uint16_t SlaveAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    /*вкл. Peripheral*/
    Enable_BIT(I2Cx->CR1,I2C_CR1_PE);

    /* откл. Pos */
    Disable_BIT(I2Cx->CR1, I2C_CR1_POS);

    /* отправка Slave Address */
    if (I2C_MasterRequestWrite(I2Cx, SlaveAddress, Timeout) != 0)
    {
        return _ERROR;
    }

    /* сброс флагов*/
    I2C_Clear_Status_Reg(I2Cx);

    while (Size > 0U) /* количество проходов в соответ. с длинной массива pData */
    {
        /* Пищем каждый елемент массива pData в DR */
        Write_REG(I2Cx->DR, *pData);

        pData++;
        Size--;

        /* Ждем флаг не использования TXE */
        I2C_WaitOnTXEFlagUntilTimeout(I2Cx);
    }

    /* Generate Stop */
    Enable_BIT(I2Cx->CR1, I2C_CR1_STOP);
    return _OK;
}

uint8_t I2C_Master_Receive(I2C_TypeDef *I2Cx, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    /* ждем пока не будет сброшен занятый флаг BUSY*/
    if (I2C_WaitOnFlagUntilTimeout(I2Cx, I2C_FLAG_BUSY, SET, I2C_TIMEOUT_BUSY_FLAG) != 0)
    {
        return _ERROR_BUSY;
    }

    /* Disable Pos */
    Disable_BIT(I2Cx->CR1, I2C_CR1_POS);

    /* Send Slave Address */
    if (I2C_MasterRequestRead(I2Cx, DevAddress, Timeout) != 0)
    {
        return 1;
    }

    if (Size == 0U)
    {
        I2C_Clear_Status_Reg(I2Cx);

        /* Generate Stop */
        Enable_BIT(I2Cx->CR1, I2C_CR1_STOP);
    }
    else if (Size == 1U)
    {
        // откл проверку адреса
        Disable_BIT(I2Cx->CR1, I2C_CR1_ACK);

        I2C_Clear_Status_Reg(I2Cx);

        /* Generate Stop */
        Enable_BIT(I2Cx->CR1, I2C_CR1_STOP);
    }
    else if (Size == 2U)
    {
        // откл проверку адреса
        Disable_BIT(I2Cx->CR1, I2C_CR1_ACK);

        /* Enable Pos */
        Enable_BIT(I2Cx->CR1, I2C_CR1_POS);

        I2C_Clear_Status_Reg(I2Cx);
    }
    else
    {

        // вкл проверку адреса
        Enable_BIT(I2Cx->CR1, I2C_CR1_ACK);

        I2C_Clear_Status_Reg(I2Cx);
    }

    while (Size > 0U)
    {
        if (Size <= 3U)
        {
            /* One byte */
            if (Size == 1U)
            {
                /*Эта функция обрабатывает тайм-аут связи I2C для определенного использования флага RXE.*/
                I2C_WaitOnRXNEFlagUntilTimeout(I2Cx);

                /* Read data from DR */
                *pData = Read_REG(I2Cx->DR);
                pData++;
                Size--;
            }
            /* Two bytes */
            else if (Size == 2U)
            {

                /* ждем пока не будет сброшен занятый флаг BTF*/
                if (I2C_WaitOnFlagUntilTimeout(I2Cx, I2C_FLAG_BTF, RESET, Timeout) != 0)
                {
                    return _ERROR_TIMEOUT;
                }

                /* Generate Stop */
                Enable_BIT(I2Cx->CR1, I2C_CR1_STOP);

                /* Read data from DR */
                *pData = Read_REG(I2Cx->DR);
                pData++;
                Size--;

                /* Read data from DR */
                *pData = Read_REG(I2Cx->DR);
                pData++;
                Size--;
            }
            /* 3 Last bytes */
            else
            {
                if (I2C_WaitOnFlagUntilTimeout(I2Cx, I2C_FLAG_BTF, RESET, Timeout) != 0)
                {
                    return _ERROR;
                }

                // откл проверку адреса
                Disable_BIT(I2Cx->CR1, I2C_CR1_ACK);

                /* Read data from DR */
                *pData = Read_REG(I2Cx->DR);
                pData++;
                Size--;

                if (I2C_WaitOnFlagUntilTimeout(I2Cx, I2C_FLAG_BTF, RESET, Timeout) != 0)
                {
                    return _ERROR;
                }

                /* Generate Stop */
                Enable_BIT(I2Cx->CR1, I2C_CR1_STOP);

                /* Read data from DR */
                *pData = Read_REG(I2Cx->DR);
                pData++;
                Size--;

                /* Read data from DR */
                *pData = Read_REG(I2Cx->DR);
                pData++;
                Size--;
            }
        }
        else
        {
            /*Эта функция обрабатывает тайм-аут связи I2C для определенного использования флага RXE.*/
            I2C_WaitOnRXNEFlagUntilTimeout(I2Cx);

            /* Read data from DR */
            *pData = Read_REG(I2Cx->DR);
            pData++;
            Size--;

            if (I2C_Get_Flag(I2Cx, I2C_FLAG_BTF) == SET)
            {
                /* Read data from DR */
                *pData = Read_REG(I2Cx->DR);
                pData++;
                Size--;
            }
        }
    }

    return 0;
}

uint8_t I2C_Mem_Write(I2C_TypeDef *I2Cx, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    /* Check the parameters */
    assert_param(IS_I2C_MEMADD_SIZE(MemAddSize));

    /* Wait until BUSY flag is reset */
    if (I2C_WaitOnFlagUntilTimeout(I2Cx, I2C_FLAG_BUSY, SET, I2C_TIMEOUT_BUSY_FLAG) != 0)
    {
        // return HAL_BUSY;
        return _ERROR_BUSY;
    }

    /* Check if the I2C is already enabled */
    Enable_BIT(I2Cx->CR1,I2C_CR1_PE);

    /* Disable Pos */
    Disable_BIT(I2Cx->CR1, I2C_CR1_POS);

    /* Send Slave Address and Memory Address */
    if (I2C_RequestMemoryWrite(I2Cx, DevAddress, MemAddress, MemAddSize, Timeout) != 0)
    {
        // return HAL_ERROR;
        return 1;
    }

    while (Size > 0U)
    {
        /* Ждем флаг не использования TXE */
        I2C_WaitOnTXEFlagUntilTimeout(I2Cx);

        /* Write data to DR */
        Write_REG(I2Cx->DR, *pData);
        /* Increment Buffer pointer */
        pData++;
        Size--;

        if (I2C_Get_Flag(I2Cx, I2C_FLAG_BTF) == SET)
        {
            /* Write data to DR */
            Write_REG(I2Cx->DR, *pData);
            /* Increment Buffer pointer */
            pData++;
            Size--;
        }

    }

    /* Wait until BTF flag is set */
    if (I2C_WaitOnBTFFlagUntilTimeout(I2Cx, Timeout) != 0)
    {
        return 1;
    }

    /* Generate Stop */
    Enable_BIT(I2Cx->CR1, I2C_CR1_STOP);

    return 0;
}
uint8_t I2C_Mem_Read(I2C_TypeDef *I2Cx, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    /* Check the parameters */
    assert_param(IS_I2C_MEMADD_SIZE(MemAddSize));

    /* Wait until BUSY flag is reset */
    if (I2C_WaitOnFlagUntilTimeout(I2Cx, I2C_FLAG_BUSY, SET, I2C_TIMEOUT_BUSY_FLAG) != 0)
    {
        // return HAL_BUSY;
        return _ERROR_BUSY;
    }

    /* Enable I2C peripheral */
    Enable_BIT(I2Cx->CR1,I2C_CR1_PE);


    /* Disable Pos */
    Disable_BIT(I2Cx->CR1, I2C_CR1_POS);

    /* Send Slave Address and Memory Address */
    if (I2C_RequestMemoryRead(I2Cx, DevAddress, MemAddress, MemAddSize, Timeout) != 0)
    {
        // return HAL_ERROR;
        return 1;
    }

    if (Size == 0U)
    {
        /* Clear ADDR flag */
        I2C_Clear_Status_Reg(I2Cx);

        /* Generate Stop */
        Enable_BIT(I2Cx->CR1, I2C_CR1_STOP);
    }
    else if (Size == 1U)
    {
        /* Disable Acknowledge */
        Disable_BIT(I2Cx->CR1, I2C_CR1_ACK);

        /* Clear ADDR flag */
        I2C_Clear_Status_Reg(I2Cx);

        /* Generate Stop */
        Enable_BIT(I2Cx->CR1, I2C_CR1_STOP);
    }
    else if (Size == 2U)
    {
        /* Disable Acknowledge */
        Disable_BIT(I2Cx->CR1, I2C_CR1_ACK);

        /* Enable Pos */
        Enable_BIT(I2Cx->CR1, I2C_CR1_POS);

        /* Clear ADDR flag */
        I2C_Clear_Status_Reg(I2Cx);
    }
    else
    {
        /* Clear ADDR flag */
        I2C_Clear_Status_Reg(I2Cx);
    }

    while (Size > 0U)
    {
        if (Size <= 3U)
        {
            /* One byte */
            if (Size == 1U)
            {
                /*Эта функция обрабатывает тайм-аут связи I2C для определенного использования флага RXE.*/
                I2C_WaitOnRXNEFlagUntilTimeout(I2Cx);

                /* Read data from DR */
                *pData = Read_REG(I2Cx->DR);
                pData++;
                Size--;
            }
            /* Two bytes */
            else if (Size == 2U)
            {
                /* Wait until BTF flag is set */
                if (I2C_WaitOnFlagUntilTimeout(I2Cx, I2C_FLAG_BTF, RESET, Timeout) != 0)
                {
                    // return HAL_ERROR;
                    return _ERROR_TIMEOUT;
                }

                /* Generate Stop */
                Enable_BIT(I2Cx->CR1, I2C_CR1_STOP);

                /* Read data from DR */
                *pData = Read_REG(I2Cx->DR);
                pData++;
                Size--;

                /* Read data from DR */
                *pData = Read_REG(I2Cx->DR);
                pData++;
                Size--;
            }
            /* 3 Last bytes */
            else
            {
                /* Wait until BTF flag is set */
                if (I2C_WaitOnFlagUntilTimeout(I2Cx, I2C_FLAG_BTF, RESET, Timeout) != 0)
                {
                    // return HAL_ERROR;
                    return _ERROR;
                }

                /* Disable Acknowledge */
                Disable_BIT(I2Cx->CR1, I2C_CR1_ACK);

                /* Read data from DR */
                *pData = Read_REG(I2Cx->DR);
                pData++;
                Size--;

                /* Wait until BTF flag is set */
                if (I2C_WaitOnFlagUntilTimeout(I2Cx, I2C_FLAG_BTF, RESET, Timeout) != 0)
                {
                    // return HAL_ERROR;
                    return _ERROR;
                }

                /* Generate Stop */
                Enable_BIT(I2Cx->CR1, I2C_CR1_STOP);

                /* Read data from DR */
                *pData = Read_REG(I2Cx->DR);
                pData++;
                Size--;

                /* Read data from DR */
                *pData = Read_REG(I2Cx->DR);
                pData++;
                Size--;
            }
        }
        else
        {
            /*Эта функция обрабатывает тайм-аут связи I2C для определенного использования флага RXE.*/
            I2C_WaitOnRXNEFlagUntilTimeout(I2Cx);

            /* Read data from DR */
            *pData = Read_REG(I2Cx->DR);
            pData++;
            Size--;

            if (I2C_Get_Flag(I2Cx, I2C_FLAG_BTF) == SET)
            {
                /* Read data from DR */
                *pData = Read_REG(I2Cx->DR);
                pData++;
                Size--;
            }
        }
    }

    return 0;
}

/*-------------------------------------------------------------------------*/

/*----------------------------Служебные функции----------------------------*/

/*Опрос регистра на флаг*/
FlagStatus I2C_Get_Flag(I2C_TypeDef *I2Cx, uint32_t flag)
{
    uint32_t rezult_flag = flag & I2C_FLAG_MASK; /*получим флаг*/

    if ((flag >> 16) == 1)/*SR1*/
    {
        if (Read_BIT(I2Cx->SR1,rezult_flag) == rezult_flag)
        {
            return SET;
        }
        else
        {
            return RESET;
        }
    }
    else /*SR2*/
    {
        if (Read_BIT(I2Cx->SR2,rezult_flag) == rezult_flag)
        {
            return SET;
        }
        else
        {
            return RESET;
        }
    }
}

/*I2C_ Ожидание флага до истечения времени Timeout*/
uint8_t I2C_WaitOnFlagUntilTimeout(I2C_TypeDef *I2Cx, uint32_t flag, FlagStatus Status, uint32_t Timeout)
{
    while (I2C_Get_Flag(I2Cx, flag) == Status)
    {
        if (Timeout-- == 0)
        {
            return _ERROR_TIMEOUT;
        }
    }
    return _OK;
}

/*Эта функция обрабатывает тайм-аут связи I2C для конкретного использования флага BTF.*/
uint8_t I2C_WaitOnBTFFlagUntilTimeout(I2C_TypeDef *I2Cx, uint32_t Timeout)
{
    while (I2C_Get_Flag(I2Cx, I2C_FLAG_BTF) == _ERROR)
    {
        if (I2C_IsAcknowledgeFailed(I2Cx) != _OK)
        {
            return _ERROR;
        }

        if (Timeout-- == 0)
        {
            return _ERROR_TIMEOUT;
        }
    }
    return _OK;
}

/*I2C_ Ожидание флага главного адреса до истечения времени ожидания*/
void I2C_WaitOnMasterAddressFlagUntilTimeout(I2C_TypeDef *I2Cx, uint32_t Flag)
{
    while (!(Read_BIT(I2Cx->SR1, Flag)))
    {
    }
}
/*Эта функция обрабатывает тайм-аут связи I2C для определенного использования флага TXE.*/
void I2C_WaitOnTXEFlagUntilTimeout(I2C_TypeDef *I2Cx)
{
    while (!(Read_BIT(I2Cx->SR1, I2C_SR1_TXE)))
    {
    }
}

/*Эта функция обрабатывает тайм-аут связи I2C для определенного использования флага RXE.*/
void I2C_WaitOnRXNEFlagUntilTimeout(I2C_TypeDef *I2Cx)
{
    while (!(Read_BIT(I2Cx->SR1,I2C_SR1_RXNE)))
    {
    }
}

/*Запрос от мастера*/
uint8_t I2C_MasterRequestWrite(I2C_TypeDef *I2Cx, uint16_t SlaveAddress, uint32_t Timeout)
{
    Enable_BIT(I2Cx->CR1, I2C_CR1_START);

    if (I2C_WaitOnFlagUntilTimeout(I2Cx, I2C_FLAG_SB, RESET, Timeout) != _OK)
    {
        return _ERROR_TIMEOUT;
    }

    Write_REG(I2Cx->DR, (uint8_t)(SlaveAddress & (~I2C_OAR1_ADD0)));

    /*I2C_ Ожидание флага главного адреса до истечения времени ожидания*/
    I2C_WaitOnMasterAddressFlagUntilTimeout(I2Cx, I2C_SR1_ADDR);

    return _OK;
}



uint8_t I2C_MasterRequestRead(I2C_TypeDef *I2Cx, uint16_t DevAddress, uint32_t Timeout)
{
    Enable_BIT(I2Cx->CR1, I2C_CR1_ACK);

    Enable_BIT(I2Cx->CR1, I2C_CR1_START);

    // I2C_StartBit_SetTime();
    if (I2C_WaitOnFlagUntilTimeout(I2Cx, I2C_FLAG_SB, RESET, Timeout) != 0)
    {
        // return HAL_TIMEOUT;
        return _ERROR_TIMEOUT;
    }

    Write_REG(I2Cx->DR, (uint8_t)(DevAddress | I2C_OAR1_ADD0));

    /*I2C_ Ожидание флага главного адреса до истечения времени ожидания*/
    I2C_WaitOnMasterAddressFlagUntilTimeout(I2Cx, I2C_SR1_ADDR);

    return 0;
}

/*Мастер отправляет адрес целевого устройства, а затем адрес внутренней памяти для запроса на чтение.*/
uint8_t I2C_RequestMemoryRead(I2C_TypeDef *I2Cx, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint32_t Timeout)
{
    /* Включить подтверждение */
    Enable_BIT(I2Cx->CR1, I2C_CR1_ACK);

    /* Генерировать Пуск */
    Enable_BIT(I2Cx->CR1, I2C_CR1_START);

    /* Ждем пока не будет установлен флаг SB */
    if (I2C_WaitOnFlagUntilTimeout(I2Cx, I2C_FLAG_SB, RESET, Timeout) != 0)
    {
        return _ERROR_TIMEOUT;
    }

    /* Send slave address */
    I2Cx->DR = I2C_7BIT_ADD_Write(DevAddress);

    /*I2C_ Ожидание флага главного адреса до истечения времени ожидания*/
    I2C_WaitOnMasterAddressFlagUntilTimeout(I2Cx, I2C_SR1_ADDR);

    /* Clear ADDR flag */
    I2C_Clear_Status_Reg(I2Cx);

    /* Ждем флаг не использования TXE */
    I2C_WaitOnTXEFlagUntilTimeout(I2Cx);

    /* If Memory address size is 8Bit */
    if (MemAddSize == I2C_MEMADD_SIZE_8BIT)
    {
        /* Send Memory Address */
        Write_REG(I2Cx->DR, I2C_MEM_ADD_LSB(MemAddress));
    }
    /* If Memory address size is 16Bit */
    else
    {
        /* Send MSB of Memory Address */
        Write_REG(I2Cx->DR, I2C_MEM_ADD_MSB(MemAddress));

        /* Ждем флаг не использования TXE */
        I2C_WaitOnTXEFlagUntilTimeout(I2Cx);

        /* Send LSB of Memory Address */
        Write_REG(I2Cx->DR, I2C_MEM_ADD_LSB(MemAddress));
    }

    /* Ждем флаг не использования TXE */
    I2C_WaitOnTXEFlagUntilTimeout(I2Cx);

    /* Generate Restart */
    Enable_BIT(I2Cx->CR1, I2C_CR1_START);

    /* Wait until SB flag is set */
    if (I2C_WaitOnFlagUntilTimeout(I2Cx, I2C_FLAG_SB, RESET, Timeout) != 0)
    {
//        if (Read_BIT(I2Cx->CR1, I2C_CR1_START) == I2C_CR1_START)
//        {
//        }
//        // return HAL_TIMEOUT;
        return _ERROR_TIMEOUT;
    }

    /* Send slave address */
    Write_REG(I2Cx->DR, (uint8_t)(DevAddress | I2C_OAR1_ADD0));

    /*I2C_ Ожидание флага главного адреса до истечения времени ожидания*/
    I2C_WaitOnMasterAddressFlagUntilTimeout(I2Cx, I2C_SR1_ADDR);

    return 0;
}

uint8_t I2C_RequestMemoryWrite(I2C_TypeDef *I2Cx, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint32_t Timeout)
{
    /* Generate Start */
    Enable_BIT(I2Cx->CR1, I2C_CR1_START);

    /* Wait until SB flag is set */
    if (I2C_WaitOnFlagUntilTimeout(I2Cx, I2C_FLAG_SB, RESET, Timeout) != 0)
    {
//        if (Read_BIT(I2Cx->CR1, I2C_CR1_START) == I2C_CR1_START)
//        {

//        }
        return _ERROR_TIMEOUT;
    }

    /* Send slave address */
    I2Cx->DR = I2C_7BIT_ADD_Write(DevAddress);

    /*I2C_ Ожидание флага главного адреса до истечения времени ожидания*/
    I2C_WaitOnMasterAddressFlagUntilTimeout(I2Cx, I2C_SR1_ADDR);

    /* Clear ADDR flag */
    I2C_Clear_Status_Reg(I2Cx);

    /* Ждем флаг не использования TXE */
    I2C_WaitOnTXEFlagUntilTimeout(I2Cx);

    /* If Memory address size is 8Bit */
    if (MemAddSize == I2C_MEMADD_SIZE_8BIT)
    {
        /* Send Memory Address */
        Write_REG(I2Cx->DR, I2C_MEM_ADD_LSB(MemAddress));
    }
    /* If Memory address size is 16Bit */
    else
    {
        /* Send MSB of Memory Address */
        Write_REG(I2Cx->DR, I2C_MEM_ADD_MSB(MemAddress));

        /* Ждем флаг не использования TXE */
        I2C_WaitOnTXEFlagUntilTimeout(I2Cx);

        /* Send LSB of Memory Address */
        Write_REG(I2Cx->DR, I2C_MEM_ADD_LSB(MemAddress));
    }

    return 0;
}

/*очистим статусные регистры чтением*/
void I2C_Clear_Status_Reg(I2C_TypeDef *I2Cx)
{
    I2Cx->SR1;
    I2Cx->SR2;
}

/* очистка флага в регистре SR1*/
void I2C_Clear_Flag(I2C_TypeDef *I2Cx, uint32_t flag)
{
    I2Cx->SR1 = ~(flag & I2C_FLAG_MASK);
}

/*I2c_ является подтверждением отказа флага I2C_FLAG_AF*/
uint8_t I2C_IsAcknowledgeFailed(I2C_TypeDef *I2Cx)
{
    if (I2C_Get_Flag(I2Cx, I2C_FLAG_AF) == SET)
    {
        /* очистка флага в регистре SR1*/
        I2C_Clear_Flag(I2Cx, I2C_FLAG_AF);

        return _ERROR;
    }
    return _OK;
}

uint8_t I2C_7BIT_ADD_Write(uint8_t SlaveAddress)
{
    return (uint8_t)(SlaveAddress & (~I2C_OAR1_ADD0));
}

uint8_t I2C_IsDeviceReady(I2C_TypeDef *I2Cx, uint16_t DevAddress, uint32_t Trials, uint32_t Timeout)
{
    uint32_t I2C_Trials = 0;
    FlagStatus tmp1;
    FlagStatus tmp2;

    /* Wait until BUSY flag is reset */
    if (I2C_WaitOnFlagUntilTimeout(I2Cx, I2C_FLAG_BUSY, SET, I2C_TIMEOUT_BUSY_FLAG) != 0)
    {
        // return HAL_BUSY;
        return _ERROR_BUSY;
    }
    Enable_BIT(I2Cx->CR1,I2C_CR1_PE);

    /* Disable Pos */
    Disable_BIT(I2Cx->CR1, I2C_CR1_POS);

    do
    {
        /* Generate Start */
        Enable_BIT(I2Cx->CR1, I2C_CR1_START);

        /* Wait until SB flag is set */
        if (I2C_WaitOnFlagUntilTimeout(I2Cx, I2C_FLAG_SB, RESET, Timeout) != 0)
        {
            return _ERROR_TIMEOUT;
        }

        /* Send slave address */
        I2Cx->DR = I2C_7BIT_ADD_Write(DevAddress);

        tmp1 = I2C_Get_Flag(I2Cx, I2C_FLAG_ADDR);
        tmp2 = I2C_Get_Flag(I2Cx, I2C_FLAG_AF);

        while ((tmp1 == RESET) && (tmp2 == RESET))
        {
            tmp1 = I2C_Get_Flag(I2Cx, I2C_FLAG_ADDR);
            tmp2 = I2C_Get_Flag(I2Cx, I2C_FLAG_AF);
        }

        /* Check if the ADDR flag has been set */
        if (I2C_Get_Flag(I2Cx, I2C_FLAG_ADDR) == SET)
        {
            /* Generate Stop */
            Enable_BIT(I2Cx->CR1, I2C_CR1_STOP);

            /* Clear ADDR Flag */
            I2C_Clear_Status_Reg(I2Cx);

            /* Wait until BUSY flag is reset */
            if (I2C_WaitOnFlagUntilTimeout(I2Cx, I2C_FLAG_BUSY, SET, I2C_TIMEOUT_BUSY_FLAG) != 0)
            {
                // return HAL_ERROR;
                return _ERROR;
            }

            // return HAL_OK;
            return 0;
        }
        else
        {
            /* Generate Stop */
            Enable_BIT(I2Cx->CR1, I2C_CR1_STOP);

            /* очистка флага в регистре SR1*/
            I2C_Clear_Flag(I2Cx, I2C_FLAG_AF);

            /* Wait until BUSY flag is reset */
            if (I2C_WaitOnFlagUntilTimeout(I2Cx, I2C_FLAG_BUSY, SET, I2C_TIMEOUT_BUSY_FLAG) != 0)
            {
                return _ERROR;
            }
        }

        /* Increment Trials */
        I2C_Trials++;
    }
    while (I2C_Trials < Trials);

    return 1;
}

////IRQ
void Error_Handler(void)
{
    //    __disable_irq();
    //    while (1)
    //    {
    //    }
}
