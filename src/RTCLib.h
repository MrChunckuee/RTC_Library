/* 
 * File:   RTCLib.h
 * Author: mrchunckuee_electronics
 *
 * Created on 16 de mayo de 2026, 10:34 PM
 */

#ifndef RTCLIB_H
#define	RTCLIB_H

#ifdef	__cplusplus
extern "C" {
#endif


    #include <xc.h>
    #include <stdint.h>
    #include <stdbool.h>

    // --- ENUM DE RELOJES COMPATIBLES ---
    typedef enum {
        RTC_MODEL_BQ32000 = 0,
        RTC_MODEL_DS1307,
        RTC_MODEL_DS3231
    } RTC_Model_t;

    // Direcciones I2C por defecto de fábrica (por si el usuario quiere usarlas)
    #define BQ32000_DEVICE_ADDRESS      0xD0

    #define RTC_WRITE_BIT           0x00
    #define RTC_READ_BIT            0x01

    // RTC register addresses:
    #define RTC_SECONDS_ADDRESS     0x00   
    #define RTC_NUMBER_DAY_ADDRESS  0x03   
    
    // Define Time Modes
    #define AM_Time               0
    #define PM_Time               1
    #define TwentyFourHoursMode   2

    // Define days
    #define Sunday      1
    #define Monday      2
    #define Tuesday     3
    #define Wednesday   4   
    #define Thursday    5
    #define Friday      6
    #define Saturday    7

    typedef struct {
        uint8_t hours;
        uint8_t minutes;
        uint8_t seconds;
        uint8_t timeMode;
    } Time_t;

    typedef struct {
        uint16_t year;
        uint8_t month;
        uint8_t day;
        uint8_t weekday;  
    } Date_t;

    /*********** P R O T O T Y P E S **********************************************/
    void RTC_Initialize(RTC_Model_t model, uint8_t i2cAddress);
    void RTC_StartClock(void);
    void RTC_WriteByte(uint8_t addr, uint8_t data);
    uint8_t RTC_ReadByte(uint8_t addr);
    void RTC_WriteBytes(uint8_t addr, uint8_t *data, uint8_t len);
    void RTC_ReadBytes(uint8_t addr, uint8_t *data, uint8_t len);
    void RTC_SetTime(const Time_t *time);
    bool RTC_GetTime(Time_t *result);
    void RTC_SetDate(const Date_t *date);
    bool RTC_GetDate(Date_t *result);


#ifdef	__cplusplus
}
#endif

#endif	/* RTCLIB_H */

