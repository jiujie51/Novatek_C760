/*
    Display object object panel configuration parameters for AM350Y10_54E_N02

    @file       AM350Y10_54E_N02.c
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

#define Delayms(value)	{CMDDELAY_MS, value}
#define wr_cmd(value)	{IFCMD_CMD, value}
#define wr_dat(value)	{IFCMD_DATA, value}

#define PANEL_WIDTH     640
#define PANEL_HEIGHT    480

#define HSYNCT      40 //2
#define VSYNCT     18 //  6


/*
    Panel Parameters for T30P105
*/
const tPANEL_CMD tCmdStandby[] =
{
  //  {0x2B, 0x00}
};


const tPANEL_CMD tCmdModeRgbd320[] =
{ 
#if 1
//PAGE 1
    wr_cmd(0xFF),
    wr_dat(0x30),
    wr_dat(0x52),
    wr_dat(0x01),

    wr_cmd(0xE3),
    wr_dat(0x00),

    wr_cmd(0x40),
    wr_dat(0x00),

    wr_cmd(0x03),
    wr_dat(0x40),
    wr_cmd(0x04),
    wr_dat(0x00),   
  	wr_cmd(0x05),
    wr_dat(0x03),   
    wr_cmd(0x08),
    wr_dat(0x00),    
    wr_cmd(0x09),
    wr_dat(0x07),    
    wr_cmd(0x0A),
    wr_dat(0x01),
    wr_cmd(0x0B),
    wr_dat(0x32),
    wr_cmd(0x0C),
    wr_dat(0x32),   
    wr_cmd(0x0D),
    wr_dat(0x0B),   
    wr_cmd(0x0E),
    wr_dat(0x00),
    
    wr_cmd(0x23),
    wr_dat(0xA2),    
    wr_cmd(0x24),
    wr_dat(0x0C),   
    wr_cmd(0x25),
    wr_dat(0x06),   
    wr_cmd(0x26),
    wr_dat(0x14),   
    wr_cmd(0x27),
    wr_dat(0x14),

    wr_cmd(0x38),
    wr_dat(0x9C),   
    wr_cmd(0x39),
    wr_dat(0xA7),   
    wr_cmd(0x3A),
    wr_dat(0x3A),

    wr_cmd(0x28),
    wr_dat(0x40),   
    wr_cmd(0x29),
    wr_dat(0x01),   
    wr_cmd(0x2A),
    wr_dat(0xDF),

    wr_cmd(0x49),
    wr_dat(0x3C),   
    wr_cmd(0x91),
    wr_dat(0x57),   
    wr_cmd(0x92),
    wr_dat(0x57),

    wr_cmd(0xA0),
    wr_dat(0x55),   
    wr_cmd(0xA1),
    wr_dat(0x50),   
    wr_cmd(0xA4),
    wr_dat(0x9C),

    wr_cmd(0xA7),
    wr_dat(0x02),   
    wr_cmd(0xA8),
    wr_dat(0x01),   
    wr_cmd(0xA9),
    wr_dat(0x01),
    wr_cmd(0xAA),
    wr_dat(0xFC),   
    wr_cmd(0xAB),
    wr_dat(0x28),   
    wr_cmd(0xAC),
    wr_dat(0x06),
    wr_cmd(0xAD),
    wr_dat(0x06),   
    wr_cmd(0xAE),
    wr_dat(0x06),   
    wr_cmd(0xAF),
    wr_dat(0x03),

    wr_cmd(0xB0),
    wr_dat(0x08),
    wr_cmd(0xB1),
    wr_dat(0x26),   
    wr_cmd(0xB2),
    wr_dat(0x28),   
    wr_cmd(0xB3),
    wr_dat(0x28),
    wr_cmd(0xB4),
    wr_dat(0x03),   
    wr_cmd(0xB5),
    wr_dat(0x08),   
    wr_cmd(0xB6),
    wr_dat(0x26),
    wr_cmd(0xB7),
    wr_dat(0x08),   
    wr_cmd(0xB8),
    wr_dat(0x26),
    
    wr_cmd(0xF0),
    wr_dat(0x00),   
    wr_cmd(0xF6),
    wr_dat(0xC0),

//PART2
    wr_cmd(0xFF),
    wr_dat(0x30),   
    wr_dat(0x52),
    wr_dat(0x02),
    
    wr_cmd(0xB0),
    wr_dat(0x0B),
    wr_cmd(0xB1),
    wr_dat(0x16),   
    wr_cmd(0xB2),
    wr_dat(0x17),   
    wr_cmd(0xB3),
    wr_dat(0x2C),
    wr_cmd(0xB4),
    wr_dat(0x32),   
    wr_cmd(0xB5),
    wr_dat(0x3B),   
    wr_cmd(0xB6),
    wr_dat(0x29),
    wr_cmd(0xB7),
    wr_dat(0x40),   
    wr_cmd(0xB8),
    wr_dat(0x0D),
    wr_cmd(0xB9),
    wr_dat(0x05),
    wr_cmd(0xBA),
    wr_dat(0x12),   
    wr_cmd(0xBB),
    wr_dat(0x10),   
    wr_cmd(0xBC),
    wr_dat(0x12),
    wr_cmd(0xBD),
    wr_dat(0x15),   
    wr_cmd(0xBE),
    wr_dat(0x19),
    wr_cmd(0xBF),
    wr_dat(0x0E),

    wr_cmd(0xC0),
    wr_dat(0x16),   
    wr_cmd(0xC1),
    wr_dat(0x0A),  
    
    wr_cmd(0xD0),
    wr_dat(0x0C),
    wr_cmd(0xD1),
    wr_dat(0x17),   
    wr_cmd(0xD2),
    wr_dat(0x14),
    wr_cmd(0xD3),
    wr_dat(0x2E),
    wr_cmd(0xD4),
    wr_dat(0x32),
    wr_cmd(0xD5),
    wr_dat(0x3C),   
    wr_cmd(0xD6),
    wr_dat(0x22),
    wr_cmd(0xD7),
    wr_dat(0x3D),
    wr_cmd(0xD8),
    wr_dat(0x0D),
    wr_cmd(0xD9),
    wr_dat(0x07),
    wr_cmd(0xDA),
    wr_dat(0x13),
    wr_cmd(0xDB),
    wr_dat(0x13),   
    wr_cmd(0xDC),
    wr_dat(0x11),
    wr_cmd(0xDD),
    wr_dat(0x15),
    wr_cmd(0xDE),
    wr_dat(0x19),
    wr_cmd(0xDF),
    wr_dat(0x10),
    
    wr_cmd(0xE0),
    wr_dat(0x17),   
    wr_cmd(0xE1),
    wr_dat(0x0A),
    
 //PART 3   
    wr_cmd(0xFF),
    wr_dat(0x30),
    wr_dat(0x52),
    wr_dat(0x03),

    wr_cmd(0x00),
    wr_dat(0x2A),   
    wr_cmd(0x01),
    wr_dat(0x2A),
    wr_cmd(0x02),
    wr_dat(0x2A),
    wr_cmd(0x03),
    wr_dat(0x2A),   
    wr_cmd(0x04),
    wr_dat(0x61),
    wr_cmd(0x05),
    wr_dat(0x80),
    wr_cmd(0x06),
    wr_dat(0xC7),   
    wr_cmd(0x07),
    wr_dat(0x01),
    wr_cmd(0x08),
    wr_dat(0x03),
    wr_cmd(0x09),
    wr_dat(0x04),
    
    wr_cmd(0x70),
    wr_dat(0x22),
    wr_cmd(0x71),
    wr_dat(0x80),

    wr_cmd(0x30),
    wr_dat(0x2A),   
    wr_cmd(0x31),
    wr_dat(0x2A),
    wr_cmd(0x32),
    wr_dat(0x2A),
    wr_cmd(0x33),
    wr_dat(0x2A),   
    wr_cmd(0x34),
    wr_dat(0x61),
    wr_cmd(0x35),
    wr_dat(0xC5),
    wr_cmd(0x36),
    wr_dat(0x80),   
    wr_cmd(0x37),
    wr_dat(0x23),
    
    wr_cmd(0x40),
    wr_dat(0x03),
    wr_cmd(0x41),
    wr_dat(0x04),
    wr_cmd(0x42),
    wr_dat(0x05),
    wr_cmd(0x43),
    wr_dat(0x06),
    wr_cmd(0x44),
    wr_dat(0x11),
    wr_cmd(0x45),
    wr_dat(0xE8),
    wr_cmd(0x46),
    wr_dat(0xE9),
    wr_cmd(0x47),
    wr_dat(0x11),
    wr_cmd(0x48),
    wr_dat(0xEA),
    wr_cmd(0x49),
    wr_dat(0xEB),

    wr_cmd(0x50),
    wr_dat(0x07),
    wr_cmd(0x51),
    wr_dat(0x08),
    wr_cmd(0x52),
    wr_dat(0x09),
    wr_cmd(0x53),
    wr_dat(0x0A),
    wr_cmd(0x54),
    wr_dat(0x11),
    wr_cmd(0x55),
    wr_dat(0xEC),
    wr_cmd(0x56),
    wr_dat(0xED),
    wr_cmd(0x57),
    wr_dat(0x11),
    wr_cmd(0x58),
    wr_dat(0xEF),
    wr_cmd(0x59),
    wr_dat(0xF0),

    wr_cmd(0xB1),
    wr_dat(0x01),
    wr_cmd(0xB4),
    wr_dat(0x15),
    wr_cmd(0xB5),
    wr_dat(0x16),
    wr_cmd(0xB6),
    wr_dat(0x09),
    wr_cmd(0xB7),
    wr_dat(0x0F),
    wr_cmd(0xB8),
    wr_dat(0x0D),
    wr_cmd(0xB9),
    wr_dat(0x11),
    wr_cmd(0xBA),
    wr_dat(0xEF),


	
	wr_cmd(0xFF),
	wr_dat(0x30),
	wr_dat(0x52),
	wr_dat(0x00),
	
	wr_cmd(0x11),
	Delayms (220),
	wr_cmd(0x29),
	Delayms (10),

    
#endif
};


const tLCD_PARAM tMode[] =
{
    /***********       RGB Dummy-320 MODE         *************/
    {
        // tPANEL_PARAM
        {
            /* Old prototype */
            PINMUX_LCDMODE_RGB_PARALL565,			//!< LCDMode
            20.0f,//29.0,//27.0f,									//!< fDCLK 24.54f,
            682,//40 + 640,								//!< uiHSyncTotalPeriod
            640,									//!< uiHSyncActivePeriod
            20,										//!< uiHSyncBackPorch
            498,//20 + 480,								//!< uiVSyncTotalPeriod
            480,									//!< uiVSyncActivePeriod
            6,//4,//12,								//!< uiVSyncBackPorchOdd  29 15
            6,//12,//4,//12,						//!< uiVSyncBackPorchEven 29.5 15
            640,									//!< uiBufferWidth
            480,									//!< uiWindowHeight
            640,									//!< uiWindowWidth
            480,									//!< uiWindowHeight
            FALSE,									//!< bYCbCrFormat

            /* New added parameters */
            HSYNCT,                           //!< uiHSyncSYNCwidth
            VSYNCT,                           //!< uiVSyncSYNCwidth
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
    //{0x04,0x1b}
    {0x0B,0x00},
};
const tPANEL_CMD tCmdRotate_180[] =
{
    //{0x04,0x1B}
    {0x0B,0x03},

};
const tLCD_ROT tRotate[] =
{
    {DISPDEV_LCD_ROTATE_NONE,(tPANEL_CMD*)tCmdRotate_None,1},
    {DISPDEV_LCD_ROTATE_180,(tPANEL_CMD*)tCmdRotate_180,1}
};
//@}

tLCD_ROT* dispdev_getLcdRotateCmdParal(UINT32 *ModeNumber)
{CHKPNT;
CHKPNT;
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
{CHKPNT;
CHKPNT;
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
{CHKPNT;
CHKPNT;
CHKPNT;
    UINT32                  uiSIFData;
    DISPDEV_IOCTRL_PARAM    DevIOCtrl;
 
    if (addr == IFCMD_CMD) {
         uiSIFData = (value & 0x00ff) << (32-9);
         pDispDevControl(DISPDEV_IOCTRL_GET_REG_IF, &DevIOCtrl);
         sif_send(DevIOCtrl.SEL.GET_REG_IF.uiSifCh, uiSIFData, 0, 0);
 
    } else if (addr == IFCMD_DATA) {
         uiSIFData = ((value & 0x00ff) | 0x100) << (32-9);
         pDispDevControl(DISPDEV_IOCTRL_GET_REG_IF, &DevIOCtrl);
         sif_send(DevIOCtrl.SEL.GET_REG_IF.uiSifCh, uiSIFData, 0, 0);
    }
}

void dispdev_writeToLcdGpioParal(DISPDEV_IOCTRL pDispDevControl, UINT32 addr, UINT32 value)
#if 0
{
    //UINT32                  uiSIFData,j,i;
    UINT32                  j;
    UINT32                  SifClk,SifSen,SifData;
    DISPDEV_IOCTRL_PARAM    DevIOCtrl;

	//DBG_DUMP("^C dispdev_writeToLcdGpioParal!\r\n");
    pDispDevControl(DISPDEV_IOCTRL_GET_REG_IF, &DevIOCtrl);
    SifClk  = DevIOCtrl.SEL.GET_REG_IF.uiGpioClk;
    SifSen  = DevIOCtrl.SEL.GET_REG_IF.uiGpioSen;
    SifData = DevIOCtrl.SEL.GET_REG_IF.uiGpioData;
	//DBG_DUMP("^C SifClk %d, SifSen %d, SifData %d\r\n", SifClk, SifSen, SifData);

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
#else
{
CHKPNT;
CHKPNT;
CHKPNT;
CHKPNT;
CHKPNT;
	UINT32                  uiSIFData, j;
	UINT32                  SifClk, SifSen, SifData;
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

	uiSIFData = (UINT32)addr << 9 | (UINT32)value;

	Delay_DelayUs(500);

	gpio_clearPin(SifSen);

	for (j = 16; j > 0; j--) {
		if (((uiSIFData >> (j - 1)) & 0x01)) {
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

	Delay_DelayUs(500);
	gpio_setPin(SifSen);

}
#endif

void dispdev_readFromLcdGpioParal(DISPDEV_IOCTRL pDispDevControl, UINT32 addr, UINT32* p_value)
{CHKPNT;
CHKPNT;
    //debug_err(("Has not implement dispdev_readFromLcdGpio \r\n"));
}
#endif
