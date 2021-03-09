/**
    AWB parameter

    @file       awb_gc2053_param_evb
    @ingroup    LibExt\AWB
    @note       Nothing (or anything need to be mentioned).

    Copyright   Novatek Microelectronics Corp. 2017.  All rights reserved.
*/
#include "Type.h"
#include "awb_alg.h"
#include "awb_api.h"
#include "awb_proc_nvt_int.h"
#include "pq_obj_drv.h"
/******************************/
/**
    AWB CA infomation definition
*/
static awb_ca_info awb_param_ca_info = {
    AWB_WIN, AWB_WIN,
    4095,       //UINT32 RGu; (R/G)<<10
    0,          //UINT32 RGl;
    4095,       //UINT32 GGu; G (12Bits)
    0,          //UINT32 GGl;
    4095,       //UINT32 BGu; (B/G)<<10
    0,          //UINT32 BGl;
    4095,       //UINT32 RBGu;((R+B)/G)<<9
    0           //UINT32 RBGl;
};

/******************************/
/**
    CT gain
*/
#define AWB_CT_2300K_RGAIN 270
#define AWB_CT_2300K_BGAIN 835
	
#define AWB_CT_2800K_RGAIN 287
#define AWB_CT_2800K_BGAIN 822
	
#define AWB_CT_3700K_RGAIN 330
#define AWB_CT_3700K_BGAIN 700
	
#define AWB_CT_5000K_RGAIN 412
#define AWB_CT_5000K_BGAIN 692
	
#define AWB_CT_6500K_RGAIN 475
#define AWB_CT_6500K_BGAIN 448
	
#define AWB_CT_11000K_RGAIN 660
#define AWB_CT_11000K_BGAIN 420



/******************************/
/**
    AWB Method1 table
*/
	/*awb_th_y=5,245
	awb_th_rpb=150,358
	awb_th_rsb=-37,161
	awb_th_r2g=94,230
	awb_th_b2g=56,128
	awb_th_rmb=79,109*/

/*awb_th_y=10,255
awb_th_rpb=150,358
awb_th_rsb=-37,161
awb_th_r2g=84,230
awb_th_b2g=56,120
awb_th_rmb=62,114
*/

static awb_th awb_param_th = {
    4,   //YL
    255, //YU
    116,   //RpBL
    200, //RpBU
    -20,//RsBL
    89, //RsBU
    56,   //R2GL
    138, //R2GU
    30,   //B2GL
    90, //B2GU
    21,   //RmBL
    80, //RmBU
};

/******************************/
/**
    LV check
*/
static awb_lv_check awb_param_lv_check[AWB_TUNING_CHECK_LV_MAX] = {
    { 400,  100}, //LV4~LV1
    {1000,  600}, //LV10~LV6
    {2600, 1400}, //LV14~LV26
};

/******************************/
/**
    CT Weight
*/
static ctmp_lut ct_map[AWB_TUNING_CT_WEIGHT_MAX] = {
    { 751, 2300},
    { 824, 2800},
    { 931, 3700},
    { 999, 4700},
    {1044, 6500},
    {1184, 11000},
};
static cwgt_lut out_cwgt_lut[AWB_TUNING_CT_WEIGHT_MAX] = {
    { 2300, 0},
    { 2800, 0},
    { 3700, 1},
    { 4700, 1},
    { 6500, 1},
    {11000, 0},
};
static cwgt_lut in_cwgt_lut[AWB_TUNING_CT_WEIGHT_MAX] = {
    { 2300, 1},
    { 2800, 1},
    { 3700, 1},
    { 4700, 1},
    { 6500, 1},
    {11000, 1},
};
static cwgt_lut night_cwgt_lut[AWB_TUNING_CT_WEIGHT_MAX] = {
    { 2300, 1},
    { 2800, 1},
    { 3700, 1},
    { 4700, 1},
    { 6500, 1},
    {11000, 1},
};

static awb_ct_weight awb_param_ct_weight = {
    ct_map,
    out_cwgt_lut,
    in_cwgt_lut,
    night_cwgt_lut,
};

/******************************/
/**
    AWB target
*/
static awb_target awb_param_target[AWB_TUNING_TARGET_MAX] = {
    //cx rgratio bgratio
    { 824, 1024, 1024}, //L-CT
    { 999, 1024, 1024}, //M-CT
    {1044, 1024, 1024}, //H-CT
};

/******************************/
/**
    AWB CT table
    The color temperature must be steeled sort
*/
static awb_ct_info awb_param_ct_table[AWB_TUNING_CT_MAX] = {
    {2300, { AWB_CT_2300K_RGAIN,  256, AWB_CT_2300K_BGAIN}},
    {2800, { AWB_CT_2800K_RGAIN,  256, AWB_CT_2800K_BGAIN}},
    {3700, { AWB_CT_3700K_RGAIN,  256, AWB_CT_3700K_BGAIN}},
    {5000, { AWB_CT_5000K_RGAIN,  256, AWB_CT_5000K_BGAIN}},
    {6500, { AWB_CT_6500K_RGAIN,  256, AWB_CT_6500K_BGAIN}},
    {11000, { AWB_CT_11000K_RGAIN,  256, AWB_CT_11000K_BGAIN}},
};

/******************************/
/**
    AWB Manual table
*/
static awb_mwb_gain awb_param_mwb_tab[AWB_TUNING_MWB_MAX] = {
    { 256, 256, 256},
    { AWB_CT_2300K_RGAIN, 256, AWB_CT_2300K_BGAIN},
    { AWB_CT_2800K_RGAIN, 256, AWB_CT_2800K_BGAIN},   //AWB_MODE_Alight
    { AWB_CT_3700K_RGAIN, 256, AWB_CT_3700K_BGAIN},   //AWB_MODE_TL84
    { AWB_CT_5000K_RGAIN, 256, AWB_CT_5000K_BGAIN},
    { AWB_CT_6500K_RGAIN, 256, AWB_CT_6500K_BGAIN},   //AWB_MODE_DAYLIGHT
    { AWB_CT_11000K_RGAIN, 256, AWB_CT_11000K_BGAIN},
    { 256, 256, 256},
    { 256, 256, 256},
    { 256, 256, 256},
    { 256, 256, 256},
    { 256, 256, 256}
};

static awb_converge awb_converge_par = {
      4, //skip frame
     10, //speed
    259, //tolerance 259/256=1%
};

/*
120,   //RpBL
192, //RpBU
*/
/*
4,	 //YL
255, //YU
120,   //RpBL
192, //RpBU
-37,//RsBL
101, //RsBU
51,   //R2GL
138, //R2GU
30,   //B2GL
80, //B2GU
21,   //RmBL
72, //RmBU

*/
/*
typedef struct _awb_expand_block {
	UINT32 mode; //0:disable, 1:add, 2:remove
	UINT32 lv_l;
	UINT32 lv_h;
	INT32 y_l;
	INT32 y_u;
	INT32 rpb_l;
	INT32 rpb_u;
	INT32 rsb_l;
	INT32 rsb_u;
} awb_expand_block;

*/

static awb_expand_block awb_expand_block_par[AWB_TUNING_BLOCK_MAX] = {
    { 2, 1000, 2600, 4, 256, 116, 128, -20, 89},
    { 2, 1000, 2600,  4, 256,  116, 200, 72, 89},
    { 2, 800, 1000,  4,  256,  116,  120, -20, 89},
    { 0, 600, 1000, 5, 245, 114, 128,-67,-53},
};

awb_param awb_param_gc2053_evb = {
    &awb_param_ca_info,
    &awb_param_th,
    &awb_param_lv_check[0],
    &awb_param_ct_weight,
    &awb_param_target[0],
    &awb_param_ct_table[0],
    &awb_param_mwb_tab[0],
    &awb_converge_par,
    &awb_expand_block_par[0],
};
    
void awb_get_param_gc2053_evb(UINT32* param);
    
void awb_get_param_gc2053_evb(UINT32* param)
{
	*param =  (UINT32)(&awb_param_gc2053_evb);
		//DBG_ERR("%x \r\n", *param);
}
