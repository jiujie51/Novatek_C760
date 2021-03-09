/**
    @file       imx415m.c

    @brief      imx415m

    @note       Nothing (or anything need to be mentioned).

    Copyright   Novatek Microelectronics Corp. 2010.  All rights reserved.
*/
//----------------------------------------------------------------------------------------------------
#include "Utility.h"
#include "imx415m_param.h"
#include "sensor_cmd_if.h"
#include "NvtVerInfo.h"

NVTVER_VERSION_ENTRY(CMOS_IMX415M, 1, 00, 000, 00)

#define __MODULE__ IMX415M
#define __DBGLVL__          2 // 0=FATAL, 1=ERR, 2=WRN, 3=UNIT, 4=FUNC, 5=IND, 6=MSG, 7=VALUE, 8=USER
#define __DBGFLT__          "*" // *=All, [mark]=CustomClass
#define __DBGFLT__ "*"      // *=All
#include "DebugModule.h"

#define MAX_VD_PERIOD         0xFFFFF
#define MIN_EXPOSURE_LINE     1
#define MIN_SHS1              1
#define MIN_GAIN_RATIO        1000
#define MAX_GAIN_RATIO        3981000 // 72dB

//Function prototype
#if 0
#endif

static UINT32 sensor_option_init[SENSOR_ID_MAX_NUM] = {0};
static UINT32 sensor_option[SENSOR_ID_MAX_NUM] = {0};
static UINT32 vd_period[SENSOR_ID_MAX_NUM] = {0}; 

static ER init_imx415m(SENSOR_ID id, SEN_DRV_INIT_OBJ *drv_init_obj);
static ER uninit_imx415m(SENSOR_ID id);
static ER sleep_imx415m(SENSOR_ID id);
static ER wakeup_imx415m(SENSOR_ID id);
static ER write_reg_imx415m(SENSOR_ID id, SENSOR_CMD *cmd);
static ER read_reg_imx415m(SENSOR_ID id, SENSOR_CMD *cmd);
static ER chg_mode_imx415m(SENSOR_ID id, SEN_CHGMODE_DATA chg_mode_data, GAIN_SETTING *preset_gain, EXPOSURE_SETTING *preset_expt);
static ER chg_fps_imx415m(SENSOR_ID id, SEN_CHGFPS_DATA chg_fps_data, GAIN_SETTING *preset_gain, EXPOSURE_SETTING *preset_expt);
static ER set_info_imx415m(SENSOR_ID id, SEN_SET_DRV_ITEM set_drv_item, void *data, SEN_COM_INFO com_info);
static ER get_info_imx415m(SENSOR_ID id, SEN_GET_DRV_ITEM get_drv_item, void *data);

//Internal API
static ER set_exptime_imx415m(SENSOR_ID id, EXPOSURE_SETTING *expt_setting, SENSOR_MODE mode);
static ER set_gain_imx415m(SENSOR_ID id, GAIN_SETTING *gain, SENSOR_MODE mode);
static ER set_vd_period_register_imx415m(SENSOR_ID id, SENSOR_MODE mode, UINT32 frame_rate);
static ER set_grs_mode_imx415m(SENSOR_ID id, void *param);
static ER set_hdr_info_imx415m(SENSOR_ID id, HDR_SET_INFO *hdr_info);
static ER set_user_option_imx415m(SENSOR_ID id, UINT32 option);
static ER get_expo_setting_imx415m(SENSOR_ID id, SENSOR_MODE mode, UINT32 *exp_time, EXPOSURE_SETTING *expt_setting, UINT32 *compensate_ratio);
static ER get_gain_setting_imx415m(SENSOR_ID id, SENSOR_MODE mode, UINT32 *gain_ratio, GAIN_SETTING *gain);
static ER get_min_shr0_imx415m(SENSOR_MODE mode, UINT32 *min_shr0, UINT32 *min_exp);

static SENSOR_DRV_TAB m_p_imx415m_drv = {
    init_imx415m,
    uninit_imx415m,
    sleep_imx415m,
    wakeup_imx415m,
    write_reg_imx415m,
    read_reg_imx415m,
    chg_mode_imx415m,
    chg_fps_imx415m,
    set_info_imx415m,
    get_info_imx415m,
    get_sensor_drv_info_imx415m,
    get_mode_info_imx415m,
    get_sensor_cmd_info_imx415m,
    get_sensor_if_info_imx415m,
};

#if 0
-Intern api
#endif

static SENSOR_CMD set_cmd_info_imx415m(UINT32 addr, UINT32 data_length, UINT32 data0, UINT32 data1)
{
    SENSOR_CMD cmd;
    cmd.ui_addr = addr;
    cmd.ui_data_len = data_length;
    cmd.ui_data[0] = data0;
    cmd.ui_data[1] = data1;
    return cmd;
}

static ER init_imx415m(SENSOR_ID id, SEN_DRV_INIT_OBJ *drv_init_obj)
{
    sensor_option_init[id] = drv_init_obj->option;
    sensor_option[id] = 0;
    
    return E_OK;
}

static ER uninit_imx415m(SENSOR_ID id)
{
    UINT32 cnt;
    for (cnt = 0; cnt < MODE_MAX; cnt++) {
        set_sensor_chgmode_frame_rate_imx415m(id, cnt, 0);
    }
    return E_OK;
}

static ER sleep_imx415m(SENSOR_ID id)
{
    return E_OK;
}

static ER wakeup_imx415m(SENSOR_ID id)
{
    return E_OK;
}

static ER write_reg_imx415m(SENSOR_ID id, SENSOR_CMD *cmd)
{
    //Use command interface (sif/i2c/io/...) write register to sensor
    ER rt = E_OK;
    rt = sensor_cmd_get_i2c_ctrl_obj()->write(id, cmd->ui_addr, cmd->ui_data[0], I2CFMT_2B1B);
    
    //re try
    if (rt != E_OK) {
        rt = sensor_cmd_get_i2c_ctrl_obj()->write(id, cmd->ui_addr, cmd->ui_data[0], I2CFMT_2B1B);
    }
    
    return rt;
}

static ER read_reg_imx415m(SENSOR_ID id, SENSOR_CMD *cmd)
{
    //Use command interface (i2c/io/...) read register from sensor
    ER rt = E_OK;
    rt = sensor_cmd_get_i2c_ctrl_obj()->read(id, cmd->ui_addr, &(cmd->ui_data[0]), I2CFMT_2B1B);
    return rt;
}

static ER chg_mode_imx415m(SENSOR_ID id, SEN_CHGMODE_DATA chg_mode_data, GAIN_SETTING *preset_gain, EXPOSURE_SETTING *preset_expt)
{
    SENSOR_CMD *p_cmd_list = NULL, cmd;
    UINT32 idx, cmd_num = 0;
    ER rt = E_OK;
    
    //Set sensor frame rate
    set_sensor_chgmode_frame_rate_imx415m(id, chg_mode_data.mode, chg_mode_data.frame_rate);
    rt |= set_vd_period_register_imx415m(id, chg_mode_data.mode, chg_mode_data.frame_rate); // set register to change VD period
    if (rt != E_OK) {
        DBG_ERR("set_vd_period_register_imx415m error %d\r\n", rt);
    }
    
    //Get & set sensor cmd table
    cmd_num = get_cmd_tab_imx415m(chg_mode_data.mode, &p_cmd_list);
    if (p_cmd_list == NULL) {
        DBG_ERR("%s: SenMode(%d) out of range!!!\r\n", __func__, chg_mode_data.mode);
        return E_SYS;
    }
    
    for (idx = 0; idx < cmd_num; idx ++) {
        if (p_cmd_list[idx].ui_addr == CMD_DELAY) {
            Delay_DelayUs((p_cmd_list[idx].ui_data[0] & 0xFF) << 10);
        } else if (p_cmd_list[idx].ui_addr == CMD_SETAE) {
            if ((preset_gain != NULL) && (preset_expt != NULL)) {
                ER rt_prset = E_OK;
                rt_prset |= set_exptime_imx415m(id, preset_expt, chg_mode_data.mode);
                rt_prset |= set_gain_imx415m(id, preset_gain, chg_mode_data.mode);
                
                if (rt_prset != E_OK) {
                    DBG_WRN("preset error %d\r\n", rt_prset);
                }
            }
        } else if (p_cmd_list[idx].ui_addr == CMD_SETVD) {
            cmd = set_cmd_info_imx415m(0x3024, 1, vd_period[id] & 0xFF, 0x00);
            rt |= write_reg_imx415m(id, &cmd);
            cmd = set_cmd_info_imx415m(0x3025, 1, (vd_period[id] >> 8 )& 0xFF, 0x00);
            rt |= write_reg_imx415m(id, &cmd);
            cmd = set_cmd_info_imx415m(0x3026, 1, (vd_period[id] >> 16)& 0x0F, 0x00);
            rt |= write_reg_imx415m(id, &cmd);			
        } else {
            cmd = set_cmd_info_imx415m(p_cmd_list[idx].ui_addr, p_cmd_list[idx].ui_data_len, p_cmd_list[idx].ui_data[0], p_cmd_list[idx].ui_data[1]);
            rt |= write_reg_imx415m(id, &cmd);
        }
    }
    
    if (rt != E_OK) {
        DBG_ERR("write register error %d\r\n", rt);
        return rt;
    }
    
    //Set user option
    rt = set_user_option_imx415m(id, sensor_option[id]);
    if (rt != E_OK) {
        DBG_ERR("set user option error %d\r\n", rt);
        return rt;
    }

    return E_OK;
}

static ER chg_fps_imx415m(SENSOR_ID id, SEN_CHGFPS_DATA chg_fps_data, GAIN_SETTING *preset_gain, EXPOSURE_SETTING *preset_expt)
{
    ER rt = E_OK;
    SENSOR_CMD cmd;

    set_sensor_chgmode_frame_rate_imx415m(id, chg_fps_data.mode, chg_fps_data.frame_rate);
    set_vd_period_register_imx415m(id, chg_fps_data.mode, chg_fps_data.frame_rate);

    if ( preset_expt != NULL ) { 
        rt |= set_exptime_imx415m(id, preset_expt, chg_fps_data.mode);
    } else {
        cmd = set_cmd_info_imx415m(0x3024, 1, vd_period[id] & 0xFF, 0x0);
        rt |= write_reg_imx415m(id, &cmd);    	
        cmd = set_cmd_info_imx415m(0x3025, 1, ( vd_period[id] >> 8 ) & 0xFF, 0x0);
        rt |= write_reg_imx415m(id, &cmd); 
        cmd = set_cmd_info_imx415m(0x3026, 1, ( vd_period[id] >> 16 )& 0x0F, 0x0);
        rt |= write_reg_imx415m(id, &cmd);                 
    }
    
    if ( preset_gain != NULL ) { 
        rt |= set_gain_imx415m(id, preset_gain, chg_fps_data.mode);
    }

    if (rt != E_OK) {
        DBG_ERR("chg fps error\r\n");
    }
    
    return rt;
}

static ER set_info_imx415m(SENSOR_ID id, SEN_SET_DRV_ITEM set_drv_item, void *data, SEN_COM_INFO com_info)
{
    ER rt = E_OK;
    GAIN_SETTING *gain_setting;
    SEN_PRESET_MODE *sen_preset_mode;
    
    if (set_drv_item >= SEN_SET_DRV_MAX_NUM) {
        DBG_ERR("%s: sensor set item(%d) out of range!!!\r\n", __func__, set_drv_item);
        return E_SYS;
    }
    
    switch (set_drv_item) {
    case SEN_SET_DRV_EXPT:
        rt |= set_exptime_imx415m(id, (EXPOSURE_SETTING *)(data), com_info.cur_sensor_mode);
        break;
    case SEN_SET_DRV_GAIN:
        gain_setting = (GAIN_SETTING *)data;
        rt |= set_gain_imx415m(id, gain_setting, com_info.cur_sensor_mode);
        break;
    case SEN_SET_DRV_USER_OPTION:
        rt |= set_user_option_imx415m(id, *((UINT32 *)data));
        break;
    case SEN_SET_DRV_GRS:
        rt |= set_grs_mode_imx415m(id, (void *)(data));
        break;
    case SEN_SET_DRV_HDR_INFO:
        rt |= set_hdr_info_imx415m(id, (HDR_SET_INFO *)data);
        break;
    case SEN_SET_DRV_TRIGGER_EVENT:
        break;       
    case SEN_SET_DRV_PRESET_MODE:
        sen_preset_mode = (SEN_PRESET_MODE *)data;
        set_sensor_chgmode_frame_rate_imx415m(id, com_info.cur_sensor_mode, sen_preset_mode->frame_rate);
        break;
    case SEN_SET_DRV_PRESET_LOCK:
        break;
    default:
        DBG_ERR("sensor set drv item error %d\r\n", set_drv_item);
        rt = E_PAR;
        break;
    }
    
    if (rt != E_OK) {
        DBG_ERR("sensor set drv item %d information error %d\r\n", set_drv_item, rt);
    }
    
    return rt;
}

static ER get_info_imx415m(SENSOR_ID id, SEN_GET_DRV_ITEM get_drv_item, void *data)
{
    ER rt = E_OK;
    EXPOSURE_PARAM *exp_param;
    GAIN_PARAM *gain_param;
    GAIN_INFO *gain_info;
    UINT32 *value;
    CHAR *name;
    FPS_PARAM *fps_param;
    UINT32 buf_cnt = 0;
    SENSOR_MODE_INFO *mode_info;
    
    switch (get_drv_item) {
    case SEN_GET_DRV_EXPO_SETTING:
        exp_param = (EXPOSURE_PARAM *)data;
        rt |= get_expo_setting_imx415m(id, exp_param->sensor_mode, &exp_param->exp_time[0], &exp_param->exp_setting, &exp_param->compensate_ratio[0]);
        break;
    case SEN_GET_DRV_GAIN_SETTING:
        gain_param = (GAIN_PARAM *)data;
        gain_param->gain_setting.rev[0] = gain_param->gain_ratio[0];
        gain_param->gain_setting.rev[1] = gain_param->gain_ratio[1];
        rt |= get_gain_setting_imx415m(id, gain_param->sensor_mode, &gain_param->gain_ratio[0], &gain_param->gain_setting);
        break;
    case SEN_GET_DRV_GAIN_INFO:
        gain_info = (GAIN_INFO *)data;
        gain_info->gain = get_mode_info_imx415m(gain_info->sensor_mode)->gain;
        break;
    case SEN_GET_DRV_NAME:
        name = (CHAR *)data;
        buf_cnt = snprintf(NULL, 0, "%s", __xstring(__section_name__));
        snprintf((char *)name, buf_cnt + 1, "%s", __xstring(__section_name__));
        break;
    case SEN_GET_DRV_FPS:
        fps_param = (FPS_PARAM *)data;
        mode_info = get_mode_info_imx415m(fps_param->sensor_mode);
        fps_param->cur_fps = get_sensor_cur_frame_rate_imx415m(id, fps_param->sensor_mode);
        fps_param->chg_fps = get_sensor_chgmode_frame_rate_imx415m(id, fps_param->sensor_mode);
        fps_param->dft_fps = mode_info->frame_rate;
        break;
    case SEN_GET_DRV_MAX_MODE:
        value = (UINT32 *)data;
        *value = MODE_MAX;
        break;
    case SEN_GET_DRV_USER_OPTION:
        value = (UINT32 *)data;
        *value = (UINT32)(sensor_option[id]);
        break;
    case SEN_GET_DRV_TEMP:
        DBG_WRN("not suppor get sensor temperature\r\n");
        value = (UINT32 *)data;
        *value = (UINT32)(0);
        break;
    case SEN_GET_DRV_HDR_STATUS:
        DBG_WRN("not suppor get sensor hdr status\r\n");
        value = (UINT32 *)data;
        *value = (UINT32)(0);
        break;
    default:
        DBG_ERR("sensor get drv item error %d\r\n", get_drv_item);
        rt = E_PAR;
        break;
    }
    
    if (rt != E_OK) {
        DBG_ERR("sensor get drv item %d information error %d\r\n", get_drv_item, rt);
    }
    
    return rt;
}

static ER get_min_shr0_imx415m(SENSOR_MODE mode, UINT32 *min_shr0, UINT32 *min_exp)
{
    ER rt = E_OK;

    switch (mode) {
    case SENSOR_MODE_1:
    case SENSOR_MODE_2:    	
        *min_shr0 = 8;
        *min_exp = 4;
        break;

    default:
        DBG_ERR("sensor mode %d no cmd table\r\n", mode);
        break;
    }
	return rt;
}

/**
 * Sensor internal AE driver API
*/
static UINT32 calc_expo_setting_imx415m(SENSOR_ID id, UINT32 exp_time, EXPOSURE_SETTING *expt_setting, SENSOR_MODE mode, UINT32 multi_frame_num)
{
    SENSOR_MODE_INFO *mode_info;
    UINT32 compensate_ratio;
    UINT32 t_row, exp_line, real_exp_time;
    UINT32 min_exp_line = 0, min_shr0 = 0;
    
    mode_info = get_mode_info_imx415m(mode);

    //Calculates the exposure setting
    t_row = 10 * (mode_info->sen_hd[multi_frame_num]->period) / ( mode_info->pclk / 1000000 );
    exp_line = exp_time * 10 / t_row ;
    
    get_min_shr0_imx415m(mode, &min_shr0, &min_exp_line);    
      
    //Limit minimun exposure line
    if (exp_line < min_exp_line) {
        exp_line = min_exp_line;
    }
  
    //Collect the exposure setting
    expt_setting->ui_frame[0] = 0;
    expt_setting->ui_line[0] = exp_line;
    expt_setting->ui_pixel[0] = 0;
    
    //Calculates compensation ratio
    real_exp_time = exp_line * t_row / 10;

    
    if (real_exp_time < exp_time) {
        compensate_ratio = (exp_time * 100) / real_exp_time;
    } else {
        compensate_ratio = 100;
    }

    return compensate_ratio;
}

static ER get_expo_setting_imx415m(SENSOR_ID id, SENSOR_MODE mode, UINT32 *exp_time, EXPOSURE_SETTING *expt_setting, UINT32 *compensate_ratio)
{
    SENSOR_MODE_INFO *mode_info;
    UINT32 frame_cnt, total_frame;
    
    mode_info = get_mode_info_imx415m(mode);
    
    //Get total frame number
    if (mode_info->multi_frame_info != NULL) {
        total_frame = mode_info->multi_frame_info->total_frm;
        if (total_frame == 0) {
            DBG_WRN("total_frame = 0, force to 1 \r\n");
            total_frame = 1;
        }
    } else {
        total_frame = 1;
    }
    
    //Calculates and set the exposure setting
    for (frame_cnt = 0; frame_cnt < total_frame; frame_cnt++) {
        EXPOSURE_SETTING exp_time_set = {0};
        compensate_ratio[frame_cnt] = calc_expo_setting_imx415m(id, exp_time[frame_cnt], &(exp_time_set), mode, frame_cnt);
        expt_setting->ui_frame[frame_cnt] = exp_time_set.ui_frame[0];
        expt_setting->ui_line[frame_cnt] = exp_time_set.ui_line[0];
        expt_setting->ui_pixel[frame_cnt] = exp_time_set.ui_pixel[0];
        DBG_IND("Time = %d %d %d %d\r\n", exp_time[frame_cnt], expt_setting->ui_frame[frame_cnt], expt_setting->ui_line[frame_cnt], expt_setting->ui_pixel[frame_cnt]);
    }
    return E_OK;
}

static UINT32 ratio_log_2(UINT32 devider, UINT32 devident)
{
    UINT32 ratio;
    UINT32 idx, ret;
    static UINT32 log_tbl[20] = {104, 107, 112, 115, 119, 123, 128, 131, 137, 141, 147, 152, 157, 162, 168, 174, 181, 187, 193, 200};
    
    if (devident == 0) {
        return 1;
    }
    
    ret = 0;
    ratio = (devider * 100) / devident;
    while (ratio >= 200) {
        ratio /= 2;
        ret += 100;
    }
    
    for (idx = 0; idx < 20; idx ++) {
        if (ratio < log_tbl[idx]) {
            break;
        }
    }
    
    return ret + idx * 5;
}

static ER calc_gain_setting_imx415m(SENSOR_ID id, UINT32 gain_ratio, GAIN_SETTING *gain)
{
    UINT32 analog_gain;

    if ( MIN_GAIN_RATIO > gain_ratio ) {
        gain_ratio = MIN_GAIN_RATIO;
    } else if ( MAX_GAIN_RATIO < gain_ratio ) {
        gain_ratio = MAX_GAIN_RATIO;
    }
    
    //Calculates the analog gain
    analog_gain = (6 * ratio_log_2(gain_ratio, 1000)) / 30;
    if (analog_gain > (0xF0)) {
        DBG_ERR("gain overflow gain_ratio = %d gain->data1[0] = 0x%.8x\r\n", gain_ratio, gain->data1[0]);
        analog_gain = 0xF0;
    }

	//Collect the gain setting
	gain->data1[0] = analog_gain;
	gain->data2[0] = SEN_IGNORE;
	gain->data3[0] = SEN_IGNORE;

	return E_OK;
}

static ER get_gain_setting_imx415m(SENSOR_ID id, SENSOR_MODE mode, UINT32 *gain_ratio, GAIN_SETTING *gain)
{
    SENSOR_MODE_INFO *mode_info;
    UINT32 frame_cnt, total_frame;
    
    mode_info = get_mode_info_imx415m(mode);
    
    if (mode_info->multi_frame_info != NULL) {
        total_frame = mode_info->multi_frame_info->total_frm;
    } else {
        total_frame = 1;
    }
    
    for (frame_cnt = 0; frame_cnt < total_frame; frame_cnt++) {
        GAIN_SETTING analog_gain = {0};
        calc_gain_setting_imx415m(id, gain_ratio[frame_cnt], &(analog_gain));
        gain->data1[frame_cnt] = analog_gain.data1[0];
        gain->data2[frame_cnt] = analog_gain.data2[0];
        gain->data3[frame_cnt] = analog_gain.data3[0];
        DBG_IND("gain_ratio = %d 0x%.8x 0x%.8x 0x%.8x\r\n", gain_ratio[frame_cnt], gain->data1[frame_cnt], gain->data2[frame_cnt], gain->data3[frame_cnt]);
    }
    return E_OK;
}

static ER set_exptime_imx415m(SENSOR_ID id, EXPOSURE_SETTING *expt_setting, SENSOR_MODE mode)
{
    SENSOR_CMD cmd;
    SENSOR_DRV_INFO *sensor_drv_info;
    SENSOR_MODE_INFO *mode_info;
    UINT32 exp_time = 0, sen_vd_period = 0, cur_fps = 0, new_fps = 0;
    UINT32 t_row = 0, sen_temp_vd = 0;
    UINT32 min_exp_line = 0, min_shr0 = 0, shr0 = 0;
    ER rt = E_OK;
    
    //Get information
    mode_info = get_mode_info_imx415m(mode);
    
    if (mode_info == NULL) {
        DBG_ERR("Sensor mode(%d) overflow\r\n", mode);
        return E_SYS;
    }
    
    sensor_drv_info = get_sensor_drv_info_imx415m();

    if (sensor_drv_info == NULL) {
        DBG_ERR("get sensor info error\r\n");
        return E_SYS;
    }
    
    //Set frame rate
    if (get_sensor_chgmode_frame_rate_imx415m(id, mode) != 0) {
        SEN_FPS_CONV_PARAM(mode_info->sen_vd[0]->period, &sen_vd_period
        , mode_info->frame_rate, get_sensor_chgmode_frame_rate_imx415m(id, mode));
    } else {
        sen_vd_period = mode_info->sen_vd[0]->period;
    }
    
    t_row = 10 * (mode_info->sen_hd[0]->period) /( mode_info->pclk / 1000000 );
                                                                                 
    exp_time = t_row * (expt_setting->ui_line[0]) / 10; 

    SEN_FPS_EXPT_CONV2_FMT(exp_time, &new_fps);
    cur_fps = (SEN_FPS_FMT_CONV2_REAL(new_fps) < (SEN_FPS_FMT_CONV2_REAL(get_sensor_chgmode_frame_rate_imx415m(id, mode)))) ? new_fps : get_sensor_chgmode_frame_rate_imx415m(id, mode);
    SEN_FPS_CONV_PARAM(mode_info->sen_vd[0]->period, &sen_temp_vd, mode_info->frame_rate, cur_fps);
    
    set_sensor_cur_frame_rate_imx415m(id, mode, cur_fps);

    if ( sen_temp_vd > sen_vd_period ) {
        sen_vd_period = sen_temp_vd;
    }

    if ( sen_vd_period > MAX_VD_PERIOD ) {
        DBG_ERR("max vmax overflow\r\n");    	
        sen_vd_period = MAX_VD_PERIOD;
    }

    get_min_shr0_imx415m(mode, &min_shr0, &min_exp_line);      
    
    if (expt_setting->ui_line[0] > (MAX_VD_PERIOD - min_shr0)) {
        DBG_ERR("max line overflow\r\n");
        expt_setting->ui_line[0] = (MAX_VD_PERIOD - min_shr0);
    }

    //Calculate shr0
    if (expt_setting->ui_line[0] > (sen_vd_period - min_shr0 )) {
        shr0 = min_shr0;
    } else {
        shr0 = sen_vd_period - (expt_setting->ui_line[0]);
    }
    
    //Set vmax to sensor 
    cmd = set_cmd_info_imx415m(0x3024, 1, sen_vd_period & 0xFF, 0);
    rt |= write_reg_imx415m(id, &cmd);
    cmd = set_cmd_info_imx415m(0x3025, 1, (sen_vd_period >> 8)& 0xFF, 0);
    rt |= write_reg_imx415m(id, &cmd);
    cmd = set_cmd_info_imx415m(0x3026, 1, (sen_vd_period >> 16)& 0x0F, 0);
    rt |= write_reg_imx415m(id, &cmd);
    
    //Set exposure line to sensor (shr0)
    cmd = set_cmd_info_imx415m(0x3050, 1, shr0 & 0xFF, 0);
    rt |= write_reg_imx415m(id, &cmd);
    cmd = set_cmd_info_imx415m(0x3051, 1, (shr0 >> 8) & 0xFF, 0);
    rt |= write_reg_imx415m(id, &cmd);
    cmd = set_cmd_info_imx415m(0x3052, 1, (shr0 >> 16) & 0x0F, 0);
    rt |= write_reg_imx415m(id, &cmd);

    if (rt != E_OK) {
        DBG_ERR("write register error %d\r\n", rt);
    }
    
    return rt;
}

static ER set_gain_imx415m(SENSOR_ID id, GAIN_SETTING *gain, SENSOR_MODE mode)
{
    SENSOR_MODE_INFO *mode_info;
    SENSOR_CMD cmd;
    ER rt = E_OK;
    
    //Get mode information
    mode_info = get_mode_info_imx415m(mode);

    //Set analog gain
    cmd = set_cmd_info_imx415m(0x3090, 1, gain->data1[0] & 0xFF, 0x0);
    write_reg_imx415m(id, &cmd);

    cmd = set_cmd_info_imx415m(0x3091, 1, (gain->data1[0] >> 8) & 0x01, 0x0);
    write_reg_imx415m(id, &cmd);

    if (rt != E_OK) {
        DBG_ERR("write register error%d\r\n", rt);
    }

    return rt;
}

/**
 * Sensor internal API
*/
static ER set_vd_period_register_imx415m(SENSOR_ID id, SENSOR_MODE mode, UINT32 frame_rate)
{
    ER rt = E_OK;
    SENSOR_MODE_INFO *mode_info;
    SENSOR_DRV_INFO *sensor_drv_info;
    UINT32 new_vd_period, update_input_frame_rate, target_fps_src = 30, target_fps_div = 1, ori_fps_src = 30, ori_fps_div = 1;
    
    sensor_drv_info = get_sensor_drv_info_imx415m();
    if (!sensor_drv_info->fps_info.support_chg) {
        DBG_ERR("sensor driver not support frame rate adjust\r\n");
        return E_SYS;
    }
    
    //Get previous information
    mode_info = get_mode_info_imx415m(mode);
    
    //Convert frame rate format
    SEN_FPS_CONV2_DIV(frame_rate, &target_fps_src, &target_fps_div);
    SEN_FPS_CONV2_DIV(mode_info->frame_rate, &ori_fps_src, &ori_fps_div);
    if ( target_fps_div < 1 ) target_fps_div = 1;
    if ( ori_fps_div < 1 ) ori_fps_div = 1;
    if (( target_fps_src << 10 / target_fps_div ) > ( ori_fps_src << 10 / ori_fps_div )) {
        frame_rate = mode_info->frame_rate;
    }
    SEN_FPS_CONV_PARAM(mode_info->sen_vd[0]->period, &new_vd_period, mode_info->frame_rate, frame_rate);
    
    if (new_vd_period > MAX_VD_PERIOD) {
        new_vd_period = MAX_VD_PERIOD;
        update_input_frame_rate = (mode_info->sen_vd[0]->period * (ori_fps_src * target_fps_div)) / new_vd_period;
        DBG_ERR("frame rate %d/%d out of sensor driver range, force to sensor driver min support fps %d\r\n"
        , target_fps_src, target_fps_div, update_input_frame_rate);
    }
	
    //Set new period
    vd_period[id] = new_vd_period;
    
    return rt;
}

static ER set_grs_mode_imx415m(SENSOR_ID id, void *param)
{
    return E_OK;
}

static ER set_hdr_info_imx415m(SENSOR_ID id, HDR_SET_INFO *hdr_info)
{
    return E_OK;
}

static ER set_user_option_imx415m(SENSOR_ID id, UINT32 option)
{
    ER rt = E_OK;
    SENSOR_CMD cmd;
    UINT32 cmd_value, init_option = sensor_option_init[id];
    
    //Set sensor option
    if (option & SENSOR_OPTION_MIRROR) {
        sensor_option[id] |= SENSOR_OPTION_MIRROR;
    } else {
        sensor_option[id] &= ~SENSOR_OPTION_MIRROR;
    }
    
    if (option & SENSOR_OPTION_FLIP) {
        sensor_option[id] |= SENSOR_OPTION_FLIP;
    } else {
        sensor_option[id] &= ~SENSOR_OPTION_FLIP;
    }
    
    //Set sensor register
    cmd = set_cmd_info_imx415m(0x3030, 1, 0x0, 0x0);
    rt |= read_reg_imx415m(id, &cmd);
    cmd_value = cmd.ui_data[0];
    
    if ((option & SENSOR_OPTION_MIRROR) != (init_option & SENSOR_OPTION_MIRROR)) {
        cmd_value = cmd_value | 0x01;
    } else {
        cmd_value = cmd_value & (~0x01);
    }
    
    if ((option & SENSOR_OPTION_FLIP) != (init_option & SENSOR_OPTION_FLIP)) {
        cmd_value = cmd_value | 0x02;
    } else {
        cmd_value = cmd_value & (~0x02);
    }
    
    cmd = set_cmd_info_imx415m(0x3030, 1, cmd_value, 0x0);
    rt |= write_reg_imx415m(id, &cmd);
    
    if (rt != E_OK) {
        DBG_ERR("set sensor option error %d\r\n", rt);
    }
    
    return E_OK;
}

#if 0
-Extern api
#endif
SENSOR_DRV_TAB *sensor_get_drv_tab_imx415m(void)
{
	return &m_p_imx415m_drv;
}
//@@ EOF
