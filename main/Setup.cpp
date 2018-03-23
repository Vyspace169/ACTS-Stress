#include "Setup.hpp"
static gpio_num_t UsedLEDPin;
static int UsedLEDTimeOn;
static int UsedLEDTimeOff;

void blink_task(void *pvParameter)
{
    while(1) {
    	gpio_set_level(UsedLEDPin, 1);
    	vTaskDelay(UsedLEDTimeOff / portTICK_PERIOD_MS); // @suppress("Invalid arguments") // @suppress("Symbol is not resolved")
    	gpio_set_level(UsedLEDPin, 0);
    	vTaskDelay(UsedLEDTimeOn / portTICK_PERIOD_MS); // @suppress("Invalid arguments") // @suppress("Symbol is not resolved")
    }
}

void sntp_task(void* param) {

	ESP_LOGI("SNTP TASK", "Initializing wifi and reading time"); // @suppress("Symbol is not resolved")
    WiFiInitialize(WIFI_SSID, WIFI_PASSWORD);

    int WakeUpCause = esp_sleep_get_wakeup_cause();
    switch(WakeUpCause) {
    case ESP_SLEEP_WAKEUP_TIMER:
    case ESP_SLEEP_WAKEUP_EXT1:
    case ESP_SLEEP_WAKEUP_EXT0:
    	xEventGroupSetBits(GlobalEventGroupHandle, SNTPTaskDoneFlag);
    	while(1) {
    	   	vTaskDelay(5000 / portTICK_PERIOD_MS); // @suppress("Symbol is not resolved") // @suppress("Invalid arguments")
    	}
    	break;
    }

	bool enabled = WiFiConnect(WIFI_CONNECT_TIMEOUT);
	GlobalStartTime = WiFiGetTime(SNTP_READ_TIME_RETRY);
	WiFiDisconnect();

	if(GlobalStartTime == 0) {
		GlobalStartTime = GlobalTimeValNow.tv_sec;
	}
	else {
		GlobalTimeValNow.tv_sec = GlobalStartTime;
	}
	settimeofday(&GlobalTimeValNow, NULL);

	xEventGroupSetBits(GlobalEventGroupHandle, SNTPTaskDoneFlag);

	while(1) {
	   	vTaskDelay(5000 / portTICK_PERIOD_MS); // @suppress("Symbol is not resolved") // @suppress("Invalid arguments")
	}
}

void gpio_init_all() {
	// Enable GPIO_SD_DETECT again since it was assigned to RTC
	rtc_gpio_deinit(GPIO_SD_DETECT);

	// initalize SD_DETECT pin
	gpio_pad_select_gpio(GPIO_SD_DETECT);
	gpio_set_direction(GPIO_SD_DETECT, GPIO_MODE_INPUT);

	// Enable power to SD card
	gpio_pad_select_gpio(GPIO_SD_POWER);
	gpio_set_direction(GPIO_SD_POWER, GPIO_MODE_OUTPUT);
	gpio_set_level(GPIO_SD_POWER, 0);

	// Set MPU9250 interrupt as input
	gpio_pad_select_gpio(GPIO_MPU_INT);
	gpio_set_direction(GPIO_MPU_INT, GPIO_MODE_INPUT);

	// init battery adc pin
	adc1_config_width(ADC_WIDTH_12Bit);
	adc1_config_channel_atten(ADC_BATTERY, ADC_ATTEN_11db);

	// init sd detect pin and variables
	gpio_pad_select_gpio(GPIO_SD_DETECT);
	gpio_set_direction(GPIO_SD_DETECT, GPIO_MODE_INPUT);

	// Set charge detect pin as input
	gpio_pad_select_gpio(GPIO_CHARGE_DETECT);
	gpio_pad_pullup(GPIO_CHARGE_DETECT);

	// Set LED pins as output and state high
	gpio_pad_select_gpio(GPIO_LED_RED);
	gpio_pad_select_gpio(GPIO_LED_GREEN);
	gpio_pad_select_gpio(GPIO_LED_BLUE);
	gpio_set_direction(GPIO_LED_RED, GPIO_MODE_OUTPUT);
	gpio_set_direction(GPIO_LED_GREEN, GPIO_MODE_OUTPUT);
	gpio_set_direction(GPIO_LED_BLUE, GPIO_MODE_OUTPUT);
	gpio_set_level(GPIO_LED_RED, 1);
	gpio_set_level(GPIO_LED_GREEN, 1);
	gpio_set_level(GPIO_LED_BLUE, 1);

	// init ecg adc pin
	adc1_config_width(ADC_WIDTH_12Bit);
	adc1_config_channel_atten(ADC_ECG, ADC_ATTEN_11db);
}

void blink_set_led(gpio_num_t led, int time_on, int time_off) {
	if(led == GPIO_LED_RED || led == GPIO_LED_GREEN || led == GPIO_LED_BLUE) {
		UsedLEDPin = led;
		UsedLEDTimeOn = time_on;
		UsedLEDTimeOff = time_off;
	}
}

void i2c_master_init() {
	// give scl pin some clocks
	gpio_pad_select_gpio(GPIO_SCL);
	gpio_set_direction(GPIO_SCL, GPIO_MODE_OUTPUT);
	for(int i=0;i<100;i++) {
		gpio_set_level(GPIO_SCL, 1);
		vTaskDelay(1);
		gpio_set_level(GPIO_SCL, 0);
		vTaskDelay(1);
	}

	// initialize bus
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = GPIO_SDA;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = GPIO_SCL;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_SPEED;

	if(i2c_param_config(I2C_NUM_0, &conf) != ESP_OK)  {
		ESP_LOGE("INIT_ERROR", "Error code:%i , message: i2c param config failed!", ERROR_I2C_PARAM_CONFIG); // @suppress("Symbol is not resolved")
	}

	if(i2c_driver_install(I2C_NUM_0, conf.mode, 0, 0, 0) != ESP_OK) {
		ESP_LOGE("INIT_ERROR", "Error code:%i , message: i2c driver install failed!", ERROR_I2C_DRIVER_INSTALL); // @suppress("Symbol is not resolved")
    }
}


void error_flash_init() {

    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }

    nvs_handle my_handle;
    err = nvs_open("storage", NVS_READWRITE, &my_handle);
    int32_t error_value = (int32_t) ErrorCode::NO_ERROR; // value will default to 0, if not set yet in NVS
    err = nvs_get_i32(my_handle, "error_value", &error_value);
    bool failed = false;
    switch (err) {
        case ESP_OK:
        	ESP_LOGI("INIT", "Done"); // @suppress("Symbol is not resolved")
        	ESP_LOGI("INIT", "Restart counter = %d", error_value); // @suppress("Symbol is not resolved")
            break;
        case ESP_ERR_NVS_NOT_FOUND:
            failed = true;
            break;
        default :
        	ESP_LOGI("INIT", "Error (%d) reading!", err); // @suppress("Symbol is not resolved")
    }

    if(failed)  {
        //int32_t error_code = (int32_t) error::errCode; // value will default to 0, if not set yet in NVS
        err = nvs_set_i32(my_handle, "error_value", (int32_t) ErrorCode::NO_ERROR);
        err = nvs_commit(my_handle);
        nvs_close(my_handle);
    }
    else    {
        nvs_close(my_handle);
    }

    if(error_value != (int32_t) ErrorCode::NO_ERROR) {
        ESP_LOGI("INIT", "Last ran ended with error code: %i\n", error_value); // @suppress("Symbol is not resolved")
    }
    else    {
        ESP_LOGI("INIT", "NO EXCEPTION FOUND IN LAST RUN"); // @suppress("Symbol is not resolved")
    }
}

void BuildFileName(char *TimeStringBuffer, int str_len) {

	if(GlobalStartTime == 0) {
		// If time was not received from wifi, read string from flash
		esp_err_t err;
		nvs_handle file_name_handle;

		nvs_open("storage", NVS_READWRITE, &file_name_handle);
		err = nvs_get_str(file_name_handle, "Fname", TimeStringBuffer, (size_t *)&str_len);
		if(err != ESP_OK) {
			strcpy(TimeStringBuffer, "GenericName");
			ESP_LOGI("MAIN", "Error (%d) reading!", err); // @suppress("Symbol is not resolved")
		}
		nvs_close(file_name_handle);

		ESP_LOGI("MAIN", "Filename from flash: %s", TimeStringBuffer); // @suppress("Symbol is not resolved")
	}
	else {
		// If time was received from wifi, build strings
		struct tm timeinfo;
		localtime_r(&GlobalStartTime, &timeinfo);
		strftime(TimeStringBuffer, str_len, "%Y_%m_%d_%H_%M_%S", &timeinfo);
		strcat(TimeStringBuffer, ".bin");

		nvs_handle file_name_handle;
		nvs_open("storage", NVS_READWRITE, &file_name_handle);
		nvs_set_str(file_name_handle, "Fname", TimeStringBuffer);
		nvs_commit(file_name_handle);
		nvs_close(file_name_handle);

		ESP_LOGI("MAIN", "Filename from wifi: %s", TimeStringBuffer); // @suppress("Symbol is not resolved")
	}
}

void CheckForSdcard() {
	// init sd detect pin and variables
	ESP_LOGI("MAIN", "Checking if SD-Card is in socket"); // @suppress("Symbol is not resolved")
	gpio_pad_select_gpio(GPIO_SD_DETECT);
	gpio_set_direction(GPIO_SD_DETECT, GPIO_MODE_INPUT);
	if(gpio_get_level(GPIO_SD_DETECT) == 0) {
		ESP_LOGI("MAIN", "No SD-Card found, going to sleep"); // @suppress("Symbol is not resolved")
		esp_sleep_enable_ext1_wakeup((1<<GPIO_SD_DETECT), ESP_EXT1_WAKEUP_ANY_HIGH);
		esp_deep_sleep_start();
	}
}
