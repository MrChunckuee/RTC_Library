## RTC Library
A C library for interfacing with Real-Time Clock (RTC) modules using Microchip PIC microcontrollers. Designed for ease of use, it manages time and date records via I2C communication and is compatible with PIC16 or PIC18 device using the XC8 compiler.

## Features
* **I2C Protocol:** Efficient communication over the standard I2C bus.
* **Struct Configuration:** Uses a structure-based initialization for easy integration.
* **Time & Date Management:** Easily read and write hours, minutes, seconds, day of the week, day of the month, month, and year.
* **BCD Functions:** Includes commands for BCD (Binary Coded Decimal) conversion and direct register configuration.

## Supported / Tested Devices
This library is designed to work with standard I2C RTC integrated circuits. The following chips have been explicitly tested and verified:

| Microcontroller | RTC Chip | Status | Notes |
| :--- | :--- | :--- | :--- |
| **PIC18F25K22** | **TI BQ32000** | ✔ Fully Tested | Verified via physical hardware implementation. |
| *Generic PIC16 / PIC18* | **DS1307 / DS3231** | ⚠ Compatible | Compatible via standard I2C register mapping. |

## Hardware Connection
Typical connection for I2C operation (using a standard module like Tiny RTC):

| RTC Pin | Function | PIC Connection (Example) |
| :--- | :--- | :--- |
| **VCC** | Power | +5V / +3.3V |
| **GND** | Ground | GND |
| **SDA** | Serial Data | I2C Data Pin (with pull-up resistor) |
| **SCL** | Serial Clock | I2C Clock Pin (with pull-up resistor) |
| **SQW** | Square Wave Output | Optional / Interrupt Pin (if used) |

## Usage Example
This example shows how to initialize the RTC module, set the initial time and date, and read them back continuously.

```c
#include <xc.h>
#include "RTCLib.h"

void main(void) {
    Time_t current_time;
    Date_t current_date;

    // Initialize system ports and I2C peripheral
    SYSTEM_Initialize();
    I2C_Initialize();
    
    // Initialize RTC module
    RTC_Init();
    
    // Initialize time and date structures
    Time_t horaInicial = {.hours = 18, .minutes = 12, .seconds = 0, .timeMode = TwentyFourHoursMode};
    Date_t fechaInicial = {.year = 2026, .month = 5, .day = 17, .weekday = Sunday};
    
    // Set initial time and date to the RTC
    RTC_SetTime(&horaInicial);
    RTC_SetDate(&fechaInicial);
    
    while(1) {
        // Read current time and date from RTC
        RTC_GetTime(&current_time);
        RTC_GetDate(&current_date);
        
        // Your application logic (e.g., print time/date or log data)
        __delay_ms(1000);
    }
}
```

## Project Structure
* `src/`: Core library files (`RTCLib.c` and `RTCLib.h`).
* `LICENSE`: MIT License.
* `CHANGELOG`: History of updates and bug fixes.

## Documentation & Tutorial
For a detailed implementation explanation and step-by-step guide, you can review the following examples:
* https://mrchunckuee.blogspot.com/
