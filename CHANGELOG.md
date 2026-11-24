# RTC Library

FileName:        RTCLib.c and RTCLib.h <br>
Processor:       PICxxxxxx <br>
Complier:        XC8 <br>
Author:          Pedro Sanchez (@mrchunckuee_electronics) <br>
Blog:            http://mrchunckuee.blogspot.com/ <br>
Email:           mrchunckuee.electronics@gmail.com <br>
Description:     Firmware change history, bug fixes, and new version implementation.


## 23/11/2026 _v0.0.2_
Pedro Sanchez (@mrchunckuee_electronics)
### Changed
- Optimized RTC_StartClock for BQ32000 model. If STOP == 0, the register is not modified: this avoids resetting the crystal each time the device is powered on.

<br>
<br>

***


## 17/05/2026 _v0.0.1_
Pedro Sanchez (@mrchunckuee_electronics)
### Notes
- Support for BQ32000, DS1307, or DS3231 (in this version I have only tested with the BQ32000).
### Added
- Created library


<br>
<br>

***


