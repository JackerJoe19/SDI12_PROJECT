// This Is Mike Library
// This Library Is Mainly Use For Checking SD Card Installation Process And Store Information From The Sensor To The Data Log File

#ifndef DATALOG_H
#include <Arduino.h>
#include <RTClib.h>
#include <Wire.h>
//#include <SD.h>
#include<SdFat.h>
#include<SdFatConfig.h>
#include <Adafruit_I2CDevice.h>
#include <SPI.h>
//#include <DueTimer.h>
#include "../lib/sensors.h"
//#include "../lib/menus.h"
#include <RTCDue.h>

#if SPI_DRIVER_SELECT == 2  // Must be set in SdFat/SdFatConfig.h
//DO THIS BEFORE YOU COMPILE, OTHERWISE The Device Will Not Work

// SD_FAT_TYPE = 0 for SdFat/File as defined in SdFatConfig.h,
// 1 for FAT16/FAT32, 2 for exFAT, 3 for FAT16/FAT32 and exFAT.
#define SD_FAT_TYPE 0
//
// Chip select may be constant or RAM variable.
const uint8_t SD_CS_PIN = A3;
//
// Pin numbers in templates must be constants.
const uint8_t SOFT_MISO_PIN = 12;
const uint8_t SOFT_MOSI_PIN = 11;
const uint8_t SOFT_SCK_PIN = 13;

// SdFat software SPI template
SoftSpiDriver<SOFT_MISO_PIN, SOFT_MOSI_PIN, SOFT_SCK_PIN> softSpi;
// Speed argument is ignored for software SPI.
#if ENABLE_DEDICATED_SPI
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, DEDICATED_SPI, SD_SCK_MHZ(0), &softSpi)
#else  // ENABLE_DEDICATED_SPI
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, SHARED_SPI, SD_SCK_MHZ(0), &softSpi)
#endif  // ENABLE_DEDICATED_SPI

// Chosing The Right Card And The Right File
#if SD_FAT_TYPE == 0
SdFat sd;
File file;
#elif SD_FAT_TYPE == 1
SdFat32 sd;
File32 file;
#elif SD_FAT_TYPE == 2
SdExFat sd;
ExFile file;
#elif SD_FAT_TYPE == 3
SdFs sd;
FsFile file;
#else  // SD_FAT_TYPE
#error Invalid SD_FAT_TYPE
#endif  // SD_FAT_TYPE
#endif

/* Create an rtc object and select Slowclock source */
// RTCDue rtc(RC);
RTCDue rtc(XTAL);
enum SD_settings
// Connect SD intergration gate to the right pin number    
{
    SD_CS = A3,
    // SD_RST = 6,
    // SD_DC = 7,
    SD_MISO = 12,
    SD_SCLK = 13,
    SD_MOSI = 11
};
// RTC_DS1307 rtc;
// Define the File Named logFile
File logfile;
// Define eject button always set as false because we don't want to eject the SD card at first
bool eject_triggered_previously = false;
void logsensordata()
{
   // Serial.print("We reach logsensor");
   // Serial.flush();
    airoutput output = sensors.airreading();
    //Serial.print("We reach air");
   // Serial.flush();
    Lightoutput lightoutput = sensors.lightreading();
    //Serial.print("We reach light");
    //Serial.flush();
    logfile.print(rtc.unixtime(), DEC);
    logfile.print(",");
    logfile.print(output.temp);
    logfile.print(",");
    logfile.print(output.humidity);
    logfile.print(",");
    logfile.print(output.pressure);
    logfile.print(",");
    logfile.print(lightoutput.lux);
    logfile.println(); // End the Line With A Newline Under
    logfile.flush();
    
    // Close the file
    //logfile.close();   
    //Serial.println("Instance SD write");
}


// Create an Eject function for SD card Reader
void SD_eject(){
    //eject_triggered_previously set as false above
    if(eject_triggered_previously){
        return;
    }
    //if eject button clicked: ---> close all file and exit the program
    logfile.close();
    sd.end();
    eject_triggered_previously = true;
}
void SD_init()
{
    if (!sd.begin(SD_CONFIG))
    {
        // This Function Is Used To Check The SD Installation Process Again To See Whether It Is Properly Inserted And Connected.
        // Halting the program execution to prevent any further code execution that relies on the SD card.
        sd.initErrorHalt();
    }
    Serial.println("Initialization Successfully.");
    
    // 0_RDWR : Allow To Read And Write On The File
    // 0_CREAT: Allow To Create The File If It Doesn't Exist
    // 0_SYNC: The File Operations should be synchronized with the underlying storage medium. It ensures that data is written to the storage device immediately after each write operation
    logfile = sd.open("sensor_log.CSV", O_RDWR | O_CREAT | O_SYNC);
    if(!logfile){
        sd.errorHalt(F("Error Opening File. Please Check And Try Again"));
    }
    if (logfile.size() == 0)
    {
        Serial.println("Writing CSV Header");
        logfile.println("EPOCH,Temperature,Humidity,Pressure,Light Intensity");
        //Write The Data Directly To SD Card Immediately. 
        logfile.flush();
    }
    rtc.begin();

}
#endif
