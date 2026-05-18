/*******************************************************************************
 *
 *                  RTC Library
 *
 *******************************************************************************
 * FileName:        RTCLib.c
 * Complier:        XC8 v2.31
 * Author:          Pedro Sanchez Ramirez (@mrchunckuee_electronics)
 * Blog:            http://mrchunckuee.blogspot.com/
 * Email:           mrchunckuee.electronics@gmail.com
 * Description:     Library for RTC use in PIC18
 *******************************************************************************
 *                  MIT License
 * 
 * Copyright (c) 2016 Pedro Sanchez Ramirez
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/

#include <stdbool.h>
#include "RTCLib.h"
#include "i2c2.h"

static RTC_Model_t _selectedModel;
static uint8_t     _rtcI2cAddress;

/*******************************************************************************
 * Function:        void RTC_Initialize(RTC_Model_t model, uint8_t i2cAddress)
 * Description:     Initializes the library setting up the selected RTC model 
 *                  and its physical I2C slave address.
 * Precondition:    I2C peripheral module must be initialized beforehand.
 * Parameters:      model - Enum value representing BQ32000, DS1307, or DS3231.
 *                  i2cAddress - The 8-bit or 7-bit I2C write address (e.g., 0xD0).
 * Return Values:   None
 * Remarks:         None
 ******************************************************************************/
void RTC_Initialize(RTC_Model_t model, uint8_t i2cAddress){
    _selectedModel = model;
    _rtcI2cAddress = i2cAddress & 0xFE; // Asegura que el bit de lectura/escritura empiece en 0
    
    // Encendemos el oscilador automáticamente dependiendo del chip
    RTC_StartClock();
}

/*******************************************************************************
 * Function:        void RTC_StartClock(void)
 * Description:     Starts the internal oscillator modifying the status bit 
 *                  according to the selected hardware model.
 * Precondition:    RTC_Initialize must be called first.
 * Parameters:      None
 * Return Values:   None
 * Remarks:         BQ32000 requires bit7=0 to run. DS1307 requires bit7=0 (CH=0).
 *                  DS3231 oscillator runs automatically but checks status logs.
 ******************************************************************************/
void RTC_StartClock(void){
    uint8_t tempReg;
    
    if(_selectedModel == RTC_MODEL_BQ32000){
        // BQ32000: Bit 7 en 1 detiene el oscilador temporalmente, poner a 0 lo activa
        tempReg = RTC_ReadByte(RTC_SECONDS_ADDRESS);
        tempReg |= 0x80;
        RTC_WriteByte(RTC_SECONDS_ADDRESS, tempReg);
        tempReg &= 0x7F;
        RTC_WriteByte(RTC_SECONDS_ADDRESS, tempReg);
    } 
    else if(_selectedModel == RTC_MODEL_DS1307){
        // DS1307: Bit 7 del registro 0x00 es CH (Clock Halt). Debe ser 0 para que cuente.
        tempReg = RTC_ReadByte(RTC_SECONDS_ADDRESS);
        tempReg &= 0x7F; // CH = 0 (reloj corre)
        RTC_WriteByte(RTC_SECONDS_ADDRESS, tempReg);
    }
    else if(_selectedModel == RTC_MODEL_DS3231){
        // DS3231: El cristal está integrado y siempre corre, pero se puede limpiar 
        // el bit EOSC en el registro de control (0x0E) si fuera necesario.
        // Por simplicidad, se puede dejar vacío o limpiar alarmas.
    }
}

/*******************************************************************************
 * Function:        static bool RTC_WaitReady(void)
 * Description:     Polls the I2C bus to check if the RTC device is ready 
 *                  for communication.
 * Precondition:    None
 * Parameters:      None
 * Return Values:   true if the device responds before timeout, false otherwise.
 * Remarks:         Internal use only. Returns 1 if acknowledged, 0 on timeout.
 ******************************************************************************/
static bool RTC_WaitReady(void){
    uint16_t timeout = 500;
    I2C2_Start();
    
    while(I2C2_Send(_rtcI2cAddress | RTC_WRITE_BIT) == 1 && timeout > 0) {
        I2C2_Start();
        timeout--;
    }
    
    if(timeout == 0){
        I2C2_Stop();
        return 0;
    }
    
    return 1;
}

/*******************************************************************************
 * Function:        void RTC_WriteByte(uint8_t addr, uint8_t data)
 * Description:     Writes a single byte of data to a specific register address.
 * Precondition:    I2C module must be initialized.
 * Parameters:      addr - Target register address.
 *                  data - Data byte to be written.
 * Return Values:   None
 * Remarks:         None
 ******************************************************************************/
void RTC_WriteByte(uint8_t addr, uint8_t data){
    if(RTC_WaitReady()){
        I2C2_Send(addr);     
        I2C2_Send(data);    
        I2C2_Stop();
    }
}

/*******************************************************************************
 * Function:        uint8_t RTC_ReadByte(uint8_t addr)
 * Description:     Reads a single byte of data from a specific register address.
 * Precondition:    I2C module must be initialized.
 * Parameters:      addr - Target register address to read from.
 * Return Values:   The byte received from the RTC register.
 * Remarks:         None
 ******************************************************************************/
uint8_t RTC_ReadByte(uint8_t addr){
    uint8_t data = 0;
    
    if (RTC_WaitReady()) {
        I2C2_Send(addr);
        I2C2_Start();
        I2C2_Send(_rtcI2cAddress | RTC_READ_BIT);
        data = I2C2_Read();
        I2C2_Stop();
    }
    
    return data;
}

/*******************************************************************************
 * Function:        void RTC_WriteBytes(uint8_t addr, uint8_t *data, uint8_t len)
 * Description:     Writes multiple consecutive bytes starting from a given 
 *                  register address using a data pointer.
 * Precondition:    I2C module must be initialized.
 * Parameters:      addr - Starting register address (0x00 to 0xFF).
 *                  data - Pointer to the array containing the bytes to write.
 *                  len  - Number of bytes to be written.
 * Return Values:   None
 * Remarks:         None
 ******************************************************************************/
void RTC_WriteBytes(uint8_t addr, uint8_t *data, uint8_t len){
    if(RTC_WaitReady()){          
        I2C2_Send(addr);       
        for(uint8_t i=0; i<len; i++){     
            I2C2_Send(*data++);
        }
        I2C2_Stop();              
    }
}

/*******************************************************************************
 * Function:        void RTC_ReadBytes(uint8_t addr, uint8_t *data, uint8_t len)
 * Description:     Reads multiple consecutive bytes starting from a given 
 *                  register address and stores them in an array.
 * Precondition:    I2C module must be initialized.
 * Parameters:      addr - Starting register address.
 *                  data - Pointer to the destination array.
 *                  len  - Number of bytes to read.
 * Return Values:   None (Read bytes are returned in data array).
 * Remarks:         None
 ******************************************************************************/
void RTC_ReadBytes(uint8_t addr, uint8_t *data, uint8_t len){
    if(RTC_WaitReady()){                       
        I2C2_Send(addr);                      
        I2C2_Start();                            
        I2C2_Send(_rtcI2cAddress | RTC_READ_BIT); 
         for(uint8_t i = 0; i<len; i++){
            data[i] = I2C2_Read();     
            if(i < (len - 1)){
                I2C2_Send_ACK();
            }else{
                I2C2_Send_NACK();
            }
         }
        I2C2_Stop();                           
    }
}

/*******************************************************************************
 * Function:        inline uint8_t DEC_to_BCD(uint8_t decimal)
 * Description:     Converts a standard decimal integer value (0-99) to Binary 
 *                  Coded Decimal (BCD) format.
 * Precondition:    None
 * Parameters:      decimal - The standard integer value to convert.
 * Return Values:   The converted value in BCD format.
 * Remarks:         Inline function for speed optimization.
 ******************************************************************************/
inline uint8_t DEC_to_BCD(uint8_t decimal){
    //return ((decimal / 10) << 4) | (decimal % 10); 
    return (decimal / 10 * 16) | (decimal % 10); 
}

/*******************************************************************************
 * Function:        inline uint8_t BCD_to_DEC(uint8_t bcd)
 * Description:     Converts a Binary Coded Decimal (BCD) formatted value (0-99) 
 *                  to a standard decimal integer.
 * Precondition:    None
 * Parameters:      bcd - The BCD value to convert.
 * Return Values:   The converted standard integer value.
 * Remarks:         Inline function for speed optimization.
 ******************************************************************************/
inline uint8_t BCD_to_DEC(uint8_t bcd){
    // (bcd >> 4) extrae las decenas. Multiplicamos por 10.
    // (bcd & 0x0F) extrae las unidades (limpia los 4 bits superiores).
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

/*******************************************************************************
 * Function:        void RTC_SetTime(const Time_t *time)
 * Description:     Converts and sets the time parameters into the RTC 
 *                  hardware time registers.
 * Precondition:    None
 * Parameters:      time - Pointer to a constant Time_t structure containing hours, 
 *                  minutes, seconds, and timeMode.
 * Return Values:   None
 * Remarks:         Supports AM, PM, and 24-Hour modes formatting.
 ******************************************************************************/
void RTC_SetTime(const Time_t *time){
    uint8_t dataBuff[3];
    uint8_t HourBcd;
    
    dataBuff[0] = DEC_to_BCD(time->seconds);
    dataBuff[1] = DEC_to_BCD(time->minutes);
    HourBcd = DEC_to_BCD(time->hours);
    
    // Solo aplicar máscaras AM/PM si el chip NO es el BQ32000 y se seleccionó el modo
    if (_selectedModel != RTC_MODEL_BQ32000){
        if (time->timeMode == PM_Time) {
            HourBcd |= 0x60; // 12-Hour Mode + PM
        } else if (time->timeMode == AM_Time) {
            HourBcd |= 0x40; // 12-Hour Mode + AM
        }
    }
    dataBuff[2] = HourBcd;
    
    RTC_WriteBytes(RTC_SECONDS_ADDRESS, dataBuff, 3);
}

/*******************************************************************************
 * Function:        bool RTC_GetTime(Time_t *result)
 * Description:     Reads raw time buffers from the RTC registers, converts 
 *                  them from BCD to decimal, and performs data validation.
 * Precondition:    None
 * Parameters:      result - Pointer to Time_t structure where decoded time is saved.
 * Return Values:   true if reading is successful and ranges are valid, false otherwise.
 * Remarks:         Applies 0x7F mask to filter out the STOP oscillator bit.
 ******************************************************************************/
bool RTC_GetTime(Time_t *result) {
    uint8_t dataBuff[3];

    RTC_ReadBytes(RTC_SECONDS_ADDRESS, dataBuff, 3);

    // El BQ32000 y el DS1307 usan el Bit 7 de los segundos para banderas del oscilador.
    // Lo filtramos de forma segura con un bitwise AND (0x7F)
    result->seconds = BCD_to_DEC(dataBuff[0] & 0x7F);
    result->minutes = BCD_to_DEC(dataBuff[1]);
    
    // Decodificar horas según el chip y su configuración interna
    if (_selectedModel != RTC_MODEL_BQ32000 && (dataBuff[2] & 0x40)) {
        // Si el bit 6 está arriba, significa modo 12 Horas activo en DS1307/DS3231
        result->hours = BCD_to_DEC(dataBuff[2] & 0x1F);
        result->timeMode = (dataBuff[2] & 0x20) ? PM_Time : AM_Time;
    } else {
        // Modo 24 Horas estándar (Para el BQ32000)
        result->hours   = BCD_to_DEC(dataBuff[2] & 0x3F);
        result->timeMode = TwentyFourHoursMode;
    }

    if (result->seconds > 59 || result->minutes > 59 || result->hours > 23) {
        return false;
    }
    
    return true;
}

/*******************************************************************************
 * Function:        void RTC_SetDate(const Date_t *date)
 * Description:     Converts and updates the RTC calendar registers with the 
 *                  provided date structure parameters.
 * Precondition:    None
 * Parameters:      date - Pointer to a constant Date_t structure containing 
 *                  year, month, day, and weekday.
 * Return Values:   None
 * Remarks:         Converts the year to short format (e.g., 25 for 2025).
 ******************************************************************************/
void RTC_SetDate(const Date_t *date){
    uint8_t dataBuff[4];
    dataBuff[0] = DEC_to_BCD(date->weekday);
    dataBuff[1] = DEC_to_BCD(date->day);    
    dataBuff[2] = DEC_to_BCD(date->month);  
    dataBuff[3] = DEC_to_BCD((uint8_t)(date->year % 100));

    RTC_WriteBytes(RTC_NUMBER_DAY_ADDRESS, dataBuff, 4);
}

/*******************************************************************************
 * Function:        bool RTC_GetDate(Date_t *result)
 * Description:     Reads raw calendar data from the RTC, decodes from BCD 
 *                  to decimal, and validates parameters.
 * Precondition:    None
 * Parameters:      result - Pointer to Date_t structure where decoded date is saved.
 * Return Values:   true if parsing and validation succeed, false on bounds error.
 * Remarks:         None
 ******************************************************************************/
bool RTC_GetDate(Date_t *result) {
    uint8_t dataBuff[4];

    RTC_ReadBytes(RTC_NUMBER_DAY_ADDRESS, dataBuff, 4);
    
    result->weekday = dataBuff[0];
    result->day   = BCD_to_DEC(dataBuff[1]);
    result->month = BCD_to_DEC(dataBuff[2]);
    result->year  = BCD_to_DEC(dataBuff[3]) + 2000;

    if (result->month < 1 || result->month > 12 || 
        result->day < 1   || result->day > 31   || 
        result->weekday < 1 || result->weekday > 7) {
        return false;
    }

    return true;
}