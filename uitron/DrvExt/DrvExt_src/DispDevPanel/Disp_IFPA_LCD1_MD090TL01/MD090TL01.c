/*
    Display object object panel configuration parameters for T30P105

    @file       T30P106.c
    @ingroup
    @note       Nothing

    Copyright   Novatek Microelectronics Corp. 2011.  All rights reserved.
*/

#include "kernel.h"
#include "Debug.h"
#include "display.h"
#include "dispdev_IFParal.h"
#include "dispdev_panlcomn.h"

#define IFCMD_CMD		0x01000000
#define IFCMD_DATA		0x02000000

#define PANEL_WIDTH     1920//360
#define PANEL_HEIGHT    384

#define HSYNCT      30
#define VSYNCT      4


/*
    Panel Parameters for T30P105
*/
const tPANEL_CMD tCmdStandby[] =
{
  //  {0x2B, 0x00}
};


const tPANEL_CMD tCmdModeRgbd320[] =
{
};


const tLCD_PARAM tMode[] =
{
    /***********       RGB Dummy-320 MODE         *************/
    {
        // tPANEL_PARAM
        {
            /* Old prototype */
            PINMUX_LCDMODE_RGB_PARALL565,//PINMUX_LCDMODE_RGB_THROUGH,         //!< LCDMode
            48.0f,//33.0f,                         //!< fDCLK 24.54f,
            (150+PANEL_WIDTH),//1920+150,                           //!< uiHSyncTotalPeriod
            PANEL_WIDTH,//1920,                           //!< uiHSyncActivePeriod
            60,//36,                            //!< uiHSyncBackPorch
            0x52 + PANEL_HEIGHT, //0X52+384                           //!< uiVSyncTotalPeriod
            PANEL_HEIGHT,//384                          //!< uiVSyncActivePeriod
            10,//20, //27                            //!< uiVSyncBackPorchOdd  29
            10,//20,//27.5                             //!< uiVSyncBackPorchEven 29.5
            PANEL_WIDTH,                            //!< uiBufferWidth
            PANEL_HEIGHT,                            //!< uiWindowHeight
            PANEL_WIDTH,                            //!< uiWindowWidth
            PANEL_HEIGHT,                            //!< uiWindowHeight
            FALSE,                          //!< bYCbCrFormat

            /* New added parameters */
            HSYNCT,                           //!< uiHSyncSYNCwidth
            0x00,                           //!< uiVSyncSYNCwidth
        },

        // tIDE_PARAM
        {
            /* Old prototype */
            PINMUX_LCD_SEL_PARALLE_RGB565|PINMUX_LCD_SEL_DE_ENABLE,//PINMUX_LCD_SEL_SERIAL_RGB_8BITS,//!< pinmux_select_lcd;
            ICST_CCIR601,                   //!< icst;
            {FALSE,FALSE},                  //!< dithering[2];
            DISPLAY_DEVICE_AU,              //!< **DONT-CARE**
            IDE_PDIR_RGB,                   //!< pdir;
            IDE_LCD_G,                      //!< odd;
            IDE_LCD_G,                      //!< even;
            TRUE,                           //!< hsinv;
            TRUE,                           //!< vsinv;
            FALSE,                          //!< hvldinv;
            FALSE,                          //!< vvldinv;
            TRUE,                           //!< clkinv;
            FALSE,                          //!< fieldinv;
            FALSE,                          //!< **DONT-CARE**
            FALSE,                          //!< interlace;
            FALSE,                          //!< **DONT-CARE**
            0x40,                           //!< ctrst;
            0x00,                           //!< brt;
            0x58,                           //!< cmults;
            FALSE,                          //!< cex;
            FALSE,                          //!< **DONT-CARE**
            TRUE,                           //!< **DONT-CARE**
            TRUE,                           //!< tv_powerdown;
            {0x00,0x00},                    //!< **DONT-CARE**

            /* New added parameters */
            FALSE,                          //!< YCex
            FALSE,                          //!< HLPF
            {FALSE,FALSE,FALSE},            //!< subpixOdd[3]
            {FALSE,FALSE,FALSE},            //!< subpixEven[3]
            {IDE_DITHER_6BITS,IDE_DITHER_6BITS,IDE_DITHER_6BITS}//!< DitherBits[3]
        },

        (tPANEL_CMD*)tCmdModeRgbd320,                  //!< pCmdQueue
        sizeof(tCmdModeRgbd320)/sizeof(tPANEL_CMD),    //!< nCmd
    }
};
const tPANEL_CMD tCmdRotate_None[] =
{
    {0x0B,0x00},
};
const tPANEL_CMD tCmdRotate_180[] =
{
    {0x0B,0x03},
};
const tLCD_ROT tRotate[] =
{
    {DISPDEV_LCD_ROTATE_NONE,(tPANEL_CMD*)tCmdRotate_None,1},
    {DISPDEV_LCD_ROTATE_180,(tPANEL_CMD*)tCmdRotate_180,1}
};
//@}

tLCD_ROT* dispdev_getLcdRotateCmdParal(UINT32 *ModeNumber)
{
    if(tRotate != NULL)
    {
        *ModeNumber = sizeof(tRotate)/sizeof(tLCD_ROT);
    }
    else
    {
        *ModeNumber = 0;
    }
    return (tLCD_ROT*)tRotate;
}

tLCD_PARAM* dispdev_getConfigModeParal(UINT32 *ModeNumber)
{
    *ModeNumber = sizeof(tMode)/sizeof(tLCD_PARAM);
    return (tLCD_PARAM*)tMode;
}

tPANEL_CMD* dispdev_getStandbyCmdParal(UINT32 *CmdNumber)
{
    *CmdNumber = sizeof(tCmdStandby)/sizeof(tPANEL_CMD);
    return (tPANEL_CMD*)tCmdStandby;
}

#if 1
void dispdev_writeToLcdSifParal(DISPDEV_IOCTRL pDispDevControl, UINT32 addr, UINT32 value)
{
    UINT32                  uiSIFData;
    DISPDEV_IOCTRL_PARAM    DevIOCtrl;

    uiSIFData = ((((UINT32)addr << 8)|((UINT32)value)) << 16);

    pDispDevControl(DISPDEV_IOCTRL_GET_REG_IF, &DevIOCtrl);
    sif_send(DevIOCtrl.SEL.GET_REG_IF.uiSifCh, uiSIFData, 0, 0);
}

void dispdev_writeToLcdGpioParal(DISPDEV_IOCTRL pDispDevControl, UINT32 addr, UINT32 value)
{
    //UINT32                  uiSIFData,j,i;
    UINT32                  j;
    UINT32                  SifClk,SifSen,SifData;
    DISPDEV_IOCTRL_PARAM    DevIOCtrl;

    pDispDevControl(DISPDEV_IOCTRL_GET_REG_IF, &DevIOCtrl);
    SifClk  = DevIOCtrl.SEL.GET_REG_IF.uiGpioClk;
    SifSen  = DevIOCtrl.SEL.GET_REG_IF.uiGpioSen;
    SifData = DevIOCtrl.SEL.GET_REG_IF.uiGpioData;

    gpio_setDir(SifSen, GPIO_DIR_OUTPUT);
    gpio_setDir(SifClk, GPIO_DIR_OUTPUT);
    gpio_setDir(SifData, GPIO_DIR_OUTPUT);

    gpio_setPin(SifSen);
    gpio_setPin(SifData);
    gpio_setPin(SifClk);

    //uiSIFData = (UINT32)addr << 8 | (UINT32)value;
    //debug_msg("Has not implement dispdev_readFromLcdGpio%x %x  %x\r\n",uiSIFData,addr,value);
    Delay_DelayUs(500);

    gpio_clearPin(SifSen);

	if (addr == IFCMD_CMD) {
        value = (value & 0x00ff);
		for (j = 9; j > 0; j--) {
			if (((value >> (j - 1)) & 0x01)) {
				gpio_setPin(SifData);
			} else {
				gpio_clearPin(SifData);
			}

			Delay_DelayUs(100);
			gpio_clearPin(SifClk);
			Delay_DelayUs(200);
			gpio_setPin(SifClk);
			Delay_DelayUs(100);
		}
	} else if (addr == IFCMD_DATA) {
		value = ((value & 0x00ff) | 0x100);
		for (j = 9; j > 0; j--) {
			if (((value >> (j - 1)) & 0x01)) {
				gpio_setPin(SifData);
			} else {
				gpio_clearPin(SifData);
			}

			Delay_DelayUs(100);
			gpio_clearPin(SifClk);
			Delay_DelayUs(200);
			gpio_setPin(SifClk);
			Delay_DelayUs(100);
		}
	}

    Delay_DelayUs(500);
    gpio_setPin(SifSen);

}


void dispdev_readFromLcdGpioParal(DISPDEV_IOCTRL pDispDevControl, UINT32 addr, UINT32* p_value)
{
    //debug_err(("Has not implement dispdev_readFromLcdGpio \r\n"));
}
#endif
