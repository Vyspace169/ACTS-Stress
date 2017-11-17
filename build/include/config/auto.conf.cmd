deps_config := \
	/home/jop/esp/esp-idf/components/app_trace/Kconfig \
	/home/jop/esp/esp-idf/components/aws_iot/Kconfig \
	/home/jop/esp/esp-idf/components/bt/Kconfig \
	/home/jop/esp/esp-idf/components/esp32/Kconfig \
	/home/jop/esp/esp-idf/components/ethernet/Kconfig \
	/home/jop/esp/esp-idf/components/fatfs/Kconfig \
	/home/jop/esp/esp-idf/components/freertos/Kconfig \
	/home/jop/esp/esp-idf/components/heap/Kconfig \
	/home/jop/esp/esp-idf/components/log/Kconfig \
	/home/jop/esp/esp-idf/components/lwip/Kconfig \
	/home/jop/esp/esp-idf/components/mbedtls/Kconfig \
	/home/jop/esp/esp-idf/components/openssl/Kconfig \
	/home/jop/esp/esp-idf/components/pthread/Kconfig \
	/home/jop/esp/esp-idf/components/spi_flash/Kconfig \
	/home/jop/esp/esp-idf/components/spiffs/Kconfig \
	/home/jop/esp/esp-idf/components/tcpip_adapter/Kconfig \
	/home/jop/esp/esp-idf/components/wear_levelling/Kconfig \
	/home/jop/esp/esp-idf/components/bootloader/Kconfig.projbuild \
	/home/jop/esp/esp-idf/components/esptool_py/Kconfig.projbuild \
	/home/jop/esp/esp-idf/components/partition_table/Kconfig.projbuild \
	/home/jop/esp/esp-idf/Kconfig

include/config/auto.conf: \
	$(deps_config)


$(deps_config): ;
