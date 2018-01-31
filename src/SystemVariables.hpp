#ifndef SYSTEM_VARS_HPP
#define SYSTEM_VARS_HPP

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

extern RTC_DATA_ATTR struct timeval SleepEnterTime;

extern EventGroupHandle_t GlobalEventGroupHandle;

extern struct timeval GlobalTimeValNow;

extern time_t GlobalStartTime;

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
} SampleData;

/*! Flag for SensorTask to act upon */
#define SensorMeasurementFlag 		( 1 << 0 )
/*! Flags for WifiTask to act upon */
#define WifiActivateFlag 			( 1 << 1 )
#define WifiReadyFlag 				( 1 << 2 )
/*! Flag for SdWriterTask to act upon */
#define SensorBufferSdReady 		( 1 << 4 )
/*! Flag to signal movement timeout count has been reached */
#define MovementTimeoutReached 		( 1 << 5 )
/*! Flags for tasks stating that they are ready to sleep */
#define StandbySensorTaskUnhandled 	( 1 << 6 )
#define StandbyWifiTaskUnhandled   	( 1 << 7 )
#define StandbyWriterTaskUnhandled 	( 1 << 8 )
/*! Flag for system errors */
#define SystemErrorFlag      		( 1 << 9 )
#define SystemErrorBit       		( 1 << 10 )
/*! Flag to signal that he SNTP task is done receiving time */
#define SNTPTaskDoneFlag			( 1 << 11 )

/*! LED GPIO defines */
#define GPIO_LED_BLUE			GPIO_NUM_13
#define GPIO_LED_GREEN			GPIO_NUM_14
#define GPIO_LED_RED			GPIO_NUM_17
/*! SD card GPIO defines */
#define GPIO_SD_DETECT			GPIO_NUM_4
#define GPIO_SD_POWER			GPIO_NUM_27
/*! Power GPIO defines */
#define GPIO_CHARGE_DETECT		GPIO_NUM_21
#define GPIO_PW_ADC				GPIO_NUM_34   //adc 1 channel 6
/*! MPU9250 interrupt GPIO define */
#define GPIO_MPU_INT			GPIO_NUM_35
/*! Battery ADC channel */
#define ADC_BATTERY				ADC1_CHANNEL_6

/*! Sample rate in hz define */
#define SAMPLE_RATE_H			100
/*! Sample rate in ms define */
#define SAMPE_TIME_MS			10
/*! Binary buffer size, the system initializes two of these on startup */
#define BINARY_BUFFER_SIZE		1000

/*! SDWriter pin defines */
#define PIN_NUM_MISO 			GPIO_NUM_23
#define PIN_NUM_MOSI 			GPIO_NUM_19
#define PIN_NUM_CLK 			GPIO_NUM_18
#define PIN_NUM_CS 				GPIO_NUM_5

/*! SDWriter spi speed define in KHZ */
#define SD_CARD_SPI_SPEED_KHZ	1000

/*! SDWriter init retry define */
#define SDMMC_INIT_RETRIES		4

/*! I2C SDA pin define */
#define GPIO_SDA				GPIO_NUM_25

/*! I2C SCL pin dfine */
#define GPIO_SCL				GPIO_NUM_26

/*! I2C bus speed in Hz */
#define I2C_SPEED				400000

/*! When the ADC reads a voltage lower than this, the device goes to sleep */
#define TURN_OFF_VOLTAGE		3.0

/*! ADC division factor for battery voltage reading */
#define ADC_TO_BAT_VOLTAGE		587.0

/*! SD Detect debounce time in milliseconds */
#define SD_DET_DEBOUNCE_MS		50

/*! TCP server ip */
#define WIFI_TCP_SERVER			"192.168.8.101"

/*! TCP server port */
#define WIFI_TCP_PORT			3010

/*! Wifi ssid to connect to */
#define WIFI_SSID				"ACTS"

/*! Wifi password associated with the ssid */
#define WIFI_PASSWORD			"12345678"

/*! Wifi poll frequency in seconds */
#define WIFI_POLL_FREQUENCY_SEC	10

/*! Wifi timeout in milliseconds */
#define WIFI_CONNECT_TIMEOUT	10000

/*! SNTP read time retry count.
 * This defines how many times the SNTP client
 * will try to read the time from the ntp server.
 */
#define SNTP_READ_TIME_RETRY	50

/*! User key, change to patient key as stored in MySql Database*/
#define MQTT_USER_KEY_CODE		"4a6bcefb6217984f477b27c8226200fe3c2a414d"

/*! MovementStack maximum queue size.
 * Whenever the dataprocessor's data cant be send,
 * the data will be writen to the MovementStack.
 * This constant declares te maximum size of the
 * MovementStack.
 */
#define QUEUE_MAX_SIZE			300

/*! Dataprocessor trigger. Whenever dataprocessors
 * timeout counter rises above this value, the
 * timeout counter will be reset.
 */
#define DP_SLEEP_THRESHOLD		500

/*! Wake up threshold.
 * ( value * 4mG )
 */
#define WAKE_UP_THRESSHOLD 		40

/*! Movement timeout in seconds */
#define TIMEOUT_TIME_SEC		60

/*! Sensortask defines */
#define SENSORTASK_CORE_NUM 	1
#define SENSORTASK_PRIORITY 	2
#define SENSORTASK_STACK_SIZE 	2048

/*! Writertask defines */
#define WRITERTASK_CORE_NUM 	0
#define WRITERTASK_PRIORITY 	2
#define WRITERTASK_STACK_SIZE 	4096

/*! Wifitask defines */
#define WIFITASK_CORE_NUM 		0
#define WIFITASK_PRIORITY 		0
#define WIFITASK_STACK_SIZE 	8192

/*! Standbycontroller defines */
#define STANDBYCONT_CORE_NUM 	1
#define STANDBYCONT_PRIORITY 	4
#define STANDBYCONT_STACK_SIZE 	2048
#define STANDBYCONT_LOOP_DELAY	250

/*! SNTPtask defines */
#define SNTPTASK_CORE_NUM 		0
#define SNTPTASK_PRIORITY 		1
#define SNTPTASK_STACK_SIZE 	8000

/*! SNTPtask defines */
#define BLINKTASK_CORE_NUM 		0
#define BLINKTASK_PRIORITY 		5
#define BLINKTASK_STACK_SIZE 	2048

/*! Wifi event bit used in WiFiC.c file */
#define WIFI_EVENT_BIT			1


#endif //SYSTEM_VARS_HPP
