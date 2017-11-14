deps_config := \
	/c/ESP32/esp-idf/components/app_trace/Kconfig \
	/c/ESP32/esp-idf/components/aws_iot/Kconfig \
	/c/ESP32/esp-idf/components/bt/Kconfig \
	/c/ESP32/esp-idf/components/esp32/Kconfig \
	/c/ESP32/esp-idf/components/ethernet/Kconfig \
	/c/ESP32/esp-idf/components/fatfs/Kconfig \
	/c/ESP32/esp-idf/components/freertos/Kconfig \
	/c/ESP32/esp-idf/components/heap/Kconfig \
	/c/ESP32/esp-idf/components/libsodium/Kconfig \
	/c/ESP32/esp-idf/components/log/Kconfig \
	/c/ESP32/esp-idf/components/lwip/Kconfig \
	/c/ESP32/esp-idf/components/mbedtls/Kconfig \
	/c/ESP32/esp-idf/components/openssl/Kconfig \
	/c/ESP32/esp-idf/components/pthread/Kconfig \
	/c/ESP32/esp-idf/components/spi_flash/Kconfig \
	/c/ESP32/esp-idf/components/spiffs/Kconfig \
	/c/ESP32/esp-idf/components/tcpip_adapter/Kconfig \
	/c/ESP32/esp-idf/components/wear_levelling/Kconfig \
	/c/ESP32/esp-idf/components/bootloader/Kconfig.projbuild \
	/c/ESP32/esp-idf/components/esptool_py/Kconfig.projbuild \
	/c/ESP32/esp-idf/components/partition_table/Kconfig.projbuild \
	/c/ESP32/esp-idf/Kconfig

include/config/auto.conf: \
	$(deps_config)


$(deps_config): ;
