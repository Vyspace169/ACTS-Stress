/*!
 * @file SystemVariables.hpp
 * @data 21 september, 2017
 *
 * This file contains all global defined variables and
 * defines that change the behaviour of the complete
 * program.
 */

#ifndef SYSTEM_VARS_HPP
#define SYSTEM_VARS_HPP

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

extern RTC_DATA_ATTR struct timeval SleepEnterTime;

extern EventGroupHandle_t GlobalEventGroupHandle;

extern struct timeval GlobalTimeValNow;

extern time_t GlobalStartTime;

/*! Movement index data type */
typedef unsigned int movement_type_t;

/**
 * @brief Sample data structure
 *
 * This structure includes all data that will be sampled by the sensortask and
 * will be written by the SDWriterTask. The doublebuffer uses this struct to
 * initialize its buffers.
 */
typedef struct {
	long long microTime;	/**< Timestamp derived from systick */
	short accelX;			/**< Accelerometer X axis */
	short accelY;			/**< Accelerometer Y axis */
	short accelZ;			/**< Accelerometer Z axis */
	short gyroX;			/**< Gyroscope X axis */
	short gyroY;			/**< Gyroscope Y axis */
	short gyroZ;			/**< Gyroscope Z axis */
	short magnetoX;			/**< Magnetometer X axis */
	short magnetoY;			/**< Magnetometer Y axis */
	short magnetoZ;			/**< Magnetometer Z axis */
	int temp;				/**< Temperature */
	int pressure;			/**< Air pressure */
	int ECGSampleValue;		/**< ECG sample value */
	int ECGSampleNumber;	/**< ECG sample number */
} SampleData;

typedef struct {
	int potentialRPeak;		/**< ECG sample values greater then set threshold */
	int sampleNr;			/**< sample number of ECG sample mentioned above */
} RData;

typedef struct {
	int RRInterval;			/**< RR-interval is milliseconds */
	int RRTotal;			/**< Cumulative RR-intervals */
} RRSeries;

typedef struct {
	int LFPower;			/**< Power in 0.04-xx Hz band in m/s^2 */
	int HFPower;			/**< Power in 0.4-xx Hz band in m/s^2 */
	float LFHFRatio;		/**< Ratio of Power in LF-band divided by power in HF-band */
	int StressIndex;		/**< Normalized stress index */
} HRVData;

/*! Flag for SensorTask to act upon */
#define SensorMeasurementFlag 		( 1 << 0 )
/*! Flags for WifiTask to act upon */
#define WifiActivateFlag 			( 1 << 1 )
/*! Flags for WifiTask to start sending data (not implemented yet) */
#define WifiReadyFlag 				( 1 << 2 )
/*! Flag for SdWriterTask to act upon */
#define SensorBufferSdReady 		( 1 << 4 )
/*! Flag to signal movement timeout count has been reached */
#define MovementTimeoutReached 		( 1 << 5 )
/*! Flags for sensor task stating that it is ready to sleep */
#define StandbySensorTaskUnhandled 	( 1 << 6 )
/*! Flags for wifi task stating that it is ready to sleep */
#define StandbyWifiTaskUnhandled   	( 1 << 7 )
/*! Flags for writer task stating that it is ready to sleep */
#define StandbyWriterTaskUnhandled 	( 1 << 8 )
/*! Flag for system errors */
#define SystemErrorFlag      		( 1 << 9 )
/*! Flag for system errors */
#define SystemErrorBit       		( 1 << 10 )
/*! Flag to signal that he SNTP task is done receiving time */
#define SNTPTaskDoneFlag			( 1 << 11 )
/*! Flag to signal that one of the R-buffers is full */
#define RBufferReadyFlag			( 1 << 12 )
/*! Flag to signal that one of the RR-interval buffers is full */
#define RRBufferReadyFlag			( 1 << 13 )
/*! Flag to signal that one of the RR-interval buffers is full */
#define HRVBufferReadyFlag			( 1 << 14 )

/*! Blue LED GPIO define */
#define GPIO_LED_BLUE			GPIO_NUM_13
/*! Green LED GPIO define */
#define GPIO_LED_GREEN			GPIO_NUM_14
/*! Red LED GPIO define */
#define GPIO_LED_RED			GPIO_NUM_17
/*! SD card detect GPIO define */
#define GPIO_SD_DETECT			GPIO_NUM_4
/*! SD card power enable GPIO define */
#define GPIO_SD_POWER			GPIO_NUM_27
/*! Charge detect GPIO define */
#define GPIO_CHARGE_DETECT		GPIO_NUM_21
/*! Battery voltage measurement GPIO define */
#define GPIO_PW_ADC				GPIO_NUM_34   //adc 1 channel 6
/*! MPU9250 interrupt GPIO define */
#define GPIO_MPU_INT			GPIO_NUM_35
/*! Battery ADC channel */
#define ADC_BATTERY				ADC1_CHANNEL_6
/*! Ecg ADC channel */
#define ADC_ECG					ADC1_CHANNEL_7

/*! Sample rate in hz define */
#define SAMPLE_RATE_H			125
/*! Sample rate in ms define */
#define SAMPE_TIME_MS			8
/*! Binary buffer size, the system initializes two of these on startup */
#define BINARY_BUFFER_SIZE		1000

/*! SDWriter MISO pin define */
#define PIN_NUM_MISO 			GPIO_NUM_23
/*! SDWriter MOSI pin define */
#define PIN_NUM_MOSI 			GPIO_NUM_19
/*! SDWriter CLK pin define */
#define PIN_NUM_CLK 			GPIO_NUM_18
/*! SDWriter CS pin define */
#define PIN_NUM_CS 				GPIO_NUM_15

/*! SDWriter spi speed define in KHZ */
#define SD_CARD_SPI_SPEED_KHZ	1000

/*! SDWriter init retry define */
#define SDMMC_INIT_RETRIES		4

/*!
 * Directory used for all the movement data
 * The root partition in a FAT32 memory is limited
 * to a single cluster. There will be a lot of files
 * in this folder, so a single cluster will not be
 * sufficient. This define declares which folder
 * (not root) the measured data should be stored in.
 */
#define DIRECTORY_NAME			"Sensor Data"

/*! I2C SDA pin define */
#define GPIO_SDA				GPIO_NUM_25

/*! I2C SCL pin dfine */
#define GPIO_SCL				GPIO_NUM_26

/*! I2C bus speed in Hz */
#define I2C_SPEED				400000

/*! When the ADC reads a voltage lower than this, the device goes to sleep */
#define TURN_OFF_VOLTAGE		3.0

/*! Define which tells when the LED should blink red
 * due to low battery.
 */
#define BAT_LED_VALUE_VOLTAGE	3.45

/*! ADC division factor for battery voltage reading */
#define ADC_TO_BAT_VOLTAGE		554.0

/*! SD Detect debounce time in milliseconds */
#define SD_DET_DEBOUNCE_MS		50

/*! Wifi ssid to connect to */
#define WIFI_SSID				"espconnect"

/*! Wifi password associated with the ssid */
#define WIFI_PASSWORD			"12345678"

/*! Wifi poll frequency in seconds.
 * This tells after how many seconds the device
 * will try to connect to WiFi.
 */
#define WIFI_POLL_FREQUENCY_SEC	15

/*! Wifi timeout in milliseconds.
 * Whenever the devices tries to connect
 * to the WiFi, this defines how many milliseconds
 * the device will try to connect.
 */
#define WIFI_CONNECT_TIMEOUT	10000

/*! SNTP read time retry count.
 * This defines how many times the SNTP client
 * will try to read the time from the ntp server.
 */
#define SNTP_READ_TIME_RETRY	50

/*! User key, change to patient key as stored in MySql Database*/
#define MQTT_USER_KEY_CODE		"4a6bcefb6217984f477b27c8226200fe3c2a4149"

/*!
 * Blob size for NVS library
 * This value is the maximum amount of bytes that
 * can be written to a single blob in a NVS section.
 * See ESP32 NVS documentation for more info.
 */
#define NFS_BLOB_SIZE			1984

/*! MovementStack maximum queue size.
 * Whenever the dataprocessor's data cant be send,
 * the data will be writen to the MovementStack.
 * This constant declares te maximum size of the
 * MovementStack.
 */
#define QUEUE_MAX_SIZE			NFS_BLOB_SIZE / sizeof(movement_type_t)

/*! Dataprocessor trigger. Whenever dataprocessors
 * timeout counter rises above this value, the
 * timeout counter will be reset.
 */
#define DP_SLEEP_THRESHOLD		500

/*! Wake up threshold.
 * When going to sleep due to a activity timeout,
 * the MPU9250 will enable its motion interrupt.
 * If the movement is higher than
 * ( WAKE_UP_THRESSHOLD * 4mG ) the MPU9250 will
 * set its interrupt pin and wake up the ESP32.
 */
#define WAKE_UP_THRESSHOLD 		40

/*! Movement timeout in seconds.
 * This define tells the movement stack after how
 * many seconds of inactivety the device needs to
 * go to sleep.
 */
#define TIMEOUT_TIME_SEC		60

/*! R-peak threshold in binary value.
 * This define tells the SensorController which
 * Values should be written to a separate buffer
 * for R-peak detection.
 */
#define R_PEAK_THRESHOLD		1000 //TODO determine threshold value

/*! Sensortask core num define */
#define SENSORTASK_CORE_NUM 	1
/*! Sensortask priority define */
#define SENSORTASK_PRIORITY 	2
/*! Sensortask stack size define */
#define SENSORTASK_STACK_SIZE 	4096

/*! Writertask core num define */
#define WRITERTASK_CORE_NUM 	0
/*! Writertask priority define */
#define WRITERTASK_PRIORITY 	2
/*! Writertask stack size define */
#define WRITERTASK_STACK_SIZE 	4096

/*! Wifitask core num define */
#define WIFITASK_CORE_NUM 		0
/*! Wifitask priority define */
#define WIFITASK_PRIORITY 		0
/*! Wifitask stack size define */
#define WIFITASK_STACK_SIZE 	8192

/*! Standbycontroller core num define */
#define STANDBYCONT_CORE_NUM 	1
/*! Standbycontroller priority define */
#define STANDBYCONT_PRIORITY 	4
/*! Standbycontroller stack size define */
#define STANDBYCONT_STACK_SIZE 	2048
/*! Standbycontroller loop delay define */
#define STANDBYCONT_LOOP_DELAY	250

/*! SNTPtask core num define */
#define SNTPTASK_CORE_NUM 		0
/*! SNTPtask priority define */
#define SNTPTASK_PRIORITY 		1
/*! SNTPtask stack size define */
#define SNTPTASK_STACK_SIZE 	6144

/*! SNTPtask core num define */
#define BLINKTASK_CORE_NUM 		0
/*! SNTPtask priority define */
#define BLINKTASK_PRIORITY 		5
/*! SNTPtask stack size define */
#define BLINKTASK_STACK_SIZE 	2048

/*! HRVTask core num define */
#define HRVTASK_CORE_NUM 		0
/*! HRVTask priority define */
#define HRVTASK_PRIORITY 		6
/*! HRVTask stack size define */
#define HRVTASK_STACK_SIZE 	2048

/*! Wifi event bit used in WiFiC.c file */
#define WIFI_EVENT_BIT			1

#endif //SYSTEM_VARS_HPP
