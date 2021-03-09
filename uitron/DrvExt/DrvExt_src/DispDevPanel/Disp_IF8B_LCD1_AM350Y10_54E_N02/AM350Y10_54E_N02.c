/*
    Display object object panel configuration parameters for AM350Y10_54E_N02

    @file       AM350Y10_54E_N02.c
    @ingroup
    @note       Nothing

    Copyright   Novatek Microelectronics Corp. 2011.  All rights reserved.
*/
#include "Debug.h"
#include "display.h"
#include "dispdev_IF8bits.h"
#include "dispdev_panlcomn.h"

#define IFCMD_CMD   0x01000000
#define IFCMD_DATA  0x02000000


const tPANEL_CMD tCmdStandby[] =
{

//  {MICMD_CMD,0x28}, //Display off
  {CMDDELAY_US, 1},
//  {MICMD_CMD,0x10}, //enter sleep mode
//  {MICMD_CMD,0x2c}, //memory write
};

const tPANEL_CMD tCmdModeRgbd320[] =
{  
#if 0 //lcd init code
	{CMDDELAY_MS, 5},
	//Part 1
	{0xFF,0x30},
	{0xFF,0x52},
	{0xFF,0x01},
	{0xE3,0x00},
	{0x40,0x00},
	{0x03,0x40},
	{0x04,0x00},
	{0x05,0x03},
	{0x08,0x00},
	{0x09,0x07},
	{0x0A,0x01},
	{0x0B,0x32},
	{0x0C,0x32},
	{0x0D,0x0B},
	{0x0E,0x00},
	{0x23,0xA2},	
	{0x24,0x0C},
	{0x25,0x06},
	{0x26,0x14},
	{0x27,0x14},	
	{0x38,0x9C},
	{0x39,0xA7},
	{0x3A,0x3a},	
	{0x28,0x40},
	{0x29,0x01},
	{0x2A,0xDF},
	{0x49,0x3C},
	{0x91,0x57},
	{0x92,0x57},
	{0xA0,0x55},
	{0xA1,0x50},
	{0xA4,0x9C},
	{0xA7,0x02},	
	{0xA8,0x01},
	{0xA9,0x01},
	{0xAA,0xFC},
	{0xAB,0x28},
	{0xAC,0x06},
	{0xAD,0x06},
	{0xAE,0x06},
	{0xAF,0x03},
	{0xB0,0x08},
	{0xB1,0x26},
	{0xB2,0x28},
	{0xB3,0x28},
	{0xB4,0x03},//{0xB4,0x33},
	{0xB5,0x08},
	{0xB6,0x26},
	{0xB7,0x08},
	{0xB8,0x26},
	{0xF0,0x00},
	{0xF6,0xC0},

//Part 2
	{0xFF,0x30},
	{0xFF,0x52},
	{0xFF,0x02},
	{0xB0,0x0B},
	{0xB1,0x16},
	{0xB2,0x17},
	{0xB3,0x2C},
	{0xB4,0x32},
	{0xB5,0x3B},
	{0xB6,0x29},
	{0xB7,0x40},
	{0xB8,0x0D},
	{0xB9,0x05},
	{0xBA,0x12},
	{0xBB,0x10},
	{0xBC,0x12},
	{0xBD,0x15},
	{0xBE,0x19},
	{0xBF,0x0E},
	{0xC0,0x16},
	{0xC1,0x0A},
	{0xD0,0x0C},
	{0xD1,0x17},
	{0xD2,0x14},
	{0xD3,0x2E},
	{0xD4,0x32},
	{0xD5,0x3C},
	{0xD6,0x22},
	{0xD7,0x3D},
	{0xD8,0x0D},
	{0xD9,0x07},
	{0xDA,0x13},
	{0xDB,0x13},
	{0xDC,0x11},
	{0xDD,0x15},
	{0xDE,0x19},
	{0xDF,0x10},
	{0xE0,0x17},
	{0xE1,0x0A},
	
	//Part 3
	{0xFF,0x30},
	{0xFF,0x52},
	{0xFF,0x03},
	{0x00,0x2A},
	{0x01,0x2A},
	{0x02,0x2A},
	{0x03,0x2A},
	{0x04,0x61},
	{0x05,0x80},	
	{0x06,0xC7},
	{0x07,0x01},
	{0x08,0x03},
	{0x09,0x04},
	{0x70,0x22},	
	{0x71,0x80},
	
	{0x30,0x2A},
	{0x31,0x2A},
	{0x32,0x2A},
	{0x33,0x2A},	
	{0x34,0x61},
	{0x35,0xC5},
	{0x36,0x80},
	{0x37,0x23},
	
	{0x40,0x03},
	{0x41,0x04},
	{0x42,0x05},
	{0x43,0x06},
	{0x44,0x11},
	{0x45,0xE8},
	{0x46,0xE9},
	{0x47,0x11},
	{0x48,0xEA},
	{0x49,0xEB},
	
	{0x50,0x07},
	{0x51,0x08},
	{0x52,0x09},
	{0x53,0x0A},
	{0x54,0x11},
	{0x55,0xEC},
	{0x56,0xED},
	{0x57,0x11},
	{0x58,0xEF},
	{0x59,0xF0},
	
	{0xB1,0x01},
	{0xB4,0x15},
	{0xB5,0x16},
	{0xB6,0x09},
	{0xB7,0x0F},
	{0xB8,0x0D},
	{0xB9,0x0B},
	{0xBA,0x00},

	{0xC7,0x02},
	{0xCA,0x17},
	{0xCB,0x18},
	{0xCC,0x0A},
	{0xCD,0x10},
	{0xCE,0x0E},
	{0xCF,0x0C},
	{0xD0,0x00},

	{0x81,0x00},
	{0x84,0x15},
	{0x85,0x16},
	{0x86,0x10},
	{0x87,0x0A},
	{0x88,0x0C},
	{0x89,0x0E},
	{0x8A,0x02},
	
	{0x97,0x00},
	{0x9A,0x17},
	{0x9B,0x18},
	{0x9C,0x0F},
	{0x9D,0x09},
	{0x9E,0x0B},
	{0x9F,0x0D},	
	{0xA0,0x01},
//
	{0xFF,0x30},
	{0xFF,0x52},
	{0xFF,0x02},
	{0x01,0x01},
	{0x02,0xDA},
	{0x03,0xBA},
	{0x04,0xA8},
	{0x05,0x9A},	
	{0x06,0x70},
	{0x07,0xFF},
	{0x08,0x91},
	{0x09,0x90},
	{0x0A,0xFF},	
	{0x0B,0x8F},
	{0x0C,0x60},
	{0x0D,0x58},	
	{0x0E,0x48},
	{0x0F,0x38},
	{0x10,0x2B},
	{0xFF,0x30},
	{0xFF,0x52},	
	{0xFF,0x00},
	{0x36,0x0A},
	{0x11,0x00},//Sleep out
	{CMDDELAY_MS, 200},
	{0x29,0x00},//display on
	{CMDDELAY_MS, 10},
#endif	
	
};

const tLCD_PARAM tMode[] =
{
    /***********       RGB Dummy-320 MODE         *************/
    {
        // tPANEL_PARAM
        {
            /* Old prototype */
         PINMUX_LCDMODE_RGB_THROUGH,         //!< LCDMode
         20,//6.25f,//6.35f,//6.25,//,                          //!< fDCLK
         680,//390,//1560/4,                           //!< uiHSyncTotalPeriod
         640,//1280/4,                           //!< uiHSyncActivePeriod
         20,//23,//22,//241/4,                            //!< uiHSyncBackPorch  
         500,//262,                            //!< uiVSyncTotalPeriod
         480,//240,                            //!< uiVSyncActivePeriod
         12,//21,                             //!< uiVSyncBackPorchOdd
         12,//21,                             //!< uiVSyncBackPorchEven
         640,                            //!< uiBufferWidth
         480,                            //!< uiBufferHeight
         640,                            //!< uiWindowWidth
         480,                            //!< uiWindowHeight
         FALSE,                          //!< bYCbCrFormat

          /* New added parameters */
         0x02,                           //!< uiHSyncSYNCwidth
         0x02                            //!< uiVSyncSYNCwidth
           
        },

        // tIDE_PARAM
        {
            /* Old prototype */
            PINMUX_LCD_SEL_SERIAL_RGB_8BITS,//!< pinmux_select_lcd;
            ICST_CCIR601,                   //!< icst;
            {FALSE,FALSE},                  //!< dithering[2];
            DISPLAY_DEVICE_CASIO2G,         //!< **DONT-CARE**
            IDE_PDIR_RGB,//IDE_PDIR_RBG,                   //!< pdir;
            IDE_LCD_B,//IDE_LCD_G,                      //!< odd;
            IDE_LCD_B,//IDE_LCD_G,                      //!< even;
            TRUE,                           //!< hsinv;
            TRUE,                           //!< vsinv;
            
            FALSE,                          //!< hvldinv;
            FALSE,                          //!< vvldinv;
            
            TRUE,                           //!< clkinv;
            TRUE,                          //!< fieldinv;
            
            FALSE,                          //!< **DONT-CARE**
            FALSE,                          //!< interlace;
            FALSE,                          //!< **DONT-CARE**
            0x3A,                           //!< ctrst;
            0x00,                           //!< brt;
            0x40,                           //!< cmults;
            FALSE,                          //!< cex;
            FALSE,                          //!< **DONT-CARE**
            TRUE,                           //!< **DONT-CARE**
            TRUE,                           //!< tv_powerdown;
            {0x00,0x00},                    //!< **DONT-CARE**

            /* New added parameters */
            FALSE,                          //!< YCex
            FALSE,                          //!< HLPF
            {FALSE,FALSE,FALSE},               //!< subpixOdd[3]
            {FALSE,FALSE,FALSE},            //!< subpixEven[3]
            {IDE_DITHER_6BITS,IDE_DITHER_6BITS,IDE_DITHER_6BITS}//!< DitherBits[3]
        },
        (tPANEL_CMD*)tCmdModeRgbd320,                  //!< pCmdQueue
        sizeof(tCmdModeRgbd320)/sizeof(tPANEL_CMD),    //!< nCmd
    }

};


#define ROTATE_PARA_COUNT 5
const tPANEL_CMD tCmdRotate_None[] =
{
 /*       {IFCMD_CMD, 0x11}, //Exit Sleep
        {IFCMD_CMD, 0x36},
        {IFCMD_DATA,0x08},//0xc8 for rotate
        {IFCMD_CMD, 0x29}, //Display ON
        {IFCMD_CMD,0x2c}, //memory write
        */
        {CMDDELAY_MS, 120},
};
const tPANEL_CMD tCmdRotate_180[] =
{
    /*    {IFCMD_CMD, 0x11}, //Exit Sleep
        {IFCMD_CMD, 0x36},
        {IFCMD_DATA,0xC8},//0xc8 for rotate
        {IFCMD_CMD, 0x29}, //Display ON
        {IFCMD_CMD,0x2c}, //memory write
        */
        {CMDDELAY_MS, 120},
};
const tLCD_ROT tRotate[] =
{
    {DISPDEV_LCD_ROTATE_NONE,(tPANEL_CMD*)tCmdRotate_None,ROTATE_PARA_COUNT},
    {DISPDEV_LCD_ROTATE_180,(tPANEL_CMD*)tCmdRotate_180,ROTATE_PARA_COUNT}
};

tLCD_ROT* dispdev_getLcdRotateCmd(UINT32 *ModeNumber)
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

tLCD_PARAM* dispdev_getConfigMode(UINT32 *ModeNumber)
{
    *ModeNumber = sizeof(tMode)/sizeof(tLCD_PARAM);
    return (tLCD_PARAM*)tMode;
}

tPANEL_CMD* dispdev_getStandbyCmd(UINT32 *CmdNumber)
{
    *CmdNumber = sizeof(tCmdStandby)/sizeof(tPANEL_CMD);
    return (tPANEL_CMD*)tCmdStandby;
}

void dispdev_writeToLcdSif(DISPDEV_IOCTRL pDispDevControl, UINT32 addr, UINT32 value)
{
    UINT32                  uiSIFData;
    DISPDEV_IOCTRL_PARAM    DevIOCtrl;

    uiSIFData = ((((UINT32)addr << 8)|((UINT32)value)) << 16);

    pDispDevControl(DISPDEV_IOCTRL_GET_REG_IF, &DevIOCtrl);
    sif_send(DevIOCtrl.SEL.GET_REG_IF.uiSifCh, uiSIFData, 0, 0);
}

static void dispdev_writeToLcdGpio_Cmd(DISPDEV_IOCTRL pDispDevControl, UINT32 addr, UINT32 value)
{//CHKPNT;
    UINT32                  uiSIFData,j;
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
    gpio_clearPin(SifClk);

    uiSIFData = (UINT32)value;

    Delay_DelayUs(500);

    gpio_clearPin(SifSen);
    for(j=9; j>0; j--)
    {
        if(((uiSIFData >> (j - 1)) & 0x01))
        {
            gpio_setPin(SifData);
        }
        else
        {
            gpio_clearPin(SifData);
        }

        Delay_DelayUs(100);
        gpio_setPin(SifClk);
        Delay_DelayUs(200);
        gpio_clearPin(SifClk);
        Delay_DelayUs(100);
    }
    Delay_DelayUs(500);

    gpio_setPin(SifSen);

}


static void dispdev_writeToLcdGpio_data(DISPDEV_IOCTRL pDispDevControl, UINT32 addr, UINT32 value)
{//CHKPNT;
    UINT32                  uiSIFData,j;
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
    gpio_clearPin(SifClk);

    uiSIFData = (UINT32)value;
    uiSIFData |= 0x100;

    Delay_DelayUs(500);

    gpio_clearPin(SifSen);

    for(j=9; j>0; j--)
    {
        if(((uiSIFData >> (j - 1)) & 0x01))
        {
            gpio_setPin(SifData);
        }
        else
        {
            gpio_clearPin(SifData);
        }

        Delay_DelayUs(100);
        gpio_setPin(SifClk);
        Delay_DelayUs(200);
        gpio_clearPin(SifClk);
        Delay_DelayUs(100);
    }
    Delay_DelayUs(500);	
    gpio_setPin(SifSen);
}
void dispdev_writeToLcdGpio(DISPDEV_IOCTRL pDispDevControl, UINT32 addr, UINT32 value)
{
   if(addr & IFCMD_DATA)
  		dispdev_writeToLcdGpio_data(pDispDevControl,addr,value);
    else
        dispdev_writeToLcdGpio_Cmd(pDispDevControl,addr,value);
}

void dispdev_readFromLcdGpio(DISPDEV_IOCTRL pDispDevControl, UINT32 addr, UINT32* p_value)
{
   // debug_err(("Has not implement dispdev_readFromLcdGpio \r\n"));
}

