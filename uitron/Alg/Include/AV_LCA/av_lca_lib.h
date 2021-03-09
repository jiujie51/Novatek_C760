/**
    AlphaVision Rear Assistance System Library.

    @file       av_lca_lib.h

    include LCA 

    Copyright  ALPHAVISON . 2019.  All rights reserved.
*/
#ifndef _AV_LCA_LIB_H
#define _AV_LCA_LIB_H


#include "photo_task.h"

/**
    
*/
typedef enum 
{
	AVLCA_L_WRN_NONE = 0,          //
	AVLCA_L_WRN_LEVEL_L= 1,       //画面左边区域车辆超过第1条报警线，距离远
	AVLCA_L_WRN_LEVEL_H= 2,       //画面左边区域车辆超过第2条报警线，距离近
} AVLCA_LEFT_WARNMSG;


typedef enum 
{
	AVLCA_R_WRN_NONE = 0,          //
	AVLCA_R_WRN_LEVEL_L= 1,       //画面右边区域车辆超过第1条报警线，距离远
	AVLCA_R_WRN_LEVEL_H= 2,       //画面右边区域车辆超过第2条报警线，距离近
} AVLCA_RIGHT_WARNMSG;


typedef struct _AVLCA_WARNING_RESULT
{
	AVLCA_LEFT_WARNMSG        	LWarnStatus;
	AVLCA_RIGHT_WARNMSG			RWarnStatus;
	 	 
} AVLCA_WARNING_RESULT;


typedef struct _AVLCA_VEHICLE_INFO //检查到的车辆矩形区域
{
	INT32      iVehicleNum;
	IRECT	   iVehicleRect[20]; //最多20个车的信息
	 	 
} AVLCA_VEHICLE_INFO;

extern INT32 Anfa_LCACopySrcDataInt(IMG_BUF *pInputImg);

extern void AV_LCAInit(MEM_RANGE *buf, MEM_RANGE *cachebuf);

extern VOID AV_LCAProcess(void);

extern UINT32 AV_CalcLCABuffSize(VOID);

extern UINT32 AV_CalcLCACacheBuffSize(VOID);



extern INT32 AV_GetLCAWarnRslt(AVLCA_WARNING_RESULT *pWarnRslt);//获取报警结果

extern INT32 AV_GetLCAVehicleInfo(AVLCA_VEHICLE_INFO *pVehicleInfo);//获取检测到的车辆矩形区域

extern INT32 AV_GetInstallCoord(IPOINT *pCoord);//获取安装设备的参考坐标
#endif
