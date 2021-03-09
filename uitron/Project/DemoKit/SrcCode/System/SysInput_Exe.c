/*
    System Input Callback

    System Callback for Input Module.

    @file       SysInput_Exe.c
    @ingroup    mIPRJSYS

    @note

    Copyright   Novatek Microelectronics Corp. 2010.  All rights reserved.
*/

////////////////////////////////////////////////////////////////////////////////
#include "SysCommon.h"
#include "AppCommon.h"
////////////////////////////////////////////////////////////////////////////////
#include "UIFrameworkExt.h"
#include "UICommon.h"
#include "AppLib.h"
#include "GxInput.h"
#include "GxPower.h"
#if (IPCAM_FUNC != ENABLE)
#include "SoundData.h"
#endif
#if (GSENSOR_FUNCTION == ENABLE)
#include "GSensor.h"
#endif
#include "UIFlowMovieIcons.h"
#include "rtc.h"
#include "DxPower.h"

//global debug level: PRJ_DBG_LVL
#include "PrjCfg.h"
//local debug level: THIS_DBGLVL
#define THIS_DBGLVL         2 // 0=FATAL, 1=ERR, 2=WRN, 3=UNIT, 4=FUNC, 5=IND, 6=MSG, 7=VALUE, 8=USER
///////////////////////////////////////////////////////////////////////////////
#define __MODULE__          SysInputExe
#define __DBGLVL__          ((THIS_DBGLVL>=PRJ_DBG_LVL)?THIS_DBGLVL:PRJ_DBG_LVL)
#define __DBGFLT__          "*" //*=All, [mark]=CustomClass
#include "DebugModule.h"
///////////////////////////////////////////////////////////////////////////////

#include "SysInput_API.h"
#include "WifiAppCmd.h"

//#NT#Refine code for continue key
#define    BURSTKEY_DEBOUNCE     800//ms
#define    BURSTKEY_INTERVAL     200//ms
#define TOUCH_TIMER_CNT            1//20ms
#define    DOUBLECLICK_INTERVAL     500//ms

extern void UIFlowWndMovie_LcdBackLightCrl(BOOL crl_flag);
extern BOOL UIFlowWndMovie_GetLcdBacklightStus(void);
void LowPowerPro(void);

int SX_TIMER_DET_KEY_ID = -1;
int SX_TIMER_DET_TOUCH_ID = -1;
int SX_TIMER_DET_PWR_ID = -1;
int SX_TIMER_DET_MODE_ID = -1;
int SX_TIMER_AUTO_INPUT_ID = -1;
int SX_TIMER_DET_GSENSOR_ID = -1;
int SX_TIMER_DET_USER_ID = -1;


void UI_DetPwrKey(void);
void UI_DetNormalKey(void);
void UI_DetStatusKey(void);
void UI_DetCustom1Key(void);
void UI_DetUserFunc(void);


#if (POWERKEY_FUNCTION == ENABLE)
SX_TIMER_ITEM(Input_DetPKey, UI_DetPwrKey, 5, FALSE)
#endif
#if (NORMALKEY_FUNCTION == ENABLE)
SX_TIMER_ITEM(Input_DetNKey, UI_DetNormalKey, 1, FALSE)
#endif
#if (STATUSKEY_FUNCTION == ENABLE)
SX_TIMER_ITEM(Input_DetSKey, UI_DetStatusKey, 7, FALSE)
#endif
#if (GSENSOR_FUNCTION == ENABLE)
SX_TIMER_ITEM(Input_DetC1Key, UI_DetCustom1Key, 3, FALSE)
#endif
#if defined(_TOUCH_ON_)
SX_TIMER_ITEM(Input_DetTP, GxTouch_DetTP, TOUCH_TIMER_CNT, FALSE)
#endif

SX_TIMER_ITEM(Input_DetUserFunc, UI_DetUserFunc, 50, FALSE)

static BOOL         m_uiAnyKeyUnlockEn        = FALSE;

#if defined(_TOUCH_ON_)
static TOUCH_OBJ g_TouchTable[] = {
	{TP_GESTURE_PRESS,         NVTEVT_PRESS,        DEMOSOUND_SOUND_KEY_TONE},
	{TP_GESTURE_MOVE,          NVTEVT_MOVE,         0},
	{TP_GESTURE_HOLD,          NVTEVT_HOLD,         0},
	{TP_GESTURE_RELEASE,       NVTEVT_RELEASE,      DEMOSOUND_SOUND_KEY_TONE},
	{TP_GESTURE_CLICK,         NVTEVT_CLICK,        0},
	{TP_GESTURE_SLIDE_LEFT,    NVTEVT_SLIDE_LEFT,   0},
	{TP_GESTURE_SLIDE_RIGHT,   NVTEVT_SLIDE_RIGHT,  0},
	{TP_GESTURE_SLIDE_UP,      NVTEVT_SLIDE_UP,     0},
	{TP_GESTURE_SLIDE_DOWN,    NVTEVT_SLIDE_DOWN,   0}
};
#endif

#if 1//defined(_KEY_METHOD_4KEY_)
static KEY_OBJ g_KeyTable[] = {
#if 0//defined(_UI_STYLE_CARDV_)
	//POWER KEY
	{FLGKEY_KEY_POWER,    KEY_PRESS,       NVTEVT_KEY_POWER_REL,        0,                0},
	//NORMAL KEY
	{FLGKEY_MENU,         KEY_PRESS,       NVTEVT_KEY_MENU,         NVTEVT_KEY_PRESS,     DEMOSOUND_SOUND_KEY_TONE},
	{FLGKEY_MODE,         KEY_RELEASE,     NVTEVT_KEY_MODE,         NVTEVT_KEY_RELEASE,     0},
	{FLGKEY_UP,           KEY_PRESS,       NVTEVT_KEY_UP,           NVTEVT_KEY_PRESS,     DEMOSOUND_SOUND_KEY_TONE},
	{FLGKEY_UP,           KEY_CONTINUE,    NVTEVT_KEY_UP,           NVTEVT_KEY_CONTINUE,     0},
	{FLGKEY_UP,           KEY_RELEASE,     NVTEVT_KEY_UP,           NVTEVT_KEY_RELEASE,     0},
	{FLGKEY_DOWN,         KEY_PRESS,       NVTEVT_KEY_DOWN,         NVTEVT_KEY_PRESS,     DEMOSOUND_SOUND_KEY_TONE},
	{FLGKEY_DOWN,         KEY_CONTINUE,    NVTEVT_KEY_DOWN,         NVTEVT_KEY_CONTINUE,     0},
	{FLGKEY_DOWN,         KEY_RELEASE,     NVTEVT_KEY_DOWN,         NVTEVT_KEY_RELEASE,     0},
	{FLGKEY_SHUTTER2,     KEY_PRESS,       NVTEVT_KEY_SHUTTER2,     NVTEVT_KEY_PRESS,     DEMOSOUND_SOUND_KEY_TONE},
	{FLGKEY_SHUTTER2,     KEY_RELEASE,     NVTEVT_KEY_SHUTTER2,     NVTEVT_KEY_RELEASE,     0},
	{FLGKEY_ENTER,        KEY_PRESS,       NVTEVT_KEY_ENTER,        NVTEVT_KEY_PRESS,     DEMOSOUND_SOUND_KEY_TONE},
	{FLGKEY_ENTER,        KEY_RELEASE,     NVTEVT_KEY_ENTER,        NVTEVT_KEY_RELEASE,     0},
	{FLGKEY_CUSTOM1,      KEY_PRESS,       NVTEVT_KEY_CUSTOM1,      NVTEVT_KEY_PRESS,       DEMOSOUND_SOUND_NONE},
	{FLGKEY_CUSTOM1,      KEY_RELEASE,     NVTEVT_KEY_CUSTOM1,      NVTEVT_KEY_RELEASE,     0},
#else// _UI_STYLE_SPORTCAM_
	//POWER KEY
	{FLGKEY_KEY_POWER,    KEY_RELEASE,     NVTEVT_KEY_POWER_REL,        0,                0},
	//NORMAL KEY
	{FLGKEY_UP,           KEY_PRESS,       NVTEVT_KEY_UP,           NVTEVT_KEY_PRESS,     DEMOSOUND_SOUND_KEY_TONE},
	{FLGKEY_UP,           KEY_CONTINUE,    NVTEVT_KEY_UP,           NVTEVT_KEY_CONTINUE,     0},
	{FLGKEY_UP,           KEY_RELEASE,     NVTEVT_KEY_UP,           NVTEVT_KEY_RELEASE,     0},
	{FLGKEY_DOWN,         KEY_PRESS,       NVTEVT_KEY_DOWN,         NVTEVT_KEY_PRESS,     DEMOSOUND_SOUND_KEY_TONE},
	{FLGKEY_DOWN,         KEY_CONTINUE,    NVTEVT_KEY_DOWN,         NVTEVT_KEY_CONTINUE,     0},
	{FLGKEY_DOWN,         KEY_RELEASE,     NVTEVT_KEY_DOWN,         NVTEVT_KEY_RELEASE,     0},
	{FLGKEY_LEFT,         KEY_PRESS,       NVTEVT_KEY_MOVIE,         NVTEVT_KEY_PRESS,     DEMOSOUND_SOUND_KEY_TONE},
	//{FLGKEY_LEFT,         KEY_CONTINUE,    NVTEVT_KEY_PREV,         NVTEVT_KEY_CONTINUE,     0},
	//{FLGKEY_LEFT,         KEY_RELEASE,     NVTEVT_KEY_PREV,         NVTEVT_KEY_RELEASE,     0},
	{FLGKEY_SHUTTER2,     KEY_PRESS,       NVTEVT_KEY_SHUTTER2,     NVTEVT_KEY_PRESS,     DEMOSOUND_SOUND_KEY_TONE},
	{FLGKEY_SHUTTER2,     KEY_RELEASE,     NVTEVT_KEY_SHUTTER2,     NVTEVT_KEY_RELEASE,     0},
	{FLGKEY_RIGHT,        KEY_PRESS,       NVTEVT_KEY_SELECT,        NVTEVT_KEY_PRESS,     DEMOSOUND_SOUND_KEY_TONE},
	{FLGKEY_RIGHT,        KEY_CONTINUE,    NVTEVT_KEY_SELECT,        NVTEVT_KEY_CONTINUE,     0},
	{FLGKEY_RIGHT,        KEY_RELEASE,     NVTEVT_KEY_SELECT,        NVTEVT_KEY_RELEASE,     0},
	{FLGKEY_CUSTOM1,      KEY_PRESS,       NVTEVT_KEY_CUSTOM1,      NVTEVT_KEY_PRESS,       DEMOSOUND_SOUND_NONE},
	{FLGKEY_CUSTOM1,      KEY_RELEASE,     NVTEVT_KEY_CUSTOM1,      NVTEVT_KEY_RELEASE,     0},
	{FLGKEY_MENU,         KEY_PRESS,       NVTEVT_KEY_MENU,         NVTEVT_KEY_PRESS,       DEMOSOUND_SOUND_KEY_TONE},
	{FLGKEY_MENU,         KEY_CONTINUE,    NVTEVT_KEY_MENU,         NVTEVT_KEY_CONTINUE,    0},
	{FLGKEY_MENU,         KEY_RELEASE,     NVTEVT_KEY_MENU,         NVTEVT_KEY_RELEASE,     0},
	{FLGKEY_ENTER,        KEY_PRESS,       NVTEVT_KEY_ENTER,        NVTEVT_KEY_PRESS,       DEMOSOUND_SOUND_KEY_TONE},
#endif
};
#else
//_KEY_METHOD_2KEY_ (for SPORTCAM only)
static KEY_OBJ g_KeyTable[] = {
	//POWER KEY
	{FLGKEY_KEY_POWER,    KEY_RELEASE,     NVTEVT_KEY_POWER_REL,        0,                0},
	//NORMAL KEY
	{FLGKEY_ENTER,        KEY_PRESS,       NVTEVT_KEY_ENTER,        NVTEVT_KEY_PRESS,     DEMOSOUND_SOUND_KEY_TONE},
	{FLGKEY_MENU,         KEY_PRESS,       NVTEVT_KEY_MENU,         NVTEVT_KEY_PRESS,     DEMOSOUND_SOUND_KEY_TONE},
	{FLGKEY_MODE,         KEY_PRESS,       NVTEVT_KEY_MODE,         NVTEVT_KEY_PRESS,     DEMOSOUND_SOUND_KEY_TONE},
	{FLGKEY_PLAYBACK,     KEY_PRESS,       NVTEVT_KEY_PLAYBACK,     NVTEVT_KEY_PRESS,     DEMOSOUND_SOUND_KEY_TONE},
	{FLGKEY_UP,           KEY_PRESS,       NVTEVT_KEY_UP,           NVTEVT_KEY_PRESS,     DEMOSOUND_SOUND_KEY_TONE},
	{FLGKEY_UP,           KEY_CONTINUE,    NVTEVT_KEY_UP,           NVTEVT_KEY_CONTINUE,     0},
	{FLGKEY_UP,           KEY_RELEASE,     NVTEVT_KEY_UP,           NVTEVT_KEY_RELEASE,     0},
	{FLGKEY_DOWN,         KEY_PRESS,       NVTEVT_KEY_DOWN,         NVTEVT_KEY_PRESS,     DEMOSOUND_SOUND_KEY_TONE},
	{FLGKEY_DOWN,         KEY_CONTINUE,    NVTEVT_KEY_DOWN,         NVTEVT_KEY_CONTINUE,     0},
	{FLGKEY_DOWN,         KEY_RELEASE,     NVTEVT_KEY_DOWN,         NVTEVT_KEY_RELEASE,     0},
	{FLGKEY_LEFT,         KEY_PRESS,       NVTEVT_KEY_SELECT,         NVTEVT_KEY_PRESS,     DEMOSOUND_SOUND_KEY_TONE},
	{FLGKEY_LEFT,         KEY_CONTINUE,    NVTEVT_KEY_SELECT,         NVTEVT_KEY_CONTINUE,     0},
	{FLGKEY_LEFT,         KEY_RELEASE,     NVTEVT_KEY_SELECT,         NVTEVT_KEY_RELEASE,     0},
	{FLGKEY_RIGHT,        KEY_PRESS,       NVTEVT_KEY_NEXT,        NVTEVT_KEY_PRESS,     DEMOSOUND_SOUND_KEY_TONE},
	{FLGKEY_RIGHT,        KEY_CONTINUE,    NVTEVT_KEY_NEXT,        NVTEVT_KEY_CONTINUE,     0},
	{FLGKEY_RIGHT,        KEY_RELEASE,     NVTEVT_KEY_NEXT,        NVTEVT_KEY_RELEASE,     0},
	{FLGKEY_DOWN,         KEY_PRESS,       NVTEVT_KEY_DOWN,         NVTEVT_KEY_PRESS,     DEMOSOUND_SOUND_KEY_TONE},
	{FLGKEY_DOWN,         KEY_CONTINUE,    NVTEVT_KEY_DOWN,         NVTEVT_KEY_CONTINUE,     0},
	{FLGKEY_DOWN,         KEY_RELEASE,     NVTEVT_KEY_DOWN,         NVTEVT_KEY_RELEASE,     0},
	{FLGKEY_ZOOMIN,       KEY_PRESS,       NVTEVT_KEY_ZOOMIN,       NVTEVT_KEY_PRESS,     DEMOSOUND_SOUND_KEY_TONE},
	{FLGKEY_ZOOMIN,       KEY_CONTINUE,    NVTEVT_KEY_ZOOMIN,       NVTEVT_KEY_CONTINUE,     0},
	{FLGKEY_ZOOMIN,       KEY_RELEASE,     NVTEVT_KEY_ZOOMIN,       NVTEVT_KEY_RELEASE,     0},
	{FLGKEY_ZOOMOUT,      KEY_PRESS,       NVTEVT_KEY_ZOOMOUT,      NVTEVT_KEY_PRESS,     DEMOSOUND_SOUND_KEY_TONE},
	{FLGKEY_ZOOMOUT,      KEY_CONTINUE,    NVTEVT_KEY_ZOOMOUT,      NVTEVT_KEY_CONTINUE,     0},
	{FLGKEY_ZOOMOUT,      KEY_RELEASE,     NVTEVT_KEY_ZOOMOUT,      NVTEVT_KEY_RELEASE,     0},
	{FLGKEY_SHUTTER2,     KEY_PRESS,       NVTEVT_KEY_SHUTTER2,     NVTEVT_KEY_PRESS,     DEMOSOUND_SOUND_KEY_TONE},
	{FLGKEY_SHUTTER2,     KEY_RELEASE,     NVTEVT_KEY_SHUTTER2,     NVTEVT_KEY_RELEASE,     0},
	{FLGKEY_SHUTTER1,     KEY_PRESS,       NVTEVT_KEY_SHUTTER1,     NVTEVT_KEY_PRESS,     DEMOSOUND_SOUND_KEY_TONE},
	{FLGKEY_SHUTTER1,     KEY_RELEASE,     NVTEVT_KEY_SHUTTER1,     NVTEVT_KEY_RELEASE,     0},
	{FLGKEY_CUSTOM1,      KEY_PRESS,       NVTEVT_KEY_CUSTOM1,      NVTEVT_KEY_PRESS,       DEMOSOUND_SOUND_NONE},
	{FLGKEY_CUSTOM1,      KEY_RELEASE,     NVTEVT_KEY_CUSTOM1,      NVTEVT_KEY_RELEASE,     0},
};
#endif
void KeySoundCB(UINT32 uiSoundID)
{
	if (uiSoundID) {
#if(UI_FUNC==ENABLE)
		UISound_Play(uiSoundID);
		//if(UIFlowWndMovie_GetLcdBacklightStus() == FALSE)
		//	UIFlowWndMovie_LcdBackLightCrl(TRUE);
#endif
	}
}

UINT32 Input_GroupStatus2Event(UINT32 status)
{
	UINT32 i = 0;
	for (i = 0; i < sizeof(g_KeyTable) / sizeof(KEY_OBJ); i++) {
		if ((g_KeyTable[i].uiKeyFlag == STATUS_KEY_GROUP1) && (g_KeyTable[i].status == status)) {
			return g_KeyTable[i].uiKeyEvent;
		}
	}
	return 0;
}

/**
  convert GPIO key to UI key event
  [InputCB internal API]

  @param UINT32 keys: Input key code detected from GPIO mapping
  @return UINT32: NVTEVT
**/
#if 0
static UINT32 Input_Key2Evt(UINT32 keys)
{
	if (keys & FLGKEY_MOVIE) {
		return NVTEVT_KEY_MOVIE;
	}
	if (keys & FLGKEY_I) {
		return NVTEVT_KEY_I;
	}
	if (keys & FLGKEY_MODE) {
		return NVTEVT_KEY_MODE;
	}
	if (keys & FLGKEY_PLAYBACK) {
		return NVTEVT_KEY_PLAYBACK;
	}
	if (keys & FLGKEY_MENU) {
		return NVTEVT_KEY_MENU;
	}
	if (keys & FLGKEY_FACEDETECT) {
		return NVTEVT_KEY_FACEDETECT;
	}
	if (keys & FLGKEY_DEL) {
		return NVTEVT_KEY_DEL;
	}
	if (keys & FLGKEY_LEFT) {
		return NVTEVT_KEY_LEFT;
	}
	if (keys & FLGKEY_RIGHT) {
		return NVTEVT_KEY_RIGHT;
	}
	if (keys & FLGKEY_ENTER) {
		return NVTEVT_KEY_ENTER;
	}
	if (keys & FLGKEY_SHUTTER1) {
		return NVTEVT_KEY_SHUTTER1;
	}
	if (keys & FLGKEY_SHUTTER2) {
		return NVTEVT_KEY_SHUTTER2;
	}
	if (keys & FLGKEY_ZOOMOUT) {
		return NVTEVT_KEY_ZOOMOUT;
	}
	if (keys & FLGKEY_ZOOMIN) {
		return NVTEVT_KEY_ZOOMIN;
	}
	if (keys & FLGKEY_UP) {
		return NVTEVT_KEY_UP;
	}
	if (keys & FLGKEY_DOWN) {
		return NVTEVT_KEY_DOWN;
	} else {
		return NVTEVT_NULL;
	}
}
#endif
//just for backward compatible
void Input_SetKeyMask(KEY_STATUS uiMode, UINT32 uiMask)
{
	SysMan_SetKeyMask(uiMode, uiMask);
}
UINT32 Input_GetKeyMask(KEY_STATUS uiMode)
{
	return SysMan_GetKeyMask(uiMode);
}
void Input_SetKeySoundMask(KEY_STATUS uiMode, UINT32 uiMask)
{
	SysMan_SetKeySoundMask(uiMode, uiMask);
}
UINT32 Input_GetKeySoundMask(KEY_STATUS uiMode)
{
	return SysMan_GetKeySoundMask(uiMode);
}

/**
    reset all mask,usually in new winodw onOpen
*/
void Input_ResetMask(void)
{
	// Set key mask as default
	SysMan_SetKeyMask(KEY_PRESS, FLGKEY_KEY_MASK_DEFAULT);
	SysMan_SetKeyMask(KEY_RELEASE, FLGKEY_KEY_MASK_DEFAULT);
	SysMan_SetKeyMask(KEY_CONTINUE, FLGKEY_KEY_MASK_NULL);
#if defined(_TOUCH_ON_)
	SysMan_SetTouchMask(TOUCH_MASK_DEFAULT);
#endif
	SysMan_SetKeySoundMask(KEY_PRESS, FLGKEY_SOUND_MASK_DEFAULT);
	SysMan_SetKeySoundMask(KEY_RELEASE, FLGKEY_KEY_MASK_NULL);
	SysMan_SetKeySoundMask(KEY_CONTINUE, FLGKEY_SOUND_MASK_DEFAULT);
}
/**
    for some case,press any key unlock all,and post NVTEVT_KEY_PRESS
    ex:Timelapse or smile detect
*/
void Input_SetAnyKeyUnlock(BOOL en)
{
	m_uiAnyKeyUnlockEn = en;
}


UINT32 Input_Key2Mode(UINT32 keys)
{
#if (STATUSKEY_FUNCTION == ENABLE)
	switch (keys) {
	case NVTEVT_KEY_STATUS1:
		return DSC_MODE_MOVIE;
	case NVTEVT_KEY_STATUS2:
		return DSC_MODE_PHOTO_MANUAL;
	case NVTEVT_KEY_STATUS3:
		return DSC_MODE_PHOTO_MANUAL;
	case NVTEVT_KEY_STATUS4:
		return DSC_MODE_PHOTO_MANUAL;
	case NVTEVT_KEY_STATUS5:
		return DSC_MODE_PHOTO_MANUAL;
	case NVTEVT_KEY_STATUS6:
		return DSC_MODE_PHOTO_SCENE;
	case NVTEVT_KEY_STATUS7:
		return DSC_MODE_PHOTO_MANUAL;
	case NVTEVT_KEY_STATUS8:
		return DSC_MODE_PHOTO_MANUAL;
	case NVTEVT_KEY_STATUS9:
		return DSC_MODE_PHOTO_MANUAL;
	case NVTEVT_KEY_STATUS10:
		return DSC_MODE_PHOTO_MANUAL;
	default:
		return DSC_MODE_PHOTO_AUTO;
	}
#else
	return 0;
#endif
}
extern SX_CMD_ENTRY key[];
extern SX_CMD_ENTRY ts[];
#if (defined(_NVT_ETHREARCAM_RX_) ||defined(_NVT_ETHREARCAM_TX_))
extern void EthCamNet_LinkDet(void);
extern int SX_TIMER_ETHCAM_LINKDET_ID;
#if defined(_NVT_ETHREARCAM_RX_)
SX_TIMER_ITEM(ETHCAM_LinkDet, EthCamNet_LinkDet, 100, FALSE)
#else
SX_TIMER_ITEM(ETHCAM_LinkDet, EthCamNet_LinkDet, 25, FALSE)
#endif
#endif

#if (defined(_NVT_ETHREARCAM_RX_))
#if (ETH_REARCAM_CAPS_COUNT>=2)
extern void EthCamNet_EthHubLinkDetInit(void);
extern void EthCamNet_EthHubLinkDet(void);
extern int SX_TIMER_ETHCAM_ETHHUB_LINKDET_ID;
SX_TIMER_ITEM(ETHCAM_EthHubLinkDet, EthCamNet_EthHubLinkDet, 50, FALSE)
#endif
extern void EthCamNet_EthLinkRetry(void);
extern int SX_TIMER_ETHCAM_ETHLINKRETRY_ID;
SX_TIMER_ITEM(ETHCAM_EthHubLinkRetry, EthCamNet_EthLinkRetry, 50, FALSE)
#endif
void System_OnInputInit(void)
{
	//PHASE-1 : Init & Open Drv or DrvExt
	{
		GxKey_RegCB(Key_CB);         //Register CB function of GxInput
#if defined(_TOUCH_ON_)
		GxTouch_RegCB(Touch_CB);     //Register CB function of GxInput
#endif
		GxKey_Init();
		SxCmd_AddTable(key);
		SxCmd_AddTable(ts);
#if (defined(_NVT_ETHREARCAM_RX_) ||defined(_NVT_ETHREARCAM_TX_))
		SX_TIMER_ETHCAM_LINKDET_ID = SxTimer_AddItem(&Timer_ETHCAM_LinkDet);
#endif
#if (defined(_NVT_ETHREARCAM_RX_))
#if (ETH_REARCAM_CAPS_COUNT>=2)
		EthCamNet_EthHubLinkDetInit();
		SX_TIMER_ETHCAM_ETHHUB_LINKDET_ID = SxTimer_AddItem(&Timer_ETHCAM_EthHubLinkDet);
		SxTimer_SetFuncActive(SX_TIMER_ETHCAM_ETHHUB_LINKDET_ID, TRUE);
#endif
		SX_TIMER_ETHCAM_ETHLINKRETRY_ID = SxTimer_AddItem(&Timer_ETHCAM_EthHubLinkRetry);
#endif
	}
	//PHASE-2 : Init & Open Lib or LibExt
	{
		//1.砞﹚init
		//2.砞﹚CB,
		GxKey_SetContDebounce(BURSTKEY_DEBOUNCE / SxTimer_GetData(SXTIMER_TIMER_BASE));
		GxKey_SetRepeatInterval(BURSTKEY_INTERVAL / SxTimer_GetData(SXTIMER_TIMER_BASE));
		SysMan_RegKeySoundCB(KeySoundCB);
		SysMan_RegKeyTable(g_KeyTable, sizeof(g_KeyTable) / sizeof(KEY_OBJ));
		//3.爹SxJob狝叭 ---------> System Job
		//4.爹SxTimer狝叭 ---------> Detect Job

#if (POWERKEY_FUNCTION == ENABLE)
		SX_TIMER_DET_PWR_ID = SxTimer_AddItem(&Timer_Input_DetPKey);
#endif
#if (NORMALKEY_FUNCTION == ENABLE)
		SX_TIMER_DET_KEY_ID = SxTimer_AddItem(&Timer_Input_DetNKey);
#endif
#if (STATUSKEY_FUNCTION == ENABLE)
		SX_TIMER_DET_MODE_ID = SxTimer_AddItem(&Timer_Input_DetSKey);
#endif
#if (GSENSOR_FUNCTION == ENABLE)
		SX_TIMER_DET_GSENSOR_ID = SxTimer_AddItem(&Timer_Input_DetC1Key);
#endif
#if defined(_TOUCH_ON_)
		SX_TIMER_DET_TOUCH_ID = SxTimer_AddItem(&Timer_Input_DetTP);
#endif

        SX_TIMER_DET_USER_ID = SxTimer_AddItem(&Timer_Input_DetUserFunc);

#if (STATUSKEY_FUNCTION == ENABLE)
		GxKey_DetStatusKey();
#endif

#if (POWERKEY_FUNCTION == ENABLE)
		SxTimer_SetFuncActive(SX_TIMER_DET_PWR_ID, TRUE);
#endif
#if (NORMALKEY_FUNCTION == ENABLE)
		SxTimer_SetFuncActive(SX_TIMER_DET_KEY_ID, TRUE);
#endif
#if (STATUSKEY_FUNCTION == ENABLE)
		SxTimer_SetFuncActive(SX_TIMER_DET_MODE_ID, TRUE);
#endif
#if (GSENSOR_FUNCTION == ENABLE)
		SxTimer_SetFuncActive(SX_TIMER_DET_GSENSOR_ID, TRUE);
#endif

        SxTimer_SetFuncActive(SX_TIMER_DET_USER_ID, TRUE);

#if defined(_TOUCH_ON_)
		GxTouch_Init();
		GxTouch_SetCtrl(GXTCH_DOUBLE_CLICK_INTERVAL,
						DOUBLECLICK_INTERVAL / TOUCH_TIMER_CNT / SxTimer_GetData(SXTIMER_TIMER_BASE));
		SysMan_RegTouchSoundCB(KeySoundCB);
		SysMan_RegTouchTable(g_TouchTable, sizeof(g_TouchTable) / sizeof(TOUCH_OBJ));
		SxTimer_SetFuncActive(SX_TIMER_DET_TOUCH_ID, TRUE);
#endif
	}
}

void System_OnInputExit(void)
{
	//PHASE-2 : Close Lib or LibExt
	{
	}
	//PHASE-1 : Close Drv or DrvExt
	{
	}
}

/////////////////////////////////////////////////////////////////////////////

void UI_DetPwrKey(void)
{
	if (!UI_IsForceLock()) {
		GxKey_DetPwrKey();
	}
	
	LowPowerPro();

}

void UI_DetNormalKey(void)
{
	static UINT32 keyDetectCount = 0;
	if (!UI_IsForceLock()) {
		GxKey_DetNormalKey();
		if (keyDetectCount < 4) {
			keyDetectCount++;
		}

		if (keyDetectCount == 2) {
			//recover the key detection after system boot up
			GxKey_SetFirstKeyInvalid(KEY_PRESS, 0);
		}
	}
}

void UI_DetStatusKey(void)
{
//check mode key
	if ((!UI_IsForceLock()) && (!UI_IsForceLockStatus())) {
		GxKey_DetStatusKey();
	}
}


#if (GSENSOR_FUNCTION == ENABLE)
void UI_DetCustom1Key(void)
{
	BOOL   bGSensorStatus = FALSE;
	Gsensor_Data GS_Data = {0};

	bGSensorStatus = GSensor_GetStatus(&GS_Data);
	if (bGSensorStatus == TRUE) {CHKPNT;
		Ux_PostEvent(NVTEVT_KEY_CUSTOM1, 1, NVTEVT_KEY_PRESS);
	}
	//DBGD(bGSensorStatus);
}
#endif

/////////////////////////////////////////////////////////////////////////////
extern void System_ResetDetCloseLenCount(void);
extern void System_ResetPowerSaveCount(void);
BOOL g_bConsumeStatus = FALSE;

INT32 System_UserKeyFilter(NVTEVT evt, UINT32 paramNum, UINT32 *paramArray)
{
	UINT32 key = evt;
	if (IN_RANGE_EVENT(key, NVTEVT_KEY_STATUS_START, NVTEVT_KEY_STATUS_END)) { //Status class
		System_ResetDetCloseLenCount();
		System_ResetPowerSaveCount();
#if (STATUSKEY_FUNCTION == ENABLE)
		DBG_IND("^Bgroup key event=0x%x\r\n", key);

		if (g_bConsumeStatus) {
			g_bConsumeStatus = 0;
		} else {
			UINT32 uiDscMode = Input_Key2Mode(key);
			DBG_IND("^YDscMode=0x%x\r\n", uiDscMode);
			//FlowMode_OnKeyMode(uiDscMode);
			UI_Switch_DscMode(uiDscMode, DSCMODE_SWITCH_FORCE, DSCMODE_SWITCHDIR_DONT_CARE);
		}
#endif
		return NVTEVT_CONSUME;
	} else if (IN_RANGE_EVENT(key, NVTEVT_KEY_BUTTON_START, NVTEVT_KEY_BUTTON_END)) { //Button class
		if (IN_RANGE_EVENT(key, NVTEVT_KEY_PRESS_START, NVTEVT_KEY_PRESS_END)) { //Press key
			if (key == NVTEVT_KEY_POWER) {
				return NVTEVT_CONSUME;
			}

			System_ResetDetCloseLenCount();
			if (GxPower_GetControl(GXPWR_CTRL_SLEEP_LEVEL) > 1) { // drop key if sleep level > 1
				//NOTE! do not set GXPWR_CTRL_SLEEP_RESET before get GXPWR_CTRL_SLEEP_LEVEL,
				//  because GXPWR_CTRL_SLEEP_LEVEL status maybe alter by sleep wakeup flow~
				System_ResetPowerSaveCount();
				return NVTEVT_CONSUME;
			} else {
				System_ResetPowerSaveCount();
				if (m_uiAnyKeyUnlockEn) {
					Ux_PostEvent(NVTEVT_KEY_PRESS_START, 1, key);
					return NVTEVT_CONSUME;
				}
				return NVTEVT_PASS;
			}
		} else if (IN_RANGE_EVENT(key, NVTEVT_KEY_CONTINUE_START, NVTEVT_KEY_CONTINUE_END)) { //Contine key
			if (key == NVTEVT_KEY_POWER_CONT) {
				return NVTEVT_CONSUME;
			}

			System_ResetDetCloseLenCount();
			System_ResetPowerSaveCount();
			return NVTEVT_PASS;
		} else if (IN_RANGE_EVENT(key, NVTEVT_KEY_RELEASE_START, NVTEVT_KEY_RELEASE_END)) { //Release key
			if (key == NVTEVT_KEY_POWER_REL) {
				System_ResetDetCloseLenCount();
				if (GxPower_GetControl(GXPWR_CTRL_SLEEP_LEVEL) > 1) { // drop key if sleep level > 1
					//NOTE! do not set GXPWR_CTRL_SLEEP_RESET before get GXPWR_CTRL_SLEEP_LEVEL,
					//  because GXPWR_CTRL_SLEEP_LEVEL status maybe alter by sleep wakeup flow~
					System_ResetPowerSaveCount();
				} else {
					//#NT#2016/03/07#KCHong -begin
					//#NT#Low power timelapse function
#if (TIMELAPSE_LPR_FUNCTION == ENABLE)
					MovieTLLPR_Process(TIMELAPSE_FROM_PWRKEY);
#endif
					//#NT#2016/03/07#KCHong -end
					System_PowerOff(SYS_POWEROFF_NORMAL);
				}
				return NVTEVT_CONSUME;
			}

			System_ResetDetCloseLenCount();
			System_ResetPowerSaveCount();
			return NVTEVT_PASS;
		}
	}
	return NVTEVT_PASS;
}

#if defined(_TOUCH_ON_)
INT32 System_UserTouchFilter(NVTEVT evt, UINT32 paramNum, UINT32 *paramArray)
{
	INT32 Ret = NVTEVT_PASS;
	//DBG_DUMP("^Bevt=%d, point=(%d, %d)\r\n", evt - NVTEVT_PRESS, paramArray[0], paramArray[1]);
	if (evt >= NVTEVT_PRESS && evt <= NVTEVT_SLIDE_DOWN) {
		if (GxPower_GetControl(GXPWR_CTRL_SLEEP_LEVEL) > 1) { // drop key if sleep level > 1
			//NOTE! do not set GXPWR_CTRL_SLEEP_RESET before get GXPWR_CTRL_SLEEP_LEVEL,
			//  because GXPWR_CTRL_SLEEP_LEVEL status maybe alter by sleep wakeup flow~
			System_ResetPowerSaveCount();
			return NVTEVT_CONSUME;
		} else {
			System_ResetPowerSaveCount();
			return NVTEVT_PASS;
		}
	}
	return Ret;
}
#endif


#if (ESD_RESET_FUNCTION==ENABLE)
#include "wdt.h"
static BOOL g_IsESDReset = FALSE;
static UINT32 CheckAECnt =0;
static UINT32 CheckAECntTemp =0;
static UINT32 IsESDCnt =0;
void	SetSysWdtReset(UINT32 uMS)
{
	wdt_open();
	wdt_setConfig(WDT_CONFIG_ID_MODE,WDT_MODE_RESET);
	wdt_setConfig(WDT_CONFIG_ID_TIMEOUT,uMS);	//ms
	wdt_trigger();
	wdt_enable();
	DBG_ERR("[WDT] SysWdtReset when after %d ms\r\n",uMS);
}
void SystemSet_ESDReset(void)
{
	IPL_SIE_CUR_FRAME_INFO sie_frm_info = {0};	
	sie_frm_info.id = IPL_ID_1;	
	sie_frm_info.pnext = NULL;	
	IPL_GetCmd(IPL_GET_SIE_CUR_FRAME_INFOR, (void *)&sie_frm_info);
	CheckAECnt=sie_frm_info.frame_cnt;
	
	//debug_msg("\r\n CheckAECnt:%d\r\n",CheckAECnt);
	//debug_msg("\r\n CheckAECntTemp:%d\r\n",CheckAECntTemp);
	
	if(CheckAECnt == CheckAECntTemp) {
		IsESDCnt++;
	}
	else{
		CheckAECntTemp = CheckAECnt;
		IsESDCnt=0;
	}
	if (IsESDCnt >3)
	{
		IsESDCnt= 0;
		#if 0	//
		Ux_PostEvent(NVTEVT_ESD_RESTART_REC, 1, 0);
		#else
		if(g_IsESDReset == FALSE)
		{
			SetSysWdtReset(500);
			g_IsESDReset = TRUE;
		}
		#endif
	}
}
#endif


///////////////////////////user function  start///////////////////
void  PowerKeyEvent(UINT32 EventID)
{
	switch(EventID)
	{
		case 0://短按power键,息屏
			DBG_DUMP(" System start power normal key !!!(%d)   \r\n",__LINE__);
			/*{		
				Ux_PostEvent(NVTEVT_KEY_ENTER, 1, NVTEVT_KEY_PRESS);
				if(System_GetState(SYS_STATE_CURRMODE) != PRIMARY_MODE_PHOTO)
				{
					UISound_Play(DEMOSOUND_SOUND_KEY_TONE);
				}
			}	*/	
			//if(System_GetState(SYS_STATE_CURRMODE) == PRIMARY_MODE_MOVIE)
			{
				if(UIFlowWndMovie_GetLcdBacklightStus())
				{
					CHKPNT;
					UIFlowWndMovie_LcdBackLightCrl(FALSE);
				}
				else
				{
					CHKPNT;
					UIFlowWndMovie_LcdBackLightCrl(TRUE);
				}
			}
			break;
		
		case 1://power key
			DBG_DUMP(" System start power off !!!(%d)   \r\n",__LINE__);
			Ux_PostEvent(NVTEVT_KEY_POWER_REL, 1, NVTEVT_KEY_PRESS);	 
			break;
	}
}

//GPS Function///////////////////////////////////////
#if ((GPS_FUNCTION==ENABLE))

#if 0//
extern GSVINFO GSVInfo;
extern UINT32  SenNum;
#endif

void Gps_ShowOSDString(CHAR *pString, UINT16 uiX, UINT16 uiY, UINT16 uiColor)
#if 0
{
		DC**	 pDCList;
		
  		//UI_SetDisplayDirty(TRUE); //set TRUE to force dirty current begin/end

		pDCList = (DC **)UI_BeginScreen();
		GxGfx_SetTextStroke((const FONT *)gDemoKit_Font, FONTSTYLE_NORMAL, SCALE_1X);
		GxGfx_SetTextColor(_OSD_INDEX_TRANSPART, _OSD_INDEX_TRANSPART, 0);

		GxGfx_SetTextColor(uiColor, _OSD_INDEX_GRAY, 0);
		GxGfx_Text(((DC **)pDCList)[GxGfx_OSD], uiX, uiY, pString);
		UI_EndScreen((UINT32)pDCList);
}
#else
{
	DC **pDCList;
	UINT32 uiWidth;

	pDCList = (DC **)UI_BeginScreen();
	uiWidth = strlen(pString) * 12;
	if (uiWidth > OSD_W) {
		uiWidth = OSD_W;
	}

	if (1) {
		GxGfx_SetTextStroke((const FONT *)gDemoKit_Font, FONTSTYLE_NORMAL, SCALE_1X);
		// Clear before draw (clear whole area of OSD width with font height)
		GxGfx_SetShapeColor(CLRID_IDX_TRANSPART, CLRID_IDX_TRANSPART, 0);
		//GxGfx_FillRect(pDCList[GxGfx_OSD], 0, y, OSD_W, y+24);
		GxGfx_FillRect(pDCList[GxGfx_OSD], 0, uiY, uiWidth, uiY + 26);
	}

	//GxGfx_SetTextColor(uiColor, CLRID_IDX_TRANSPART, 0);
	GxGfx_SetTextColor(uiColor, CLRID_IDX_BLACK, 0);
	GxGfx_Text(pDCList[GxGfx_OSD], uiX, uiY, pString);
	UI_EndScreen((UINT32)pDCList);
}
#endif

void Gps_InfoShow(void)
{
#if 0//
	char pString[32];
	if (SenNum >= 1) 
	{
		snprintf(pString, 512,"Sat1:%2d  %dDB",GSVInfo.SAT01.SatNum,GSVInfo.SAT01.SNR);
		Gps_ShowOSDString(pString,50,40,3);
		snprintf(pString, 512,"Sat2:%2d  %dDB",GSVInfo.SAT02.SatNum,GSVInfo.SAT02.SNR);
		Gps_ShowOSDString(pString,50,60,3);
		snprintf(pString, 512,"Sat3:%2d  %dDB",GSVInfo.SAT03.SatNum,GSVInfo.SAT03.SNR);
		Gps_ShowOSDString(pString,50,80,3);
		snprintf(pString, 512,"Sat4:%2d  %dDB",GSVInfo.SAT04.SatNum,GSVInfo.SAT04.SNR);
		Gps_ShowOSDString(pString,50,100,3);
 
	}
	if (SenNum >= 2) 
	{
		snprintf(pString, 512,"Sat1:%2d  %dDB",GSVInfo.SAT05.SatNum,GSVInfo.SAT05.SNR);
		Gps_ShowOSDString(pString,50,120,3);
		snprintf(pString, 512,"Sat2:%2d  %dDB",GSVInfo.SAT06.SatNum,GSVInfo.SAT06.SNR);
		Gps_ShowOSDString(pString,50,140,3);
		snprintf(pString, 512,"Sat3:%2d  %dDB",GSVInfo.SAT07.SatNum,GSVInfo.SAT07.SNR);
		Gps_ShowOSDString(pString,50,160,3);
		snprintf(pString, 512,"Sat4:%2d  %dDB",GSVInfo.SAT08.SatNum,GSVInfo.SAT08.SNR);
		Gps_ShowOSDString(pString,50,180,3);
 
	}
	if (SenNum >= 3) 
	{
		snprintf(pString, 512,"Sat1:%2d  %dDB",GSVInfo.SAT09.SatNum,GSVInfo.SAT09.SNR);
		Gps_ShowOSDString(pString,200,40,3);
		snprintf(pString, 512,"Sat2:%2d  %dDB",GSVInfo.SAT10.SatNum,GSVInfo.SAT10.SNR);
		Gps_ShowOSDString(pString,200,60,3);
		snprintf(pString, 512,"Sat3:%2d  %dDB",GSVInfo.SAT11.SatNum,GSVInfo.SAT11.SNR);
		Gps_ShowOSDString(pString,200,80,3);
		snprintf(pString, 512,"Sat4:%2d  %dDB",GSVInfo.SAT12.SatNum,GSVInfo.SAT12.SNR);
		Gps_ShowOSDString(pString,200,100,3);
 
	}
#endif
}

BOOL  SynGpsTime(RMCINFO pRMCInfo)
{
	 BOOL  vRet = FALSE;
	 RTC_DATE	 Date;
	 RTC_TIME	 Time;


	if (pRMCInfo.Status == 'A')
	{
		Date.s.year = pRMCInfo.Year;
		if( Date.s.year <100 )
			Date.s.year = Date.s.year  +2000;
		Date.s.month = pRMCInfo.Month;
		Date.s.day= pRMCInfo.Day;
		Time.s.hour = pRMCInfo.Hour;
		Time.s.minute= pRMCInfo.Minute;
		Time.s.second= pRMCInfo.Second;
		// adjust time for china
		Time.s.hour = Time.s.hour +8;
		if( Time.s.hour >23 )
		{
			Time.s.hour = Time.s.hour -24;
			Date.s.day ++;

			switch(Date.s.month)
			{
			case 1:
			case 3:
			case 5:
			case 7:
			case 8:
			case 10:
				if( Date.s.day >31)
				{
					Date.s.day = 1;
					Date.s.month ++;
				}
			break;
			case 12:
				if( Date.s.day >31)
				{
					Date.s.day = 1;
					Date.s.month =1;
					Date.s.year ++;
				}
			break;
			case 2:
				if( Date.s.year % 4 == 0 ) 
				{
					if( Date.s.day > 29 )
					{
						Date.s.day = 1;
						Date.s.month ++;
					}
				}
				else
				{
					if( Date.s.day > 28 )
					{
						Date.s.day = 1;
						Date.s.month ++;
					}
				}
			break;
			case 4:
			case 6:
			case 9:
			case 11:
			default:
				if( Date.s.day >30)
				{
					Date.s.day = 1;
					Date.s.month ++;
				}
			break;
			}
		}
		
		struct tm cUIDateTime = {0};
		cUIDateTime.tm_year = Date.s.year;
		cUIDateTime.tm_mon	= Date.s.month;
		cUIDateTime.tm_mday = Date.s.day;
		cUIDateTime.tm_hour = Time.s.hour;
		cUIDateTime.tm_min	= Time.s.minute;
		cUIDateTime.tm_sec	= Time.s.second;
		HwClock_SetTime(TIME_ID_CURRENT, cUIDateTime, 0);

		//rtc_setDate(Date.s.year, Date.s.month, Date.s.day);
		//rtc_setTime(Time.s.hour, Time.s.minute, Time.s.second);
		//rtc_triggerCSET();
		debug_msg("GPS UTC to Beijing time:%d-%d-%d %d:%d:%d\r\n",Date.s.year,Date.s.month,Date.s.day,Time.s.hour,Time.s.minute,Time.s.second);
		vRet  = 	TRUE;
	}
	return vRet;
}




#if 1
void record_gps_log(UINT8 *pBuf)
{
    char szRec[256];
    UINT32 size = 256;
    char srcPath[] = "A:\\gps_log.txt";
    INT32 rval = 0;
    static FST_FILE fileHdl = NULL;
    if(!fileHdl){
        fileHdl = FileSys_OpenFile(srcPath, FST_OPEN_WRITE|FST_OPEN_ALWAYS);
        debug_msg("fileHdl = %p\r\n", fileHdl);
    }
    sprintf(szRec, "%s\r\n", pBuf);
    size = strlen(szRec);
    if(fileHdl){
        rval = FileSys_WriteFile(fileHdl, (UINT8 *)szRec, &size, FST_FLAG_NONE, NULL);
        if(rval == FST_STA_OK)
    	{
        	//debug_msg("fileHdl=%p, rval=%d:%s\r\n", fileHdl, rval, szRec);
    	}
        else{
            fileHdl = FileSys_OpenFile(srcPath, FST_OPEN_WRITE|FST_OPEN_ALWAYS);
            debug_msg("Failed to write data into disk\r\n");
        }
    }
}
#endif


static INT16  g_GpsStatus= -1;
static UINT32	g_GpsJitterCount = 0;
void  SynGpsStatus(RMCINFO pRMCInfo)
{
	if(g_GpsStatus == 0)		//GPS 初始app
	{
		g_GpsStatus = 1;
	}
	else if((g_GpsStatus == 1)&&(pRMCInfo.Status == 'A'))//	GPS 定位成功
	{
		debug_msg("\r\n^gps first Positioning success!\r\n");
		g_GpsStatus = 2;
		FlowMovie_IconDrawGPS(&UIFlowWndMovie_Status_GPSCtrl,0);
	}
	else if((g_GpsStatus == 2)&&(pRMCInfo.Status == 'A'))
	{//CHKPNT;
		g_GpsJitterCount = 0;
	}	
	else if((g_GpsStatus == 2)&&(pRMCInfo.Status != 'A'))//GPS信号丢失
	{
		debug_msg("\r\n^gps lost!\r\n");
		 g_GpsJitterCount++;
		if(g_GpsJitterCount >= 5)	//5s
		{
			g_GpsStatus = 3;
			g_GpsJitterCount = 0;
			FlowMovie_IconDrawGPS(&UIFlowWndMovie_Status_GPSCtrl,0);
		}
	}
	else if((g_GpsStatus == 3)&&(pRMCInfo.Status == 'A'))//	GPS信号正常
	{
		debug_msg("\r\n^gps success!\r\n");
		g_GpsStatus = 2;
		FlowMovie_IconDrawGPS(&UIFlowWndMovie_Status_GPSCtrl,0);
	}
	//debug_msg("\r\n^g_GpsStatus:%d\r\n",g_GpsStatus);
}

void  EnableSynGpsStatus(void)
{
	if(g_GpsStatus < 0)
	{
		g_GpsStatus = 0;
	}
}
extern GPSDATA    gpsdata;
void UI_DetGps(void)
{
	//CHKPNT;
  	RMCINFO RMCInfo;

      GPSRec_GetRMCDate(&RMCInfo);
      memcpy(&(gpsdata.rmcinfo),&RMCInfo,sizeof(RMCINFO));
      #if 0
      debug_msg("lati:%1f, NS:%c, longi:%1f,EW:%c\r\n",gpsdata.rmcinfo.Latitude,gpsdata.rmcinfo.NSInd,gpsdata.rmcinfo.Longitude,gpsdata.rmcinfo.EWInd);
      #endif

	//synchronizes with UTC to Beijing time
	static	 BOOL  g_bSysTimeOk = FALSE;
	if(( g_bSysTimeOk == FALSE )/*&&(UI_GetData(FL_TIME_SYNC) == TIME_SYNC_GPS)*/)
	{
		g_bSysTimeOk = SynGpsTime(RMCInfo) ;
	}
	/*else if(( g_bSysTimeOk == TRUE )&&(UI_GetData(FL_TIME_SYNC) == TIME_SYNC_MOBILEPHONE))
	{
		g_bSysTimeOk = FALSE;
	}*/
	
	 SynGpsStatus(RMCInfo);
	
	 //Gps_InfoShow();

}
#endif
//Gps Function end//////////////////////////////////////////////////////

static BOOL bParkPwrOn;
static INT32 iParkPwrOffTime = -1;
UINT32 uiAccPowerOffCount;
BOOL bAccPowerOff = FALSE;

BOOL System_GetGsensorPwrOn(void)
{
  return bParkPwrOn;
}
void System_SetGsensorPwrOn(BOOL bValue)
{
  bParkPwrOn = bValue;
}
void System_DecParkPwroffTimeCount(void)
{
   if(iParkPwrOffTime >= 0)
   {
   	  DBGD(iParkPwrOffTime);
      iParkPwrOffTime --;
	  if(iParkPwrOffTime == -1)
	  {
	      bAccPowerOff = TRUE;
		  Ux_PostEvent(NVTEVT_KEY_POWER_REL, 1, NVTEVT_KEY_RELEASE); 
	  }
   }
}
void System_SetParkPwroffTimeCount(INT32 iValue)
{
	iParkPwrOffTime = iValue;
}

extern BOOL bDemoStart;
static BOOL TimeLapseStatus = FALSE;
BOOL GetTimeLapseStatus(void)
{
	return TimeLapseStatus;
}
void SetTimeLapseStatus(BOOL flg)
{
	TimeLapseStatus = flg;
}



static BOOL accoffstatus = FALSE;
static BOOL accpower = FALSE;
extern UINT32 GetTimelapseTimeLimit(void);
BOOL DrvKey_DetAccPower(void)
{
	if(!accpower)
	{
		if(rtc_getPWR2Status())//ACC接入
		{
			accpower = TRUE;
		}
	}	
	if(!accpower)
		return FALSE;
	else
		return TRUE;
}
//
/*降压模式供电及ACC断开判断*/
/*步骤1:rtc_getPWR3Status()为1,降压线供电*/
/*步骤2:rtc_getPWR2Status()为0,acc off*/
//
BOOL DrvKey_DetAcc(void)
{
	if(rtc_getPWR3Status())//acc on
	{
   		if(rtc_getPWR2Status())
   		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
		return FALSE;
		
}
//MT pqw 20200327/////////////////////////////
/*
when acc off,entry park or timelapse function
*//////////////////////////////////////////////
extern BOOL	g_uiRecordIngMotionDet;
extern void SetMotionRecStatus(BOOL status);
void AccOnOffFunc(void)
{
	if(!DrvKey_DetAccPower())//非?笛瓜吖┑?,不做ACC ON/OFF判断,例如普通USB车充线供电
		return;
	
	static UINT32 uiTimeLapseRecTimeCount;
	
	if(!DrvKey_DetAcc())//acc off
	{
		if(bDemoStart)
		{
			Ux_PostEvent(NVTEVT_KEY_POWER_REL, 1, NVTEVT_KEY_RELEASE);
			return;
		}
			
		if(!accoffstatus)
		{
			uiAccPowerOffCount ++;
		}
		if(uiAccPowerOffCount >= 5)
		{
			accoffstatus = TRUE;
			uiAccPowerOffCount = 0;
			debug_msg("\r\n AccOff...\r\n");
		}

		if(accoffstatus && !System_GetGsensorPwrOn())
		{
			if (UI_GetData(FL_MOVIE_MOTION_DET) == MOVIE_MOTIONDET_ON)
			{
				
				if(FlowMovie_GetMovDataState()==MOV_ST_REC  && g_uiRecordIngMotionDet==FALSE)
				{
					debug_msg(" If is recording,must stop Rec,change to MD mode !!!![%06d], %s\r\n", __LINE__, __func__);
					FlowMovie_StopRec();
				}
				if(!g_uiRecordIngMotionDet)
					g_uiRecordIngMotionDet = TRUE;//acc off,entery motion detect pro
				return;
			}
		}
		
		if(SysGetFlag(FL_movie_timelapse_set) == MOVIE_TIMELAPSE_SET_OFF)
		{
			if(System_GetGsensorPwrOn() && SysGetFlag(FL_MOVIE_PARK) != MOVIE_PARK_OFF)
				accoffstatus = FALSE;//uiAccPowerOffCount = 0;
			//else
			//	uiAccPowerOffCount ++;
			
			if(accoffstatus)
            {
                bAccPowerOff = TRUE;
                Ux_PostEvent(NVTEVT_KEY_POWER_REL, 1, NVTEVT_KEY_RELEASE);
                //Ux_OpenWindow(&UIFlowWndAccPowerOffMsgCtrl, 0);
            }
			
		}
		else//if timelapse function opened
		{			
			if(accoffstatus)
			{
				if(!GetTimeLapseStatus())
				{
					Ux_PostEvent(NVTEVT_ACC_OFF_TIMELAPSE, 0);
					SetTimeLapseStatus(TRUE);
				}
				uiTimeLapseRecTimeCount++;
				//debug_msg("uiTimeLapseRecTimeCount:%d",uiTimeLapseRecTimeCount);
				if(uiTimeLapseRecTimeCount >= GetTimelapseTimeLimit())
				{
				    bAccPowerOff = TRUE;
	                Ux_PostEvent(NVTEVT_KEY_POWER_REL, 1, NVTEVT_KEY_RELEASE);
					uiTimeLapseRecTimeCount = 0;
					//Ux_OpenWindow(&UIFlowWndAccPowerOffMsgCtrl, 0);
				}
			}
		}
	}
	else//acc on
	{
		//GxSystem_CancelHWReset();
		//debug_msg("\r\n AccOn...\r\n");
        uiAccPowerOffCount = 0;
		accoffstatus = FALSE;
		
		if(g_uiRecordIngMotionDet)//MD mode change to normal rec mode
		{
			SetMotionRecStatus(FALSE);
			if(FlowMovie_GetMovDataState()!=MOV_ST_REC)
			{
				debug_msg(" If is not recording,must start Rec !!!![%06d], %s\r\n", __LINE__, __func__);
				Ux_PostEvent(NVTEVT_KEY_SHUTTER2, 1, NVTEVT_KEY_PRESS);
			}
			g_uiRecordIngMotionDet = FALSE;
		}
		
		uiTimeLapseRecTimeCount = 0;
        if(System_GetGsensorPwrOn())
        {  
            debug_msg("ACC IN\r\n");  
			FlowMovie_DrawLock(FALSE);
            System_SetGsensorPwrOn(FALSE);
            System_SetParkPwroffTimeCount(-1);
        }
		
		if(GetTimeLapseStatus())
		{
			Ux_PostEvent(NVTEVT_ACC_OFF_EXIT_TIMELAPSE, 0);
			SetTimeLapseStatus(FALSE);
		}
    }
}

#define ADC_LOW_BATTERY  355
#if 1
extern UINT32 DrvPower_GetBatteryValueMt(void);
//法拉电容低电保护
void LowPowerPro(void)
{
	if(GxUSB_GetIsUSBPlug())
		return;
	
	UINT32 getAccADC = 0;
	getAccADC = DrvPower_GetBatteryValueMt();	
	if(getAccADC <= ADC_LOW_BATTERY)
	{CHKPNT;
		GxSystem_ShutdownNOW();
	}
}
#endif

extern BOOL IsWaitingGsensorInt(void);
void WaitForGsensorInt(BOOL bValue);
static UINT32  uiGSIntCurStatus = FALSE;
static UINT32  uiGSIntPrevStatus = FALSE;
void UI_DetUserFunc(void)
{
	if(bDemoStart)//factory test
	{
		static UINT32 uiWaitIntTime = 0;
        if (IsWaitingGsensorInt())
        {
           CHKPNT;
		   uiWaitIntTime ++;
           uiGSIntCurStatus = rtc_getPWR4Status();
    	   if(uiGSIntCurStatus && !uiGSIntPrevStatus)
    	   {
    	      uiWaitIntTime = 0;
    	      debug_msg("^GGet Gsensor Int Signal Done!!\r\n");
    	      WifiCmd_Done(WIFIFLAG_GSENSOR_PARK_INT_DONE, 0);
		      ClearIntSts();
			  GSensor_ParkingMode(FALSE);
    	      WaitForGsensorInt(FALSE);
    	   }
		   if(uiWaitIntTime >= 20)
		   { 
		      debug_msg("^RWait Gsensor Int Signal TimeOut!!\r\n");
			  uiWaitIntTime = 0;
			  GSensor_ParkingMode(FALSE);
			  WaitForGsensorInt(FALSE);
		      WifiCmd_Done(WIFIFLAG_GSENSOR_PARK_INT_DONE, WIFIAPP_RET_GSENSOR_INT_TIMEOUT);
		   }
		   uiGSIntPrevStatus = uiGSIntCurStatus;

        }
		else
		{
		   uiWaitIntTime = 0;
		   ClearIntSts();
		}
	}
	
#if 1//
	//LowPowerPro();
#endif

    System_DecParkPwroffTimeCount();//停车监控60秒倒计时
	AccOnOffFunc();
	
	#if (ESD_RESET_FUNCTION==ENABLE)
	{
		if( System_GetState(SYS_STATE_CURRMODE) ==  PRIMARY_MODE_MOVIE ) 
			SystemSet_ESDReset();
	}
	#endif
	
	#if ((GPS_FUNCTION==ENABLE))
	UI_DetGps(); 
	#endif
	
}
///////////////////////////user function  end///////////////////

