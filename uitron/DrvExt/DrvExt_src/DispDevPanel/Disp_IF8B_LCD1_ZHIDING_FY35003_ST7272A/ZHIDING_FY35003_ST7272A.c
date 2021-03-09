/*
    Display object object panel configuration parameters for ZHIDING_FY35003_ST7272A

    @file       ZHIDING_FY35003_ST7272A.c
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
#if 1
#if 0//pqw
	{0x05,0x5F},
    {CMDDELAY_MS, 5},
	{0x05,0x1F},
	{CMDDELAY_MS, 10},
	{0x05,0x5F},
	{CMDDELAY_MS, 50},
	{0x2B,0x01},
	{0x00,0x08},
	
	{0x01,0xA8},//9f

       {0x03,0x20}, // 40  // brightness 

	{0x04,0x03},  //0x08
	
	{0x06,0x15},
	{0x07,0x46},
	{0x08,0x00},
	{0x0B,0x83},
	{0x0C,0x06},
	{0x0D,0x46},
	{0x0E,0x40},
	{0x0F,0x40},
	{0x10,0x40},
	{0x11,0x40},
	{0x95,0x80},
	{0x12,0x00},
	
	{0x16,0x00},
	{0x17,0x52},
	{0x18,0x77},
	{0x19,0x75},
	{0x1A,0x77},
	{0x3C,0x77},
	{0x2F,0x69},
	{0xA5,0x22},
	{0xA6,0x12},
	{0xA7,0x06},
	{0xAC,0xD0},
#else
//command tab1
	/*{0x10,0x08},
	{0x11,0x40},
	{0x12,0x40},
	{0x13,0x40},
	{0x14,0x40},
	{0x15,0x08},
	{0x16,0x40},
	{0x17,0x2B},
	{0x18,0x0C},
	{0x19,0x40},
	{0x10,0x6D},
	{0x1A,0xF7},
	{0x1B,0x0C},
	{0x1C,0x38},*/

//command tab2
	/*{0x40,0x40},
	{0x41,0x2B},
	{0x44,0x0C},
	{0x45,0x40},
	{0x46,0x6D},
	{0x47,0xF7},
	{0x49,0x0C},
	{0x4a,0x38},*/

#if 1 //lcd init code
	{CMDDELAY_MS, 5},
	{0x7F,0x81},
	{0xE5,0x19},
	{0x7F,0x00},
	{0x40,0x77},
	{0x41,0x27},
	
	//{0x19,0x6D},
	//{0x1A,0xF3},
	
	{0x22,0x87},
	{0x32,0x87},
	{0x23,0xAE},
	{0x33,0xB0},
	{0x24,0xA1},
	
	{0x34,0xA1},
	{0x25,0x81},
	{0x35,0x81},
	{0x26,0x80},
	{0x36,0x80},
	
	{0x27,0x68},
	{0x37,0x69},
	{0x28,0x16},
	{0x38,0x15},
	{0x20,0x03},

	{0x30,0x03},
	{0x21,0x0F},
	{0x31,0x0F},
	{0x29,0x1F},
	{0x39,0x1F},
	
	{0xE6,0x05},
	{0xE6,0x70},
	{0xE6,0xA3},
#endif	
	
#endif
#else
   {0x05, 0x1E},
    {CMDDELAY_MS, 150},
    {0x05, 0x5f},
    {CMDDELAY_MS, 150},

    {0x2B,0x01},
   
    {0x00,0x0F},//0f
    {0x01,0xA8},//9f
    {0x03,0x30}, // 40
    {0x0b,0x80}, // 80 

    {0x0d,0x30},
  //  {0x0d,0x38},

    {0x2f,0x6d},
    {0x5a,0x03},

    {0x2b,0x01},
    {0x04,0x1B},

    {0xb3,0x01},
    {0xb5,0x06},
    {0x13,0xf0},
    {0x14,0xf0},
    {0x15,0xf0},
    {0x16,0xf0},
    {0x17,0xf0},

    {0xab,0x00},
    {0xaa,0x33}


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
         5.58f,//6.25f,//6.35f,//6.25,//,                          //!< fDCLK
         350,//390,//1560/4,                           //!< uiHSyncTotalPeriod
         320,//1280/4,                           //!< uiHSyncActivePeriod
         14,//23,//22,//241/4,                            //!< uiHSyncBackPorch  
         266,//262,                            //!< uiVSyncTotalPeriod
         240,//240,                            //!< uiVSyncActivePeriod
         12,//21,                             //!< uiVSyncBackPorchOdd
         12,//21,                             //!< uiVSyncBackPorchEven
         320,                            //!< uiBufferWidth
         240,                            //!< uiBufferHeight
         320,                            //!< uiWindowWidth
         240,                            //!< uiWindowHeight
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

