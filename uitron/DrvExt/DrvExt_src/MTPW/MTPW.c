#include "Type.h"
#include "DrvExt.h"
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
#include "gpio.h"
#include "pad.h"
#include "Debug.h"
#include "adc.h"
#include "rtc.h"	//'for password
#include "wdt.h"
#if 0
extern UINT32 IO_GET_GPIO_MTPW_SCK(void) ;
extern UINT32 IO_GET_PAD_MTPW_SCK(void); 
extern UINT32 IO_GET_GPIO_MTPW_SDA(void) ;
extern UINT32 IO_GET_PAD_MTPW_SDA(void) ;
#endif
extern BOOL SysInit_GetMTPW(void);
//=============================================================
static UINT32	GetMiotoneCheckSum(UINT16	Data)
{CHKPNT;      
	//------------crc checksum
	UINT16	CheckSum;
	UINT16	i,TempData,LoopData;

if( (Data & 0x3000)  == 0)
{
	CheckSum = Data ^ 0x8438;   //M1
	//CheckSum = Data ^ 0x8633;  //M2
	
	
}
	//PasswordTab
else
{
	LoopData	= 0x8000;
	CheckSum	= 0;
	//	debug_err(("nnnnnnnnnnData=%d \n\r",Data));
	for( i = 0; i<16; i++)
	{
		TempData	=	CheckSum;
		CheckSum = CheckSum<<1;
		if(TempData & 0x8000)
		{	
			CheckSum = CheckSum  ^  0x1021;   //M1
			//CheckSum = CheckSum  ^  0x1024; //M2
			
		}

		if( Data & LoopData )
		{
			CheckSum = CheckSum  ^  0x1021;   //M1
			//CheckSum = CheckSum  ^  0x1024; //M2

		}
		LoopData =  LoopData>> 1;
//	debug_err(("mmmmmmmmCheckSum=%d \n\r",CheckSum));
	}
}
	return	CheckSum;
}

static UINT32	GetMiotoneCheckSumM2(UINT16	Data)
{CHKPNT;
	//------------crc checksum
	UINT16	CheckSum;
	UINT16	i,TempData,LoopData;

	if( (Data & 0x3000)  == 0)
	{
		//CheckSum = Data ^ 0x8438;   //M1
		CheckSum = Data ^ 0x8633;  //M2			
	}
	//PasswordTab
	else
	{
		LoopData	= 0x8000;
		CheckSum	= 0;
		//	debug_err(("nnnnnnnnnnData=%d \n\r",Data));
		for( i = 0; i<16; i++)
		{
			TempData	=	CheckSum;
			CheckSum = CheckSum<<1;
			if(TempData & 0x8000)
			{	
				//CheckSum = CheckSum  ^  0x1021;   //M1
				CheckSum = CheckSum  ^  0x1024; //M2
				
			}

			if( Data & LoopData )
			{
				//CheckSum = CheckSum  ^  0x1021;   //M1
				CheckSum = CheckSum  ^  0x1024; //M2

			}
			LoopData =  LoopData>> 1;
	//	debug_err(("mmmmmmmmCheckSum=%d \n\r",CheckSum));
		}
	}
	return	CheckSum;
}

//==================================================
static UINT32	GetMiotonePassword( UINT16 Data)
{
	UINT16	TxData,RcvData;
	UINT8	i;
	UINT32 gpiosck,gpiosda,padsck,padsda;

	gpiosck = C_GPIO_5;//C_GPIO_6;//GPIO_MTPW_SCK;//IO_GET_GPIO_MTPW_SCK();
	padsck = PAD_PIN_CGPIO5;//PAD_PIN_CGPIO6;//PAD_MTPW_SCK;//IO_GET_PAD_MTPW_SCK(); 
	gpiosda = C_GPIO_6;//C_GPIO_5;//GPIO_MTPW_SDA;//IO_GET_GPIO_MTPW_SDA();
	padsda = PAD_PIN_CGPIO6;//PAD_PIN_CGPIO5;//PAD_MTPW_SDA;//IO_GET_PAD_MTPW_SDA();
//
	gpio_setDir(gpiosck, GPIO_DIR_OUTPUT);
	gpio_setPin(gpiosck);
	pad_setPullUpDown(padsck, PAD_PULLUP);
	gpio_setDir(gpiosda, GPIO_DIR_OUTPUT);
	gpio_setPin(gpiosda);
	pad_setPullUpDown(padsda, PAD_PULLUP);
	Delay_DelayMs(50);
//
	TxData	=	Data;
	for(i=0;i<16;i++)		//SEND
	{
        	if(((TxData >>i) & 0x01))
			gpio_setPin(gpiosda);
		else
		 	gpio_clearPin(gpiosda);


		gpio_clearPin(gpiosck);

		
		Delay_DelayMs(1);  

		gpio_setPin(gpiosck);
		Delay_DelayMs(1);   

	}
	
	gpio_setDir(gpiosda, GPIO_DIR_INPUT);
	//pad_setPullUpDown(PAD_PASSWORD_SDA, PAD_PULLDOWN);
		Delay_DelayMs(2);   
	
	for(i=0;i<16;i++)
	{
		gpio_clearPin(gpiosck);
		Delay_DelayMs(1);  

		RcvData =  RcvData>>1;
		if (gpio_getPin(gpiosda) == 0)
		{
			RcvData &= 0x7fff;

		}
		else	
		{
			RcvData |= 0x8000;
		}

		gpio_setPin(gpiosck);
		
		Delay_DelayMs(1);  
		
	}

		Delay_DelayMs(1); 

		gpio_setDir(gpiosda, GPIO_DIR_OUTPUT);
		gpio_setPin(gpiosda);
		pad_setPullUpDown(padsda, PAD_PULLUP);
	return(RcvData); 
}


//-------------------------------------------------------------------
static UINT32 Calltimecount =0;
BOOL SysInit_GetMTPW(void)
{
	RTC_DATE  CurDate;
	UINT16	TempDay;
	UINT16	i,TxData,RcvData=0,PCheckSum=0;

	if(Calltimecount)
		return TRUE;
	else
	{
		Calltimecount++;
		
		 for(i=0;i<5;i++)
		 {
			CurDate = rtc_getDate();
			TempDay = CurDate.s.day;
			
			TempDay  =( TempDay <<12)	& 0xf000;
			
			TxData	 = adc_readData(ADC_CHANNEL_0);    //DrvPower_GetBatteryADC();	
			TxData  |=  TempDay;
			
			PCheckSum = GetMiotoneCheckSum(TxData);
			RcvData	= GetMiotonePassword(TxData);

			debug_msg("\r\n^PCheckSum:%x\r\n",PCheckSum);
			debug_msg("\r\n^RcvData:%x\r\n",RcvData);

			
			if(PCheckSum == RcvData)
			{
				debug_msg("\r\n\r\n--------MTPW M1--------\r\n\r\n");
				break;
			}
			Delay_DelayMs(20);  

			PCheckSum = GetMiotoneCheckSumM2(TxData);
			if(PCheckSum == RcvData)
			{
				debug_msg("\r\n\r\n--------MTPW M2--------\r\n\r\n");
				break;	
			}
			Delay_DelayMs(20);  
		 }

	 	if(i>4)
	 	{
	 		debug_msg("***\r\n");
			debug_msg("mmmmmm get password fail!\r\n");
			wdt_open();
			wdt_setConfig(WDT_CONFIG_ID_MODE,WDT_MODE_RESET);
			wdt_setConfig(WDT_CONFIG_ID_TIMEOUT,100);	//ms
			wdt_trigger();
			wdt_enable();
			//debug_err(("[WDT] SysWdtReset when after 100 ms\r\n"));	
#if 0		
		 		//while(1)
				{
					//SenFP_ShowOSDString("CHECK authorization ",20,52,2);
					extern int UI_ForceShowString(CHAR * pStr, PURECT pRect, BOOL bClear);
					URECT rect = {10, 30, 640, 20};
					UI_ForceShowString("CHECK authorization ",&rect, 1);
					
					Delay_DelayMs(2000);

					return FALSE;
					//System_PowerOff(SYS_POWEROFF_NORMAL);

				};
#else
		return FALSE;
#endif
	 	}
	 else
	 	{
	 		//debug_msg("mmmmmm get password successful!\r\n");
			return TRUE;
	 	}
	}
 }
