/**
    imx415m sensor parameter .


    @file       imx415m_param.h
    @ingroup    mISYSAlg
    @note       Nothing (or anything need to be mentioned).

    Copyright   Novatek Microelectronics Corp. 2011.  All rights reserved.
*/
#ifndef _IMX415M_PARAM_H_
#define _IMX415M_PARAM_H_

#include "sensor.h"

/**
     Sensor mode
     plz map to enum: SENSOR_MODE sequenitally and continuously
*/
#define MODE_MAX    2               //total senMode supported by this sensor

// get sensor information
extern SENSOR_DRV_TAB *sensor_get_drv_tab_imx415m(void);
extern SENSOR_DRV_INFO *get_sensor_drv_info_imx415m(void);
extern SENSOR_MODE_INFO *get_mode_info_imx415m(SENSOR_MODE mode);
extern UINT32 get_cmd_tab_imx415m(SENSOR_MODE mode, SENSOR_CMD **cmd_tab);
extern SEN_CMD_INFO *get_sensor_cmd_info_imx415m(void);
extern SEN_IF_INFO *get_sensor_if_info_imx415m(void);
// frame rate
extern void set_sensor_cur_frame_rate_imx415m(SENSOR_ID id, SENSOR_MODE mode, UINT32 frame_rate);
extern UINT32 get_sensor_cur_frame_rate_imx415m(SENSOR_ID id, SENSOR_MODE mode);
extern void set_sensor_chgmode_frame_rate_imx415m(SENSOR_ID id, SENSOR_MODE mode, UINT32 frame_rate);
extern UINT32 get_sensor_chgmode_frame_rate_imx415m(SENSOR_ID id, SENSOR_MODE mode);
#endif
