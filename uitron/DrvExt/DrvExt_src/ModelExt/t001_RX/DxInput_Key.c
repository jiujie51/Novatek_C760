/**
    Copyright   Novatek Microelectronics Corp. 2009.  All rights reserved.

    @file       DetKey.c
    @ingroup    mIPRJAPKeyIO

    @brief      Scan key, modedial
                Scan key, modedial

    @note       Nothing.

    @date       2017/05/02
*/

/** \addtogroup mIPRJAPKeyIO */
//@{

#include "DxCfg.h"
#include "IOCfg.h"

#include "DxInput.h"
#include "Debug.h"
#include "KeyDef.h"
#include "rtc.h"
#include "HwPower.h"
#include "Delay.h"
#if 0 //simon_h
///////////////////////////////////////////////////////////////////////////////
#define __MODULE__          DxDrv
#define __DBGLVL__          2 // 0=FATAL, 1=ERR, 2=WRN, 3=UNIT, 4=FUNC, 5=IND, 6=MSG, 7=VALUE, 8=USER
#define __DBGFLT__          "*" //*=All, [mark]=CustomClass
#include "DebugModule.h"
///////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// ADC related
////////////////////////////////////////////////////////////////////////////////
#if (ADC_KEY == ENABLE)
#define VOLDET_KEY_ADC_LVL1          (251)
#define VOLDET_KEY_ADC_LVL2          (155)
#define VOLDET_KEY_ADC_LVL3          (51)
#define VOLDET_KEY_ADC_TH            (460)
#define VOLDET_KEY_ADC_LVL4          (358)

#define VOLDET_KEY_LVL_UNKNOWN           0xFFFFFFFF
#define VOLDET_KEY_LVL_0                 0
#define VOLDET_KEY_LVL_1                 1
#define VOLDET_KEY_LVL_2                 2
#define VOLDET_KEY_LVL_3                 3
#define VOLDET_KEY_LVL_4                 4
#define VOLDET_KEY_LVL_5                 5
#endif

#if (ADC_KEY == ENABLE)
static UINT32 VolDet_GetKey1ADC(void)
{
#if (VOLDET_ADC_CONT_MODE == DISABLE)
	UINT32 uiADCValue;

	uiADCValue = adc_readData(ADC_CH_VOLDET_KEY1);
	// One-Shot Mode, trigger one-shot
	adc_triggerOneShot(ADC_CH_VOLDET_KEY1);

	return uiADCValue;
#else
	return adc_readData(ADC_CH_VOLDET_KEY1);
#endif
}
/**
  Get ADC key voltage level

  Get  ADC key  2 voltage level.

  @param void
  @return UINT32 key level, refer to VoltageDet.h -> VOLDET_MS_LVL_XXXX
*/
static UINT32 VolDet_GetKey1Level(void)
{
	static UINT32   uiRetKey1Lvl;
	UINT32          uiKey1ADC, uiCurKey2Lvl;

	uiKey1ADC = VolDet_GetKey1ADC();
//	DBG_DUMP("uiKey1ADC %d \r\n", uiKey1ADC);
	if (uiKey1ADC < VOLDET_KEY_ADC_TH) {
		if (uiKey1ADC < VOLDET_KEY_ADC_LVL3) {
			uiCurKey2Lvl = VOLDET_KEY_LVL_4;
		} else if (uiKey1ADC < VOLDET_KEY_ADC_LVL2) {
			uiCurKey2Lvl = VOLDET_KEY_LVL_3;
		} else if (uiKey1ADC < VOLDET_KEY_ADC_LVL1) {
			uiCurKey2Lvl = VOLDET_KEY_LVL_2;
		} else if (uiKey1ADC < VOLDET_KEY_ADC_LVL4) {
			uiCurKey2Lvl = VOLDET_KEY_LVL_1;
		} else {
			uiCurKey2Lvl = VOLDET_KEY_LVL_0;
		}
	} else {
		uiCurKey2Lvl = VOLDET_KEY_LVL_UNKNOWN;
	}

	uiRetKey1Lvl = uiCurKey2Lvl;

	return uiRetKey1Lvl;
}

/**
  Detect Mode Switch state.

  Detect Mode Switch state.

  @param void
  @return UINT32 Mode Switch state (DSC Mode)
*/
#endif

////////////////////////////////////////////////////////////////////////////////
// GPIO related

//static BOOL g_bIsShutter2Pressed = FALSE;

/**
  Delay between toggle GPIO pin of input/output

  Delay between toggle GPIO pin of input/output

  @param void
  @return void
*/
static void DrvKey_DetKeyDelay(void)
{
	gpio_readData(0);
	gpio_readData(0);
	gpio_readData(0);
	gpio_readData(0);
}

void DrvKey_Init(void)
{
}
/**
  Detect normal key is pressed or not.

  Detect normal key is pressed or not.
  Return key pressed status (refer to KeyDef.h)

  @param void
  @return UINT32
*/
UINT32 DrvKey_DetNormalKey(void)
{
	UINT32 uiKeyCode = 0;

#if (ADC_KEY == ENABLE)
	UINT32 uiKey1Lvl = VolDet_GetKey1Level();
	switch (uiKey1Lvl) {
	case VOLDET_KEY_LVL_UNKNOWN:
	default:
		break;
	case VOLDET_KEY_LVL_0:
		uiKeyCode |= FLGKEY_RIGHT;
		break;
	case VOLDET_KEY_LVL_1:
		uiKeyCode |= FLGKEY_DOWN;
		break;
	case VOLDET_KEY_LVL_2:
		uiKeyCode |= FLGKEY_SHUTTER2;
		break;
	case VOLDET_KEY_LVL_3:
		uiKeyCode |= FLGKEY_UP;
		break;
	case VOLDET_KEY_LVL_4:
		uiKeyCode |= FLGKEY_ENTER;
		break;
	}
#endif


#if (GPIO_KEY == ENABLE)
	if (gpio_getPin(GPIO_KEY_LEFT)) {
		uiKeyCode |= FLGKEY_LEFT;
	}
	if (gpio_getPin(GPIO_KEY_SHUTTER1)) {
		uiKeyCode |= FLGKEY_SHUTTER1;
	}
	if (gpio_getPin(GPIO_KEY_SHUTTER2)) {
		uiKeyCode |= FLGKEY_SHUTTER2;
	}
	if (gpio_getPin(GPIO_KEY_ZOOMIN)) {
		uiKeyCode |= FLGKEY_ZOOMIN;
	}
	if (gpio_getPin(GPIO_KEY_ZOOMOUT)) {
		uiKeyCode |= FLGKEY_ZOOMOUT;
	}
	if (gpio_getPin(GPIO_KEY_ENTER)) {
		uiKeyCode |= FLGKEY_ENTER;
	}
#endif
	/*
	    //detect if power-on by press playback key
	    if (!HwPower_GetPowerKey(POWER_ID_PSW2))
	    {
	        uiKeyCode |= FLGKEY_PLAYBACK;
	    }
	*/
	DBG_IND("KEY=%08x\r\n", uiKeyCode);

	DrvKey_DetKeyDelay();
	return uiKeyCode;
}
#else
///////////////////////////////////////////////////////////////////////////////
#define __MODULE__          DxDrv
#define __DBGLVL__          2 // 0=FATAL, 1=ERR, 2=WRN, 3=UNIT, 4=FUNC, 5=IND, 6=MSG, 7=VALUE, 8=USER
#define __DBGFLT__          "*" //*=All, [mark]=CustomClass
#include "DebugModule.h"
///////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// ADC related
////////////////////////////////////////////////////////////////////////////////
#if (ADC_KEY == ENABLE)
//default adc value is 511
#define VOLDET_KEY_ADC_LVL0          (20)
#define VOLDET_KEY_ADC_LVL1          (120)
#define VOLDET_KEY_ADC_LVL2          (220)
#define VOLDET_KEY_ADC_LVL3          (320)


#define VOLDET_KEY_LVL_UNKNOWN           0xFFFFFFFF
#define VOLDET_KEY_LVL_0                 0
#define VOLDET_KEY_LVL_1                 1
#define VOLDET_KEY_LVL_2                 2
#define VOLDET_KEY_LVL_3                 3
#define VOLDET_KEY_LVL_4                 4
#define VOLDET_KEY_LVL_5                 5
#endif

extern void Multi_SendKey(UINT32 KeyID);
#define MULTI_KEY_MAKS_MENU     0x01
#define MULTI_KEY_MAKS_SHUTTER2 0x02


#if (ADC_KEY == ENABLE)
static UINT32 VolDet_GetKey1ADC(void)
{
#if (VOLDET_ADC_CONT_MODE == DISABLE)
	UINT32 uiADCValue;

	uiADCValue = adc_readData(ADC_CH_VOLDET_KEY1);
	// One-Shot Mode, trigger one-shot
	adc_triggerOneShot(ADC_CH_VOLDET_KEY1);

	return uiADCValue;
#else
	return adc_readData(ADC_CH_VOLDET_KEY1);
#endif
}
/**
  Get ADC key voltage level

  Get  ADC key  2 voltage level.

  @param void
  @return UINT32 key level, refer to VoltageDet.h -> VOLDET_MS_LVL_XXXX
*/
static UINT32 VolDet_GetKey1Level(void)
{
	UINT32          uiKey1ADC, uiRetKeyLvl;

	uiKey1ADC = VolDet_GetKey1ADC();
//	DBG_DUMP("uiKey1ADC %d \r\n", uiKey1ADC);
	if (uiKey1ADC < VOLDET_KEY_ADC_LVL0) {
		uiRetKeyLvl = VOLDET_KEY_LVL_0;
	} else if (uiKey1ADC < VOLDET_KEY_ADC_LVL1){
		uiRetKeyLvl = VOLDET_KEY_LVL_1;
	}else if (uiKey1ADC < VOLDET_KEY_ADC_LVL2){
		uiRetKeyLvl = VOLDET_KEY_LVL_2;
	}else if (uiKey1ADC < VOLDET_KEY_ADC_LVL3){ //simon_h
		uiRetKeyLvl = VOLDET_KEY_LVL_3;
	}else {
		uiRetKeyLvl = VOLDET_KEY_LVL_UNKNOWN;
	}

	return uiRetKeyLvl;
}

/**
  Detect Mode Switch state.

  Detect Mode Switch state.

  @param void
  @return UINT32 Mode Switch state (DSC Mode)
*/
#endif

////////////////////////////////////////////////////////////////////////////////
// GPIO related

//static BOOL g_bIsShutter2Pressed = FALSE;

/**
  Delay between toggle GPIO pin of input/output

  Delay between toggle GPIO pin of input/output

  @param void
  @return void
*/
static void DrvKey_DetKeyDelay(void)
{
	gpio_readData(0);
	gpio_readData(0);
	gpio_readData(0);
	gpio_readData(0);
}

void DrvKey_Init(void)
{
}
/**
  Detect normal key is pressed or not.

  Detect normal key is pressed or not.
  Return key pressed status (refer to KeyDef.h)

  @param void
  @return UINT32
*/


#define KEYSCAN_PWROFF_INIT_STATE       0
#define KEYSCAN_PWROFF_RELEASE_STATE    1
#define KEYSCAN_PWROFF_PRESS_STATE      2
#define KEYSCAN_KEY_PRESS_STATE		3

#define KEYSCAN_PWRKEY_UNKNOWN          0xFFFFFFFF
#define KEYSCAN_PWRKEY_RELEASED         0
#define KEYSCAN_PWRKEY_PRESSED          1


static UINT32 KEYLEVEL_MAP[6][3] = {
{FLGKEY_UP,            FLGKEY_I,            FLGKEY_I},//VOLDET_KEY_LVL_0
{FLGKEY_MENU,        FLGKEY_MODE,     FLGKEY_MODE},//VOLDET_KEY_LVL_1
{FLGKEY_DOWN,       FLGKEY_DOWN,    FLGKEY_DOWN},//VOLDET_KEY_LVL_2
//{FLGKEY_CUSTOM1,   FLGKEY_CUSTOM2,FLGKEY_CUSTOM2},//VOLDET_KEY_LVL_3
{FLGKEY_CUSTOM1,   FLGKEY_CUSTOM1,FLGKEY_CUSTOM1},//VOLDET_KEY_LVL_4
{FLGKEY_CUSTOM1,   FLGKEY_CUSTOM1,FLGKEY_CUSTOM1},//VOLDET_KEY_LVL_5
};

static UINT32  uiKeyCnt=0;
static UINT32  KeyPress=VOLDET_KEY_LVL_UNKNOWN;

UINT32 DrvKey_DetNormalKey(void)
{
    UINT32 uiKeyCode = 0;
    UINT32 keyRet,i;
    static UINT32 uiKeyCode_reSend=0;

    if(uiKeyCnt >100) //very long press
    {
        i=2;
    }
    else if(uiKeyCnt >20) //long time press
    {
        i=1;
    }
    else
    {
        i=0;
    }

    keyRet = VolDet_GetKey1Level();

    if(keyRet  == VOLDET_KEY_LVL_UNKNOWN)
    {
        if(KeyPress < 6)
        {
            uiKeyCode = KEYLEVEL_MAP[KeyPress][i];
            uiKeyCode_reSend = uiKeyCode;
        }
        else
        {
            if(uiKeyCode_reSend !=0)
            {
                uiKeyCode = uiKeyCode_reSend;
                uiKeyCode_reSend = 0;
            }
            else
            {
                uiKeyCode = 0;
            }
        }
        
        uiKeyCnt = 0;
        KeyPress = VOLDET_KEY_LVL_UNKNOWN;

    }
    else
    {
        KeyPress = keyRet;
        uiKeyCnt ++;
    }

    if(uiKeyCode>0){
        DBG_IND("KEY=%08x\r\n", uiKeyCode);
    }

    DrvKey_DetKeyDelay();
    return uiKeyCode;
}
#endif


#if 1 //simon_h
/**
  Detect power key is pressed or not.

  Detect power key is pressed or not.
  Return key pressed status (refer to KeyDef.h)

  @param void
  @return UINT32
*/
extern void system_poweroff(void);
extern void UISound_Play(UINT32 index);

UINT32 DrvKey_DetPowerKey(void)
{
	UINT32 uiKeyCode = 0;
	static UINT8 PwrPressCnt = 0;//long press shutdown

//    DBG_DUMP("ACC-Gsensor-B+-Powerkey   %d:%d:%d:%d \r\n",rtc_getPWRStatus(),rtc_getPWR2Status(),rtc_getPWR3Status(),rtc_getPWR4Status());

	if (HwPower_GetPowerKey(POWER_ID_PSW1)) {
  //  CHKPNT;
		//uiKeyCode = FLGKEY_KEY_POWER;
        //DBGD(PwrPressCnt);
        if(PwrPressCnt++ > 29)//0.1s timer
        {
            //DBG_ERR("\r\n ***%s, system_poweroff \r\n",__func__);
        	//system_poweroff();
        }
        
		// Reset shutdown timer
		HwPower_SetPowerKey(POWER_ID_PSW1, 0xf0);
	}
/*     else
    {
        if(PwrPressCnt > 0 && PwrPressCnt < 10)//short press release
        {
   //         CHKPNT;
            //uiKeyCode = FLGKEY_SHUTTER2;

            UISound_Play(1);//simon_h
            Multi_SendKey(MULTI_KEY_MAKS_SHUTTER2);  //  WiFi key
        }
        PwrPressCnt = 0;//clear
    }
*/

	return uiKeyCode;
}

#else
UINT32  uikeyCountPower=0;
UINT32 POWER_KEY_PRESS=0;
UINT32 POWER_KEY_PRESS_RELEASE=0;

UINT32 DrvKey_DetPowerKey(void)
{
    UINT32 uiKeyCode = 0;
///DBG_DUMP("uikeyCountPower=%d\r\n",uikeyCountPower);
    if(HwPower_GetPowerKey(POWER_ID_PSW4))
    	{
	uikeyCountPower++;
	POWER_KEY_PRESS_RELEASE = 4;
       rtc_resetShutdownTimer();	
	}
    else
  	{
	    if(uikeyCountPower>=1)
	    	{
		      if(uikeyCountPower<20)  //15
			    {
			    uiKeyCode = FLGKEY_SHUTTER2;	//down is  3KEY  up IS 4KEY
		      	}
		      else
		      	{
			    uiKeyCode = FLGKEY_KEY_POWER;
		      	}
		  	
	      }
	    }
  if(--POWER_KEY_PRESS_RELEASE==0)
  {
  	uikeyCountPower = 0;		 
	POWER_KEY_PRESS_RELEASE = 0;
	uiKeyCode=0;
	
  }		
//debug_msg("uikeyCountPower=%d,uiKeyCode=%d POWER_KEY_PRESS_RELEASE=%d \r\n", uikeyCountPower,uiKeyCode,POWER_KEY_PRESS_RELEASE);

    return uiKeyCode;
	  
}
#endif

UINT32 DrvKey_DetStatusKey(DX_STATUS_KEY_GROUP KeyGroup)
{
	UINT32 uiReturn = STATUS_KEY_LVL_UNKNOWN;
	switch (KeyGroup) {
	case DX_STATUS_KEY_GROUP1:
		break;

	case DX_STATUS_KEY_GROUP2:
		break;

	case DX_STATUS_KEY_GROUP3:
		break;

	case DX_STATUS_KEY_GROUP4:
		break;

	case DX_STATUS_KEY_GROUP5:
		break;

	default:
		DBG_ERR("[StatusKey]no this attribute");
		break;
	}
	return uiReturn;
}
