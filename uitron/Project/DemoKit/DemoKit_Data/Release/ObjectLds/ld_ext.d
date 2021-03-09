DemoKit_Data/Release/ObjectLds/MODELEXT_MIO_ETHCAM_RX/MODELEXT_MIO_ETHCAM_RX.a:../../ARC_eCos/Drv/Release/MODELEXT_MIO_ETHCAM_RX.a
	@echo Copying ObjectLds:  $(notdir $@)
	@rm -rf $(dir $@)
	@mkdir -p $(dir $@)
	cp $< $@
	chmod 777 $@
	@echo Extracting: DemoKit_Data/Release/ObjectLds/MODELEXT_MIO_ETHCAM_RX/modelext_info.o
	$(AR) x $@ $(notdir DemoKit_Data/Release/ObjectLds/MODELEXT_MIO_ETHCAM_RX/modelext_info.o)
	$(AR) d $@ $(notdir DemoKit_Data/Release/ObjectLds/MODELEXT_MIO_ETHCAM_RX/modelext_info.o)
	mv $(notdir DemoKit_Data/Release/ObjectLds/MODELEXT_MIO_ETHCAM_RX/modelext_info.o) DemoKit_Data/Release/ObjectLds/MODELEXT_MIO_ETHCAM_RX/modelext_info.o;
	@echo Extracting: DemoKit_Data/Release/ObjectLds/MODELEXT_MIO_ETHCAM_RX/bin_info.o
	$(AR) x $@ $(notdir DemoKit_Data/Release/ObjectLds/MODELEXT_MIO_ETHCAM_RX/bin_info.o)
	$(AR) d $@ $(notdir DemoKit_Data/Release/ObjectLds/MODELEXT_MIO_ETHCAM_RX/bin_info.o)
	mv $(notdir DemoKit_Data/Release/ObjectLds/MODELEXT_MIO_ETHCAM_RX/bin_info.o) DemoKit_Data/Release/ObjectLds/MODELEXT_MIO_ETHCAM_RX/bin_info.o;
	@echo Extracting: DemoKit_Data/Release/ObjectLds/MODELEXT_MIO_ETHCAM_RX/pinmux_cfg.o
	$(AR) x $@ $(notdir DemoKit_Data/Release/ObjectLds/MODELEXT_MIO_ETHCAM_RX/pinmux_cfg.o)
	$(AR) d $@ $(notdir DemoKit_Data/Release/ObjectLds/MODELEXT_MIO_ETHCAM_RX/pinmux_cfg.o)
	mv $(notdir DemoKit_Data/Release/ObjectLds/MODELEXT_MIO_ETHCAM_RX/pinmux_cfg.o) DemoKit_Data/Release/ObjectLds/MODELEXT_MIO_ETHCAM_RX/pinmux_cfg.o;
	@echo Extracting: DemoKit_Data/Release/ObjectLds/MODELEXT_MIO_ETHCAM_RX/intdir_cfg.o
	$(AR) x $@ $(notdir DemoKit_Data/Release/ObjectLds/MODELEXT_MIO_ETHCAM_RX/intdir_cfg.o)
	$(AR) d $@ $(notdir DemoKit_Data/Release/ObjectLds/MODELEXT_MIO_ETHCAM_RX/intdir_cfg.o)
	mv $(notdir DemoKit_Data/Release/ObjectLds/MODELEXT_MIO_ETHCAM_RX/intdir_cfg.o) DemoKit_Data/Release/ObjectLds/MODELEXT_MIO_ETHCAM_RX/intdir_cfg.o;
	@echo Extracting: DemoKit_Data/Release/ObjectLds/MODELEXT_MIO_ETHCAM_RX/emb_partition_info.o
	$(AR) x $@ $(notdir DemoKit_Data/Release/ObjectLds/MODELEXT_MIO_ETHCAM_RX/emb_partition_info.o)
	$(AR) d $@ $(notdir DemoKit_Data/Release/ObjectLds/MODELEXT_MIO_ETHCAM_RX/emb_partition_info.o)
	mv $(notdir DemoKit_Data/Release/ObjectLds/MODELEXT_MIO_ETHCAM_RX/emb_partition_info.o) DemoKit_Data/Release/ObjectLds/MODELEXT_MIO_ETHCAM_RX/emb_partition_info.o;
	@echo Extracting: DemoKit_Data/Release/ObjectLds/MODELEXT_MIO_ETHCAM_RX/gpio_info.o
	$(AR) x $@ $(notdir DemoKit_Data/Release/ObjectLds/MODELEXT_MIO_ETHCAM_RX/gpio_info.o)
	$(AR) d $@ $(notdir DemoKit_Data/Release/ObjectLds/MODELEXT_MIO_ETHCAM_RX/gpio_info.o)
	mv $(notdir DemoKit_Data/Release/ObjectLds/MODELEXT_MIO_ETHCAM_RX/gpio_info.o) DemoKit_Data/Release/ObjectLds/MODELEXT_MIO_ETHCAM_RX/gpio_info.o;
	@echo Extracting: DemoKit_Data/Release/ObjectLds/MODELEXT_MIO_ETHCAM_RX/dram_partition_info.o
	$(AR) x $@ $(notdir DemoKit_Data/Release/ObjectLds/MODELEXT_MIO_ETHCAM_RX/dram_partition_info.o)
	$(AR) d $@ $(notdir DemoKit_Data/Release/ObjectLds/MODELEXT_MIO_ETHCAM_RX/dram_partition_info.o)
	mv $(notdir DemoKit_Data/Release/ObjectLds/MODELEXT_MIO_ETHCAM_RX/dram_partition_info.o) DemoKit_Data/Release/ObjectLds/MODELEXT_MIO_ETHCAM_RX/dram_partition_info.o;
	@echo Extracting: DemoKit_Data/Release/ObjectLds/MODELEXT_MIO_ETHCAM_RX/model_cfg.o
	$(AR) x $@ $(notdir DemoKit_Data/Release/ObjectLds/MODELEXT_MIO_ETHCAM_RX/model_cfg.o)
	$(AR) d $@ $(notdir DemoKit_Data/Release/ObjectLds/MODELEXT_MIO_ETHCAM_RX/model_cfg.o)
	mv $(notdir DemoKit_Data/Release/ObjectLds/MODELEXT_MIO_ETHCAM_RX/model_cfg.o) DemoKit_Data/Release/ObjectLds/MODELEXT_MIO_ETHCAM_RX/model_cfg.o;

DemoKit_Data/Release/ObjectLds/Driver_Sys/Driver_Sys.a:../../ARC_eCos/Drv/Release/Driver_Sys.a
	@echo Copying ObjectLds:  $(notdir $@)
	@rm -rf $(dir $@)
	@mkdir -p $(dir $@)
	cp $< $@
	chmod 777 $@
	@echo Extracting: DemoKit_Data/Release/ObjectLds/Driver_Sys/clock_power_down.o
	$(AR) x $@ $(notdir DemoKit_Data/Release/ObjectLds/Driver_Sys/clock_power_down.o)
	$(AR) d $@ $(notdir DemoKit_Data/Release/ObjectLds/Driver_Sys/clock_power_down.o)
	mv $(notdir DemoKit_Data/Release/ObjectLds/Driver_Sys/clock_power_down.o) DemoKit_Data/Release/ObjectLds/Driver_Sys/clock_power_down.o;

