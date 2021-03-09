/**
    DxSensor

    Sensor common interface

    @file       DxSensor.c
    @ingroup    mISYSAlg
    @note       Nothing (or anything need to be mentioned).

    Copyright   Novatek Microelectronics Corp. 2011.  All rights reserved.
*/

#include "Delay.h"
// sensor
#include "DxSensor.h"
#include "sensor_if.h"
#include "sensor_drv.h"
#include "IOCfg.h"
// Driver_Sys
#include "lvds.h"
#include "sif.h"

#define __MODULE__          DxDrv
#define __DBGLVL__          2 // 0=FATAL, 1=ERR, 2=WRN, 3=UNIT, 4=FUNC, 5=IND, 6=MSG, 7=VALUE, 8=USER
#define __DBGFLT__          "*" // *=All, [mark]=CustomClass
#include "DebugModule.h"

typedef enum {
	POW_CTRL_TURN_ON = 0,
	POW_CTRL_TURN_OFF,
	POW_CTRL_SAVE_ON,
	POW_CTRL_SAVE_OFF,
} POW_CTRL_FLAG;

typedef SENSOR_DRV_TAB *(*SEN_GET_DRV)(void);
typedef SENSOR_INIT_OBJ(*SEN_GET_INI_OBJ)(SENSOR_ID id);
typedef BOOL (*SEN_PLUG_IN)(SENSOR_ID id);
typedef void (*SEN_POWER_CTRL)(SENSOR_ID id, POW_CTRL_FLAG flag);

typedef struct {
	CHAR *name;
	SEN_GET_DRV drv_tab;
	SEN_GET_INI_OBJ drv_obj;
	SEN_POWER_CTRL pow_ctrl;
	SEN_PLUG_IN det_plug_in;
	SEN_PLUG_IN plug_in_cb;
} SEN_MAP_INFO;

#if 1
static CHAR sensor_name[SENSOR_ID_MAX_NUM][30] = {
	_SENSOR1_,
	_SENSOR2_,
};
#else
static CHAR sensor_name[SENSOR_ID_MAX_NUM][30] = {
	{"CMOS_GC2053P"},
	{"CMOS_PR2020K"},
};
#endif

#if (defined(_CMOS_AR0237L_))
static SENSOR_INIT_OBJ sensor_get_obj_ar0237l(SENSOR_ID id)
{
	// set sensor command interface info (Vx1/SIF/I2C/IO/...)
	SENSOR_INIT_OBJ init_obj = {0};
	init_obj.cmd_info.cmd_type = SENSOR_CMD_I2C;
	init_obj.cmd_info.info.i2c.i2c = SENSOR_I2C2_1ST;
	init_obj.cmd_info.info.i2c.w_addr_sel = SEN_I2C_W_ADDR_DFT;
	init_obj.sen_map_if = SENSOR_MAP_IF_DEFAULT;

	// set mclk change enable to notify sensor driver
	init_obj.sie_mclk_info.mclk_src = SEN_SIEMCLK_SRC_DFT;
	init_obj.sie_mclk_info.chg_mclk_en = ENABLE;

	// set sensor interface pin map (LVDS/CSI/...)
	init_obj.sen_2_serial_pin_map[0] = 0;
	init_obj.sen_2_serial_pin_map[1] = 1;
	init_obj.sen_2_serial_pin_map[2] = 2;
	init_obj.sen_2_serial_pin_map[3] = 3;
	return init_obj;
}
static void sen_power_ctrl_ar0237l(SENSOR_ID id, POW_CTRL_FLAG flag)
{
	switch (flag) {
	case POW_CTRL_TURN_ON:
		// set clock information
		sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK, ENABLE);

		// set power on sequence
		// reset pin
		UINT32 reset_pin = S_GPIO_4;
		gpio_setDir(reset_pin, GPIO_DIR_OUTPUT);
		gpio_clearPin(reset_pin);
		Delay_DelayMs(30);
		gpio_setPin(reset_pin);
		Delay_DelayMs(1);

		break;

	case POW_CTRL_TURN_OFF:
		// set power off sequence
		sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK, DISABLE);
		break;

	case POW_CTRL_SAVE_ON:
		break;

	case POW_CTRL_SAVE_OFF:
		break;

	default:
		DBG_ERR("SenPowerCtrl fail (%d)\r\n", flag);
		break;
	}
}
#endif

#if (defined(_CMOS_IMX291M_))
#if 0
static void csi_hw_rst_imx291cqc(SENSOR_ID sensor_id)
{
    debug_msg("~G csi_hw_rst_imx291cqc \r\n");
	gpio_setDir(S_GPIO_4, GPIO_DIR_OUTPUT);
	gpio_clearPin(S_GPIO_4);
	Delay_DelayMs(1);
	gpio_setPin(S_GPIO_4);
	Delay_DelayUs(10);
}
#endif

static SENSOR_INIT_OBJ sensor_get_obj_imx291m(SENSOR_ID id)
{
	// set sensor command interface info (Vx1/SIF/I2C/IO/...)
	SENSOR_INIT_OBJ init_obj = {0};
	init_obj.cmd_info.cmd_type = SENSOR_CMD_I2C;
	init_obj.cmd_info.info.i2c.i2c = SENSOR_I2C1_1ST;
	init_obj.cmd_info.info.i2c.w_addr_sel = SEN_I2C_W_ADDR_DFT;
    //init_obj.interface_info.csi.fp_hw_rst = csi_hw_rst_imx291cqc;
	init_obj.sen_map_if = SENSOR_MAP_IF_DEFAULT;

	// set mclk change enable to notify sensor driver
	init_obj.sie_mclk_info.mclk_src = SEN_SIEMCLK_SRC_DFT;
	init_obj.sie_mclk_info.chg_mclk_en = ENABLE;

	// set sensor interface pin map (LVDS/CSI/...)
	init_obj.sen_2_serial_pin_map[0] = 0;
	init_obj.sen_2_serial_pin_map[1] = 1;
	init_obj.sen_2_serial_pin_map[2] = 2;
	init_obj.sen_2_serial_pin_map[3] = 3;
	return init_obj;
}
static void sen_power_ctrl_imx291m(SENSOR_ID id, POW_CTRL_FLAG flag)
{
	switch (flag) {
	case POW_CTRL_TURN_ON:
		// set clock information
		sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK, ENABLE);

		// set power on sequence
		gpio_setDir(GPIO_SENSOR_RESET, GPIO_DIR_OUTPUT);
		gpio_clearPin(GPIO_SENSOR_RESET);
		Delay_DelayMs(1);
		gpio_setPin(GPIO_SENSOR_RESET);
		Delay_DelayMs(1);
		break;

	case POW_CTRL_TURN_OFF:
		// set power off sequence
		sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK, DISABLE);

		// set power off sequence
		gpio_setDir(GPIO_SENSOR_RESET, GPIO_DIR_OUTPUT);
		gpio_clearPin(GPIO_SENSOR_RESET);
		Delay_DelayMs(1);
		break;

	case POW_CTRL_SAVE_ON:
		break;

	case POW_CTRL_SAVE_OFF:
		break;

	default:
		DBG_ERR("SenPowerCtrl fail (%d)\r\n", flag);
		break;
	}
}
#endif

#if (defined(_CMOS_GC2023M_))
static SENSOR_INIT_OBJ sensor_get_obj_gc2023m(SENSOR_ID id)
{
	// set sensor command interface info (Vx1/SIF/I2C/IO/...)
	SENSOR_INIT_OBJ init_obj = {0};
	init_obj.cmd_info.cmd_type = SENSOR_CMD_I2C;
	init_obj.cmd_info.info.i2c.i2c = SENSOR_I2C1_1ST;
	init_obj.cmd_info.info.i2c.w_addr_sel = SEN_I2C_W_ADDR_DFT;
    //init_obj.interface_info.csi.fp_hw_rst = csi_hw_rst_imx291cqc;
	init_obj.sen_map_if = SENSOR_MAP_IF_DEFAULT;

	// set mclk change enable to notify sensor driver
	init_obj.sie_mclk_info.mclk_src = SEN_SIEMCLK_SRC_DFT;
	init_obj.sie_mclk_info.chg_mclk_en = ENABLE;

	// set sensor interface pin map (LVDS/CSI/...)
	init_obj.sen_2_serial_pin_map[0] = 0;
	init_obj.sen_2_serial_pin_map[1] = 1;
	init_obj.sen_2_serial_pin_map[2] = 2;
	init_obj.sen_2_serial_pin_map[3] = 3;
	return init_obj;
}
static void sen_power_ctrl_gc2023m(SENSOR_ID id, POW_CTRL_FLAG flag)
{
	switch (flag) {
	case POW_CTRL_TURN_ON:
		CHKPNT;
		// set clock information
		gpio_setPin(GPIO_SENSOR_PWR1_8);	//SN2.8
		Delay_DelayMs(10);
		gpio_setPin(GPIO_SENSOR_PWR1_2);	//SN2.8
		Delay_DelayMs(1);
		gpio_setPin(GPIO_SENSOR_PWR2_8);	//SN1.8
		Delay_DelayMs(1);

		// set power on sequence
		gpio_setDir(GPIO_SENSOR_RESET, GPIO_DIR_OUTPUT);

		gpio_setPin(GPIO_SENSOR_RESET);

		Delay_DelayMs(10);
		gpio_clearPin(GPIO_SENSOR_RESET);
		Delay_DelayMs(10);
		gpio_setPin(GPIO_SENSOR_RESET);
		Delay_DelayMs(2);
		sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK, ENABLE);
		break;

	case POW_CTRL_TURN_OFF:

		gpio_setDir(GPIO_SENSOR_RESET, GPIO_DIR_OUTPUT);
		gpio_clearPin(GPIO_SENSOR_RESET);
		Delay_DelayMs(2);
		// set power off sequence
		sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK, DISABLE);
		Delay_DelayMs(1);
		gpio_clearPin(GPIO_SENSOR_PWR2_8);	//SN2.8
		Delay_DelayMs(1);
		gpio_clearPin(GPIO_SENSOR_PWR1_2);	//SN2.8
		Delay_DelayMs(1);
		gpio_clearPin(GPIO_SENSOR_PWR1_8);	//SN1.8
		Delay_DelayMs(1);

		// set power off sequence
		break;

	case POW_CTRL_SAVE_ON:
		break;

	case POW_CTRL_SAVE_OFF:
		break;

	default:
		DBG_ERR("SenPowerCtrl fail (%d)\r\n", flag);
		break;
	}
}
#endif

#if (defined(_CMOS_GC2053M_))
static SENSOR_INIT_OBJ sensor_get_obj_gc2053m(SENSOR_ID id)
{

	// set sensor command interface info (Vx1/SIF/I2C/IO/...)
	SENSOR_INIT_OBJ init_obj = {0};
	init_obj.cmd_info.cmd_type = SENSOR_CMD_I2C;
	init_obj.cmd_info.info.i2c.i2c = SENSOR_I2C1_1ST;
	init_obj.cmd_info.info.i2c.w_addr_sel = SEN_I2C_W_ADDR_DFT;
    //init_obj.interface_info.csi.fp_hw_rst = csi_hw_rst_imx291cqc;
	init_obj.sen_map_if = SENSOR_MAP_IF_DEFAULT;

	// set mclk change enable to notify sensor driver
	init_obj.sie_mclk_info.mclk_src = SEN_SIEMCLK_SRC_DFT;
	init_obj.sie_mclk_info.chg_mclk_en = ENABLE;

	// set sensor interface pin map (LVDS/CSI/...)
	init_obj.sen_2_serial_pin_map[0] = 0;
	init_obj.sen_2_serial_pin_map[1] = 1;
	init_obj.sen_2_serial_pin_map[2] = 2;
	init_obj.sen_2_serial_pin_map[3] = 3;

	debug_msg("mmmmm sensor_get_obj_gc2053m");

	return init_obj;

}
static void sen_power_ctrl_gc2053m(SENSOR_ID id, POW_CTRL_FLAG flag)
{CHKPNT;
	switch (flag) {
	case POW_CTRL_TURN_ON:
	debug_msg("mmmmm sen_power_ctrl_gc2053m");
		// set clock information
		// set power on sequence
		gpio_setDir(GPIO_SENSOR_RESET, GPIO_DIR_OUTPUT);

        	gpio_clearPin(GPIO_SENSOR_RESET);


		gpio_setPin(GPIO_SENSOR_PWR1_8);	//SN2.8
		Delay_DelayMs(2);
		gpio_setPin(GPIO_SENSOR_PWR1_2);	//SN2.8
		Delay_DelayMs(2);
		gpio_setPin(GPIO_SENSOR_PWR2_8);	//SN1.8
		Delay_DelayMs(2);

		gpio_setPin(GPIO_SENSOR_RESET);
		Delay_DelayMs(2);
             sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK, ENABLE);


		break;

	case POW_CTRL_TURN_OFF:
		/*
		gpio_setDir(GPIO_SENSOR_RESET, GPIO_DIR_OUTPUT);
		gpio_clearPin(GPIO_SENSOR_RESET);
		Delay_DelayMs(2);
		// set power off sequence
		Delay_DelayMs(1);
		gpio_clearPin(GPIO_SENSOR_PWR2_8);	//SN2.8
		Delay_DelayMs(1);
		gpio_clearPin(GPIO_SENSOR_PWR1_2);	//SN2.8
		Delay_DelayMs(1);
		gpio_clearPin(GPIO_SENSOR_PWR1_8);	//SN1.8
		Delay_DelayMs(1);*/
		sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK, DISABLE);

		// set power off sequence
		break;

	case POW_CTRL_SAVE_ON:
		break;

	case POW_CTRL_SAVE_OFF:
		break;

	default:
		DBG_ERR("SenPowerCtrl fail (%d)\r\n", flag);
		break;
	}
}
#endif


#if (defined(_CMOS_GC4653M_))
static SENSOR_INIT_OBJ sensor_get_obj_gc4653m(SENSOR_ID id)
{CHKPNT;
	// set sensor command interface info (Vx1/SIF/I2C/IO/...)
	SENSOR_INIT_OBJ init_obj = {0};
	init_obj.cmd_info.cmd_type = SENSOR_CMD_I2C;
	init_obj.cmd_info.info.i2c.i2c = SENSOR_I2C1_1ST;
	init_obj.cmd_info.info.i2c.w_addr_sel = SEN_I2C_W_ADDR_DFT;
    //init_obj.interface_info.csi.fp_hw_rst = csi_hw_rst_imx291cqc;
	init_obj.sen_map_if = SENSOR_MAP_IF_DEFAULT;

	// set mclk change enable to notify sensor driver
	init_obj.sie_mclk_info.mclk_src = SEN_SIEMCLK_SRC_DFT;
	init_obj.sie_mclk_info.chg_mclk_en = ENABLE;

	// set sensor interface pin map (LVDS/CSI/...)
	init_obj.sen_2_serial_pin_map[0] = 0;
	init_obj.sen_2_serial_pin_map[1] = 1;
	init_obj.sen_2_serial_pin_map[2] = 2;
	init_obj.sen_2_serial_pin_map[3] = 3;
	return init_obj;
}
static void sen_power_ctrl_gc4653m(SENSOR_ID id, POW_CTRL_FLAG flag)
{CHKPNT;
	switch (flag) {
	case POW_CTRL_TURN_ON:
		CHKPNT;
		sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK, ENABLE);
		// set clock information
		gpio_setPin(GPIO_SENSOR_PWR1_8);	//SN2.8
		Delay_DelayMs(10);
		gpio_setPin(GPIO_SENSOR_PWR1_2);	//SN2.8
		Delay_DelayMs(1);
		gpio_setPin(GPIO_SENSOR_PWR2_8);	//SN1.8
		Delay_DelayMs(1);

		// set power on sequence
		gpio_setDir(GPIO_SENSOR_RESET, GPIO_DIR_OUTPUT);

		gpio_setPin(GPIO_SENSOR_RESET);

		Delay_DelayMs(10);
		gpio_clearPin(GPIO_SENSOR_RESET);
		Delay_DelayMs(10);
		gpio_setPin(GPIO_SENSOR_RESET);
		Delay_DelayMs(2);
		break;

	case POW_CTRL_TURN_OFF:
		CHKPNT;
		gpio_setDir(GPIO_SENSOR_RESET, GPIO_DIR_OUTPUT);
		gpio_clearPin(GPIO_SENSOR_RESET);
		Delay_DelayMs(2);
		// set power off sequence
		Delay_DelayMs(1);
		gpio_clearPin(GPIO_SENSOR_PWR2_8);	//SN2.8
		Delay_DelayMs(1);
		gpio_clearPin(GPIO_SENSOR_PWR1_2);	//SN2.8
		Delay_DelayMs(1);
		gpio_clearPin(GPIO_SENSOR_PWR1_8);	//SN1.8
		Delay_DelayMs(1);
		sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK, DISABLE);

		// set power off sequence
		break;

	case POW_CTRL_SAVE_ON:
		break;

	case POW_CTRL_SAVE_OFF:
		break;

	default:
		DBG_ERR("SenPowerCtrl fail (%d)\r\n", flag);
		break;
	}
}
#endif


#if (defined(_CMOS_GC2053P_))

static SENSOR_INIT_OBJ sensor_get_obj_gc2053p(SENSOR_ID id)
{CHKPNT;
	// set sensor command interface info (Vx1/SIF/I2C/IO/...)
	SENSOR_INIT_OBJ init_obj = {0};
	init_obj.cmd_info.cmd_type = SENSOR_CMD_I2C;
	init_obj.cmd_info.info.i2c.i2c = SENSOR_I2C1_1ST;
	init_obj.cmd_info.info.i2c.w_addr_sel = SEN_I2C_W_ADDR_DFT;
    //init_obj.interface_info.csi.fp_hw_rst = csi_hw_rst_imx291cqc;
	init_obj.sen_map_if = SENSOR_MAP_IF_DEFAULT;

	// set mclk change enable to notify sensor driver
	init_obj.sie_mclk_info.mclk_src = SEN_SIEMCLK_SRC_DFT;
	init_obj.sie_mclk_info.chg_mclk_en = ENABLE;

	// set sensor interface pin map (LVDS/CSI/...)
	init_obj.sen_2_serial_pin_map[0] = 0;
	init_obj.sen_2_serial_pin_map[1] = 1;
	init_obj.sen_2_serial_pin_map[2] = 2;
	init_obj.sen_2_serial_pin_map[3] = 3;
	return init_obj;
}
static void sen_power_ctrl_gc2053p(SENSOR_ID id, POW_CTRL_FLAG flag)
{
	switch (flag) {
	case POW_CTRL_TURN_ON:
		debug_msg("Liwk -------- Sensor Power On gc2053p!!!\r\n");
		gpio_setDir(GPIO_SENSOR_RESET, GPIO_DIR_OUTPUT);
        gpio_clearPin(GPIO_SENSOR_RESET);

		// set clock information
		gpio_setPin(GPIO_SENSOR_PWR1_8);	//SN2.8
		Delay_DelayMs(10);
		gpio_setPin(GPIO_SENSOR_PWR1_2);	//SN2.8
		Delay_DelayMs(1);
		gpio_setPin(GPIO_SENSOR_PWR2_8);	//SN1.8
		Delay_DelayMs(1);

		sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK, ENABLE);

		// set power on sequence
		Delay_DelayMs(2);
		gpio_setPin(GPIO_SENSOR_RESET);
		Delay_DelayMs(2);
		break;

	case POW_CTRL_TURN_OFF:
		debug_msg("Liwk -------- Sensor Power Off gc2053p!!!\r\n");
		gpio_setDir(GPIO_SENSOR_RESET, GPIO_DIR_OUTPUT);
		gpio_clearPin(GPIO_SENSOR_RESET);
		Delay_DelayMs(1);
		// set power off sequence
		sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK, DISABLE);
		Delay_DelayMs(1);
		gpio_clearPin(GPIO_SENSOR_PWR2_8);	//SN2.8
		Delay_DelayMs(1);
		// set clock information
		gpio_clearPin(GPIO_SENSOR_PWR1_2);	//SN2.8
		Delay_DelayMs(1);
		gpio_clearPin(GPIO_SENSOR_PWR1_8);	//SN1.8
		Delay_DelayMs(1);
		break;

	case POW_CTRL_SAVE_ON:
		break;

	case POW_CTRL_SAVE_OFF:
		break;

	default:
		DBG_ERR("SenPowerCtrl fail (%d)\r\n", flag);
		break;
	}
}
#endif


#if (defined(_CMOS_GC2023P_))

static SENSOR_INIT_OBJ sensor_get_obj_gc2023p(SENSOR_ID id)
{
	// set sensor command interface info (Vx1/SIF/I2C/IO/...)
	SENSOR_INIT_OBJ init_obj = {0};
	init_obj.cmd_info.cmd_type = SENSOR_CMD_I2C;
	init_obj.cmd_info.info.i2c.i2c = SENSOR_I2C1_1ST;
	init_obj.cmd_info.info.i2c.w_addr_sel = SEN_I2C_W_ADDR_DFT;
    //init_obj.interface_info.csi.fp_hw_rst = csi_hw_rst_imx291cqc;
	init_obj.sen_map_if = SENSOR_MAP_IF_DEFAULT;

	// set mclk change enable to notify sensor driver
	init_obj.sie_mclk_info.mclk_src = SEN_SIEMCLK_SRC_DFT;
	init_obj.sie_mclk_info.chg_mclk_en = ENABLE;

	// set sensor interface pin map (LVDS/CSI/...)
	init_obj.sen_2_serial_pin_map[0] = 0;
	init_obj.sen_2_serial_pin_map[1] = 1;
	init_obj.sen_2_serial_pin_map[2] = 2;
	init_obj.sen_2_serial_pin_map[3] = 3;
	return init_obj;
}
static void sen_power_ctrl_gc2023p(SENSOR_ID id, POW_CTRL_FLAG flag)
{
	switch (flag) {
	case POW_CTRL_TURN_ON:
		debug_msg("Liwk -------- Sensor Power On!!!\r\n");
		gpio_setDir(GPIO_SENSOR_RESET, GPIO_DIR_OUTPUT);
		gpio_clearPin(GPIO_SENSOR_RESET);

		gpio_setPin(GPIO_SENSOR_PWR1_2);	//SN2.8
		Delay_DelayMs(1);
		// set clock information
		gpio_setPin(GPIO_SENSOR_PWR1_8);	//SN2.8
		Delay_DelayMs(1);
		gpio_setPin(GPIO_SENSOR_PWR2_8);	//SN1.8
		Delay_DelayMs(1);

		sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK, ENABLE);

		Delay_DelayMs(1);
		gpio_setPin(GPIO_SENSOR_RESET);
		Delay_DelayMs(10);
		break;

	case POW_CTRL_TURN_OFF:
		debug_msg("Liwk -------- Sensor Power Off!!!\r\n");
		gpio_setDir(GPIO_SENSOR_RESET, GPIO_DIR_OUTPUT);
		gpio_clearPin(GPIO_SENSOR_RESET);
		Delay_DelayMs(1);
		// set power off sequence
		sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK, DISABLE);
		Delay_DelayMs(1);

		gpio_clearPin(GPIO_SENSOR_PWR2_8);	//SN2.8
		Delay_DelayMs(1);
		// set clock information
		gpio_clearPin(GPIO_SENSOR_PWR1_8);	//SN2.8
		Delay_DelayMs(1);
		gpio_clearPin(GPIO_SENSOR_PWR1_2);	//SN1.8
		Delay_DelayMs(1);

		// set power off sequence
		break;

	case POW_CTRL_SAVE_ON:
		break;

	case POW_CTRL_SAVE_OFF:
		break;

	default:
		DBG_ERR("SenPowerCtrl fail (%d)\r\n", flag);
		break;
	}
}
#endif


#if (defined(_CMOS_IMX290M_))
#if 0
static void csi_hw_rst_imx290(SENSOR_ID sensor_id)
{
    debug_msg("~G csi_hw_rst_imx290 \r\n");
	gpio_setDir(S_GPIO_4, GPIO_DIR_OUTPUT);
	gpio_clearPin(S_GPIO_4);
	Delay_DelayMs(1);
	gpio_setPin(S_GPIO_4);
	Delay_DelayUs(10);
}
#endif

static SENSOR_INIT_OBJ sensor_get_obj_imx290m(SENSOR_ID id)
{
	// set sensor command interface info (Vx1/SIF/I2C/IO/...)
	SENSOR_INIT_OBJ init_obj = {0};
	init_obj.cmd_info.cmd_type = SENSOR_CMD_I2C;
	init_obj.cmd_info.info.i2c.i2c = SENSOR_I2C2_1ST;
	init_obj.cmd_info.info.i2c.w_addr_sel = SEN_I2C_W_ADDR_DFT;
    //init_obj.interface_info.csi.fp_hw_rst = csi_hw_rst_imx290;
	init_obj.sen_map_if = SENSOR_MAP_IF_DEFAULT;

	// set mclk change enable to notify sensor driver
	init_obj.sie_mclk_info.mclk_src = SEN_SIEMCLK_SRC_DFT;
	init_obj.sie_mclk_info.chg_mclk_en = ENABLE;

	// set sensor interface pin map (LVDS/CSI/...)
	init_obj.sen_2_serial_pin_map[0] = 0;
	init_obj.sen_2_serial_pin_map[1] = 1;
	init_obj.sen_2_serial_pin_map[2] = 2;
	init_obj.sen_2_serial_pin_map[3] = 3;
	return init_obj;
}
static void sen_power_ctrl_imx290m(SENSOR_ID id, POW_CTRL_FLAG flag)
{
	UINT32 reset_pin = S_GPIO_4;
	switch (flag) {
	case POW_CTRL_TURN_ON:
		// set clock information
		sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK, ENABLE);

		// set power on sequence
		gpio_setDir(reset_pin, GPIO_DIR_OUTPUT);
		gpio_clearPin(reset_pin);
		Delay_DelayMs(1);
		gpio_setPin(reset_pin);
		Delay_DelayMs(1);

		break;

	case POW_CTRL_TURN_OFF:
		// set power off sequence
		sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK, DISABLE);

		// set power off sequence
		gpio_setDir(reset_pin, GPIO_DIR_OUTPUT);
		gpio_clearPin(reset_pin);
		Delay_DelayMs(1);

		break;

	case POW_CTRL_SAVE_ON:
		break;

	case POW_CTRL_SAVE_OFF:
		break;

	default:
		DBG_ERR("SenPowerCtrl fail (%d)\r\n", flag);
		break;
	}
}
#endif

#if (defined(_CMOS_IMX307M_))
#if 0
static void csi_hw_rst_imx307cqc(SENSOR_ID sensor_id)
{
    debug_msg("~G csi_hw_rst_imx307cqc \r\n");
	gpio_setDir(S_GPIO_4, GPIO_DIR_OUTPUT);
	gpio_clearPin(S_GPIO_4);
	Delay_DelayMs(1);
	gpio_setPin(S_GPIO_4);
	Delay_DelayUs(10);
}
#endif

static SENSOR_INIT_OBJ sensor_get_obj_imx307m(SENSOR_ID id)
{
	// set sensor command interface info (Vx1/SIF/I2C/IO/...)
	SENSOR_INIT_OBJ init_obj = {0};
	init_obj.cmd_info.cmd_type = SENSOR_CMD_I2C;
	init_obj.cmd_info.info.i2c.i2c = SENSOR_I2C1_1ST;
	init_obj.cmd_info.info.i2c.w_addr_sel = SEN_I2C_W_ADDR_DFT;
    //init_obj.interface_info.csi.fp_hw_rst = csi_hw_rst_imx307cqc;
	init_obj.sen_map_if = SENSOR_MAP_IF_DEFAULT;
    init_obj.option = SENSOR_OPTION_MIRROR|SENSOR_OPTION_FLIP;
	// set mclk change enable to notify sensor driver
	init_obj.sie_mclk_info.mclk_src = SEN_SIEMCLK_SRC_DFT;
	init_obj.sie_mclk_info.chg_mclk_en = ENABLE;

	// set sensor interface pin map (LVDS/CSI/...)
	init_obj.sen_2_serial_pin_map[0] = 0;
	init_obj.sen_2_serial_pin_map[1] = 1;
	init_obj.sen_2_serial_pin_map[2] = 2;
	init_obj.sen_2_serial_pin_map[3] = 3;
	return init_obj;
}
static void sen_power_ctrl_imx307m(SENSOR_ID id, POW_CTRL_FLAG flag)
{
	switch (flag) {
	case POW_CTRL_TURN_ON:
		// set clock information
		CHKPNT;
		gpio_setPin(GPIO_SENSOR_PWR1_8);	//SN2.8
		Delay_DelayMs(10);
		gpio_setPin(GPIO_SENSOR_PWR1_2);	//SN2.8
		Delay_DelayMs(1);
		gpio_setPin(GPIO_SENSOR_PWR2_8);	//SN1.8
		Delay_DelayMs(1);

		// set power on sequence
		gpio_setDir(GPIO_SENSOR_RESET, GPIO_DIR_OUTPUT);

		gpio_setPin(GPIO_SENSOR_RESET);

		Delay_DelayMs(10);
		gpio_clearPin(GPIO_SENSOR_RESET);
		Delay_DelayMs(10);
		gpio_setPin(GPIO_SENSOR_RESET);
		Delay_DelayMs(2);
		sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK, ENABLE);
		break;

	case POW_CTRL_TURN_OFF:
		// set power off sequence
		sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK, DISABLE);

		// set power off sequence
		gpio_setDir(GPIO_SENSOR_RESET, GPIO_DIR_OUTPUT);
		gpio_clearPin(GPIO_SENSOR_RESET);
		Delay_DelayMs(1);
		break;

	case POW_CTRL_SAVE_ON:
		break;

	case POW_CTRL_SAVE_OFF:
		break;

	default:
		DBG_ERR("SenPowerCtrl fail (%d)\r\n", flag);
		break;
	}
}
#endif

#if (defined(_CMOS_IMX335M_))
static SENSOR_INIT_OBJ sensor_get_obj_imx335m(SENSOR_ID id)
{
    // set sensor command interface info (Vx1/SIF/I2C/IO/...)
    SENSOR_INIT_OBJ init_obj = {0};
    init_obj.cmd_info.cmd_type = SENSOR_CMD_I2C;
    init_obj.cmd_info.info.i2c.i2c = SENSOR_I2C1_1ST;
    init_obj.cmd_info.info.i2c.w_addr_sel = SEN_I2C_W_ADDR_DFT;
    init_obj.sen_map_if = SENSOR_MAP_IF_DEFAULT;
    init_obj.option= SENSOR_OPTION_NONE;

    // set mclk change enable to notify sensor driver
    // init_obj.chg_mclk_en = ENABLE;
    init_obj.sie_mclk_info.mclk_src = SEN_SIEMCLK_SRC_DFT;
    init_obj.sie_mclk_info.chg_mclk_en = ENABLE;

    // set sensor interface pin map (LVDS/CSI/...)
    init_obj.sen_2_serial_pin_map[0] = 0;
    init_obj.sen_2_serial_pin_map[1] = 1;
    init_obj.sen_2_serial_pin_map[2] = 2;
    init_obj.sen_2_serial_pin_map[3] = 3;
    return init_obj;
}
static void sen_power_ctrl_imx335m(SENSOR_ID id, POW_CTRL_FLAG flag)
{
    UINT32 reset_pin = S_GPIO_9;
    switch (flag) {
    case POW_CTRL_TURN_ON:
        // set power on sequence
        gpio_setDir(reset_pin, GPIO_DIR_OUTPUT);
        gpio_setPin(reset_pin);
        Delay_DelayMs(1);
        gpio_clearPin(reset_pin);
        Delay_DelayMs(1);
        gpio_setPin(reset_pin);
        Delay_DelayMs(1);

        // set clock information
        sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK, ENABLE);
        break;

    case POW_CTRL_TURN_OFF:
        gpio_clearPin(reset_pin);
        Delay_DelayMs(1);
        // set power off sequence
        sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK, DISABLE);
        break;

    case POW_CTRL_SAVE_ON:
        break;

    case POW_CTRL_SAVE_OFF:
        break;

    default:
        DBG_ERR("SenPowerCtrl fail (%d)\r\n", flag);
        break;
    }
}
#endif

#if (defined(_CMOS_OV2715M_))
static SENSOR_INIT_OBJ sensor_get_obj_ov2715m(SENSOR_ID id)
{
	// set sensor command interface info (Vx1/SIF/I2C/IO/...)
	SENSOR_INIT_OBJ init_obj = {0};
	init_obj.cmd_info.cmd_type = SENSOR_CMD_I2C;

	//init_obj.cmd_info.info.i2c.i2c = SENSOR_I2C2_1ST;
	switch (id) {
	case SENSOR_ID_1:
    default:
        init_obj.cmd_info.info.i2c.i2c = SENSOR_I2C2_1ST;
        break;

	case SENSOR_ID_2:
		init_obj.cmd_info.info.i2c.i2c = SENSOR_I2C1_1ST;
		break;
    }

    init_obj.cmd_info.info.i2c.w_addr_sel = SEN_I2C_W_ADDR_DFT;
	init_obj.sen_map_if = SENSOR_MAP_IF_DEFAULT;
	init_obj.option= SENSOR_OPTION_FLIP;

	// set mclk change enable to notify sensor driver
	// init_obj.chg_mclk_en = ENABLE;
	init_obj.sie_mclk_info.mclk_src = SEN_SIEMCLK_SRC_DFT;
	init_obj.sie_mclk_info.chg_mclk_en = ENABLE;

	// set sensor interface pin map (LVDS/CSI/...)
	//init_obj.sen_2_serial_pin_map[0] = 0;
	init_obj.sen_2_serial_pin_map[0] = SEN_IGNORE;
	init_obj.sen_2_serial_pin_map[1] = SEN_IGNORE;
	init_obj.sen_2_serial_pin_map[2] = SEN_IGNORE;
	init_obj.sen_2_serial_pin_map[3] = SEN_IGNORE;

	switch (id) {
	case SENSOR_ID_1:
    default:
        init_obj.sen_2_serial_pin_map[0] = 0;
        break;

	case SENSOR_ID_2:
		init_obj.sen_2_serial_pin_map[2] = 0;
		break;
    }
	return init_obj;
}
static void sen_power_ctrl_ov2715m(SENSOR_ID id, POW_CTRL_FLAG flag)
{
    UINT32 reset_pin = S_GPIO_4;

	switch (flag) {
	case POW_CTRL_TURN_ON:
		// set clock information
		sensor_if_get_pll_ctrl_obj()->set_clk_en(id,SEN_CLK_SIEMCLK, ENABLE);

        switch (id) {
        case SENSOR_ID_1:
            default:
            reset_pin = S_GPIO_4;
            break;

        case SENSOR_ID_2:
            reset_pin = S_GPIO_4;
            break;
        }

        //Power on sequence
		gpio_setDir(reset_pin, GPIO_DIR_OUTPUT);
		gpio_clearPin(reset_pin);
		Delay_DelayMs(1);
		gpio_setPin(reset_pin);
		Delay_DelayMs(1);

		break;

	case POW_CTRL_TURN_OFF:
		// set power off sequence
		sensor_if_get_pll_ctrl_obj()->set_clk_en(id,SEN_CLK_SIEMCLK, DISABLE);
		break;

	case POW_CTRL_SAVE_ON:
		break;

	case POW_CTRL_SAVE_OFF:
		break;

	default:
		DBG_ERR("SenPowerCtrl fail (%d)\r\n", flag);
		break;
	}
}
#endif

#if (defined(_CMOS_OV2735M_))
static SENSOR_INIT_OBJ sensor_get_obj_ov2735m(SENSOR_ID id)
{
	// set sensor command interface info (Vx1/SIF/I2C/IO/...)
	SENSOR_INIT_OBJ init_obj = {0};
	init_obj.cmd_info.cmd_type = SENSOR_CMD_I2C;

	//init_obj.cmd_info.info.i2c.i2c = SENSOR_I2C2_1ST;
	switch (id) {
	case SENSOR_ID_1:
    default:
        init_obj.cmd_info.info.i2c.i2c = SENSOR_I2C2_1ST;
        break;

	case SENSOR_ID_2:
		init_obj.cmd_info.info.i2c.i2c = SENSOR_I2C1_1ST;
		break;
    }

    init_obj.cmd_info.info.i2c.w_addr_sel = SEN_I2C_W_ADDR_DFT;
	init_obj.sen_map_if = SENSOR_MAP_IF_DEFAULT;
	init_obj.option= SENSOR_OPTION_FLIP | SENSOR_OPTION_MIRROR;

	// set mclk change enable to notify sensor driver
	// init_obj.chg_mclk_en = ENABLE;
	init_obj.sie_mclk_info.mclk_src = SEN_SIEMCLK_SRC_DFT;
	init_obj.sie_mclk_info.chg_mclk_en = ENABLE;

	// set sensor interface pin map (LVDS/CSI/...)
	//init_obj.sen_2_serial_pin_map[0] = 0;
	init_obj.sen_2_serial_pin_map[0] = SEN_IGNORE;
	init_obj.sen_2_serial_pin_map[1] = SEN_IGNORE;
	init_obj.sen_2_serial_pin_map[2] = SEN_IGNORE;
	init_obj.sen_2_serial_pin_map[3] = SEN_IGNORE;

	switch (id) {
	case SENSOR_ID_1:
    default:
        init_obj.sen_2_serial_pin_map[0] = 0;
        break;

	case SENSOR_ID_2:
		init_obj.sen_2_serial_pin_map[2] = 0;
		break;
    }
	return init_obj;
}
static void sen_power_ctrl_ov2735m(SENSOR_ID id, POW_CTRL_FLAG flag)
{
	switch (flag) {
	case POW_CTRL_TURN_ON:
		// set clock information
		sensor_if_get_pll_ctrl_obj()->set_clk_en(id,SEN_CLK_SIEMCLK, ENABLE);

		// set power on sequence
		UINT32 reset_pin = S_GPIO_4;
        switch (id) {
        case SENSOR_ID_1:
            default:
            reset_pin = S_GPIO_4;
            break;

        case SENSOR_ID_2:
            reset_pin = S_GPIO_4;
            break;
        }
		gpio_setDir(reset_pin, GPIO_DIR_OUTPUT);
		gpio_clearPin(reset_pin);
		Delay_DelayMs(1);
		gpio_setPin(reset_pin);
		Delay_DelayMs(1);

		break;

	case POW_CTRL_TURN_OFF:
		// set power off sequence
		sensor_if_get_pll_ctrl_obj()->set_clk_en(id,SEN_CLK_SIEMCLK, DISABLE);
		break;

	case POW_CTRL_SAVE_ON:
		break;

	case POW_CTRL_SAVE_OFF:
		break;

	default:
		DBG_ERR("SenPowerCtrl fail (%d)\r\n", flag);
		break;
	}
}
#endif

#if (defined(_CMOS_OS05A10M_))
static SENSOR_INIT_OBJ sensor_get_obj_os05a10m(SENSOR_ID id)
{
	// set sensor command interface info (Vx1/SIF/I2C/IO/...)
	SENSOR_INIT_OBJ init_obj = {0};
	init_obj.cmd_info.cmd_type = SENSOR_CMD_I2C;
	init_obj.cmd_info.info.i2c.i2c = SENSOR_I2C2_1ST;
	init_obj.cmd_info.info.i2c.w_addr_sel = SEN_I2C_W_ADDR_DFT;
	init_obj.sen_map_if = SENSOR_MAP_IF_DEFAULT;
	init_obj.option= SENSOR_OPTION_FLIP;

	// set mclk change enable to notify sensor driver
	// init_obj.chg_mclk_en = ENABLE;
	init_obj.sie_mclk_info.mclk_src = SEN_SIEMCLK_SRC_DFT;
	init_obj.sie_mclk_info.chg_mclk_en = ENABLE;

	// set sensor interface pin map (LVDS/CSI/...)
	init_obj.sen_2_serial_pin_map[0] = 0;
	init_obj.sen_2_serial_pin_map[1] = 1;
	init_obj.sen_2_serial_pin_map[2] = 2;
	init_obj.sen_2_serial_pin_map[3] = 3;
	return init_obj;
}
static void sen_power_ctrl_os05a10m(SENSOR_ID id, POW_CTRL_FLAG flag)
{
	switch (flag) {
	case POW_CTRL_TURN_ON:
		// set clock information
		sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK, ENABLE);

		// set power on sequence
		UINT32 reset_pin = S_GPIO_4;
		gpio_setDir(reset_pin, GPIO_DIR_OUTPUT);
		gpio_clearPin(reset_pin);
		Delay_DelayMs(1);
		gpio_setPin(reset_pin);
		Delay_DelayMs(1);

		break;

	case POW_CTRL_TURN_OFF:
		// set power off sequence
		sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK, DISABLE);
		break;

	case POW_CTRL_SAVE_ON:
		break;

	case POW_CTRL_SAVE_OFF:
		break;

	default:
		DBG_ERR("SenPowerCtrl fail (%d)\r\n", flag);
		break;
	}
}
#endif

#if (defined(_CMOS_OS08A10M_))
static SENSOR_INIT_OBJ sensor_get_obj_os08a10m(SENSOR_ID id)
{
	// set sensor command interface info (Vx1/SIF/I2C/IO/...)
	SENSOR_INIT_OBJ init_obj = {0};
	init_obj.cmd_info.cmd_type = SENSOR_CMD_I2C;
	init_obj.cmd_info.info.i2c.i2c = SENSOR_I2C2_1ST;
	init_obj.cmd_info.info.i2c.w_addr_sel = SEN_I2C_W_ADDR_DFT;
	init_obj.sen_map_if = SENSOR_MAP_IF_DEFAULT;
	init_obj.option= SENSOR_OPTION_MIRROR;

	// set mclk change enable to notify sensor driver
	// init_obj.chg_mclk_en = ENABLE;
	init_obj.sie_mclk_info.mclk_src = SEN_SIEMCLK_SRC_DFT;
	init_obj.sie_mclk_info.chg_mclk_en = ENABLE;

	// set sensor interface pin map (LVDS/CSI/...)
	init_obj.sen_2_serial_pin_map[0] = 0;
	init_obj.sen_2_serial_pin_map[1] = 1;
	init_obj.sen_2_serial_pin_map[2] = 2;
	init_obj.sen_2_serial_pin_map[3] = 3;
	return init_obj;
}
static void sen_power_ctrl_os08a10m(SENSOR_ID id, POW_CTRL_FLAG flag)
{
	switch (flag) {
	case POW_CTRL_TURN_ON:
		// set clock information
		sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK, ENABLE);
        //DBG_ERR("MCLK Enable id=%d, flag=%d\r\n", id, flag);

		// set power on sequence
		UINT32 reset_pin = S_GPIO_4;
		gpio_setDir(reset_pin, GPIO_DIR_OUTPUT);
		gpio_clearPin(reset_pin);
		Delay_DelayMs(1);
		gpio_setPin(reset_pin);
		Delay_DelayMs(1);

		break;

	case POW_CTRL_TURN_OFF:
		// set power off sequence
		sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK, DISABLE);
		break;

	case POW_CTRL_SAVE_ON:
		break;

	case POW_CTRL_SAVE_OFF:
		break;

	default:
		DBG_ERR("SenPowerCtrl fail (%d)\r\n", flag);
		break;
	}
}
#endif

#if (defined(_CMOS_SC2235P_))
static SENSOR_INIT_OBJ sensor_get_obj_sc2235p(SENSOR_ID id)
{
	// set sensor command interface info (Vx1/SIF/I2C/IO/...)
	SENSOR_INIT_OBJ init_obj = {0};
	init_obj.cmd_info.cmd_type = SENSOR_CMD_I2C;
	init_obj.cmd_info.info.i2c.i2c = SENSOR_I2C1_1ST;//for TP ; EVB SENSOR_I2C2_1ST
	init_obj.cmd_info.info.i2c.w_addr_sel = SEN_I2C_W_ADDR_DFT;
	init_obj.sen_map_if = SENSOR_MAP_IF_DEFAULT;
	init_obj.option= SENSOR_OPTION_NONE;

	// set mclk change enable to notify sensor driver
	// init_obj.chg_mclk_en = ENABLE;
	init_obj.sie_mclk_info.mclk_src = SEN_SIEMCLK_SRC_DFT;
	init_obj.sie_mclk_info.chg_mclk_en = ENABLE;

	// set sensor interface pin map (LVDS/CSI/...)
	init_obj.sen_2_serial_pin_map[0] = 0;
	init_obj.sen_2_serial_pin_map[1] = 1;
	init_obj.sen_2_serial_pin_map[2] = 2;
	init_obj.sen_2_serial_pin_map[3] = 3;
	return init_obj;
}
static void sen_power_ctrl_sc2235p(SENSOR_ID id, POW_CTRL_FLAG flag)
{
	switch (flag) {
	case POW_CTRL_TURN_ON:
		// set clock information
		sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK, ENABLE);
        //DBG_ERR("MCLK Enable id=%d, flag=%d\r\n", id, flag);

		// set power on sequence
		UINT32 reset_pin = S_GPIO_4;
		gpio_setDir(reset_pin, GPIO_DIR_OUTPUT);
		//gpio_clearPin(reset_pin);//Low
		//Delay_DelayMs(1);
		gpio_setPin(reset_pin);//high
		Delay_DelayMs(1);
		gpio_clearPin(reset_pin);//Low
		Delay_DelayMs(1);
		gpio_setPin(reset_pin);//high
		Delay_DelayMs(1);

		break;

	case POW_CTRL_TURN_OFF:
		// set power off sequence
		sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK, DISABLE);
		break;

	case POW_CTRL_SAVE_ON:
		break;

	case POW_CTRL_SAVE_OFF:
		break;

	default:
		DBG_ERR("SenPowerCtrl fail (%d)\r\n", flag);
		break;
	}
}
#endif

#if (defined(_CMOS_OV4689M_))
static SENSOR_INIT_OBJ sensor_get_obj_ov4689m(SENSOR_ID id)
{
	// set sensor command interface info (Vx1/SIF/I2C/IO/...)
	SENSOR_INIT_OBJ init_obj = {0};
	init_obj.cmd_info.cmd_type = SENSOR_CMD_I2C;
	init_obj.cmd_info.info.i2c.i2c = SENSOR_I2C2_1ST;
	init_obj.cmd_info.info.i2c.w_addr_sel = SEN_I2C_W_ADDR_DFT;
	init_obj.sen_map_if = SENSOR_MAP_IF_DEFAULT;
	init_obj.option= SENSOR_OPTION_FLIP;

	// set mclk change enable to notify sensor driver
	// init_obj.chg_mclk_en = ENABLE;
	init_obj.sie_mclk_info.mclk_src = SEN_SIEMCLK_SRC_DFT;
	init_obj.sie_mclk_info.chg_mclk_en = ENABLE;

	// set sensor interface pin map (LVDS/CSI/...)
	init_obj.sen_2_serial_pin_map[0] = 0;
	init_obj.sen_2_serial_pin_map[1] = 1;
	init_obj.sen_2_serial_pin_map[2] = 2;
	init_obj.sen_2_serial_pin_map[3] = 3;
	return init_obj;
}
static void sen_power_ctrl_ov4689m(SENSOR_ID id, POW_CTRL_FLAG flag)
{
	switch (flag) {
	case POW_CTRL_TURN_ON:
		// set clock information
		sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK, ENABLE);

		// set power on sequence
		UINT32 reset_pin = S_GPIO_4;
		gpio_setDir(reset_pin, GPIO_DIR_OUTPUT);
		gpio_clearPin(reset_pin);
		Delay_DelayMs(1);
		gpio_setPin(reset_pin);
		Delay_DelayMs(1);

		break;

	case POW_CTRL_TURN_OFF:
		// set power off sequence
		sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK, DISABLE);
		break;

	case POW_CTRL_SAVE_ON:
		break;

	case POW_CTRL_SAVE_OFF:
		break;

	default:
		DBG_ERR("SenPowerCtrl fail (%d)\r\n", flag);
		break;
	}
}
#endif

#if (defined(_CMOS_AR0237P_))
static SENSOR_INIT_OBJ sensor_get_obj_ar0237p(SENSOR_ID id)
{
	// set sensor command interface info (Vx1/SIF/I2C/IO/...)
	SENSOR_INIT_OBJ init_obj = {0};
	init_obj.cmd_info.cmd_type = SENSOR_CMD_I2C;
	init_obj.cmd_info.info.i2c.i2c = SENSOR_I2C2_1ST;
	init_obj.cmd_info.info.i2c.w_addr_sel = SEN_I2C_W_ADDR_DFT;
	init_obj.sen_map_if = SENSOR_MAP_IF_DEFAULT;
	init_obj.option= SENSOR_OPTION_NONE;

	// set mclk change enable to notify sensor driver
	// init_obj.chg_mclk_en = ENABLE;
	init_obj.sie_mclk_info.mclk_src = SEN_SIEMCLK_SRC_DFT;
	init_obj.sie_mclk_info.chg_mclk_en = ENABLE;

	return init_obj;
}
static void sen_power_ctrl_ar0237p(SENSOR_ID id, POW_CTRL_FLAG flag)
{
	UINT32 reset_pin = S_GPIO_4;
	switch (flag) {
	case POW_CTRL_TURN_ON:
		// set power on sequence
		gpio_setDir(reset_pin, GPIO_DIR_OUTPUT);
		gpio_setPin(reset_pin);
		Delay_DelayMs(1);
		// set clock information
		sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK, ENABLE);
		Delay_DelayMs(1);
		gpio_clearPin(reset_pin);
		Delay_DelayMs(1);
		gpio_setPin(reset_pin);
		Delay_DelayMs(20);
		break;

	case POW_CTRL_TURN_OFF:
		// set power off sequence
		sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK, DISABLE);
		break;

	case POW_CTRL_SAVE_ON:
		break;

	case POW_CTRL_SAVE_OFF:
		break;

	default:
		DBG_ERR("SenPowerCtrl fail (%d)\r\n", flag);
		break;
	}
}
#endif

#if (defined(_CMOS_AR0237IRP_))
static SENSOR_INIT_OBJ sensor_get_obj_ar0237irp(SENSOR_ID id)
{
	// set sensor command interface info (Vx1/SIF/I2C/IO/...)
	SENSOR_INIT_OBJ init_obj = {0};
	init_obj.cmd_info.cmd_type = SENSOR_CMD_I2C;
	init_obj.cmd_info.info.i2c.i2c = SENSOR_I2C2_1ST;
	init_obj.cmd_info.info.i2c.w_addr_sel = SEN_I2C_W_ADDR_DFT;
	init_obj.sen_map_if = SENSOR_MAP_IF_DEFAULT;
	init_obj.option= SENSOR_OPTION_NONE;

	// set mclk change enable to notify sensor driver
	// init_obj.chg_mclk_en = ENABLE;
	init_obj.sie_mclk_info.mclk_src = SEN_SIEMCLK_SRC_DFT;
	init_obj.sie_mclk_info.chg_mclk_en = ENABLE;

	return init_obj;
}
static void sen_power_ctrl_ar0237irp(SENSOR_ID id, POW_CTRL_FLAG flag)
{
	UINT32 reset_pin = S_GPIO_4;

	switch (flag) {
	case POW_CTRL_TURN_ON:
		gpio_setDir(reset_pin, GPIO_DIR_OUTPUT);
		gpio_setPin(reset_pin);
		Delay_DelayMs(1);
		// set clock information
		sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK, ENABLE);
		Delay_DelayMs(1);
		gpio_clearPin(reset_pin);
		Delay_DelayMs(1);
		gpio_setPin(reset_pin);
		Delay_DelayMs(20);
		break;

	case POW_CTRL_TURN_OFF:
		// set power off sequence
		sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK, DISABLE);
		break;

	case POW_CTRL_SAVE_ON:
		break;

	case POW_CTRL_SAVE_OFF:
		break;

	default:
		DBG_ERR("SenPowerCtrl fail (%d)\r\n", flag);
		break;
	}
}
#endif

#if (defined(_CMOS_AR0521M_))
static SENSOR_INIT_OBJ sensor_get_obj_ar0521m(SENSOR_ID id)
{
	// set sensor command interface info (Vx1/SIF/I2C/IO/...)
	SENSOR_INIT_OBJ init_obj = {0};
	init_obj.cmd_info.cmd_type = SENSOR_CMD_I2C;
	init_obj.cmd_info.info.i2c.i2c = SENSOR_I2C2_1ST;
	init_obj.cmd_info.info.i2c.w_addr_sel = SEN_I2C_W_ADDR_DFT;
	init_obj.sen_map_if = SENSOR_MAP_IF_DEFAULT;
	init_obj.option= SENSOR_OPTION_MIRROR | SENSOR_OPTION_FLIP;

	// set mclk change enable to notify sensor driver
	// init_obj.chg_mclk_en = ENABLE;
	init_obj.sie_mclk_info.mclk_src = SEN_SIEMCLK_SRC_DFT;
	init_obj.sie_mclk_info.chg_mclk_en = ENABLE;

	// set sensor interface pin map (LVDS/CSI/...)
	init_obj.sen_2_serial_pin_map[0] = 0;
	init_obj.sen_2_serial_pin_map[1] = 1;
	init_obj.sen_2_serial_pin_map[2] = 2;
	init_obj.sen_2_serial_pin_map[3] = 3;
	return init_obj;
}
static void sen_power_ctrl_ar0521m(SENSOR_ID id, POW_CTRL_FLAG flag)
{
    UINT32 reset_pin = S_GPIO_4;
	switch (flag) {
	case POW_CTRL_TURN_ON:
		// set clock information
		sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK, ENABLE);

		// set power on sequence
		gpio_setDir(reset_pin, GPIO_DIR_OUTPUT);
		gpio_clearPin(reset_pin);
		Delay_DelayMs(1);
		gpio_setPin(reset_pin);
		Delay_DelayMs(1);

		break;

	case POW_CTRL_TURN_OFF:
		// set power off sequence
		sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK, DISABLE);
		Delay_DelayMs(1);
		gpio_clearPin(reset_pin);
		break;

	case POW_CTRL_SAVE_ON:
		break;

	case POW_CTRL_SAVE_OFF:
		break;

	default:
		DBG_ERR("SenPowerCtrl fail (%d)\r\n", flag);
		break;
	}
}
#endif

#if (defined(_CMOS_AR0330P_)) // for TP-LINK
static SENSOR_INIT_OBJ sensor_get_obj_ar0330p(SENSOR_ID id)
{
	// set sensor command interface info (Vx1/SIF/I2C/IO/...)
	SENSOR_INIT_OBJ init_obj = {0};
	init_obj.cmd_info.cmd_type = SENSOR_CMD_I2C;
	init_obj.cmd_info.info.i2c.i2c = SENSOR_I2C1_1ST;
	init_obj.cmd_info.info.i2c.w_addr_sel = SEN_I2C_W_ADDR_DFT;
	init_obj.sen_map_if = SENSOR_MAP_IF_DEFAULT;
	init_obj.option= SENSOR_OPTION_MIRROR | SENSOR_OPTION_FLIP;;

	// set mclk change enable to notify sensor driver
	// init_obj.chg_mclk_en = ENABLE;
	init_obj.sie_mclk_info.mclk_src = SEN_SIEMCLK_SRC_DFT;
	init_obj.sie_mclk_info.chg_mclk_en = ENABLE;

	return init_obj;
}
static void sen_power_ctrl_ar0330p(SENSOR_ID id, POW_CTRL_FLAG flag)
{
	UINT32 reset_pin = S_GPIO_4;
	switch (flag) {
	case POW_CTRL_TURN_ON:
		// set power on sequence
		gpio_setDir(reset_pin, GPIO_DIR_OUTPUT);
		gpio_setPin(reset_pin);
		Delay_DelayMs(1);
		// set clock information
		sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK, ENABLE);
		Delay_DelayMs(1);
		gpio_clearPin(reset_pin);
		Delay_DelayMs(1);
		gpio_setPin(reset_pin);
		Delay_DelayMs(20);
		break;

	case POW_CTRL_TURN_OFF:
		// set power off sequence
		sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK, DISABLE);
		break;

	case POW_CTRL_SAVE_ON:
		break;

	case POW_CTRL_SAVE_OFF:
		break;

	default:
		DBG_ERR("SenPowerCtrl fail (%d)\r\n", flag);
		break;
	}
}
#endif

#if (defined(_CMOS_AR0330M_))
static SENSOR_INIT_OBJ sensor_get_obj_ar0330m(SENSOR_ID id)
{
	// set sensor command interface info (Vx1/SIF/I2C/IO/...)
	SENSOR_INIT_OBJ init_obj = {0};
	init_obj.cmd_info.cmd_type = SENSOR_CMD_I2C;
	init_obj.cmd_info.info.i2c.i2c = SENSOR_I2C2_1ST;
	init_obj.cmd_info.info.i2c.w_addr_sel = SEN_I2C_W_ADDR_DFT;
	init_obj.sen_map_if = SENSOR_MAP_IF_DEFAULT;
	init_obj.option= SENSOR_OPTION_MIRROR | SENSOR_OPTION_FLIP;

	// set mclk change enable to notify sensor driver
	// init_obj.chg_mclk_en = ENABLE;
	init_obj.sie_mclk_info.mclk_src = SEN_SIEMCLK_SRC_DFT;
	init_obj.sie_mclk_info.chg_mclk_en = ENABLE;

	// set sensor interface pin map (LVDS/CSI/...)
	init_obj.sen_2_serial_pin_map[0] = 0;
	init_obj.sen_2_serial_pin_map[1] = 1;
	init_obj.sen_2_serial_pin_map[2] = 2;
	init_obj.sen_2_serial_pin_map[3] = 3;
	return init_obj;
}

static void sen_power_ctrl_ar0330m(SENSOR_ID id, POW_CTRL_FLAG flag)
{
    UINT32 reset_pin = S_GPIO_4;
	switch (flag) {
	case POW_CTRL_TURN_ON:
		// set clock information
		sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK, ENABLE);

		// set power on sequence
		gpio_setDir(reset_pin, GPIO_DIR_OUTPUT);
		gpio_clearPin(reset_pin);
		Delay_DelayMs(1);
		gpio_setPin(reset_pin);
		Delay_DelayMs(1);

		break;

	case POW_CTRL_TURN_OFF:
		// set power off sequence
		sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK, DISABLE);
		Delay_DelayMs(1);
		gpio_clearPin(reset_pin);
		break;

	case POW_CTRL_SAVE_ON:
		break;

	case POW_CTRL_SAVE_OFF:
		break;

	default:
		DBG_ERR("SenPowerCtrl fail (%d)\r\n", flag);
		break;
	}
}
#endif

#if (defined(_CMOS_PS5250M_))
static SENSOR_INIT_OBJ sensor_get_obj_ps5250m(SENSOR_ID id)
{
	// set sensor command interface info (Vx1/SIF/I2C/IO/...)
	SENSOR_INIT_OBJ init_obj = {0};
	init_obj.cmd_info.cmd_type = SENSOR_CMD_I2C;
	init_obj.cmd_info.info.i2c.i2c = SENSOR_I2C2_1ST;
	init_obj.cmd_info.info.i2c.w_addr_sel = SEN_I2C_W_ADDR_DFT;
	init_obj.sen_map_if = SENSOR_MAP_IF_DEFAULT;
	init_obj.option= SENSOR_OPTION_MIRROR | SENSOR_OPTION_FLIP;

	// set mclk change enable to notify sensor driver
	// init_obj.chg_mclk_en = ENABLE;
	init_obj.sie_mclk_info.mclk_src = SEN_SIEMCLK_SRC_DFT;
	init_obj.sie_mclk_info.chg_mclk_en = ENABLE;

	// set sensor interface pin map (LVDS/CSI/...)
	init_obj.sen_2_serial_pin_map[0] = 0;
	init_obj.sen_2_serial_pin_map[1] = 1;
	init_obj.sen_2_serial_pin_map[2] = SEN_IGNORE;
	init_obj.sen_2_serial_pin_map[3] = SEN_IGNORE;
	return init_obj;
}

static void sen_power_ctrl_ps5250m(SENSOR_ID id, POW_CTRL_FLAG flag)
{
    UINT32 reset_pin = S_GPIO_4;
	switch (flag) {
	case POW_CTRL_TURN_ON:


		// set power on sequence
		gpio_setDir(reset_pin, GPIO_DIR_OUTPUT);
		gpio_setPin(reset_pin);
		Delay_DelayMs(1);
		gpio_clearPin(reset_pin);
		Delay_DelayMs(1);
		gpio_setPin(reset_pin);
		Delay_DelayMs(1);

		// set clock information
		sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK, ENABLE);

		break;

	case POW_CTRL_TURN_OFF:
		// set power off sequence
		sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK, DISABLE);
		Delay_DelayMs(1);
		gpio_clearPin(reset_pin);
		break;

	case POW_CTRL_SAVE_ON:
		break;

	case POW_CTRL_SAVE_OFF:
		break;

	default:
		DBG_ERR("SenPowerCtrl fail (%d)\r\n", flag);
		break;
	}
}
#endif

#if (defined(_CMOS_SC4236M_))
static SENSOR_INIT_OBJ sensor_get_obj_sc4236m(SENSOR_ID id)
{
	// set sensor command interface info (Vx1/SIF/I2C/IO/...)
	SENSOR_INIT_OBJ init_obj = {0};
	init_obj.cmd_info.cmd_type = SENSOR_CMD_I2C;
	init_obj.cmd_info.info.i2c.i2c = SENSOR_I2C2_1ST;
	init_obj.cmd_info.info.i2c.w_addr_sel = SEN_I2C_W_ADDR_DFT;
	init_obj.sen_map_if = SENSOR_MAP_IF_DEFAULT;
	init_obj.option= SENSOR_OPTION_MIRROR | SENSOR_OPTION_FLIP;

	// set mclk change enable to notify sensor driver
	// init_obj.chg_mclk_en = ENABLE;
	init_obj.sie_mclk_info.mclk_src = SEN_SIEMCLK_SRC_DFT;
	init_obj.sie_mclk_info.chg_mclk_en = ENABLE;

	// set sensor interface pin map (LVDS/CSI/...)
	init_obj.sen_2_serial_pin_map[0] = 0;
	init_obj.sen_2_serial_pin_map[1] = 1;
	init_obj.sen_2_serial_pin_map[2] = SEN_IGNORE;
	init_obj.sen_2_serial_pin_map[3] = SEN_IGNORE;
	return init_obj;
}

static void sen_power_ctrl_sc4236m(SENSOR_ID id, POW_CTRL_FLAG flag)
{
    UINT32 reset_pin = S_GPIO_4;
	switch (flag) {
	case POW_CTRL_TURN_ON:
		// set power on sequence
		gpio_setDir(reset_pin, GPIO_DIR_OUTPUT);
		gpio_setPin(reset_pin);
		Delay_DelayMs(2); //>=1ms
		gpio_clearPin(reset_pin);
		Delay_DelayMs(2); //>=1ms
		gpio_setPin(reset_pin);
		Delay_DelayMs(2); //>=1ms

		// set clock information
		sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK, ENABLE);

		break;

	case POW_CTRL_TURN_OFF:
		// set power off sequence
		sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK, DISABLE);
		Delay_DelayMs(2);
		gpio_clearPin(reset_pin);
		break;

	case POW_CTRL_SAVE_ON:
		break;

	case POW_CTRL_SAVE_OFF:
		break;

	default:
		DBG_ERR("SenPowerCtrl fail (%d)\r\n", flag);
		break;
	}
}
#endif

#if (defined(_CMOS_SC5035M_))
static SENSOR_INIT_OBJ sensor_get_obj_sc5035m(SENSOR_ID id)
{
	// set sensor command interface info (Vx1/SIF/I2C/IO/...)
	SENSOR_INIT_OBJ init_obj = {0};
	init_obj.cmd_info.cmd_type = SENSOR_CMD_I2C;
	init_obj.cmd_info.info.i2c.i2c = SENSOR_I2C2_1ST;
	init_obj.cmd_info.info.i2c.w_addr_sel = SEN_I2C_W_ADDR_DFT;
	init_obj.sen_map_if = SENSOR_MAP_IF_DEFAULT;
	init_obj.option= SENSOR_OPTION_NONE;//SENSOR_OPTION_MIRROR | SENSOR_OPTION_FLIP;

	// set mclk change enable to notify sensor driver
	// init_obj.chg_mclk_en = ENABLE;
	init_obj.sie_mclk_info.mclk_src = SEN_SIEMCLK_SRC_DFT;
	init_obj.sie_mclk_info.chg_mclk_en = ENABLE;

	// set sensor interface pin map (LVDS/CSI/...)
	init_obj.sen_2_serial_pin_map[0] = 0;
	init_obj.sen_2_serial_pin_map[1] = 1;
	init_obj.sen_2_serial_pin_map[2] = SEN_IGNORE;
	init_obj.sen_2_serial_pin_map[3] = SEN_IGNORE;
	return init_obj;
}

static void sen_power_ctrl_sc5035m(SENSOR_ID id, POW_CTRL_FLAG flag)
{
    UINT32 reset_pin = S_GPIO_4;
	switch (flag) {
	case POW_CTRL_TURN_ON:
		// set power on sequence
		gpio_setDir(reset_pin, GPIO_DIR_OUTPUT);
		gpio_setPin(reset_pin);
		Delay_DelayMs(2); //>=1ms
		gpio_clearPin(reset_pin);
		Delay_DelayMs(2); //>=1ms
		gpio_setPin(reset_pin);
		Delay_DelayMs(2); //>=1ms

		// set clock information
		sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK, ENABLE);

		break;

	case POW_CTRL_TURN_OFF:
		// set power off sequence
		sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK, DISABLE);
		Delay_DelayMs(2);
		gpio_clearPin(reset_pin);
		break;

	case POW_CTRL_SAVE_ON:
		break;

	case POW_CTRL_SAVE_OFF:
		break;

	default:
		DBG_ERR("SenPowerCtrl fail (%d)\r\n", flag);
		break;
	}
}
#endif

#if (defined(_CMOS_TP2825_))
static SENSOR_INIT_OBJ sensor_get_obj_tp2825(SENSOR_ID id)
{CHKPNT;
#if 0
	// set sensor command interface info (Vx1/SIF/I2C/IO/...)
	SENSOR_INIT_OBJ init_obj = {0};
	init_obj.cmd_info.cmd_type = SENSOR_CMD_I2C;
	init_obj.cmd_info.info.i2c.i2c = SENSOR_I2C2_1ST;
	init_obj.cmd_info.info.i2c.w_addr_sel = SEN_I2C_W_ADDR_DFT;
	init_obj.sen_map_if = SENSOR_MAP_IF_DEFAULT;
	//init_obj.serial_clane_switch = TRUE;

	init_obj.option = SENSOR_OPTION_USER;

	// set mclk change enable to notify sensor driver
	// init_obj.chg_mclk_en = ENABLE;
	init_obj.sie_mclk_info.mclk_src = SEN_SIEMCLK_SRC_MCLK2;
	init_obj.sie_mclk_info.chg_mclk_en = ENABLE;
#else
	SENSOR_INIT_OBJ init_obj = {0};
	init_obj.cmd_info.cmd_type = SENSOR_CMD_I2C;
	init_obj.cmd_info.info.i2c.i2c = SENSOR_I2C3_1ST;
	init_obj.cmd_info.info.i2c.w_addr_sel = SEN_I2C_W_ADDR_DFT;
	init_obj.sen_map_if = SENSOR_MAP_IF_DEFAULT;
	//init_obj.serial_clane_switch = TRUE;

	init_obj.option = SENSOR_OPTION_USER;

	// set mclk change enable to notify sensor driver
	// init_obj.chg_mclk_en = ENABLE;
	init_obj.sie_mclk_info.mclk_src = SEN_SIEMCLK_SRC_MCLK2;
	init_obj.sie_mclk_info.chg_mclk_en = ENABLE;
#endif

	return init_obj;
}
static void sen_power_ctrl_tp2825(SENSOR_ID id, POW_CTRL_FLAG flag)
{CHKPNT;

	static BOOL bFirstTime = TRUE;
	switch (flag) {

	case POW_CTRL_TURN_ON:
		#if 0
		if (bFirstTime) {
			bFirstTime = FALSE;
			// set clock information
			//	pll_setPLLEn(PLL_ID_5,      DISABLE);
			//	pll_setPLL(PLL_ID_5,        0x90000);//54MHz
			//	pll_setPLLEn(PLL_ID_5,      ENABLE);
			//pll_selectClkSrc(PLL_CLK_SIEMCLK, PLL_CLKSRC_PLL5);
			//sensor_if_get_pll_ctrl_obj()->set_clk_src(id,PLL_CLK_SIEMCLK

			//pll_setClkFreq(PLL_CLK_SIEMCLK, 27000000);

			// reset pin
			//gpio_clearPin(GPIO_TVI_RESET);
			Delay_DelayMs(10);
			// 1.2V (default enable in gpio_info.c)
			//gpio_setPin(GPIO_TVI_12EN);
			//Delay_DelayMs(2);

			sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK2, ENABLE);

			// reset pin
			//gpio_setPin(GPIO_TVI_RESET);
		}
		#else
		if (bFirstTime)
		{
			bFirstTime = FALSE;
			// set clock information
			//	pll_setPLLEn(PLL_ID_5,      DISABLE);
			//	pll_setPLL(PLL_ID_5,        0x90000);//54MHz
			//	pll_setPLLEn(PLL_ID_5,      ENABLE);
			//pll_selectClkSrc(PLL_CLK_SIEMCLK, PLL_CLKSRC_PLL5);
			//sensor_if_get_pll_ctrl_obj()->set_clk_src(id,PLL_CLK_SIEMCLK

			//pll_setClkFreq(PLL_CLK_SIEMCLK, 27000000);


			gpio_setDir(GPIO_SENSOR2_1_2EN, GPIO_DIR_OUTPUT);
			gpio_setDir(GPIO_SENSOR_PWR1_8, GPIO_DIR_OUTPUT);
			gpio_setDir(GPIO_SENSOR2_3_3EN, GPIO_DIR_OUTPUT);
			gpio_setDir(GPIO_SENSOR2_RESET, GPIO_DIR_OUTPUT);
			// 1_2EN pin
			gpio_setPin(GPIO_SENSOR2_1_2EN);
			Delay_DelayMs(20);
			// GPIO_SENSOR_PWR1_8 pin
			gpio_setPin(GPIO_SENSOR_PWR1_8);
			Delay_DelayMs(20);
			// 3_3EN pin
			gpio_setPin(GPIO_SENSOR2_3_3EN);
			Delay_DelayMs(20);



			sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK2, ENABLE);

			// reset pin
			gpio_setPin(GPIO_SENSOR2_RESET);
			Delay_DelayMs(20);
			gpio_clearPin(GPIO_SENSOR2_RESET);
			Delay_DelayMs(150);
			gpio_setPin(GPIO_SENSOR2_RESET);
			Delay_DelayMs(20);
		}
		#endif
		break;

	case POW_CTRL_TURN_OFF:
		// set power off sequence
		//	pll_setClkEn(PLL_CLK_SIEMCLK, DISABLE);
		//	pll_setPLLEn(PLL_ID_5, DISABLE);
		//sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK2, DISABLE);
		break;

	case POW_CTRL_SAVE_ON:
		break;

	case POW_CTRL_SAVE_OFF:
		break;

	default:
		DBG_ERR("SenPowerCtrl fail (%d)\r\n", flag);
		break;
	}
}

static BOOL sen_det_plug_in_tp2825(SENSOR_ID id)
{
#if 0
#if 1

	BOOL rt = E_OK;
	SENSOR_HANDLE sen_handle = {0};

	if (sensor_is_open(id) == FALSE) {

		drv_sensor_power_turn_on(id);

		if (drv_sensor_init(id, &sen_handle) == FALSE) {
			DBG_ERR("sensor_init fail\r\n");
			return FALSE;
		}

		if (sen_handle.drv_tab == NULL) {
			DBG_ERR("drv_tab NULL\r\n");
			return E_SYS;
		}

		if (sensor_open(id, &sen_handle) != E_OK) {
			DBG_ERR("sensor open fail\r\n");
			return FALSE;
		}
	}

	if (sensor_get_info(id, SEN_GET_DET_PLUG, &rt) != E_OK) {
		DBG_ERR("get sensor plugin fail\r\n");
		return FALSE;
	}

	return rt;

#else

	// detected by GPIO
	//DBG_DUMP("^C sen_det_plug_in_tp2825: %d\r\n", gpio_getPin(GPIO_DET_REAR));
	//return (gpio_getPin(GPIO_DET_REAR) == 0);
	return (gpio_getPin(GPIO_DET_REAR) == 1);

#endif
#endif
#if 1
	BOOL rt = E_OK;
	return TRUE;

	SENSOR_HANDLE sen_handle = {0};

	if (sensor_is_open(id) == FALSE) {

		drv_sensor_power_turn_on(id);

		if (drv_sensor_init(id, &sen_handle) == FALSE) {
			DBG_ERR("sensor_init fail\r\n");
			return FALSE;
		}

		if (sen_handle.drv_tab == NULL) {
			DBG_ERR("drv_tab NULL\r\n");
			return E_SYS;
		}

		if (sensor_open(id, &sen_handle) != E_OK) {
			DBG_ERR("sensor open fail\r\n");
			return FALSE;
		}
	}

	if (sensor_get_info(id, SEN_GET_DET_PLUG, &rt) != E_OK) {
		DBG_ERR("get sensor plugin fail\r\n");
		return FALSE;
	}

	return rt;

#else

	// detected by GPIO
	//DBG_DUMP("^C sen_det_plug_in_pr2020k: %d\r\n", gpio_getPin(GPIO_DET_REAR));
	//return (gpio_getPin(GPIO_DET_REAR) == 0);
	return (gpio_getPin(GPIO_DET_REAR) == 1);

#endif

}
#endif

#if (defined(_CMOS_PR2020K_))
static SENSOR_INIT_OBJ sensor_get_obj_pr2020k(SENSOR_ID id)
{
	// set sensor command interface info (Vx1/SIF/I2C/IO/...)
	SENSOR_INIT_OBJ init_obj = {0};
	init_obj.cmd_info.cmd_type = SENSOR_CMD_I2C;
	init_obj.cmd_info.info.i2c.i2c = SENSOR_I2C3_1ST;
	init_obj.cmd_info.info.i2c.w_addr_sel = SEN_I2C_W_ADDR_DFT;
	init_obj.sen_map_if = SENSOR_MAP_IF_DEFAULT;
	//init_obj.serial_clane_switch = TRUE;

	init_obj.option = SENSOR_OPTION_USER;

	// set mclk change enable to notify sensor driver
	// init_obj.chg_mclk_en = ENABLE;
	init_obj.sie_mclk_info.mclk_src = SEN_SIEMCLK_SRC_MCLK2;
	init_obj.sie_mclk_info.chg_mclk_en = ENABLE;
	debug_msg("mmmmm sensor_get_obj_pr2020k");

	return init_obj;
}

static void sen_power_ctrl_pr2020k(SENSOR_ID id, POW_CTRL_FLAG flag)
{
	static BOOL bFirstTime = TRUE;
	switch (flag) {
	case POW_CTRL_TURN_ON:
		if (0)//(!bFirstTime)
		{
			Delay_DelayMs(20);
			break;
		}
			bFirstTime = FALSE;
			// 1_2EN pin
			gpio_setPin(GPIO_SENSOR2_1_2EN);
			Delay_DelayMs(20);
			// 3_3EN pin
			gpio_setPin(GPIO_SENSOR2_3_3EN);
			Delay_DelayMs(20);

			sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK2, ENABLE);

			// reset pin
			gpio_setPin(GPIO_SENSOR2_RESET);
			Delay_DelayMs(20);
			gpio_clearPin(GPIO_SENSOR2_RESET);
			Delay_DelayMs(150);
			gpio_setPin(GPIO_SENSOR2_RESET);
			Delay_DelayMs(20);
			debug_msg("mmmmm sen_power_ctrl_pr2020k");

		break;

	case POW_CTRL_TURN_OFF:
		// set power off sequence
		//	pll_setClkEn(PLL_CLK_SIEMCLK, DISABLE);
		//	pll_setPLLEn(PLL_ID_5, DISABLE);
		//sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK2, DISABLE);
		break;

	case POW_CTRL_SAVE_ON:
		break;

	case POW_CTRL_SAVE_OFF:
		break;

	default:
		DBG_ERR("SenPowerCtrl fail (%d)\r\n", flag);
		break;
	}
}

static BOOL sen_det_plug_in_pr2020k(SENSOR_ID id)
{

#if 1
	BOOL rt = E_OK;
	SENSOR_HANDLE sen_handle = {0};
	if (sensor_is_open(id) == FALSE) {

		drv_sensor_power_turn_on(id);
		if (drv_sensor_init(id, &sen_handle) == FALSE) {
			debug_msg("MMMM sensor_init fail\r\n");
			return FALSE;
		}

		if (sen_handle.drv_tab == NULL) {
			debug_msg("MMMM drv_tab NULL\r\n");
			return E_SYS;
		}

		if (sensor_open(id, &sen_handle) != E_OK) {
			debug_msg("MMMM sensor open fail\r\n");
			return FALSE;
		}
	}

	if (sensor_get_info(id, SEN_GET_DET_PLUG, &rt) != E_OK) {
		debug_msg("MMMMM get sensor plugin fail\r\n");
		return FALSE;
	}

//	debug_msg("mmmmmmmmmm sen_det_plug_in_tp2825=%d\r\n",rt);

	return rt;




#else

	// detected by GPIO
	//DBG_DUMP("^C sen_det_plug_in_pr2020k: %d\r\n", gpio_getPin(GPIO_DET_REAR));
	//return (gpio_getPin(GPIO_DET_REAR) == 0);
	return (gpio_getPin(GPIO_DET_REAR) == 1);

#endif
}
#endif

#if (defined(_CMOS_AD_pr2020k_))
static SENSOR_INIT_OBJ sensor_get_obj_ad_pr2020k(SENSOR_ID id)
{
	// set sensor command interface info (Vx1/SIF/I2C/IO/...)
	SENSOR_INIT_OBJ init_obj = {0};
	init_obj.cmd_info.cmd_type = SENSOR_CMD_I2C;
	init_obj.cmd_info.info.i2c.i2c = SENSOR_I2C3_1ST;
	init_obj.cmd_info.info.i2c.w_addr_sel = SEN_I2C_W_ADDR_DFT;
	init_obj.sen_map_if = SENSOR_MAP_IF_DEFAULT;
	//init_obj.serial_clane_switch = TRUE;

	init_obj.option = SENSOR_OPTION_USER;

	// set mclk change enable to notify sensor driver
	// init_obj.chg_mclk_en = ENABLE;
	init_obj.sie_mclk_info.mclk_src = SEN_SIEMCLK_SRC_MCLK2;
	init_obj.sie_mclk_info.chg_mclk_en = ENABLE;
	debug_msg("mmmmm sensor_get_obj_pr2020k");

	return init_obj;
}

static void sen_power_ctrl_ad_pr2020k(SENSOR_ID id, POW_CTRL_FLAG flag)
{
	static BOOL bFirstTime = TRUE;
	switch (flag) {
	case POW_CTRL_TURN_ON:
		if (0)//(!bFirstTime)
		{
			Delay_DelayMs(20);
			break;
		}
			bFirstTime = FALSE;
			// 1_2EN pin
			gpio_setPin(GPIO_SENSOR2_1_2EN);
			Delay_DelayMs(20);
			// 3_3EN pin
			gpio_setPin(GPIO_SENSOR2_3_3EN);
			Delay_DelayMs(20);

			sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK2, ENABLE);

			// reset pin
			gpio_setPin(GPIO_SENSOR2_RESET);
			Delay_DelayMs(20);
			gpio_clearPin(GPIO_SENSOR2_RESET);
			Delay_DelayMs(150);
			gpio_setPin(GPIO_SENSOR2_RESET);
			Delay_DelayMs(20);
			debug_msg("mmmmm sen_power_ctrl_pr2020k");

		break;

	case POW_CTRL_TURN_OFF:
		// set power off sequence
		//	pll_setClkEn(PLL_CLK_SIEMCLK, DISABLE);
		//	pll_setPLLEn(PLL_ID_5, DISABLE);
		//sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK2, DISABLE);
		break;

	case POW_CTRL_SAVE_ON:
		break;

	case POW_CTRL_SAVE_OFF:
		break;

	default:
		DBG_ERR("SenPowerCtrl fail (%d)\r\n", flag);
		break;
	}
}

static BOOL sen_det_plug_in_ad_pr2020k(SENSOR_ID id)
{
	BOOL rt = E_OK;
	SENSOR_HANDLE sen_handle = {0};
	if (sensor_is_open(id) == FALSE) {

		drv_sensor_power_turn_on(id);
		if (drv_sensor_init(id, &sen_handle) == FALSE) {
			debug_msg("MMMM sensor_init fail\r\n");
			return FALSE;
		}

		if (sen_handle.drv_tab == NULL) {
			debug_msg("MMMM drv_tab NULL\r\n");
			return E_SYS;
		}

		if (sensor_open(id, &sen_handle) != E_OK) {
			debug_msg("MMMM sensor open fail\r\n");
			return FALSE;
		}
	}

	if (sensor_get_info(id, SEN_GET_DET_PLUG, &rt) != E_OK) {
		debug_msg("MMMMM get sensor plugin fail\r\n");
		return FALSE;
	}

//	debug_msg("mmmmmmmmmm sen_det_plug_in_tp2825=%d\r\n",rt);

	return rt;
}
#endif



#if (defined(_CMOS_SIEPATGEN_))
static SENSOR_INIT_OBJ sensor_get_obj_siepatgen(SENSOR_ID id)
{

	// set sensor command interface info (Vx1/SIF/I2C/IO/...)
	SENSOR_INIT_OBJ init_obj = {0};

	init_obj.sie_mclk_info.mclk_src = SEN_SIEMCLK_SRC_MCLK;
	init_obj.sie_mclk_info.chg_mclk_en = ENABLE;

	return init_obj;
}

static void sen_power_ctrl_siepatgen(SENSOR_ID id, POW_CTRL_FLAG flag)
{
	switch (flag) {
	case POW_CTRL_TURN_ON:

		if (id == SENSOR_ID_1) {
			sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK, ENABLE);
		} else {
			DBG_ERR("not ready\r\n");
		}

		break;

	case POW_CTRL_TURN_OFF:
		if (id == SENSOR_ID_1) {
			sensor_if_get_pll_ctrl_obj()->set_clk_en(id, SEN_CLK_SIEMCLK, DISABLE);
		} else {
			DBG_ERR("not ready\r\n");
		}
		break;

	case POW_CTRL_SAVE_ON:
		break;

	case POW_CTRL_SAVE_OFF:
		break;

	default:
		DBG_ERR("SenPowerCtrl fail (%d)\r\n", flag);
		break;
	}
}
#endif

static SEN_MAP_INFO sensor_map_tab[SENSOR_ID_MAX_NUM + 1] = {
#if (defined(_CMOS_AR0237L_))
	{"CMOS_AR0237L", sensor_get_drv_tab_ar0237l, sensor_get_obj_ar0237l, sen_power_ctrl_ar0237l, NULL, NULL},
#endif
#if (defined(_CMOS_IMX291M_))
	{"CMOS_IMX291M", sensor_get_drv_tab_imx291m, sensor_get_obj_imx291m, sen_power_ctrl_imx291m, NULL, NULL},
#endif
#if (defined(_CMOS_GC2023M_))
	{"CMOS_GC2023M", sensor_get_drv_tab_gc2023m, sensor_get_obj_gc2023m, sen_power_ctrl_gc2023m, NULL, NULL},
#endif
#if (defined(_CMOS_GC2053M_))
	{"CMOS_GC2053M", sensor_get_drv_tab_gc2053m, sensor_get_obj_gc2053m, sen_power_ctrl_gc2053m, NULL, NULL},
#endif
#if (defined(_CMOS_GC4653M_))
	{"CMOS_GC4653M", sensor_get_drv_tab_gc4653m, sensor_get_obj_gc4653m, sen_power_ctrl_gc4653m, NULL, NULL},
#endif
#if (defined(_CMOS_GC2023P_))
	{"CMOS_GC2023P", sensor_get_drv_tab_gc2023p, sensor_get_obj_gc2023p, sen_power_ctrl_gc2023p, NULL, NULL},
#endif
#if (defined(_CMOS_GC2053P_))
	{"CMOS_GC2053P", sensor_get_drv_tab_gc2053p, sensor_get_obj_gc2053p, sen_power_ctrl_gc2053p, NULL, NULL},
#endif
#if (defined(_CMOS_IMX290M_))
	{"CMOS_IMX290M", sensor_get_drv_tab_imx290m, sensor_get_obj_imx290m, sen_power_ctrl_imx290m, NULL, NULL},
#endif
#if (defined(_CMOS_IMX307M_))
	{"CMOS_IMX307M", sensor_get_drv_tab_imx307m, sensor_get_obj_imx307m, sen_power_ctrl_imx307m, NULL, NULL},
#endif
#if (defined(_CMOS_IMX335M_))
    {"CMOS_IMX335M", sensor_get_drv_tab_imx335m, sensor_get_obj_imx335m, sen_power_ctrl_imx335m, NULL, NULL},
#endif
#if (defined(_CMOS_OV2715M_))
	{"CMOS_OV2715M", sensor_get_drv_tab_ov2715m, sensor_get_obj_ov2715m, sen_power_ctrl_ov2715m, NULL, NULL},
#endif
#if (defined(_CMOS_OV2735M_))
	{"CMOS_OV2735M", sensor_get_drv_tab_ov2735m, sensor_get_obj_ov2735m, sen_power_ctrl_ov2735m, NULL, NULL},
#endif
#if (defined(_CMOS_OS05A10M_))
	{"CMOS_OS05A10M", sensor_get_drv_tab_os05a10m, sensor_get_obj_os05a10m, sen_power_ctrl_os05a10m, NULL, NULL},
#endif
#if (defined(_CMOS_OS08A10M_))
	{"CMOS_OS08A10M", sensor_get_drv_tab_os08a10m, sensor_get_obj_os08a10m, sen_power_ctrl_os08a10m, NULL, NULL},
#endif
#if (defined(_CMOS_OV4689M_))
	{"CMOS_OV4689M", sensor_get_drv_tab_ov4689m, sensor_get_obj_ov4689m, sen_power_ctrl_ov4689m, NULL, NULL},
#endif
#if (defined(_CMOS_AR0237P_))
	{"CMOS_AR0237P", sensor_get_drv_tab_ar0237p, sensor_get_obj_ar0237p, sen_power_ctrl_ar0237p, NULL, NULL},
#endif
#if (defined(_CMOS_AR0237IRP_))
	{"CMOS_AR0237IRP", sensor_get_drv_tab_ar0237irp, sensor_get_obj_ar0237irp, sen_power_ctrl_ar0237irp, NULL, NULL},
#endif
#if (defined(_CMOS_AR0330M_))
    {"CMOS_AR0330M", sensor_get_drv_tab_ar0330m, sensor_get_obj_ar0330m, sen_power_ctrl_ar0330m, NULL, NULL},
#endif
#if (defined(_CMOS_AR0330P_))
    {"CMOS_AR0330P", sensor_get_drv_tab_ar0330p, sensor_get_obj_ar0330p, sen_power_ctrl_ar0330p, NULL, NULL},
#endif
#if (defined(_CMOS_AR0521M_))
	{"CMOS_AR0521M", sensor_get_drv_tab_ar0521m, sensor_get_obj_ar0521m, sen_power_ctrl_ar0521m, NULL, NULL},
#endif
#if (defined(_CMOS_PS5250M_))
	{"CMOS_PS5250M", sensor_get_drv_tab_ps5250m, sensor_get_obj_ps5250m, sen_power_ctrl_ps5250m, NULL, NULL},
#endif
#if (defined(_CMOS_SC4236M_))
	{"CMOS_SC4236M", sensor_get_drv_tab_sc4236m, sensor_get_obj_sc4236m, sen_power_ctrl_sc4236m, NULL, NULL},
#endif
#if (defined(_CMOS_SC5035M_))
	{"CMOS_SC5035M", sensor_get_drv_tab_sc5035m, sensor_get_obj_sc5035m, sen_power_ctrl_sc5035m, NULL, NULL},
#endif
#if (defined(_CMOS_TP2825_))
	{"CMOS_TP2825", sensor_get_drv_tab_tp2825, sensor_get_obj_tp2825, sen_power_ctrl_tp2825, sen_det_plug_in_tp2825, NULL},
#endif
#if (defined(_CMOS_PR2020K_))
	{"CMOS_PR2020K", sensor_get_drv_tab_pr2020k, sensor_get_obj_pr2020k, sen_power_ctrl_pr2020k, sen_det_plug_in_pr2020k, NULL},
#endif
#if (defined(_CMOS_AD_pr2020k_))
	{"CMOS_AD_pr2020k", sensor_get_drv_tab_ad_std_drv_pr2020k, sensor_get_obj_ad_pr2020k, sen_power_ctrl_ad_pr2020k, sen_det_plug_in_ad_pr2020k, NULL},
#endif
#if (defined(_CMOS_SIEPATGEN_))
	{"CMOS_SIEPATGEN", sensor_get_drv_tab_siepatgen, sensor_get_obj_siepatgen, sen_power_ctrl_siepatgen, NULL, NULL},
#endif
	{"END", NULL, NULL, NULL, NULL, NULL},
};


#if 0
#endif
/************************************************************************/
/*                      common function                                 */
/************************************************************************/
static SEN_MAP_INFO *drv_sensor_search(CHAR *search_name)
{
	UINT32 i;
	SEN_MAP_INFO *info;

	DBG_IND("search drv name = %s\r\n", search_name);
	//debug_msg("\r\n---search drv name = %s---\r\n", search_name);;
	info = sensor_map_tab;

	for (i = 0; (strcmp(info[i].name, "END") != 0); i ++) {
		//debug_msg("\r\n---id:%d,name:%s---\r\n",i,info[i].name);
		if(strcmp(info[i].name, search_name) == 0)
		{
			return &info[i];
		}
	}
	return NULL;
}

BOOL drv_sensor_repalce_drv(SENSOR_ID id, CHAR *repalce_name)
{
	if (id >= SENSOR_ID_MAX_NUM) {
		DBG_ERR("id(%d) overflow\r\n");
		return FALSE;
	}
	DBG_IND("id = %d, repalce drv name = %s\r\n", id, repalce_name);

	if (drv_sensor_search(repalce_name) == NULL) {
		DBG_ERR("search sensor driver fail (%s)\r\n", repalce_name);
		return FALSE;
	}

	strncpy(&sensor_name[id][0], repalce_name, sizeof(sensor_name[id]));

	return TRUE;
}

BOOL b_set_gp_cfg = FALSE;
SENSOR_MULTI_ID sensor_gp_map[SENSOR_ID_MAX_NUM] = {0}, sensor_subgp_map[SENSOR_ID_MAX_NUM] = {0};
SENSOR_MULTI_ID sensor_gp_rec[SENSOR_ID_MAX_NUM] = {0}, sensor_subgp_rec[SENSOR_ID_MAX_NUM] = {0}; // by gp idx (not by id)
UINT32 senname_idx[SENSOR_ID_MAX_NUM] = {0};
BOOL b_powere_on_ready = FALSE;
ER drv_sensor_group_cfg(SENSOR_MULTI_ID* sensor_gp, SENSOR_MULTI_ID* sensor_subgp)
{
	UINT32 cnt, chk_cnt, idx_cnt;

	/* set gp map */
	for (cnt = 0; cnt < SENSOR_ID_MAX_NUM; cnt++) {
		sensor_gp_rec[cnt] = sensor_gp[cnt];
		if (sensor_gp[cnt] == SENSOR_MULTI_ID_NONE) {
			continue;
		}
		for (chk_cnt = 0; chk_cnt < SENSOR_ID_MAX_NUM; chk_cnt++) {
			if (sensor_chk_multiid(chk_cnt, sensor_gp[cnt])) {
				sensor_gp_map[chk_cnt] |= sensor_gp[cnt];
			}
		}
	}

	/* set subgp map */
	for (cnt = 0; cnt < SENSOR_ID_MAX_NUM; cnt++) {
		sensor_subgp_rec[cnt] = sensor_subgp[cnt];
		if (sensor_subgp[cnt] == SENSOR_MULTI_ID_NONE) {
			continue;
		}
		for (chk_cnt = 0; chk_cnt < SENSOR_ID_MAX_NUM; chk_cnt++) {
			if (sensor_chk_multiid(chk_cnt, sensor_subgp[cnt])) {
				sensor_subgp_map[chk_cnt] |= sensor_subgp[cnt];
			}
		}
	}

	/* set senname_idx */
	for (cnt = 0; cnt < SENSOR_ID_MAX_NUM; cnt++) {
		// initial
		senname_idx[cnt] = SEN_IGNORE;
	}

	idx_cnt = 0;
	for (cnt = 0; cnt < SENSOR_ID_MAX_NUM; cnt++) {
		if (strcmp(sensor_name[cnt], "CMOS_OFF") == 0) {
			continue;
		}

		if (cnt == 0) {
			// id 0 do not need to compare
		} else {
			// copare with previous id
			for (chk_cnt = cnt - 1; chk_cnt >= 0; chk_cnt--) {
				if (strcmp(sensor_name[cnt], sensor_name[chk_cnt]) == 0) {
					// update idx
					senname_idx[cnt] = senname_idx[chk_cnt];
					break;
				}
				if (chk_cnt == 0) {
					break;
				}
			}
		}

		// update new_idx
		if (senname_idx[cnt] == SEN_IGNORE) {
			senname_idx[cnt] = idx_cnt;
			idx_cnt++;
		}
	}

	b_set_gp_cfg = TRUE;

	return E_OK;
}

SENSOR_AD_EXT_INFO sensor_ad_ext_info[SENSOR_ID_MAX_NUM] = {0};
ER drv_sensor_cfg_ad_ext(SENSOR_AD_EXT_INFO* user_ad_ext_info)
{
	UINT32 i;

	if (!b_set_gp_cfg) {
		DBG_ERR("pls set drv_sensor_group_cfg first\r\n");
	}

	for (i = 0; i < SENSOR_ID_MAX_NUM; i++) {
		if (sensor_gp_map[i] == SENSOR_MULTI_ID_NONE) {
			continue;
		}
		sensor_ad_ext_info[i] = user_ad_ext_info[i];
	}

	return E_OK;
}

BOOL drv_sensor_first_cfg(void)
{
	UINT32 cnt_id;
	SEN_MAP_INFO *sen_map_info;
	ER rt = E_OK;

	for (cnt_id = 0; cnt_id < SENSOR_ID_MAX_NUM; cnt_id++) {
		if (sensor_debug_item[cnt_id] & SENSOR_DEBUG_MODELEXT) {
			debug_msg("^C%s\r\n", __func__);
		}
	}

	// Check that no sensor is open
	for (cnt_id = 0; cnt_id < SENSOR_ID_MAX_NUM; cnt_id++) {
		if (sensor_is_open(cnt_id)) {
			DBG_ERR("sensor id %d is open\r\n", cnt_id);
			return FALSE;
		}
	}

	// set group info.
	sensor_set_group_info(sensor_gp_map, sensor_subgp_map);

	// set mclk info for SIE MCLK
	for (cnt_id = 0; cnt_id < SENSOR_ID_MAX_NUM; cnt_id++) {
		sen_map_info = drv_sensor_search(sensor_name[cnt_id]);

		if ((strcmp(sensor_name[cnt_id], "CMOS_OFF") != 0) && (sen_map_info != NULL)) {
			rt = sensor_cfg_sie_mclk(cnt_id, sen_map_info->drv_obj(cnt_id), sen_map_info->drv_tab());
			if (rt != E_OK) {
				DBG_ERR("id %d sensor_cfg_sie_mclk error %d\r\n", cnt_id, rt);
			}
			rt = sensor_cfg_sie_max_clk(cnt_id, sen_map_info->drv_obj(cnt_id), sen_map_info->drv_tab());
			if (rt != E_OK) {
				DBG_ERR("id %d sensor_cfg_sie_clk error %d\r\n", cnt_id, rt);
			}
		}
	}

	// set pll clk
	rt = sensor_cfg_pll_clk();
	if (rt != E_OK) {
		DBG_ERR("sensor_cfg_pll_clk %d\r\n", rt);
	}

	return FALSE;
}

BOOL drv_sensor_init(SENSOR_ID id, SENSOR_HANDLE *handle)
{
	SEN_MAP_INFO *sensor_info = drv_sensor_search(sensor_name[id]);

	if (sensor_debug_item[id] & SENSOR_DEBUG_MODELEXT) {
		debug_msg("^C%s id %d\r\n", __func__, id);
	}

	if (sensor_info == NULL) {
		DBG_ERR("driver get fail\r\n");
		return FALSE;
	}

	if ((sensor_info->drv_tab == NULL) || (sensor_info->drv_obj == NULL)) {
		DBG_ERR("fp NULL fail\r\n");
		return FALSE;
	}

	if (handle != NULL) {
		handle->drv_tab = sensor_info->drv_tab();
		handle->ini_obj = sensor_info->drv_obj(id);
		if (handle->drv_tab == NULL) {
			DBG_ERR("input parame error 0x%.8x\r\n", handle->drv_tab);
		} else {
			return TRUE;
		}
	} else {
		DBG_ERR("input parame error 0x%.8x\r\n", handle);
	}

	return FALSE;
}

void drv_sensor_power_turn_on(SENSOR_ID id)
{
	SEN_MAP_INFO *sensor_info = drv_sensor_search(sensor_name[id]);

	if (sensor_debug_item[id] & SENSOR_DEBUG_MODELEXT) {
		debug_msg("^pqw C%s id %d\r\n", __func__, id);
	}

	if (sensor_info == NULL) {
		DBG_ERR("pqw driver get fail\r\n");
		return;
	}

	if (sensor_info->pow_ctrl == NULL) {
		DBG_IND("driver NULL\r\n");
		return;
	}

	if (sensor_chk_power_on(id)) {
		sensor_info->pow_ctrl(id, POW_CTRL_TURN_ON);
	}
}

void drv_sensor_power_turn_off(SENSOR_ID id)
{
	SEN_MAP_INFO *sensor_info = drv_sensor_search(sensor_name[id]);

	if (sensor_debug_item[id] & SENSOR_DEBUG_MODELEXT) {
		debug_msg("^C%s id %d\r\n", __func__, id);
	}

	if (sensor_info == NULL) {
		DBG_ERR("driver get fail\r\n");
		return;
	}

	if (sensor_info->pow_ctrl == NULL) {
		DBG_IND("driver NULL\r\n");
		return;
	}

	if (sensor_chk_power_off(id)) {
		sensor_info->pow_ctrl(id, POW_CTRL_TURN_OFF);
	}
}

void drv_sensor_power_save_on(SENSOR_ID id)
{
	SEN_MAP_INFO *sensor_info = drv_sensor_search(sensor_name[id]);

	if (sensor_debug_item[id] & SENSOR_DEBUG_MODELEXT) {
		debug_msg("^C%s id %d\r\n", __func__, id);
	}

	if (sensor_info == NULL) {
		DBG_ERR("driver get fail\r\n");
		return;
	}

	if (sensor_info->pow_ctrl == NULL) {
		DBG_IND("driver NULL\r\n");
		return;
	}

	if (sensor_chk_power_save_on(id)) {
		sensor_info->pow_ctrl(id, POW_CTRL_SAVE_ON);
	}
}

void drv_sensor_power_save_off(SENSOR_ID id)
{
	SEN_MAP_INFO *sensor_info = drv_sensor_search(sensor_name[id]);

	if (sensor_debug_item[id] & SENSOR_DEBUG_MODELEXT) {
		debug_msg("^C%s id %d\r\n", __func__, id);
	}

	if (sensor_info == NULL) {
		DBG_ERR("driver get fail\r\n");
		return;
	}

	if (sensor_info->pow_ctrl == NULL) {
		DBG_IND("driver NULL\r\n");
		return;
	}

	if (sensor_chk_power_save_off(id)) {
		sensor_info->pow_ctrl(id, POW_CTRL_SAVE_OFF);
	}
}

BOOL drv_sensor_det_plug_in(SENSOR_ID id)
{	//CHKPNT;

	SEN_MAP_INFO *sensor_info = drv_sensor_search(sensor_name[id]);

	if (sensor_debug_item[id] & SENSOR_DEBUG_MODELEXT) {
		debug_msg("^C%s id %d\r\n", __func__, id);
	}

	if (sensor_info == NULL) {
		DBG_ERR("pqw driver get fail\r\n");
		return FALSE;
	}

	if (sensor_info->det_plug_in == NULL) {
		DBG_IND("driver NULL\r\n");
		return FALSE;
	}

	return sensor_info->det_plug_in(id);
}

BOOL drv_sensor_plug_in_cb(SENSOR_ID id)
{
	SEN_MAP_INFO *sensor_info = drv_sensor_search(sensor_name[id]);

	if (sensor_debug_item[id] & SENSOR_DEBUG_MODELEXT) {
		debug_msg("^C%s id %d\r\n", __func__, id);
	}

	if (sensor_info == NULL) {
		DBG_ERR("driver get fail\r\n");
		return FALSE;
	}

	if (sensor_info->plug_in_cb == NULL) {
		DBG_IND("driver NULL\r\n");
		return FALSE;
	}

	return sensor_info->plug_in_cb(id);
}

/*
void drv_sensor_hot_plug_set(SENSOR_ID Id)
{
    if (SenHotPlugStatusSetTab[Id] != NULL)
    {
        SenHotPlugStatusSetTab[Id]();
    }
}

void drv_sensor_hot_plug_clear(SENSOR_ID Id)
{
    if (SenHotPlugStatusClearTab[Id] != NULL)
    {
        SenHotPlugStatusClearTab[Id]();
    }
}
*/
