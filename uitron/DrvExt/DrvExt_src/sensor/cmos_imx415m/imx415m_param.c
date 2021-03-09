/**
    General Sensor API for project layer

    Sample module detailed description.

    @file       imx415m_param.c
    @ingroup    Predefined_group_name
    @note       Nothing (or anything need to be mentioned).

    Copyright   Novatek Microelectronics Corp. 2011.  All rights reserved.
*/

#include "imx415m_param.h"
#define __MODULE__ IMX415M_PARAM
#define __DBGLVL__ 1        //ERROR mode, show err, wrn only
#define __DBGFLT__ "*"      // *=All
#include "DebugModule.h"

#define FOUR_LANE ENABLE

static SENSOR_MULTI_FRM_INFO multi_frame_info_imx415m[MODE_MAX + 1] = {
    {0, {0, 0}, 0}, // NONE
    {1, {0, 0}, SEN_BIT_OFS_NONE}, // SENSOR_MODE_1
    {1, {0, 0}, SEN_BIT_OFS_NONE}, // SENSOR_MODE_2
};

static SENSOR_MODE_MIPI mipi_mode_imx415m[MODE_MAX + 1] = {
    {0},
    {
        // 1920*1080 p60 linear 4lane
        CSI_DATLANE_NUM_4,
        SEN_MIPI_PIXDEPTH_12BIT,
        {{SEN_MIPI_MANUAL_NONE, 0}, {SEN_MIPI_MANUAL_NONE, 0}, {SEN_MIPI_MANUAL_NONE, 0},},
        FALSE
    },
    {
        // 3840*2160 p60 linear 4lane
        CSI_DATLANE_NUM_4,
        SEN_MIPI_PIXDEPTH_10BIT,
        {{SEN_MIPI_MANUAL_NONE, 0}, {SEN_MIPI_MANUAL_NONE, 0}, {SEN_MIPI_MANUAL_NONE, 0},},
        FALSE
    },    
};

static SENSOR_SIGNAL hd_trans_imx415m[MODE_MAX + 1] = {
    {0, 0, 0, 0},
    {0, 0, 6, 1920}, // 1920*1080 p60 linear 4lane
    {0, 0, 12, 3840}, // 3840*2160 p60 linear 4lane
};

static SENSOR_SIGNAL vd_trans_imx415m[MODE_MAX + 1] = {
    {0, 0, 0, 0},
    {0, 0,  7, 1080}, // 1920*1080 p60 linear 4lane
    {0, 0, 15, 2160}, // 3840*2160 p60 linear 4lane
};

static SENSOR_SIGNAL hd_sen_imx415m[MODE_MAX + 1] = {
    {0, 0, 0, 0},
    {0, 365, 0, 1932}, // 1920*1080 p60 linear 4lane         
    {0, 533, 0, 3864}, // 3840*2160 p60 linear 4lane     
};

static SENSOR_SIGNAL vd_sen_imx415m[MODE_MAX + 1] = {
    {0, 0, 0, 0},
    {0, 3288, 0, 1095}, // 1920*1080 p60 linear 4lane
    {0, 2251, 0, 2190}, // 3840*2160 p60 linear 4lane     
};

static SENSOR_MODE_INFO mode_imx415m[MODE_MAX + 1] = {
    {
        //null
        0,
        0,
        0,
        0,
        0,
        {0, 0, 0, 0, 0},
        0,
        0,
        0,
        0,
        {0, 0},
        {0, 0, 0,},
        
        {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        NULL,
        
        NULL,
        NULL,
        NULL,
        NULL,
        
        NULL,
        NULL,
        NULL,
        NULL,
    },
    {
        // 1920*1080 p60 linear mode
        SENSOR_MODE_LINEAR,
        SENSOR_STPIX_R,
        SENSOR_FMT_POGRESSIVE,
        1920,
        1080,
        {SEN_RATIO(16, 9), 1000, 1000, 1000, 1000},
        SEN_FPS(60, 1),
        72000000,
        240000000,
        200,
        {1000, 3981000},
        {0, 0, 0,},
        {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        
        &multi_frame_info_imx415m[SENSOR_MODE_1],
        {&hd_trans_imx415m[SENSOR_MODE_1], NULL},
        {&vd_trans_imx415m[SENSOR_MODE_1], NULL},
        {&hd_sen_imx415m[SENSOR_MODE_1], NULL},
        {&vd_sen_imx415m[SENSOR_MODE_1], NULL},
        
        NULL,
        &mipi_mode_imx415m[SENSOR_MODE_1],
        NULL,
        NULL,
    },
    {
        // 3840*2160 p60 linear mode
        SENSOR_MODE_LINEAR,
        SENSOR_STPIX_R,
        SENSOR_FMT_POGRESSIVE,
        3840,
        2160,
        {SEN_RATIO(16, 9), 1000, 1000, 1000, 1000},
        SEN_FPS(60, 1),
        72000000,
        288000000,
        100,
        {1000, 3981000},
        {0, 0, 0,},
        {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        
        &multi_frame_info_imx415m[SENSOR_MODE_2],
        {&hd_trans_imx415m[SENSOR_MODE_2], NULL},
        {&vd_trans_imx415m[SENSOR_MODE_2], NULL},
        {&hd_sen_imx415m[SENSOR_MODE_2], NULL},
        {&vd_sen_imx415m[SENSOR_MODE_2], NULL},
        
        NULL,
        &mipi_mode_imx415m[SENSOR_MODE_2],
        NULL,
        NULL,
    },    
};

static SEN_CMD_INFO imx415m_cmd_info = {
    {
        {SEN_I2C_W_ADDR_DFT,     0x34},
        {SEN_I2C_W_ADDR_OPTION1, 0xff},
        {SEN_I2C_W_ADDR_OPTION2, 0xff},
        {SEN_I2C_W_ADDR_OPTION3, 0xff},
        {SEN_I2C_W_ADDR_OPTION4, 0xff},
        {SEN_I2C_W_ADDR_OPTION5, 0xff},
    },
};

SEN_CMD_INFO *get_sensor_cmd_info_imx415m(void)
{
    return &imx415m_cmd_info;
}

static SEN_IF_INFO imx415m_if_info = {
    {
        NULL,
        NULL,
    },
};

SEN_IF_INFO *get_sensor_if_info_imx415m(void)
{
    return &imx415m_if_info;
}

static SENSOR_DRV_INFO g_p_imx415m_drv_info = {
    SENSOR_TYPE_CMOS,
    SENSOR_SIGNAL_MASTER,
    SENSOR_DATA_MIPI,
    {TRUE},
    {SENSOR_ACTIVE_HIGH, SENSOR_ACTIVE_HIGH, SENSOR_PHASE_RISING, SENSOR_PHASE_RISING, SENSOR_PHASE_RISING},
    {TRUE},
    {24000000},
    SENSOR_SUPPORT_MIRROR|SENSOR_SUPPORT_FLIP,
    {5602, 3178},//3864x1.45; 2192x1.45
    {3840, 2160},
    0,
    0,
};

static SENSOR_CMD imx415m_mode_1[] = {
    // MCLK=24Mhz MIPI 4lane 1080p 60FPS 12bit                                                  
    {0x3000, 1, {0x01, 0x0}}, // standby               
    {0x3002, 1, {0x01, 0x0}}, //Master mode stop  
    {0x3008, 1, {0x54, 0x0}},
    {0x300A, 1, {0x3B, 0x0}},
    {0x3020, 1, {0x01, 0x0}},
    {0x3021, 1, {0x01, 0x0}},
    {0x3022, 1, {0x01, 0x0}},
    {0x3024, 1, {0xD8, 0x0}},
    {0x3025, 1, {0x0C, 0x0}},    
    {0x3028, 1, {0x6D, 0x0}},
    {0x3029, 1, {0x01, 0x0}},
    {0x3031, 1, {0x00, 0x0}},
    {0x3032, 1, {0x01, 0x0}},    
    {0X3033, 1, {0x08, 0x0}},
    {0X3050, 1, {0x08, 0x0}},
    {0x30C1, 1,{ 0x00, 0x0}},
    {0x30D9, 1, {0x02, 0x0}},
    {0x30DA, 1, {0x01, 0x0}},
    {0x3116, 1, {0x23, 0x0}},
    {0x3118, 1, {0xB4, 0x0}},
    {0x311A, 1, {0xFC, 0x0}},
    {0x311E, 1, {0x23, 0x0}},
    //
    {0x32D4, 1, {0x21, 0x0}},
    {0x32EC, 1, {0xA1, 0x0}},
    {0x3452, 1, {0x7F, 0x0}},
    {0x3453, 1, {0x03, 0x0}},
    {0x358A, 1, {0x04, 0x0}},
    {0x35A1, 1, {0x02, 0x0}},
    {0x36BC, 1, {0x0C, 0x0}},
    {0x36CC, 1, {0x53, 0x0}},
    {0x36CD, 1, {0x00, 0x0}},
    {0x36CE, 1, {0x3C, 0x0}},
    {0x36D0, 1, {0x8C, 0x0}},
    {0x36D1, 1, {0x00, 0x0}},
    {0x36D2, 1, {0x71, 0x0}},
    {0x36D4, 1, {0x3C, 0x0}},
    {0x36D6, 1, {0x53, 0x0}},
    {0x36D7, 1, {0x00, 0x0}},
    {0x36D8, 1, {0x71, 0x0}},
    {0x36DA, 1, {0x8C, 0x0}},
    {0x36DB, 1, {0x00, 0x0}},
    {0x3701, 1, {0x00, 0x0}},
    {0x3724, 1, {0x02, 0x0}},
    {0x3726, 1, {0x02, 0x0}},
    {0x3732, 1, {0x02, 0x0}},
    {0x3734, 1, {0x03, 0x0}},
    {0x3736, 1, {0x03, 0x0}},
    {0x3742, 1, {0x03, 0x0}},
    {0x3862, 1, {0xE0, 0x0}},
    {0x38CC, 1, {0x30, 0x0}},
    {0x38CD, 1, {0x2F, 0x0}},
    {0x395C, 1, {0x0C, 0x0}},
    {0x3A42, 1, {0xD1, 0x0}},
    {0x3A4C, 1, {0x77, 0x0}},
    {0x3AE0, 1, {0x02, 0x0}},
    {0x3AEC, 1, {0x0C, 0x0}},
    {0x3B00, 1, {0x2E, 0x0}},
    {0x3B06, 1, {0x29, 0x0}},
    {0x3B98, 1, {0x25, 0x0}},
    {0x3B99, 1, {0x21, 0x0}},
    {0x3B9B, 1, {0x13, 0x0}},
    {0x3B9C, 1, {0x13, 0x0}},
    {0x3B9D, 1, {0x13, 0x0}},
    {0x3B9E, 1, {0x13, 0x0}},
    {0x3BA1, 1, {0x00, 0x0}},
    {0x3BA2, 1, {0x06, 0x0}},
    {0x3BA3, 1, {0x0B, 0x0}},
    {0x3BA4, 1, {0x10, 0x0}},
    {0x3BA5, 1, {0x14, 0x0}},
    {0x3BA6, 1, {0x18, 0x0}},
    {0x3BA7, 1, {0x1A, 0x0}},
    {0x3BA8, 1, {0x1A, 0x0}},
    {0x3BA9, 1, {0x1A, 0x0}},
    {0x3BAC, 1, {0xED, 0x0}},
    {0x3BAD, 1, {0x01, 0x0}},
    {0x3BAE, 1, {0xF6, 0x0}},
    {0x3BAF, 1, {0x02, 0x0}},
    {0x3BB0, 1, {0xA2, 0x0}},
    {0x3BB1, 1, {0x03, 0x0}},
    {0x3BB2, 1, {0xE0, 0x0}},
    {0x3BB3, 1, {0x03, 0x0}},
    {0x3BB4, 1, {0xE0, 0x0}},
    {0x3BB5, 1, {0x03, 0x0}},
    {0x3BB6, 1, {0xE0, 0x0}},
    {0x3BB7, 1, {0x03, 0x0}},
    {0x3BB8, 1, {0xE0, 0x0}},
    {0x3BBA, 1, {0xE0, 0x0}},
    {0x3BBC, 1, {0xDA, 0x0}},
    {0x3BBE, 1, {0x88, 0x0}},
    {0x3BC0, 1, {0x44, 0x0}},
    {0x3BC2, 1, {0x7B, 0x0}},
    {0x3BC4, 1, {0xA2, 0x0}},
    {0x3BC8, 1, {0xBD, 0x0}},
    {0x3BCA, 1, {0xBD, 0x0}},
    {0x4004, 1, {0x00, 0x0}},
    {0x4005, 1, {0x06, 0x0}},
    {0x4018, 1, {0x9F, 0x0}},
    {0x401A, 1, {0x57, 0x0}},
    {0x401C, 1, {0x57, 0x0}},
    {0x401E, 1, {0x87, 0x0}},
    {0x4020, 1, {0x5F, 0x0}},
    {0x4022, 1, {0xA7, 0x0}},
    {0x4024, 1, {0x5F, 0x0}},
    {0x4026, 1, {0x97, 0x0}},
    {0x4028, 1, {0x4F, 0x0}},
    {CMD_SETVD, 1, {00, 0x0}},    
    {CMD_SETAE, 1, {00, 0x0}},
    {0x3000, 1, {0x00, 0x0}},  // standby cancel
    {CMD_DELAY, 1, {25, 0x0}},
    {0x3002, 1, {0x00, 0x0}},  //Master mode start
};

static SENSOR_CMD imx415m_mode_2[] = {
    // MCLK=24Mhz MIPI 4lane 1080p 60FPS 12bit                                                  
    {0x3000, 1, {0x01, 0x0}}, // standby               
    {0x3002, 1, {0x01, 0x0}}, //Master mode stop  
    {0x3008, 1, {0x54, 0x0}},
    {0x300A, 1, {0x3B, 0x0}},
    {0x3020, 1, {0x00, 0x0}},
    {0x3021, 1, {0x00, 0x0}},
    {0x3022, 1, {0x00, 0x0}},
    {0x3024, 1, {0xCB, 0x0}},
    {0x3025, 1, {0x08, 0x0}},    
    {0x3028, 1, {0x15, 0x0}},
    {0x3029, 1, {0x02, 0x0}},
    {0x3031, 1, {0x00, 0x0}},
    {0x3032, 1, {0x00, 0x0}},    
    {0X3033, 1, {0X08, 0x0}},
    {0X3050, 1, {0x08, 0x0}},
    {0x30C1, 1, {0x00, 0x0}},
    {0x30D9, 1, {0x06, 0x0}},
    {0x30DA, 1, {0x02, 0x0}},
    {0x3116, 1, {0x23, 0x0}},
    {0x3118, 1, {0xB4, 0x0}},
    {0x311A, 1, {0xFC, 0x0}},
    {0x311E, 1, {0x23, 0x0}},
    //
    {0x32D4, 1, {0x21, 0x0}},
    {0x32EC, 1, {0xA1, 0x0}},
    {0x3452, 1, {0x7F, 0x0}},
    {0x3453, 1, {0x03, 0x0}},
    {0x358A, 1, {0x04, 0x0}},
    {0x35A1, 1, {0x02, 0x0}},
    {0x36BC, 1, {0x0C, 0x0}},
    {0x36CC, 1, {0x53, 0x0}},
    {0x36CD, 1, {0x00, 0x0}},
    {0x36CE, 1, {0x3C, 0x0}},
    {0x36D0, 1, {0x8C, 0x0}},
    {0x36D1, 1, {0x00, 0x0}},
    {0x36D2, 1, {0x71, 0x0}},
    {0x36D4, 1, {0x3C, 0x0}},
    {0x36D6, 1, {0x53, 0x0}},
    {0x36D7, 1, {0x00, 0x0}},
    {0x36D8, 1, {0x71, 0x0}},
    {0x36DA, 1, {0x8C, 0x0}},
    {0x36DB, 1, {0x00, 0x0}},
    {0x3701, 1, {0x00, 0x0}},
    {0x3724, 1, {0x02, 0x0}},
    {0x3726, 1, {0x02, 0x0}},
    {0x3732, 1, {0x02, 0x0}},
    {0x3734, 1, {0x03, 0x0}},
    {0x3736, 1, {0x03, 0x0}},
    {0x3742, 1, {0x03, 0x0}},
    {0x3862, 1, {0xE0, 0x0}},
    {0x38CC, 1, {0x30, 0x0}},
    {0x38CD, 1, {0x2F, 0x0}},
    {0x395C, 1, {0x0C, 0x0}},
    {0x3A42, 1, {0xD1, 0x0}},
    {0x3A4C, 1, {0x77, 0x0}},
    {0x3AE0, 1, {0x02, 0x0}},
    {0x3AEC, 1, {0x0C, 0x0}},
    {0x3B00, 1, {0x2E, 0x0}},
    {0x3B06, 1, {0x29, 0x0}},
    {0x3B98, 1, {0x25, 0x0}},
    {0x3B99, 1, {0x21, 0x0}},
    {0x3B9B, 1, {0x13, 0x0}},
    {0x3B9C, 1, {0x13, 0x0}},
    {0x3B9D, 1, {0x13, 0x0}},
    {0x3B9E, 1, {0x13, 0x0}},
    {0x3BA1, 1, {0x00, 0x0}},
    {0x3BA2, 1, {0x06, 0x0}},
    {0x3BA3, 1, {0x0B, 0x0}},
    {0x3BA4, 1, {0x10, 0x0}},
    {0x3BA5, 1, {0x14, 0x0}},
    {0x3BA6, 1, {0x18, 0x0}},
    {0x3BA7, 1, {0x1A, 0x0}},
    {0x3BA8, 1, {0x1A, 0x0}},
    {0x3BA9, 1, {0x1A, 0x0}},
    {0x3BAC, 1, {0xED, 0x0}},
    {0x3BAD, 1, {0x01, 0x0}},
    {0x3BAE, 1, {0xF6, 0x0}},
    {0x3BAF, 1, {0x02, 0x0}},
    {0x3BB0, 1, {0xA2, 0x0}},
    {0x3BB1, 1, {0x03, 0x0}},
    {0x3BB2, 1, {0xE0, 0x0}},
    {0x3BB3, 1, {0x03, 0x0}},
    {0x3BB4, 1, {0xE0, 0x0}},
    {0x3BB5, 1, {0x03, 0x0}},
    {0x3BB6, 1, {0xE0, 0x0}},
    {0x3BB7, 1, {0x03, 0x0}},
    {0x3BB8, 1, {0xE0, 0x0}},
    {0x3BBA, 1, {0xE0, 0x0}},
    {0x3BBC, 1, {0xDA, 0x0}},
    {0x3BBE, 1, {0x88, 0x0}},
    {0x3BC0, 1, {0x44, 0x0}},
    {0x3BC2, 1, {0x7B, 0x0}},
    {0x3BC4, 1, {0xA2, 0x0}},
    {0x3BC8, 1, {0xBD, 0x0}},
    {0x3BCA, 1, {0xBD, 0x0}},
    {0x4004, 1, {0x00, 0x0}},
    {0x4005, 1, {0x06, 0x0}},
    {0x4018, 1, {0x9F, 0x0}},
    {0x401A, 1, {0x57, 0x0}},
    {0x401C, 1, {0x57, 0x0}},
    {0x401E, 1, {0x87, 0x0}},
    {0x4020, 1, {0x5F, 0x0}},
    {0x4022, 1, {0xA7, 0x0}},
    {0x4024, 1, {0x5F, 0x0}},
    {0x4026, 1, {0x97, 0x0}},
    {0x4028, 1, {0x4F, 0x0}},
    {CMD_SETVD, 1, {00, 0x0}},    
    {CMD_SETAE, 1, {00, 0x0}},
    {0x3000, 1, {0x00, 0x0}},  // standby cancel
    {CMD_DELAY, 1, {25, 0x0}},
    {0x3002, 1, {0x00, 0x0}},  //Master mode start
};


SENSOR_MODE_INFO *get_mode_info_imx415m(SENSOR_MODE mode)
{
    if (mode >= (MODE_MAX + 1)) {
        DBG_ERR("sensor mode %d > Max(%d), force to mode 0\r\n", mode, MODE_MAX);
        //return NULL;
        mode = 0;
    }
    return &mode_imx415m[mode];
}

UINT32 get_cmd_tab_imx415m(SENSOR_MODE mode, SENSOR_CMD **cmd_tab)
{
    switch (mode) {
    case SENSOR_MODE_1:
        *cmd_tab = imx415m_mode_1;
        return sizeof(imx415m_mode_1) / sizeof(SENSOR_CMD);

    case SENSOR_MODE_2:
        *cmd_tab = imx415m_mode_2;
        return sizeof(imx415m_mode_2) / sizeof(SENSOR_CMD);

    default:
        DBG_ERR("sensor mode %d no cmd table\r\n", mode);
        *cmd_tab = NULL;
        return 0;
    }
}

SENSOR_DRV_INFO *get_sensor_drv_info_imx415m(void)
{
    return &g_p_imx415m_drv_info;
}
static UINT32 cur_frame_rate[SENSOR_ID_MAX_NUM][MODE_MAX + 1] = {0};
static UINT32 chgmode_frame_rate[SENSOR_ID_MAX_NUM][MODE_MAX + 1] = {0};
void set_sensor_cur_frame_rate_imx415m(SENSOR_ID id, SENSOR_MODE mode, UINT32 frame_rate)
{
    cur_frame_rate[id][mode] = frame_rate;
}
UINT32 get_sensor_cur_frame_rate_imx415m(SENSOR_ID id, SENSOR_MODE mode)
{
    return cur_frame_rate[id][mode];
}
void set_sensor_chgmode_frame_rate_imx415m(SENSOR_ID id, SENSOR_MODE mode, UINT32 frame_rate)
{
    chgmode_frame_rate[id][mode] = frame_rate;
    cur_frame_rate[id][mode] = frame_rate;
}
UINT32 get_sensor_chgmode_frame_rate_imx415m(SENSOR_ID id, SENSOR_MODE mode)
{
    return chgmode_frame_rate[id][mode];
}
