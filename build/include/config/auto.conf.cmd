deps_config := \
	/c/Development/esp/esp-idf/components/app_trace/Kconfig \
	/c/Development/esp/esp-idf/components/aws_iot/Kconfig \
	/c/Development/esp/esp-idf/components/bt/Kconfig \
	/c/Development/esp/esp-idf/components/esp32/Kconfig \
	/c/Development/esp/esp-idf/components/ethernet/Kconfig \
	/c/Development/esp/esp-idf/components/fatfs/Kconfig \
	/c/Development/esp/esp-idf/components/freertos/Kconfig \
	/c/Development/esp/esp-idf/components/heap/Kconfig \
	/c/Development/esp/esp-idf/components/libsodium/Kconfig \
	/c/Development/esp/esp-idf/components/log/Kconfig \
	/c/Development/esp/esp-idf/components/lwip/Kconfig \
	/c/Development/esp/esp-idf/components/mbedtls/Kconfig \
	/c/Development/esp/esp-idf/components/openssl/Kconfig \
	/c/Development/esp/esp-idf/components/pthread/Kconfig \
	/c/Development/esp/esp-idf/components/spi_flash/Kconfig \
	/c/Development/esp/esp-idf/components/spiffs/Kconfig \
	/c/Development/esp/esp-idf/components/tcpip_adapter/Kconfig \
	/c/Development/esp/esp-idf/components/wear_levelling/Kconfig \
	/c/Development/esp/esp-idf/components/bootloader/Kconfig.projbuild \
	/c/Development/esp/esp-idf/components/esptool_py/Kconfig.projbuild \
	/c/Development/esp/esp-idf/components/partition_table/Kconfig.projbuild \
	/c/Development/esp/esp-idf/Kconfig

include/config/auto.conf: \
	$(deps_config)


$(deps_config): ;
