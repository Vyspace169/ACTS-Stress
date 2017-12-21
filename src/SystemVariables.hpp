/// @file

#ifndef SYSTEM_VARS_HPP
#define SYSTEM_VARS_HPP

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

extern EventGroupHandle_t GlobalEventGroupHandle;

typedef struct {
	long long microTime;
	short accelX;
	short accelY;
	short accelZ;
	short gyroX;
	short gyroY;
	short gyroZ;
	short magnetoX;
	short magnetoY;
	short magnetoZ;
	int temp;
	int pressure;
} SampleData;
/// @defgroup FLAG_GROUP Flag Group
///	{
// -------Flags for Tasks----------------------------
/// Flag that is used to make sensortask sample for new data
#define SensorMeasurementFlag 		( 1 << 0 )
/// T.B.D.
#define WifiActivateFlag 			( 1 << 1 )
/// T.B.D.
#define WifiReadyFlag 				( 1 << 2 )
/// Flag that is set when Buffer is ready to be written by SDTask
#define SensorBufferSdReady 		( 1 << 4 )
/// T.B.D.
#define MovementTimeoutReached 		( 1 << 5 )
/// T.B.D.
#define StandbySensorTaskUnhandled 	( 1 << 6 )
/// T.B.D.
#define StandbyWifiTaskUnhandled   	( 1 << 7 )
/// T.B.D.
#define StandbyWriterTaskUnhandled 	( 1 << 8 )
/// T.B.D.
#define SystemErrorFlag      		( 1 << 9 )
/// T.B.D.
#define SystemErrorBit       		( 1 << 10 )
/// @}
// GPIO defines

/// @defgroup LED_GROUP Led Group
/// @{
/// GPIO Blue Led
#define GPIO_LED_BLUE			GPIO_NUM_13
/// GPIO Green Led
#define GPIO_LED_GREEN			GPIO_NUM_14
/// GPIO Red Led
#define GPIO_LED_RED			GPIO_NUM_12
/// @}
#define GPIO_SD_DETECT			GPIO_NUM_4
#define GPIO_CHARGE_DETECT		GPIO_NUM_21
#define GPIO_CHG_CNTRL			GPIO_NUM_27
#define GPIO_PW_GOOD			GPIO_NUM_16
#define GPIO_PW_ADC				GPI_NUM_34 //adc 1 channel 6
#define GPIO_MPU_INT			GPI_NUM_35

// Frequency defines
#define SAMPLE_RATE_H			100
#define SAMPE_TIME_MS			10
#define BINARY_BUFFER_SIZE		1000

// SDWriter defines
#define PIN_NUM_MISO 			GPIO_NUM_23
#define PIN_NUM_MOSI 			GPIO_NUM_19
#define PIN_NUM_CLK 			GPIO_NUM_18
#define PIN_NUM_CS 				GPIO_NUM_5
#define SD_CARD_SPI_SPEED_KHZ	1000
#define SDMMC_INIT_RETRIES		4

// Sleep settings
#define SLEEP_TIME_SEC 			60
#define TIMEOUT_TIME_SEC		60

// Wifi settings
#define WIFI_POLL_FREQUENCY_SEC	10
#define WIFI_CONNECT_TIMEOUT	10000
#define WIFI_SSID				"ACTS"
#define WIFI_PASSWORD			"12345678"

// I2C defines
#define GPIO_SDA				GPIO_NUM_25
#define GPIO_SCL				GPIO_NUM_26
#define I2C_SPEED				400000

// Dataprocessor defines
#define TRIGGER_VALUE_X			1000
#define TRIGGER_VALUE_Y			1000
#define TRIGGER_VALUE_Z			1000

// SensorTask defines
#define SENSORTASK_CORE_NUM 	1
#define SENSORTASK_PRIORITY 	2
#define SENSORTASK_STACK_SIZE 	2048

// SensorTask defines
#define WRITERTASK_CORE_NUM 	0
#define WRITERTASK_PRIORITY 	2
#define WRITERTASK_STACK_SIZE 	4096

// Wifitask defines
#define WIFITASK_CORE_NUM 		0
#define WIFITASK_PRIORITY 		1
#define WIFITASK_STACK_SIZE 	32000

// StandbyController defines
#define STANDBYCONT_CORE_NUM 	1
#define STANDBYCONT_PRIORITY 	4
#define STANDBYCONT_STACK_SIZE 	2048
#define STANDBYCONT_LOOP_DELAY	250

// Test funtions
//#define DATA_THROUGH_TCP		1

#define WIFI_EVENT_BIT			1

#endif //SYSTEM_VARS_HPP
