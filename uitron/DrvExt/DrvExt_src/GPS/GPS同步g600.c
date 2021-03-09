/*
    Copyright   Novatek Microelectronics Corp. 2018.  All rights reserved.

    @file       GPS.c
    @ingroup

    @brief      Execute command via UART interface.
                Execute command and output message via UART interface.

    @note       Nothing.

    @version    V1.00.000
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "Debug.h"
#include "SysKer.h"
#include "kernel.h"
#include "uart.h"
#include "GPS.h"
#include "DataQueue.h"
#include "Delay.h"// miotone_2018815 zejia add
#include "time.h"
#include "CoarseAtti.h"

//@{
#define __MODULE__					GPS
#define __DBGLVL__					1		//0=OFF, 1=ERROR, 2=TRACE
#define __DBGFLT__ 					"*"		//*=All, [mark]=CustomClass
#include "DebugModule.h"


#define _GPS_DEBUG_SYMBOL			0// miotone_2018801 zejia change 0 to 1
#if _GPS_DEBUG_SYMBOL
#define GPS_debug(arg)				DBG_DUMP arg
#else
#define GPS_debug(arg)
#endif

#define _GPS_NMEA_DEBUG_SYMBOL		0
#if _GPS_NMEA_DEBUG_SYMBOL
#define NMEA_debug(arg)				DBG_DUMP arg
#else
#define NMEA_debug(arg)
#endif
// miotone_2018727 zejia add

#define _GNXX_DEBUG_SYMBOL			1
#if _GNXX_DEBUG_SYMBOL
#define GNXX_debug(arg)				MY_MSG arg
#else
#define GNXX_debug(arg)
#endif

#define _ESF_DEBUG_SYMBOL			1
#if _ESF_DEBUG_SYMBOL
#define ESF_debug(arg)				MY_MSG arg
#else
#define ESF_debug(arg)
#endif

#define TOTAL_FRM_CNT_1SEC 40// miotone_2018802 zejia change 20 to 35
#define TOTAL_ESF_FRM_CNT_1SEC 5// miotone_2018920 zejia add
UINT8 s_nFrameCnt = 0;
UINT8 s_nESFFrameCnt = 0;// miotone_2018920 zejia add
UINT8 s_nFrameProCnt = 0;
CHAR  GpsFrameBuf[TOTAL_FRM_CNT_1SEC][NMEA_SENTENCE_SIZE] =
{
 	{0},
};
CHAR  GpsFrameProBuf[TOTAL_FRM_CNT_1SEC][NMEA_SENTENCE_SIZE] =
{
 	{0},
};

//add end

#define _GPS_CheckSum_Enable		1   //Enable to open check sum mechanism

//Declare functions
void GPSRec_NMEAParser(CHAR *NMEA_str, UINT32 Len, NMEATYPE NMEAtype);
void GPSRec_ESFParser(CHAR *NMEA_str, UINT32 Len, NMEATYPE NMEAtype);// soling_20180801 zejia add

extern VOID drawString(UINT32 *String);
//Declare variables
GSAINFO GSAInfo;
#if (GPS_GNRMC == ENABLE)
GNRMCINFO RMCInfo;
#else
RMCINFO RMCInfo;
#endif
GGAINFO GGAInfo;
GSVINFO GSVInfo;
ESFINFO ESFInfo;// soling_20180801 zejia add
GPSLOCALINFO GPSLocalInfo;
ESFINFODATATEST GPSESFDataInfo;
_ALIGNED(4) NMEASENTENCE NMEASentence={0};
_ALIGNED(4) ESFSENTENCE ESFSentence={0};// miotone_2018801 zejia add

//gyro receive
#define PRI_GPSRECEIVE            5//5  //180717,dcj change 10 to 3, in order to get RIGHT  gps data frame
#define STKSIZE_GPSRECEIVE        8192//64KB stack size
UINT32  GPSRECEIVE_ID = 0;
UINT32  FLG_ID_GPS = 0;

//gyro process
#define PRI_GPS_PROCESS            8//5  //180717,dcj change 10 to 3, in order to get RIGHT  gps data frame
#define STKSIZE_GPS_PROCESS        8192//64KB stack size
UINT32  GPS_PROCESS_ID = 0;

//for GGA version
CHAR szGGAVer[8];

// global variables for parsing NMEA code
CHAR    type[NMEA_TYPE_SIZE], RemainStr[NMEA_SENTENCE_SIZE], PostPtr[NMEA_SENTENCE_SIZE + 20], *pPostPrt;
UINT32  PostPtrLen;
UINT32  CheckSum;
CHAR type_esf[ESF_TYPE_SIZE];
//GSA used
CHAR    SelMode;
UINT32  FixMode, SatUsed01, SatUsed02, SatUsed03, SatUsed04, SatUsed05, SatUsed06, SatUsed07, SatUsed08, SatUsed09, SatUsed10, SatUsed11, SatUsed12;
double  PDOP, HDOP, VDOP;
//RMC used
UINT32  Date;
CHAR    Status,MagVarInd,Status_posMode;
double  Speed, Angle, MagVar;
//NRMC
double Roll,Pitch,Yaw;
UINT32 FusionMode;
CHAR ErrorState[2],PosMode;
//GGA used
double  UTCTime;
UINT32  PosFixInd, SatNumber, Empty1, Empty2,GpsStNum,BdStNum;
double  Lat, Lon, Alt, MSL;
CHAR    NSInd, EWInd, M1, M2;
UINT32 GGAFusionMode,GGAErrorState;// miotone_2018920 zejia add
//GSV used
UINT32  NumOfSen, SenNum, SatInView, SatNum1, Elev1, Azi1, SNR1, SatNum2, Elev2, Azi2, SNR2, SatNum3, Elev3, Azi3, SNR3, SatNum4, Elev4, Azi4, SNR4;
// soling_20180801 zejia add
//ESF used
int  gyro_x,gyro_y,gyro_z,gsensor_x,gsensor_y,gsensor_z,data_type;//everyone 4 bytes
UINT32 esf_frames_cnt = 0;
BOOL gps_receive_type;
//UINT8 gyrogsensor[ESF_MEAS_DATA_LEN];
//end

//!pdt used
char szPdtType[16];
char szPdtVer[32] = {0};
UINT32 uiPdtVal1 = 0, uiPdtVal2 = 0;

//!INSS
char szInssType[16];
UINT32 uiInssVal1, uiInssVal2, uiInssVal3, uiInssVal4, uiInssVal5, uiInssVal6, uiInssVal7, uiInssVal8, uiInssVal9;


//GPS heartbeat
static UINT32 GPSHeartBeat = 0;         //#NT#2015/10/02#KCHong#Add GPS heartbeat to detect GPS status

// miotone_2018802 zejia add
BOOL GPS_GNXXX_Print;
BOOL ESF_Print = 0;
double Gsensor_Enable = 0;
double Gsensor_Sen;
double Gsensor_Sen1;
double Gyro_Enable = 0;
double Gyro_Sen;
double Gyro_Sen1;
BOOL GPS_NMEA_Print = 0;
//int g_GPSLocationTime = 0;
//BOOL g_GPSLocationTimeFlag = FALSE;

//int gGyro_Ver_2_0 = GYRO_HARDWARE_VER_1ST;
//static AttCtrlParam stCtrlParam;

//end
// miotone_2018727 zejia add
static GPS_UART_OBJ GpsUart;
//end
// miotone_2018825 zejia add
extern BOOL g_GPSLinked;
//end

extern void GPSRecTsk(void);
extern void GPSProcessTsk(void);

int GPSRec_TransformData(CHAR *NMEA_str, UINT32 Len, CHAR *pPostPrt, UINT32 *PostPtrLen);
int GPSRec_ESFTransformData(CHAR *NMEA_str, UINT32 Len, CHAR *pPostPrt, UINT32 *PostPtrLen,UINT8 gyrosensor8[],UINT32 gyrosensor32[]);// miotone_2018801 zejia add
BOOL GPSRec_CheckSum(CHAR *NMEA_str, UINT32 Len, UINT32 CheckSum);
BOOL GPSRec_GetUTCDate(UINT32 *Year, UINT32 *Month, UINT32 *Day);
BOOL GPSRec_GetUTCTime(UINT32 *Hour, UINT32 *Minute, UINT32 *Second);

double dm2dd(double dm);
double dm2dd(double dm)
{
	double DecimalsPart=0,IntegerPart=0;
	double GpsRmcFormat=0;

	DecimalsPart=modf(dm,&IntegerPart);
	//debug_msg("--------IntegerPart=%lf\r\n",IntegerPart);
	//debug_msg("--------DecimalsPart=%lf\r\n",DecimalsPart);
	GpsRmcFormat=(double)((int)IntegerPart/100)+((double)(DecimalsPart+((int)IntegerPart-(((int)IntegerPart/100)*100)))/60);
	//debug_msg("--------GpsRmcFormat=%lf\r\n",GpsRmcFormat);
return GpsRmcFormat;
}

// miotone_2018725 zejia add,G600_CHANGE_DEBUG
void NTPrintBuf(const char *szPrompt, CHAR *pData, UINT16 ulLen);
void NTPrintBuf(const char *szPrompt, CHAR *pData, UINT16 ulLen)
{
	#define MAX_DUMP_PER_LINE	16
	UINT16 i;
	char szDbgBuf[64];
	char *ptr = szDbgBuf;

	if((pData==NULL)||(ulLen==0))
	{
		return;
	}

	if(szPrompt!=NULL)
	{
	    debug_msg("%s:\r\n",szPrompt);
	}
	
	debug_msg ("------------------------------------------------\r\n");
	for (i = 1; i <= ulLen; i++)
	{
		ptr += snprintf(ptr, sizeof(szDbgBuf), "%02X ", *pData);
		pData++;

		if(i % MAX_DUMP_PER_LINE==0)
		{
			*ptr = 0;
			debug_msg("%s\r\n", szDbgBuf);
			ptr = szDbgBuf;
		}
	}
	
	if (ulLen % MAX_DUMP_PER_LINE)
	{
		*ptr = 0;
		debug_msg("%s\r\n", szDbgBuf);
	}
	debug_msg("------------------------------------------------\r\n");
}

void SetGPSGNXXXPrint(BOOL enable)
{
    GPS_GNXXX_Print = enable;
}

BOOL GetGPSGNXXXPrint(void)
{
    return GPS_GNXXX_Print;
}

void SetESF_Print(BOOL enable)
{
    ESF_Print = enable;
}

BOOL GetESF_Print(void)
{
    return ESF_Print;
}

void SetGPSNMEA_Print(BOOL enable)
{
    GPS_NMEA_Print = enable;
}

BOOL GetGPSNMEA_Print(void)
{
    return GPS_NMEA_Print;
}

//BOOL GetGsensor_Sensitivity(void)
//{
//    return Gsensor_Sen;
//}

void SetGsensor_Sensitivity(double enable)
{
    Gsensor_Sen = enable;
}

double GetGsensor_Sensitivity(void)
{
    return Gsensor_Sen;
}

void SetGsensor_Sensitivity1(double enable)
{
    Gsensor_Sen1 = enable;
}

double GetGsensor_Sensitivity1(void)
{
    return Gsensor_Sen1;
}

//end
// miotone_2018815 zejia add
gps_nmea_set_handler gps_set_nmea_data;
UINT8 app_set_gps_cb(gps_nmea_set_handler gps_get_data)
{
	gps_set_nmea_data = gps_get_data;
	return 0;
}

//end

///////////////////////////////////////////////////////////////////////
#ifdef GPS_POWER_CTRL
void SystemSet_CheckGPSData_Status(BOOL bSet);
BOOL SystemGet_CheckGPSData_Status(void);
void SystemSet_CheckESFData_Status(BOOL bSet);
BOOL SystemGet_CheckESFData_Status(void);
static BOOL g_uiCheckGetGPSDataStatus=FALSE;
static BOOL g_uiCheckGetESFDataStatus=FALSE;
void SystemSet_CheckGPSData_Status(BOOL bSet)
{
	g_uiCheckGetGPSDataStatus=bSet;
}
BOOL SystemGet_CheckGPSData_Status(void)
{
	return g_uiCheckGetGPSDataStatus;
}
void SystemSet_CheckESFData_Status(BOOL bSet)
{
	g_uiCheckGetESFDataStatus=bSet;
}
BOOL SystemGet_CheckESFData_Status(void)
{
	return g_uiCheckGetESFDataStatus;
}
#endif
///////////////////////////////////////////////////////////////////////


void GPS_InstallID(void)
{
    //!receive msg
    OS_CONFIG_TASK(GPSRECEIVE_ID, PRI_GPSRECEIVE, STKSIZE_GPSRECEIVE, GPSRecTsk);
    OS_CONFIG_FLAG(FLG_ID_GPS);

    //!process msg
    OS_CONFIG_TASK(GPS_PROCESS_ID, PRI_GPS_PROCESS, STKSIZE_GPS_PROCESS, GPSProcessTsk);
    //OS_CONFIG_FLAG(FLG_ID_GPS);
    //debug_msg("GPS_InstallID succeed\r\n");
}

void GPS_SetUartObj(GPS_UART_OBJ *UartObj)
{
	memcpy(&GpsUart, UartObj, sizeof(GPS_UART_OBJ));
}

static BOOL bGPSRecOpened = FALSE;
ER GPSRec_Open(UART_BAUDRATE BaudRate)
{
    if (bGPSRecOpened)
        return E_SYS;

    if (GpsUart.open() == E_OK)
    {
        DBG_DUMP("UART open success\r\n");
        GpsUart.init(BaudRate, UART_LEN_L8_S1, UART_PARITY_NONE);
    }
    else
    {
        DBG_ERR("UART open fail!\r\n");
        return E_SYS;
    }
    //Init_GyroDataQueue();
    //CoarseAttInit(&stCtrlParam);
    UartDataQueue_Init();
    DBG_DUMP("Start GPS receive task\r\n");
    sta_tsk(GPSRECEIVE_ID,0);
    sta_tsk(GPS_PROCESS_ID,0);

    loc_cpu();
    bGPSRecOpened = TRUE;
    unl_cpu();

    return E_OK;
}

ER GPSRec_Close(void)
{
    if (!bGPSRecOpened)
        return E_SYS;

    if (GpsUart.close() == E_OK)
        DBG_DUMP("UART close success\r\n");
    else
        DBG_ERR("UART close fail!\r\n");

    DBG_DUMP("Terminate GPS receive task\r\n");

    ter_tsk(GPSRECEIVE_ID);
    ter_tsk(GPS_PROCESS_ID);

    loc_cpu();
    bGPSRecOpened = FALSE;
    unl_cpu();

    return E_OK;
}
//extern void SysGps_PostFramesEvt(UINT32 nFrmAddr);// miotone_2018725 zejia add,G600_CHANGE_DEBUG
#if 1//def G600_CHANGE_DEBUG
BOOL GPSCom_strchr(CHAR strs[]);
BOOL GPSCom_strchr(CHAR strs[])
{
    int return_value = 0;

    if ((strs[0] == ESF_MEAS_STR1) && (strs[1] == ESF_MEAS_STR2) && \
        (strs[2] == ESF_MEAS_STR3) && (strs[3] == ESF_MEAS_STR4))
    {
        return_value = 1;
    }

    return return_value;
}
void SysGps_PostFramesEvt(UINT32 nFrmAddr);
#define    NVTEVT_GPS_FRAMES     (0x80005000)
CHAR    RmcSentence[NMEA_SENTENCE_SIZE];

void GPSRecTsk(void)
{
    UINT32  uiLen = 0, uiRet = 0;
    UINT8   szUartLine[128];
    kent_tsk();
    clr_flg(FLG_ID_GPS, GPS_FLAG_DATAVALID);
    while(GPSRECEIVE_ID)
    {
        uiLen = 32;
        //uiRet = GpsUart.getString(c, &uiLen);
        uiRet = GpsUart.getData(szUartLine, uiLen);
        if(uiRet > 0){
            putUartData((char *)szUartLine, uiLen);
	     #ifdef GPS_POWER_CTRL
	     SystemSet_CheckGPSData_Status(TRUE);
	     #endif
        }
        else{
            Delay_DelayMs(5);
	     #ifdef GPS_POWER_CTRL
	     SystemSet_CheckGPSData_Status(FALSE);
	     #endif
        }
    }

}


void GPSProcessTsk(void)
{
    //CHAR    RecSentence[NMEA_SENTENCE_SIZE],c;
    CHAR    szGyroLine[NMEA_SENTENCE_SIZE], RecSentence[NMEA_SENTENCE_SIZE],c;
    CHAR    *p = NULL;
    UINT8   rx_flags = 0;
    static UINT8 rx_flags1 = 0;
    int     iLen = 0, i = 0;
    kent_tsk();

    MY_MSG("GPSRECEIVE_ID: %d\r\n",GPSRECEIVE_ID);
    //clr_flg(FLG_ID_GPS, GPS_FLAG_DATAVALID);

    while(GPS_PROCESS_ID)
    {

        iLen = 32;
        if(getUartData(szGyroLine, &iLen) == FALSE){
            Delay_DelayMs(4);
            debug_msg("2______________iLen = %d, used:%d__________________\r\n", iLen, getUartDataQueueLen());
            //RMCInfo.Status = 'V';
            continue;
        }
        else{
            debug_msg("get______________iLen = %d__________________\r\n", iLen);
            for(i=0; i<iLen; i++)
    		{
                c = szGyroLine[i];
    			if(rx_flags & 0x01)
                {
                    *p++ = c;
                    if(c == '\n')
    				{
    					*p++ = '\0';
    					rx_flags = 0;
    					//MY_MSG("Len: %d",(p - RecSentence));
    				}
                    else if (c == 0xB5)
                    {
                        if (RecSentence[0] == 0xB5)
                        {
                            *p = '\0';
                            rx_flags1 = 1;
                        }
                        else
                        {
                            continue;
                        }
    					//rx_flags |= 0x01;
    					//memset(RecSentence,0,NMEA_SENTENCE_SIZE);
    					//p = RecSentence;
    					//*p++ = c;
                    }
    				//error condition
    				else if( (p - RecSentence) > NMEA_BUFFER_RESERVED )
    				{
            			RecSentence[NMEA_BUFFER_RESERVED-1] = '\0';
            			//MY_MSG("Len: %d",(p - RecSentence));
            			rx_flags &= 0;
            			rx_flags1 &= 0;
            		}
            		else
            		{
            			continue;
            		}
    			}
    			else
    			{
    				//if(c == '$')
    				if ((c == '$') || (c == 0xB5))
    				{
    					rx_flags |= 0x01;
    					memset(RecSentence,0,NMEA_SENTENCE_SIZE);
    					p = RecSentence;
    					*p++ = c;
    				}
    				continue;
    			}
                memcpy(GpsFrameBuf[s_nFrameCnt],RecSentence,NMEA_SENTENCE_SIZE);
                //if( strstr(RecSentence,THE_FIRST_KEY_GPS) ){
                   debug_msg("_________%s__________\r\n", RecSentence);
                //}
    		    s_nFrameCnt ++;
                if ( s_nFrameCnt >= (TOTAL_FRM_CNT_1SEC-2) )
                {
                    MY_MSG("error: %d\r\n",s_nFrameCnt);
        			rx_flags = 0;
        			s_nFrameCnt = 0;
                }
                if( s_nFrameCnt >= 5 )
                {
                    //debug_msg("______________s_nFrameCnt:%d___________\r\n", s_nFrameCnt);
                    gps_set_nmea_data(0);
                    s_nFrameCnt = 0;
                }
    		#if 0
    		    
    		
                //if( strstr(RecSentence,THE_LAST_KEY_GPS) )
                if (GPSCom_strchr(RecSentence))
                //if( strstr(RecSentence,THE_LAST_KEY_GPS) )// miotone_2018915 zejia change
                {
                    //debug_msg("%s\r\n",RecSentence);
                    if ( s_nFrameCnt >= (TOTAL_FRM_CNT_1SEC-2) )
                    {
                        MY_MSG("error: %d\r\n",s_nFrameCnt);
        				rx_flags = 0;
        				s_nFrameCnt = 0;
                    }
                    else
                    {
                        //SysGps_PostFramesEvt(0); //require UI task to parse all gps frame
                        #if 1// miotone_2018920 zejia change
        				s_nESFFrameCnt++;
        				if (s_nESFFrameCnt >= TOTAL_ESF_FRM_CNT_1SEC )
        				{
        					s_nESFFrameCnt = 0;
        	                gps_set_nmea_data(0);
        				}
        				#else
                        gps_set_nmea_data(0);
        				#endif
                    }
                }
                #endif
                if (rx_flags1 == 1)
                {
                    rx_flags1 = 0;
                    rx_flags |= 0x01;
                    memset(RecSentence,0,NMEA_SENTENCE_SIZE);
                    p = RecSentence;
                    //*p = c;
                    *p++ = c;
                }

            }
        }
    }
}

#else
static CHAR    RecSentence[NMEA_SENTENCE_SIZE];
void GPSRecTsk(void)
{
    UINT32  uiLen;
    UINT16  CheckSentenceType1, CheckSentenceType2;
    UINT32  CheckSentenceType3;
    //CHAR    RecSentence[NMEA_SENTENCE_SIZE];
    CHAR    *pSentenceData;

    kent_tsk();

    clr_flg(FLG_ID_GPS, GPS_FLAG_DATAVALID);

    while(GPSRECEIVE_ID)
    {
        uiLen = NMEA_SENTENCE_SIZE;
        if (GpsUart.getString(RecSentence, &uiLen) == E_PAR)
        {
            DBG_ERR("UART parameter error!\r\n");
        }

        //DBG_DUMP("%s\r\n", RecSentence);
        debug_msg("%s\r\n", RecSentence);
        //Each NMEA sentence begins with '$' (0x24)
        if (RecSentence[0] == 0x24)
        {
            GPSHeartBeat ++;        //#NT#2015/10/02#KCHong#Add GPS heartbeat to detect GPS status
            pSentenceData = RecSentence;
            CheckSentenceType1 = *(UINT16 *)(pSentenceData + 2);
            CheckSentenceType2 = *(UINT16 *)(pSentenceData + 4);
            CheckSentenceType3 = ((UINT)CheckSentenceType2 << 16) | (UINT)CheckSentenceType1;

            switch(CheckSentenceType3)
            {
            case NGSA_SENTENCE:
                NMEASentence.GSA = RecSentence;
                GPSRec_NMEAParser(RecSentence, (UINT32)uiLen, GSA);
                break;

            case NRMC_SENTENCE:
                NMEASentence.RMC = RecSentence;
                GPSRec_NMEAParser(RecSentence, (UINT32)uiLen, RMC);
                break;

            case NGGA_SENTENCE:
                NMEASentence.GGA = RecSentence;
                GPSRec_NMEAParser(RecSentence, (UINT32)uiLen, GGA);
                break;

            case NGSV_SENTENCE:
                NMEASentence.GSV = RecSentence;
                GPSRec_NMEAParser(RecSentence, (UINT32)uiLen, GSV);
                break;

            case NGLL_SENTENCE:

                NMEASentence.GLL = RecSentence;
                GPSRec_NMEAParser(RecSentence, (UINT32)uiLen, GLL);
                break;

            case NVTG_SENTENCE:
                NMEASentence.VTG = RecSentence;
                GPSRec_NMEAParser(RecSentence, (UINT32)uiLen, VTG);
                break;
#if 1	//北斗模式GSA GSV未解析
            case NGSA_SENTENCE_BD:
                //NMEASentence.VTG = RecSentence;
               // GPSRec_NMEAParser(RecSentence, (UINT32)uiLen, GSA);
                break;

            case NGSV_SENTENCE_BD:
                //NMEASentence.VTG = RecSentence;
                //GPSRec_NMEAParser(RecSentence, (UINT32)uiLen, GSV);
                break;

            case TXT_SENTENCE:
                //NMEASentence.VTG = RecSentence;
                //GPSRec_NMEAParser(RecSentence, (UINT32)uiLen, TXT);
                break;
#endif
            default:
                DBG_ERR("GPS: Can't identify this sentence 0x%X \r\n",CheckSentenceType3);
                break;
            }

        }

    }

}
#endif

ER GPSRec_Reset(void)
{
    return E_OK;
}

BOOL GPSRec_CheckData(void)
{
    DBG_DUMP("GPS: DATA VALID: 0x%X\r\n", kchk_flg(FLG_ID_GPS, GPS_FLAG_DATAVALID));
    if (kchk_flg(FLG_ID_GPS, GPS_FLAG_DATAVALID))
        return TRUE;
    else
        return FALSE;
}


// $GNRMC,092846.400,A,3029.7317,N,10404.1784,E,000.0,183.8,070417,,,A*73
int GPSRec_TransformData(CHAR *NMEA_str, UINT32 Len, CHAR *pPostPrt, UINT32 *PostPtrLen)
{
    UINT32 i;
    int num = 0;

    //Process sentence, change ',' as ' '.
    //If message is null, change to corresponding value.
    for (i = 0; i <= Len; i++)
    {
        *pPostPrt = *NMEA_str;

        if (*NMEA_str == ',')
        {
            *pPostPrt = 0x20;   //Instead of ' '
            num++;
            if ((*(NMEA_str+1) == ',')|(*(NMEA_str+1) == '*'))
            {
                *++pPostPrt = 0x30;  //Insert '0'
                *PostPtrLen = *PostPtrLen + 1;
            }
        }
        else if ((*NMEA_str == '*')|(i == Len))
        {
            *pPostPrt = 0x20;   //Instead of ' '
            num++;
        }

        NMEA_str++;
        pPostPrt++;

    }

    NMEA_str -= Len;
    pPostPrt -= *PostPtrLen;
    return num;
}

int GPSRec_ESFTransformData(CHAR *NMEA_str, UINT32 Len, CHAR *pPostPrt, UINT32 *PostPtrLen,UINT8 gyrosensor8[],UINT32 gyrosensor32[])
{
    UINT32 i;
    UINT8 gyrox_flag = 0,gyroy_flag = 0,gyroz_flag = 0,gsensorx_flag = 0,gsensory_flag = 0,gsensorz_flag = 0;
    int ret_value = 0;
    int rec_data_cnt = 0;
    UINT32 checksum_a = 0,checksum_a_total = 0;

/////////////////////////////////////////////24 bits data////////////////////////////////////////////////
//                                                                       0   1   2   3   4   5   6   7   8   9  10 11 12 13 14 15 16 17 18 19 20 21 22 23      //
// 0  1    2    3   4  5    6  7  8   9  10  11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39
//B5 62 10 02 20 00 28 9F 27 07 00 00 06 00 A5  FC FF 0E A1 FA FF 0D  BF 02 00 05 FC 00 00 10 A1 FB FF 11 E1 26 00 12 19 04
//|                   |       |                |          |     |                   |                   |                   |                 |                 |                 |
//msg head    (8+4*N) timeTag        flag      id          data1          data2             data3            data4       data5          data6
//                                                                      gyro x axis    gyro y axis    gyro z axis  gsensor x axis gsensor y axis gsensor z axis  

    if (Len != ESF_MEAS_LEN) return ret_value;
    for (i = 0; i < Len; i++)
    {

        *pPostPrt = *NMEA_str;
        if ((i > ESF_MEAS_LEN_13BYTE) && (i < ESF_MEAS_LEN_38BYTE))//24 bytes
        {
            gyrosensor8[i - ESF_MEAS_LEN_14BYTE] = *pPostPrt;
        }
        if ((i >=ESF_MEAS_LEN_2BYTE) && (i < ESF_MEAS_LEN_38BYTE))//checksum total A
        {
            checksum_a_total += *pPostPrt;
        }
        if (i == ESF_MEAS_LEN_38BYTE) //checksum A
        {
            checksum_a = *pPostPrt;
        }

        NMEA_str++;
        pPostPrt++;

    }
    
    NMEA_str -= Len;
    pPostPrt -= *PostPtrLen;

    checksum_a_total = checksum_a_total & 0x000000FF;
    if (checksum_a != checksum_a_total)
    {
        MY_TRACE_LINE_COLOR;
        return ret_value;
    }

    if ((gyrosensor8[ESF_24BITDATA_GYROX] == ESF_GYROX_TYPE) && (gyrox_flag == 0))//gyro x
    {
        gyrosensor32[ESF_LASTDATA_GYROX] = ((UINT32)gyrosensor8[ESF_24BITDATA_GYROX] << 24) | ((UINT32)gyrosensor8[ESF_24BITDATA_GYROX - 1] << 16) | ((UINT32)gyrosensor8[ESF_24BITDATA_GYROX - 2] << 8) | ((UINT32)gyrosensor8[ESF_24BITDATA_GYROX - 3]);
        gyrox_flag = 1;
        rec_data_cnt ++;
    }
    if ((gyrosensor8[ESF_24BITDATA_GYROY] == ESF_GYROY_TYPE) && (gyroy_flag == 0))//gyro y
    {
        gyrosensor32[ESF_LASTDATA_GYROY] = ((UINT32)gyrosensor8[ESF_24BITDATA_GYROY] << 24) | ((UINT32)gyrosensor8[ESF_24BITDATA_GYROY - 1] << 16) | ((UINT32)gyrosensor8[ESF_24BITDATA_GYROY - 2] << 8) | ((UINT32)gyrosensor8[ESF_24BITDATA_GYROY - 3]);
        gyroy_flag = 1;
        rec_data_cnt ++;
    }
    if ((gyrosensor8[ESF_24BITDATA_GYROZ] == ESF_GYROZ_TYPE) && (gyroz_flag == 0))//gyro z
    {
        gyrosensor32[ESF_LASTDATA_GYROZ] = ((UINT32)gyrosensor8[ESF_24BITDATA_GYROZ] << 24) | ((UINT32)gyrosensor8[ESF_24BITDATA_GYROZ - 1] << 16) | ((UINT32)gyrosensor8[ESF_24BITDATA_GYROZ - 2] << 8) | ((UINT32)gyrosensor8[ESF_24BITDATA_GYROZ - 3]);
        gyroz_flag = 1;
        rec_data_cnt ++;
    }
    if ((gyrosensor8[ESF_24BITDATA_GSENSORX] == ESF_GSENSORX_TYPE) && (gsensorx_flag == 0))//gsensor x
    {
        gyrosensor32[ESF_LASTDATA_GSENSORX] = ((UINT32)gyrosensor8[ESF_24BITDATA_GSENSORX] << 24) | ((UINT32)gyrosensor8[ESF_24BITDATA_GSENSORX - 1] << 16) | ((UINT32)gyrosensor8[ESF_24BITDATA_GSENSORX - 2] << 8) | ((UINT32)gyrosensor8[ESF_24BITDATA_GSENSORX - 3]);
        gsensorx_flag = 1;
        rec_data_cnt ++;
    }
    if ((gyrosensor8[ESF_24BITDATA_GSENSORY] == ESF_GSENSORY_TYPE) && (gsensory_flag == 0))//gsensor y
    {
        gyrosensor32[ESF_LASTDATA_GSENSORY] = ((UINT32)gyrosensor8[ESF_24BITDATA_GSENSORY] << 24) | ((UINT32)gyrosensor8[ESF_24BITDATA_GSENSORY - 1] << 16) | ((UINT32)gyrosensor8[ESF_24BITDATA_GSENSORY - 2] << 8) | ((UINT32)gyrosensor8[ESF_24BITDATA_GSENSORY - 3]);
        gsensory_flag = 1;
        rec_data_cnt ++;
    }
    if ((gyrosensor8[ESF_24BITDATA_GSENSORZ] == ESF_GSENSORZ_TYPE) && (gsensorz_flag == 0))   //gsensor z
    {
        gyrosensor32[ESF_LASTDATA_GSENSORZ] = ((UINT32)gyrosensor8[ESF_24BITDATA_GSENSORZ] << 24) | ((UINT32)gyrosensor8[ESF_24BITDATA_GSENSORZ - 1] << 16) | ((UINT32)gyrosensor8[ESF_24BITDATA_GSENSORZ - 2] << 8) | ((UINT32)gyrosensor8[ESF_24BITDATA_GSENSORZ - 3]);
        gsensorz_flag = 1;
        rec_data_cnt ++;
    }
            

    //if (rec_data_cnt >= ESF_MEAS_DATA_TYPE)
    if (rec_data_cnt == ESF_MEAS_DATA_TYPE)
        ret_value = 1;
        
    return ret_value;

}

static int GPS_ESFDateFilter(int data)
{
    BOOL ret = 0;

    if (data > 0xF00000)
        data = -(0xFFFFFF -data);


    ret = data;
    return ret;
    
}
#if 0
BOOL GPSRec_CheckSum(CHAR *NMEA_str, UINT32 Len, UINT32 CheckSum)
{
    UINT32 i, LenTemp = 1;
    CHAR XORData;
    XORData = *(NMEA_str++);

    //Doing 8 bit exclusive OR of all characters between, but not including, the '$' and '*'.
    for (i = 1; i < Len; i++)
    {
        NMEA_str++;
        if (*NMEA_str == '*')
            break;

        LenTemp++;
        XORData ^= *NMEA_str;
    }
    NMEA_str -= LenTemp;

    if (XORData == (CHAR)CheckSum)
    {
        return TRUE;
    }
    else
    {
        DBG_ERR("GPS: check sum error! 0x%X != 0x%X\r\n",CheckSum, XORData);
        return FALSE;
    }

}
#else

BOOL GPSRec_CheckSum(CHAR *NMEA_str, UINT32 Len, UINT32 CheckSum)
{
    UINT32 i, LenTemp = 1;
    CHAR XORData;
    //#NT#2009/04/07#Photon Lin -begin
    //#Refine GPS firmware
    XORData = *(++NMEA_str);

    //Doing 8 bit exclusive OR of all characters between, but not including, the '$' and '*'.
    for (i = 1; i < Len; i++)
    {
        NMEA_str++;
        if (*NMEA_str == '*')
            break;

        LenTemp++;
        XORData ^= *NMEA_str;
    }
    //#NT#2009/04/07#Photon Lin -end
    NMEA_str -= LenTemp;
    //debug_msg("CheckSum:%x, XORData:%hhx\r\n", CheckSum, XORData);
    if (XORData == (CHAR)CheckSum)
        return TRUE;
    else
    {
       // DBG_ERR("GPS: check sum error! 0x%X != 0x%X\r\n",CheckSum, XORData);
		//CHAR str[20] = "gps checksum error";
		//drawString(str);
        return FALSE;
    }


}
#endif

void GPSRec_NMEAParser(CHAR *NMEA_str, UINT32 Len, NMEATYPE NMEAtype)
{
    //UINT32 i;
	BOOL checkSum_Ret = FALSE;
    int /*n = 0, */num = 0;
    //extern double dm2dd(double dm);
    RMCINFO_ext stRMCInfo = {0};

    pPostPrt = PostPtr;
    PostPtrLen = Len;

	if (gps_receive_type == GPS_TYPE_ESF)	
		GPSESFDataInfo.valid_frames = esf_frames_cnt;
	esf_frames_cnt = 0;
	gps_receive_type = GPS_TYPE_GNXXX;
    //if(NMEAtype == GGA || NMEAtype == NRMC){
	//    debug_msg("^R%s\r\n",NMEA_str);
    //}
    num = GPSRec_TransformData(NMEA_str, Len, pPostPrt, &PostPtrLen);
    pPostPrt[PostPtrLen] = '\0';
    //if(NMEAtype == GGA || NMEAtype == NRMC){
    //    pPostPrt[PostPtrLen] = '\0';
	//    debug_msg("^R[%d]%s\r\n", num, pPostPrt);
    //}
    switch(NMEAtype)
    {
    case GSA:
        sscanf_s(pPostPrt, "%s %c %d %d %d %d %d %d %d %d %d %d %d %d %d %lf %lf %lf %X\n", type, NMEA_TYPE_SIZE, &SelMode, 1, &FixMode, &SatUsed01, &SatUsed02, &SatUsed03, &SatUsed04, &SatUsed05, &SatUsed06, &SatUsed07, &SatUsed08, &SatUsed09, &SatUsed10, &SatUsed11, &SatUsed12, &PDOP, &HDOP, &VDOP, &CheckSum);
        #if (_GPS_CheckSum_Enable)
            //checkSum_Ret = GPSRec_CheckSum(NMEA_str, Len, CheckSum);
        #endif

		if (checkSum_Ret)
		{
        if (FixMode != No_Fix)
        {
            GSAInfo.SelMode = SelMode;
            GSAInfo.FixMode = FixMode;
            GSAInfo.SatUsed01 = SatUsed01;
            GSAInfo.SatUsed02 = SatUsed02;
            GSAInfo.SatUsed03 = SatUsed03;
            GSAInfo.SatUsed04 = SatUsed04;
            GSAInfo.SatUsed05 = SatUsed05;
            GSAInfo.SatUsed06 = SatUsed06;
            GSAInfo.SatUsed07 = SatUsed07;
            GSAInfo.SatUsed08 = SatUsed08;
            GSAInfo.SatUsed09 = SatUsed09;
            GSAInfo.SatUsed10 = SatUsed10;
            GSAInfo.SatUsed11 = SatUsed11;
            GSAInfo.SatUsed12 = SatUsed12;
            GSAInfo.PDOP = PDOP;
            GSAInfo.HDOP = HDOP;
            GSAInfo.VDOP = VDOP;
	            //GPS_debug(("#GSA SelMode = %c, FixMode = %d \r\n",GSAInfo.SelMode,GSAInfo.FixMode));
	            //GPS_debug(("#GSA SelMode = %c, FixMode = %d \r\n",GSAInfo.SelMode,GSAInfo.FixMode));
	            //GPS_debug(("#GSA SatUsed1 = %d, %d, %d, %d \r\n",GSAInfo.SatUsed01,GSAInfo.SatUsed02, GSAInfo.SatUsed03, GSAInfo.SatUsed04));
	            //GPS_debug(("#GSA SatUsed2 = %d, %d, %d, %d \r\n",GSAInfo.SatUsed05,GSAInfo.SatUsed06, GSAInfo.SatUsed07, GSAInfo.SatUsed08));
	            //GPS_debug(("#GSA SatUsed3 = %d, %d, %d, %d \r\n",GSAInfo.SatUsed09,GSAInfo.SatUsed10, GSAInfo.SatUsed11, GSAInfo.SatUsed12));
	            //GPS_debug(("#PDOP = %.1f, HDOP = %.1f, VDOP = %.1f CheckSum = %X\r\n",GSAInfo.PDOP,GSAInfo.HDOP, GSAInfo.VDOP,CheckSum));
	        }
		}

        //Check processed sentence
        /*
        if (PostPtrLen)
        {
            for (i = 0; i < PostPtrLen; i++)
                NMEA_debug(("%c",*pPostPrt++));
        }
        NMEA_debug(("\r\n"));
        */
        break;
#ifdef GPS_GNRMC
    case NRMC:
        sscanf_s(pPostPrt, "%s %lf %c %lf %lf %d %lf %lf %lf %d %d %c %X\n", type,NMEA_TYPE_SIZE,&UTCTime, &Status,1,&Speed,&Angle,&FusionMode,&Roll,&Pitch,&Yaw,&Date,&ErrorState,&PosMode,1,&CheckSum);

        #if (_GPS_CheckSum_Enable)
        checkSum_Ret = GPSRec_CheckSum(NMEA_str, Len, CheckSum);// miotone_2018801 zejia shrink
        #endif
		if(checkSum_Ret)// miotone_2018801 zejia shrink
        {
            RMCInfo.Hour = (UINT32)UTCTime / 10000;
            RMCInfo.Minute = ((UINT32)UTCTime % 10000) / 100;
            RMCInfo.Second = ((UINT32)UTCTime % 100);
            RMCInfo.Status = Status;
            RMCInfo.Speed = Speed;
            RMCInfo.Angle = Angle;
            RMCInfo.FusionMode = FusionMode;
            RMCInfo.Roll = Roll;
            RMCInfo.Pitch = Pitch;
            RMCInfo.Yaw = Yaw;
            RMCInfo.Day = Date / 10000;
            RMCInfo.Month = (Date % 10000) / 100;
            RMCInfo.Year = (Date % 100);
		//RMCInfo.Status = 'A';
            
	        if (RMCInfo.Status == 'A')   //Data valid
	        {
			
	            set_flg(FLG_ID_GPS, GPS_FLAG_DATAVALID);

	            if (GPS_NMEA_Print == 1)
	            {
	                GNXX_debug(("#GNRMC Hour = %d, Minute = %d, Second = %d \r\n",RMCInfo.Hour,RMCInfo.Minute,RMCInfo.Second));
	                GNXX_debug(("#GNRMC Day = %d, Month = %d, Year = %d \r\n",RMCInfo.Day,RMCInfo.Month,RMCInfo.Year));
	                GNXX_debug(("#GNRMC Status = %c, Speed = %.1f, Angle = %.1f FusionMode = %d \r\n",RMCInfo.Status,RMCInfo.Speed,RMCInfo.Angle,RMCInfo.FusionMode));
	                GNXX_debug(("#GNRMC Roll = %.1f, Pitch = %.1f, Yaw = %.1f \r\n",RMCInfo.Roll,RMCInfo.Pitch,RMCInfo.Yaw));
					GNXX_debug(("#checksum = %X \r\n",CheckSum));
	            }
	            //GPS_debug(("\x1b[34m"));   //Blue
	            //GPS_debug(("#RMC: Status = %c, Speed = %.1f, Angle = %.1f \r\n",RMCInfo.Status,RMCInfo.Speed,RMCInfo.Angle));
	            //GPS_debug(("#RMC: Date = %d\\%d\\%d CheckSum = %X\r\n",RMCInfo.Year,RMCInfo.Month,RMCInfo.Day,CheckSum));
	            //GPS_debug(("\x1b[30m"));   //Black
	        }
	        else if ((RMCInfo.Status == 'N') || (RMCInfo.Status == 'V'))
	        {
	            clr_flg(FLG_ID_GPS, GPS_FLAG_DATAVALID);
				
	        }
            {
            stRMCInfo.Date = rtc_getDate();
            stRMCInfo.Time = rtc_getTime();
            if(RMCInfo.Status == 'A'){
                stRMCInfo.Speed     = floor(RMCInfo.Speed * 1.852);
                stRMCInfo.Status    = 1;
            }
            else{
                stRMCInfo.Speed     = 0;
                stRMCInfo.Status    = 0;
            }
            if(PosFixInd == 1){
                stRMCInfo.NSInd     = GGAInfo.NSInd;
                stRMCInfo.EWInd     = GGAInfo.EWInd;
                stRMCInfo.Latitude  = dm2dd(GGAInfo.Latitude);
                stRMCInfo.Longitude = dm2dd(GGAInfo.Longitude);
            }
            else{
                stRMCInfo.NSInd     = 0;
                stRMCInfo.EWInd     = 0;
                stRMCInfo.Latitude  = 0;
                stRMCInfo.Longitude = 0;
            }
            
            stRMCInfo.Angle     = RMCInfo.Angle;
            stRMCInfo.Roll      = RMCInfo.Roll;
            stRMCInfo.Pitch     = RMCInfo.Pitch;
            stRMCInfo.Yaw       = RMCInfo.Yaw;
            //stRMCInfo.gyro_ver   = gGyro_Ver_2_0;
            stRMCInfo.fusion_mode= FusionMode;
            //debug_msg("RMC:%f\t%f\t%f\r\n", RMCInfo.Roll, RMCInfo.Pitch, RMCInfo.Yaw);
            //debug_msg("roll:%f,pitch:%f,yaw:%f|<%s>\r\n", RMCInfo.Roll, RMCInfo.Pitch, RMCInfo.Yaw, pPostPrt);
			#if 0
			if(gGyro_Ver_2_0 == GYRO_HARDWARE_VER_BB || gGyro_Ver_2_0 == GYRO_HARDWARE_VER_AA){
                QH_DataQueueSend(&stRlDataGPSQueue, &stRMCInfo, 0);
                QH_DataQueueSend(&stRecordGPSQueue, &stRMCInfo, 0);
            }
            else{
                for(n=0; n<5; n++){
                    QH_DataQueueSend(&stRlDataGPSQueue, &stRMCInfo, 0);
                    QH_DataQueueSend(&stRecordGPSQueue, &stRMCInfo, 0);
                }
            }
			#endif
	        }
	    }

        break;
#else
    case RMC:
        //sscanf_s(pPostPrt, "%s %lf %c %lf %c %lf %c %lf %lf %d %lf %c %X\n", type, NMEA_TYPE_SIZE, &UTCTime, &Status, 1, &Lat, &NSInd, 1, &Lon, &EWInd, 1, &Speed, &Angle, &Date, &MagVar, &MagVarInd, 1, &CheckSum);
		//debug_msg("^Y%s\r\n",NMEA_str);
		
		sscanf_s(pPostPrt, "%s %lf %c %lf %c %lf %c %lf %lf %d %lf %c %c %X\n", type,NMEA_TYPE_SIZE,&UTCTime, &Status,1, &Lat, &NSInd,1,&Lon, &EWInd,1, &Speed, &Angle, &Date, &MagVar, &MagVarInd,1,&Status_posMode,1,&CheckSum);
		#if 1//(_GPS_CheckSum_Enable)
            //checkSum_Ret = GPSRec_CheckSum(NMEA_str, Len, CheckSum);// miotone_2018801 zejia shrink
		
        #endif

		//if(checkSum_Ret)// miotone_2018801 zejia shrink
        {
            RMCInfo.Hour = (UINT32)UTCTime / 10000;
            RMCInfo.Minute = ((UINT32)UTCTime % 10000) / 100;
            RMCInfo.Second = ((UINT32)UTCTime % 100);
            RMCInfo.Status = Status;
            RMCInfo.Latitude = Lat;
            RMCInfo.NSInd = NSInd;
            RMCInfo.Longitude = Lon;
            RMCInfo.EWInd = EWInd;
            RMCInfo.Speed = Speed;
            RMCInfo.Angle = Angle;
            RMCInfo.Day = Date / 10000;
            RMCInfo.Month = (Date % 10000) / 100;
            RMCInfo.Year = (Date % 100);
        }
		//RMCInfo.Status = 'A';

        if (RMCInfo.Status == 'A')   //Data valid
        {
		
            set_flg(FLG_ID_GPS, GPS_FLAG_DATAVALID);

            //GNXX_debug(("#GNRPC Status = %c, Speed = %.1f, Angle = %.1f \r\n",RMCInfo.Status,RMCInfo.Speed,RMCInfo.Angle));
            //GPS_debug(("\x1b[34m"));   //Blue
            //GPS_debug(("#RMC: Status = %c, Speed = %.1f, Angle = %.1f \r\n",RMCInfo.Status,RMCInfo.Speed,RMCInfo.Angle));
            //GPS_debug(("#RMC: Date = %d\\%d\\%d CheckSum = %X\r\n",RMCInfo.Year,RMCInfo.Month,RMCInfo.Day,CheckSum));
            //GPS_debug(("\x1b[30m"));   //Black
        }
        else if ((RMCInfo.Status == 'N') || (RMCInfo.Status == 'V'))
        {
            clr_flg(FLG_ID_GPS, GPS_FLAG_DATAVALID);
			
        }

        break;
#endif
    case GGA:
        CheckSum = 0;
        //sscanf_s(pPostPrt, "%s %lf %lf %c %lf %c %d %d %lf %lf %c %lf %c %d %d %d %d%X\n", type, NMEA_TYPE_SIZE, &UTCTime, &Lat, &NSInd, 1, &Lon, &EWInd, 1, &PosFixInd, &SatNumber, &HDOP, &Alt, &M1, 1, &MSL, &M2, 1, &Empty1, &Empty2,&GpsStNum,&BdStNum,&CheckSum);
		memset(szGGAVer, '\0', sizeof(szGGAVer));
        if(num == 13){
            sscanf_s(pPostPrt, "%s %lf %lf %c %lf %c %d %d %d %lf %d %d %X\n",    type, NMEA_TYPE_SIZE, &UTCTime, &Lat, &NSInd, 1, &Lon, &EWInd, 1, &PosFixInd, &SatNumber, &BdStNum, &Alt, &GGAFusionMode, &GGAErrorState, &CheckSum);
        }
        else if(num == 14){
            //sscanf_s(pPostPrt, "%s %lf %lf %c %lf %c %d %d %d %lf %d %d %s %X\n", type, NMEA_TYPE_SIZE, &UTCTime, &Lat, &NSInd, 1, &Lon, &EWInd, 1, &PosFixInd, &SatNumber, &BdStNum, &Alt, &GGAFusionMode, &GGAErrorState, szGGAVer, 3, &CheckSum);
            sscanf(pPostPrt, "%s %lf %lf %c %lf %c %d %d %d %lf %d %d %s %X\n", type, &UTCTime, &Lat, &NSInd, &Lon, &EWInd, &PosFixInd, &SatNumber, &BdStNum, &Alt, &GGAFusionMode, &GGAErrorState, szGGAVer, &CheckSum);
            //debug_msg("________________%x________________\r\n", CheckSum);
        }
        #if (_GPS_CheckSum_Enable)
        checkSum_Ret = GPSRec_CheckSum(NMEA_str, Len, CheckSum);
        #endif
        //debug_msg("^REDcheckSum_Ret = %d, szGGAVer %s\r\n", checkSum_Ret, szGGAVer);
		if (checkSum_Ret)
		{
			#if 0
            if(szGGAVer[0] == 'A' && szGGAVer[1] == 'A'){
                gGyro_Ver_2_0 = GYRO_HARDWARE_VER_AA;
            }
            else if(szGGAVer[0] == 'B' && szGGAVer[1] == 'B'){
                gGyro_Ver_2_0 = GYRO_HARDWARE_VER_BB;
            }
            else{
                gGyro_Ver_2_0 = GYRO_HARDWARE_VER_1ST;
            }
            //debug_msg("^RED1gGyro_Ver_2_0 = %d, szGGAVer %s\r\n", gGyro_Ver_2_0, szGGAVer);
			#endif
			
	        if (PosFixInd == 1)  //Data valid
	        {
	            GGAInfo.Hour = (UINT32)UTCTime / 10000;
	            GGAInfo.Minute = ((UINT32)UTCTime % 10000) / 100;
	            GGAInfo.Second = ((UINT32)UTCTime % 100);
	            GGAInfo.Latitude = Lat;
	            GGAInfo.NSInd = NSInd;
	            GGAInfo.Longitude = Lon;
	            GGAInfo.EWInd = EWInd;
	            GGAInfo.PosFixInd = PosFixInd;
	            GGAInfo.SatNumber = SatNumber;
	            GGAInfo.HDOP = HDOP;
	            GGAInfo.Altitude = Alt;
	            GGAInfo.MSL = MSL;
				GGAInfo.GpsSatNum= GpsStNum;
	            GGAInfo.BdSatNum= BdStNum;
	            if (GPS_NMEA_Print == 1)
	            {
	                GNXX_debug(("#GGA: Time = %d:%d:%d CheckSum = %X\r\n",GGAInfo.Hour, GGAInfo.Minute, GGAInfo.Second,CheckSum));
	                GNXX_debug(("#GGA: Lat = %1f %c, Lon = %1f %c \r\n",GGAInfo.Latitude, NSInd, GGAInfo.Longitude, EWInd));
	                GNXX_debug(("#GGA: PosFixInd = %d, SatNum = %d, HDOP = %2.1f, Alt = %.1f, MSL = %.1f \r\n",GGAInfo.PosFixInd, GGAInfo.SatNumber, GGAInfo.HDOP, GGAInfo.Altitude, GGAInfo.MSL));
	                GNXX_debug(("#GGA: GpsStNum = %d, BdStNum = %d \r\n",GGAInfo.GpsSatNum, GGAInfo.BdSatNum));
	            }

	        }
		}

        break;

    case GSV:
        sscanf_s(pPostPrt, "%s %d %d %d %s\n", type, NMEA_TYPE_SIZE, &NumOfSen, &SenNum, &SatInView, RemainStr, NMEA_SENTENCE_SIZE);
        if (NumOfSen == SenNum)
        {
            if ((SatInView % 4) == 1)
                sscanf_s(pPostPrt, "%s %d %d %d %d %d %d %d %X\n", type, NMEA_TYPE_SIZE, &NumOfSen, &SenNum, &SatInView, &SatNum1, &Elev1, &Azi1, &SNR1, &CheckSum);
            else if ((SatInView % 4) == 2)
                sscanf_s(pPostPrt, "%s %d %d %d %d %d %d %d %d %d %d %d %X\n", type, NMEA_TYPE_SIZE, &NumOfSen, &SenNum, &SatInView, &SatNum1, &Elev1, &Azi1, &SNR1, &SatNum2, &Elev2, &Azi2, &SNR2, &CheckSum);
            else if ((SatInView % 4) == 3)
                sscanf_s(pPostPrt, "%s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %X\n", type, NMEA_TYPE_SIZE, &NumOfSen, &SenNum, &SatInView, &SatNum1, &Elev1, &Azi1, &SNR1, &SatNum2, &Elev2, &Azi2, &SNR2, &SatNum3, &Elev3, &Azi3, &SNR3, &CheckSum);
            else
                sscanf_s(pPostPrt, "%s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %X\n", type, NMEA_TYPE_SIZE, &NumOfSen, &SenNum, &SatInView, &SatNum1, &Elev1, &Azi1, &SNR1, &SatNum2, &Elev2, &Azi2, &SNR2, &SatNum3, &Elev3, &Azi3, &SNR3, &SatNum4, &Elev4, &Azi4, &SNR4, &CheckSum);
        }
        else
        {
            sscanf_s(pPostPrt, "%s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %X\n", type, NMEA_TYPE_SIZE, &NumOfSen, &SenNum, &SatInView, &SatNum1, &Elev1, &Azi1, &SNR1, &SatNum2, &Elev2, &Azi2, &SNR2, &SatNum3, &Elev3, &Azi3, &SNR3, &SatNum4, &Elev4, &Azi4, &SNR4, &CheckSum);
        }

        #if (_GPS_CheckSum_Enable)
         checkSum_Ret = GPSRec_CheckSum(NMEA_str, Len, CheckSum);
        #endif

		if (checkSum_Ret)
		{
	        if (SenNum == 1)   //GSV1
	        {
	            GSVInfo.NumOfSen =     NumOfSen;
	            GSVInfo.SatInView =    SatInView;
	            GSVInfo.SAT01.SatNum = SatNum1;
	            GSVInfo.SAT01.Elev =   Elev1;
	            GSVInfo.SAT01.Azi =    Azi1;
	            GSVInfo.SAT01.SNR =    SNR1;
	            GSVInfo.SAT02.SatNum = SatNum2;
	            GSVInfo.SAT02.Elev =   Elev2;
	            GSVInfo.SAT02.Azi =    Azi2;
	            GSVInfo.SAT02.SNR =    SNR2;
	            GSVInfo.SAT03.SatNum = SatNum3;
	            GSVInfo.SAT03.Elev =   Elev3;
	            GSVInfo.SAT03.Azi =    Azi3;
	            GSVInfo.SAT03.SNR =    SNR3;
	            GSVInfo.SAT04.SatNum = SatNum4;
	            GSVInfo.SAT04.Elev =   Elev4;
	            GSVInfo.SAT04.Azi =    Azi4;
	            GSVInfo.SAT04.SNR =    SNR4;
	            //GPS_debug(("\x1b[35m"));   //Violet
	            //GPS_debug(("#GSV%d SatInView = %d, CheckSum = %X\r\n",SenNum, SatInView, CheckSum));
	            //GPS_debug(("#GSV%d Sat1 = %d, %d, %d, %d, Sat2 = %d, %d, %d, %d\r\n",SenNum, SatNum1, Elev1, Azi1, SNR1, SatNum2, Elev2, Azi2, SNR2));
	            //GPS_debug(("#GSV%d Sat3 = %d, %d, %d, %d, Sat4 = %d, %d, %d, %d\r\n",SenNum, SatNum3, Elev3, Azi3, SNR3, SatNum4, Elev4, Azi4, SNR4));
	            //GPS_debug(("\x1b[30m"));      //Black

	        }
	        else if (SenNum == 2)   //GSV2
	        {
	            GSVInfo.NumOfSen =     NumOfSen;
	            GSVInfo.SatInView =    SatInView;
	            GSVInfo.SAT05.SatNum = SatNum1;
	            GSVInfo.SAT05.Elev =   Elev1;
	            GSVInfo.SAT05.Azi =    Azi1;
	            GSVInfo.SAT05.SNR =    SNR1;
	            GSVInfo.SAT06.SatNum = SatNum2;
	            GSVInfo.SAT06.Elev =   Elev2;
	            GSVInfo.SAT06.Azi =    Azi2;
	            GSVInfo.SAT06.SNR =    SNR2;
	            GSVInfo.SAT07.SatNum = SatNum3;
	            GSVInfo.SAT07.Elev =   Elev3;
	            GSVInfo.SAT07.Azi =    Azi3;
	            GSVInfo.SAT07.SNR =    SNR3;
	            GSVInfo.SAT08.SatNum = SatNum4;
	            GSVInfo.SAT08.Elev =   Elev4;
	            GSVInfo.SAT08.Azi =    Azi4;
	            GSVInfo.SAT08.SNR =    SNR4;
	            //GPS_debug(("\x1b[35m"));   //Violet
	            //GPS_debug(("#GSV%d SatInView = %d, CheckSum = %X\r\n",SenNum, SatInView, CheckSum));
	            //GPS_debug(("#GSV%d Sat5 = %d, %d, %d, %d, Sat6 = %d, %d, %d, %d\r\n",SenNum, SatNum1, Elev1, Azi1, SNR1, SatNum2, Elev2, Azi2, SNR2));
	            //GPS_debug(("#GSV%d Sat7 = %d, %d, %d, %d, Sat8 = %d, %d, %d, %d\r\n",SenNum, SatNum3, Elev3, Azi3, SNR3, SatNum4, Elev4, Azi4, SNR4));
	            //GPS_debug(("\x1b[30m"));      //Black

	        }
	        else if (SenNum == 3)   //GSV3
	        {
	            GSVInfo.NumOfSen =     NumOfSen;
	            GSVInfo.SatInView =    SatInView;
	            GSVInfo.SAT09.SatNum = SatNum1;
	            GSVInfo.SAT09.Elev =   Elev1;
	            GSVInfo.SAT09.Azi =    Azi1;
	            GSVInfo.SAT09.SNR =    SNR1;
	            GSVInfo.SAT10.SatNum = SatNum2;
	            GSVInfo.SAT10.Elev =   Elev2;
	            GSVInfo.SAT10.Azi =    Azi2;
	            GSVInfo.SAT10.SNR =    SNR2;
	            GSVInfo.SAT11.SatNum = SatNum3;
	            GSVInfo.SAT11.Elev =   Elev3;
	            GSVInfo.SAT11.Azi =    Azi3;
	            GSVInfo.SAT11.SNR =    SNR3;
	            GSVInfo.SAT12.SatNum = SatNum4;
	            GSVInfo.SAT12.Elev =   Elev4;
	            GSVInfo.SAT12.Azi =    Azi4;
	            GSVInfo.SAT12.SNR =    SNR4;
	            //GPS_debug(("\x1b[35m"));   //Violet
	            //GPS_debug(("#GSV%d SatInView = %d, CheckSum = %X\r\n",SenNum, SatInView, CheckSum));
	            //GPS_debug(("#GSV%d Sat9 = %d, %d, %d, %d,  Sat10 = %d, %d, %d, %d\r\n",SenNum, SatNum1, Elev1, Azi1, SNR1, SatNum2, Elev2, Azi2, SNR2));
	            //GPS_debug(("#GSV%d Sat11 = %d, %d, %d, %d, Sat12 = %d, %d, %d, %d\r\n",SenNum, SatNum3, Elev3, Azi3, SNR3, SatNum4, Elev4, Azi4, SNR4));
	            //GPS_debug(("\x1b[30m"));      //Black

	        }
	        else if (SenNum == 4)   //GSV4	// HOWARD add for SatInView > 12
	        {
	            GSVInfo.NumOfSen =     NumOfSen;
	            GSVInfo.SatInView =    SatInView;
	            GSVInfo.SAT09.SatNum = SatNum1;
	            GSVInfo.SAT09.Elev =   Elev1;
	            GSVInfo.SAT09.Azi =    Azi1;
	            GSVInfo.SAT09.SNR =    SNR1;
	            GSVInfo.SAT10.SatNum = SatNum2;
	            GSVInfo.SAT10.Elev =   Elev2;
	            GSVInfo.SAT10.Azi =    Azi2;
	            GSVInfo.SAT10.SNR =    SNR2;
	            GSVInfo.SAT11.SatNum = SatNum3;
	            GSVInfo.SAT11.Elev =   Elev3;
	            GSVInfo.SAT11.Azi =    Azi3;
	            GSVInfo.SAT11.SNR =    SNR3;
	            GSVInfo.SAT12.SatNum = SatNum4;
	            GSVInfo.SAT12.Elev =   Elev4;
	            GSVInfo.SAT12.Azi =    Azi4;
	            GSVInfo.SAT12.SNR =    SNR4;
	            //GPS_debug(("\x1b[35m"));   //Violet
	            //GPS_debug(("#GSV%d SatInView = %d, CheckSum = %X\r\n",SenNum, SatInView, CheckSum));
	            //GPS_debug(("#GSV%d Sat13 = %d, %d, %d, %d,  Sat14 = %d, %d, %d, %d\r\n",SenNum, SatNum1, Elev1, Azi1, SNR1, SatNum2, Elev2, Azi2, SNR2));
	            //GPS_debug(("#GSV%d Sat15 = %d, %d, %d, %d, Sat16 = %d, %d, %d, %d\r\n",SenNum, SatNum3, Elev3, Azi3, SNR3, SatNum4, Elev4, Azi4, SNR4));
	            //GPS_debug(("\x1b[30m"));      //Black

	        }		
	        else
	        {
	            //DBG_ERR("GPS: Invalid number of GSV");
	        }
		}
        break;

    //Do not support GLL and VTG yet
    case GLL:
        break;

    case VTG:
        break;
    case INSSTAT:
        //$INSSTAT,0,0,0,0,00,0,0,25,20*6F
        //$INSSTAT 0 0 0 0 00 0 0 25 20 6F
        CheckSum = 0;
        sscanf(pPostPrt, "%s %u %u %u %u %u %u %u %u %u %X\n", szInssType, &uiInssVal1, &uiInssVal2, &uiInssVal3, &uiInssVal4, &uiInssVal5, &uiInssVal6, &uiInssVal7, &uiInssVal8, &uiInssVal9, &CheckSum);
        //debug_msg("________________%x________________\r\n", CheckSum);
        #if (_GPS_CheckSum_Enable)
        checkSum_Ret = GPSRec_CheckSum(NMEA_str, Len, CheckSum);
        #endif
        if (checkSum_Ret)
        {
            //debug_msg("0[INSSTAT:%s<------>%s %u %u %u %u %u %u %u %u %u %X]\r\n", NMEA_str, szInssType, uiInssVal1, uiInssVal2, uiInssVal3, uiInssVal4, uiInssVal5, uiInssVal6, uiInssVal7, uiInssVal8, uiInssVal9, CheckSum);
        }
        else{
            //debug_msg("1[INSSTAT:%s<------>%s %u %u %u %u %u %u %u %u %u %X]\r\n", NMEA_str, szInssType, uiInssVal1, uiInssVal2, uiInssVal3, uiInssVal4, uiInssVal5, uiInssVal6, uiInssVal7, uiInssVal8, uiInssVal9, CheckSum);
        }
        break;
    case PDTINFO:
        //$PDTINFO,MXTS008-V300C001B011,,*00
        //$PDTINFO MXTS008-V300C001B011 0 0 00
        CheckSum = 0;
        sscanf(pPostPrt, "%s %s %u %u %X\n", szPdtType, szPdtVer, &uiPdtVal1, &uiPdtVal2, &CheckSum);
        //debug_msg("________________%x________________\r\n", CheckSum);
        #if (_GPS_CheckSum_Enable)
        checkSum_Ret = GPSRec_CheckSum(NMEA_str, Len, CheckSum);
        #endif
        if (checkSum_Ret)
        {
            //debug_msg("0[PDTINFO:%s<------>%s,%s,%u,%u,%u]\r\n", NMEA_str, szPdtType, szPdtVer, uiPdtVal1, uiPdtVal2, CheckSum);
        }
        break;

    default:
        break;
    }

}

// soling_20180801 zejia add
void GPSRec_ESFParser(CHAR *NMEA_str, UINT32 Len, NMEATYPE NMEAtype)
{
    //UINT32 i;
    //BOOL checkSum_Ret = FALSE;
    UINT8 gyrogsensor8[ESF_MEAS_DATA_LEN] = {0};
    UINT32 gyrogsensor32[ESF_MEAS_DATA_TYPE] = {0};//
    BOOL check_esf_data = 0;
	static int esf_buffer_cnt = 0;
    //static int cnt = 0;
    //double gsensor_data;
    ElemGyroData elem;
    //float outAtti[3];
    //debug_msg("^R%s\r\n",NMEA_str);

    pPostPrt = PostPtr;
    PostPtrLen = Len;

	gps_receive_type = GPS_TYPE_ESF;
    check_esf_data = GPSRec_ESFTransformData(NMEA_str, Len, pPostPrt, &PostPtrLen,gyrogsensor8,gyrogsensor32);
    //NTPrintBuf("=====>ESF_DATA", gyrogsensor8, ESF_MEAS_DATA_LEN);
    switch(NMEAtype)
    {

    case ESF:
    	//sscanf_s(pPostPrt, "%s %d %d %d %d\n", type_esf,ESF_TYPE_SIZE,&gyro_x,&gyro_y,&gyro_z,&gsensor_x,&gsensor_y,&gsensor_z );

        if (check_esf_data)
        {

            gyro_x = gyrogsensor32[0] & 0x00FFFFFF;
            gyro_y = gyrogsensor32[1] & 0x00FFFFFF;
            gyro_z = gyrogsensor32[2] & 0x00FFFFFF;
            gsensor_x = gyrogsensor32[3] & 0x00FFFFFF;
            gsensor_y = gyrogsensor32[4] & 0x00FFFFFF;
            gsensor_z = gyrogsensor32[5] & 0x00FFFFFF;

            if (ESF_Print == 1)
            {
                ESF_debug(("=====>gyro_x %d \r\n",gyro_x));
                ESF_debug(("=====>gyro_y %d \r\n",gyro_y));
                ESF_debug(("=====>gyro_z %d \r\n",gyro_z));

                ESF_debug(("=====>gsensor_x %d \r\n",gsensor_x));
                ESF_debug(("=====>gsensor_y %d \r\n",gsensor_y));
                ESF_debug(("=====>gsensor_z %d \r\n",gsensor_z));
            }

            gyro_x = GPS_ESFDateFilter(gyro_x);
            gyro_y = GPS_ESFDateFilter(gyro_y);
            gyro_z = GPS_ESFDateFilter(gyro_z);
            gsensor_x = GPS_ESFDateFilter(gsensor_x);
            gsensor_y = GPS_ESFDateFilter(gsensor_y);
            gsensor_z = GPS_ESFDateFilter(gsensor_z);
            
#if 0
            //gyro_x = (gyro_x >> 8) & (0x00FFFFFF);
            //MY_MSG("=====>gyro_x data %d \r\n",gyro_x);
            gsensor_data = ESFInfo.GyroX;
            ESFInfo.GyroX = (float)(gyro_x/4096.000);
            
            if (abs(ESFInfo.GyroX) >= 1000)// filter
            {
                ESFInfo.GyroX = gsensor_data;
            }

            //gyro_y = (gyro_y >> 8) & (0x00FFFFFF);
            //MY_MSG("=====>gyro_y data %d \r\n",gyro_y);
            gsensor_data = ESFInfo.GyroY;
            ESFInfo.GyroY= (float)(gyro_y/4096.000);

            if (abs(ESFInfo.GyroY) >= 1000)// filter
            {
                ESFInfo.GyroY = gsensor_data;
            }

            //gyro_z = (gyro_z >> 8) & (0x00FFFFFF);
            gsensor_data = ESFInfo.GyroZ;
            ESFInfo.GyroZ = (float)(gyro_z/4096.000);

            if (abs(ESFInfo.GyroZ) >= 1000)// filter
            {
                ESFInfo.GyroZ = gsensor_data;
            }
#endif
            ESFInfo.GyroX = (float)(gyro_x/4096.000);
            ESFInfo.GyroY= (float)(gyro_y/4096.000);
            ESFInfo.GyroZ = (float)(gyro_z/4096.000);

            if (ESF_Print == 1)
            {
                ESF_debug(("=====>gyro_x:%lf\tgyro_y:%f\tgyro_z:%f t=%d\r\n",ESFInfo.GyroX, ESFInfo.GyroY, ESFInfo.GyroZ, time(NULL)));
            }

#if 0            
            //gsensor_x = (gsensor_x >> 8) & (0x00FFFFFF);
            //MY_MSG("=====>gsensor_x data %d \r\n",gsensor_x);
            //gsensor_data = ESFInfo.GsensorX;
            ESFInfo.GsensorX = (float)(gsensor_x/1024.000);
            
            if (abs(ESFInfo.GsensorX) > 50)// filter
            {
                NTPrintBuf("=====>GsensorX trigger", pPostPrt, ESF_MEAS_LEN);
                //ESFInfo.GsensorX = gsensor_data;
            }

            //gsensor_y = (gsensor_y >> 8) & (0x00FFFFFF);
            //MY_MSG("=====>gsensor_y data %d \r\n",gsensor_y);
            //gsensor_data = ESFInfo.GsensorY;
            ESFInfo.GsensorY = (float)(gsensor_y/1024.000);
            if (abs(ESFInfo.GsensorY) > 50)// filter
            {
                NTPrintBuf("=====>GsensorY trigger", pPostPrt, ESF_MEAS_LEN);
                //ESFInfo.GsensorY = gsensor_data;
            }
            
            //gsensor_z = (gsensor_z >> 8) & (0x00FFFFFF);
            //MY_MSG("=====>gsensor_z data %d \r\n",gsensor_z);
            //gsensor_data = ESFInfo.GsensorZ;
            ESFInfo.GsensorZ = (float)(gsensor_z/1024.000);
            if (abs(ESFInfo.GsensorZ) > 50)// filter
            {
                NTPrintBuf("=====>GsensorZ trigger", pPostPrt, ESF_MEAS_LEN);
                //ESFInfo.GsensorZ = gsensor_data;
            }
#endif
            ESFInfo.GsensorX = (float)(gsensor_x/1024.000);
            ESFInfo.GsensorY = (float)(gsensor_y/1024.000);
            ESFInfo.GsensorZ = (float)(gsensor_z/1024.000);
			esf_frames_cnt ++;
            {
                elem.angu_x = ESFInfo.GyroX;
                elem.angu_y = ESFInfo.GyroY;
                elem.angu_z = ESFInfo.GyroZ;
                elem.acce_x = ESFInfo.GsensorX;
                elem.acce_y = ESFInfo.GsensorY;
                elem.acce_z = ESFInfo.GsensorZ;
                if(GPSRec_GetGpsStatus() == 'A'){
                    elem.valid = 1;
                }
                else{
                    elem.valid = 0;
                }
                GPSRec_GetSpeed(&elem.speed);
				#if 0
                //unsigned count, remain;
                //QH_DataQueueQuery(&stDrivBhGryoQueue, &count, &remain);
                //debug_msg("_______________count:%d, remain:%d__________________\r\n",count, remain);
                QH_DataQueueSend(&stDrivBhGryoQueue, &elem, 0);
                if((cnt%5) == 0){
                    //Simply_Process_Gyro_Atti(elem.acce_x, elem.acce_y, elem.acce_z, elem.angu_x, elem.angu_y, elem.angu_z, outAtti);
                    //debug_msg("[Gyro]%f:%f:%f___%f:%f:%f\r\n",
                    //    elem.acce_x, elem.acce_y, elem.acce_z, elem.angu_x, elem.angu_y, elem.angu_z);
                    //debug_msg("Cal:%f\t%f\t%f\r\n", outAtti[0], outAtti[1], outAtti[2]);
                    QH_DataQueueSend(&stRlDataGryoQueue, &elem, 0);
                    QH_DataQueueSend(&stRecordGryoQueue, &elem, 0);
                }
                cnt++;
				#endif
			}

			// miotone_2018925 zejia 
			if (esf_buffer_cnt > (ESF_MEAS_BUFFER_LEN -1))
			{
				esf_buffer_cnt = 0;
			}
			//gyro_x_array[esf_buffer_cnt] = ESFInfo.GyroX;
			//gyro_y_array[esf_buffer_cnt] = ESFInfo.GyroY;
			//gyro_z_array[esf_buffer_cnt] = ESFInfo.GyroZ;
			//gsensor_x_array[esf_buffer_cnt] = ESFInfo.GsensorX;
			//gsensor_y_array[esf_buffer_cnt] = ESFInfo.GsensorY;
			//gsensor_z_array[esf_buffer_cnt] = ESFInfo.GsensorZ;
			GPSESFDataInfo.gyro_x[esf_buffer_cnt] = ESFInfo.GyroX;
			GPSESFDataInfo.gyro_y[esf_buffer_cnt] = ESFInfo.GyroY;
			GPSESFDataInfo.gyro_z[esf_buffer_cnt] = ESFInfo.GyroZ;
			GPSESFDataInfo.gsensor_x[esf_buffer_cnt] = ESFInfo.GsensorX;
			GPSESFDataInfo.gsensor_y[esf_buffer_cnt] = ESFInfo.GsensorY;
			GPSESFDataInfo.gsensor_z[esf_buffer_cnt] = ESFInfo.GsensorZ;
			esf_buffer_cnt ++;
			//end
			
            if (ESF_Print == 1)
            {
                ESF_debug(("=====>gsensor_x data float %lf \r\n",ESFInfo.GsensorX));
                ESF_debug(("=====>gsensor_y data float %lf \r\n",ESFInfo.GsensorY));
                ESF_debug(("=====>gsensor_z data float %lf \r\n",ESFInfo.GsensorZ));
            }

            if (Gsensor_Enable == 1)
            {
                if ((abs(ESFInfo.GsensorX) > Gsensor_Sen) && (abs(ESFInfo.GsensorX) < Gsensor_Sen1))
                {
                    MY_MSG("=====>gsensor_x %lf : %lf g/10 ~ %lf g/10 trigger!!!\r\n",ESFInfo.GsensorX,Gsensor_Sen,Gsensor_Sen1);
                }
                if ((abs(ESFInfo.GsensorY) > Gsensor_Sen) && (abs(ESFInfo.GsensorY) < Gsensor_Sen1))
                {
                    MY_MSG("=====>gsensor_y %lf : %lf g/10 ~ %lf g/10 trigger!!!\r\n",ESFInfo.GsensorY,Gsensor_Sen,Gsensor_Sen1);
                }
                if ((abs(ESFInfo.GsensorZ) > Gsensor_Sen) && (abs(ESFInfo.GsensorZ) < Gsensor_Sen1))
                {
                    MY_MSG("=====>gsensor_z %lf : %lf g/10 ~ %lf g/10 trigger!!!\r\n",ESFInfo.GsensorZ,Gsensor_Sen,Gsensor_Sen1);
                }
            }
            else if (Gyro_Enable == 1)
            {
                if ((abs(ESFInfo.GyroX) > Gyro_Sen) && (abs(ESFInfo.GyroX) < Gyro_Sen1))
                {
                    MY_MSG("=====>gyro_x %lf : %lf  ~ %lf  trigger!!!\r\n",ESFInfo.GsensorX,Gyro_Sen,Gyro_Sen1);
                }
                if ((abs(ESFInfo.GyroY) > Gyro_Sen) && (abs(ESFInfo.GyroY) < Gyro_Sen1))
                {
                    MY_MSG("=====>gyro_y %lf : %lf  ~ %lf  trigger!!!\r\n",ESFInfo.GsensorY,Gyro_Sen,Gyro_Sen1);
                }
                if ((abs(ESFInfo.GyroZ) > Gyro_Sen) && (abs(ESFInfo.GyroZ) < Gyro_Sen1))
                {
                    MY_MSG("=====>gyro_z %lf : %lf  ~ %lf  trigger!!!\r\n",ESFInfo.GsensorZ,Gyro_Sen,Gyro_Sen1);
                }
            }
            

    }
            break;
        default:
            break;
        }

}

//end
// miotone_2018801 zejia add
BOOL GPSRec_GetRMCOriginalDate(char *RmcStr)
{
	//MY_MSG("%s",RmcSentence);
	if(strstr(RmcSentence,"RMC"))
	{
		strcpy(RmcStr,RmcSentence);
		return TRUE;
	}
	else
		return FALSE;
}


//end
#if (GPS_GNRMC == ENABLE)
BOOL GPSRec_GetRMCDate_Read(GNRMCINFO *pRMCInfo)
{
	memcpy(pRMCInfo,&RMCInfo,sizeof(GNRMCINFO));
        return TRUE;
}

BOOL GPSRec_GetRMCDate(GNRMCINFO *pRMCInfo)
{
    //debug_msg("g_GPSLinked:%d, RMCInfo.Status == %c\r\n", g_GPSLinked, RMCInfo.Status);
    //if (RMCInfo.Status == 'A')  //Data valid
    if (RMCInfo.Status == 'A')  // miotone_2018827 zejia change
    {
        if (g_GPSLinked == TRUE)
        {
            memcpy(pRMCInfo,&RMCInfo,sizeof(GNRMCINFO));
            return TRUE;
        }
        else
        {
        	//MY_TRACE_LINE_COLOR;
			GPSRec_ClearAllGPSData();// miotone_2018919 zejia add
            RMCInfo.Status = 'V';
            return FALSE;
        }
    }
    else
    {
        return FALSE;
    }
}
void GPSRec_SetRMCDate(GNRMCINFO *pRMCInfo)
{
    memcpy(&RMCInfo,pRMCInfo,sizeof(GNRMCINFO));
    //debug_msg("<GPSRec_SetRMCDate>Status :%c\r\n",RMCInfo.Status);
    //debug_msg("<GPSRec_SetRMCDate>Year :%d\r\n",RMCInfo.Year);
    //debug_msg("<GPSRec_SetRMCDate>Second :%d\r\n",RMCInfo.Second);
    //debug_msg("<GPSRec_SetRMCDate>Latitude :%f\r\n",RMCInfo.Latitude);
    //debug_msg("<GPSRec_SetRMCDate>Longitude :%f\r\n",RMCInfo.Longitude);
    //debug_msg("<GPSRec_SetRMCDate>Speed :%f\r\n",RMCInfo.Speed);
    //debug_msg("<GPSRec_SetRMCDate>Angle :%f\r\n",RMCInfo.Angle);
}
#else
BOOL GPSRec_GetRMCDate(RMCINFO *pRMCInfo)
{
    if (RMCInfo.Status == 'A')  //Data valid
    {
    	#if 1// miotone_2018915 zejia change
        if (g_GPSLinked == TRUE)
        {
            memcpy(pRMCInfo,&RMCInfo,sizeof(RMCINFO));
            return TRUE;
        }
        else
        {
            RMCInfo.Status = 'V';
            return FALSE;
        }
		#else
		memcpy(pRMCInfo,&RMCInfo,sizeof(RMCINFO));
        return TRUE;
		#endif
    }
    else
    {
        return FALSE;
    }
}
void GPSRec_SetRMCDate(RMCINFO *pRMCInfo)
{
    memcpy(&RMCInfo,pRMCInfo,sizeof(RMCINFO));
    //debug_msg("<GPSRec_SetRMCDate>Status :%c\r\n",RMCInfo.Status);
    //debug_msg("<GPSRec_SetRMCDate>Year :%d\r\n",RMCInfo.Year);
    //debug_msg("<GPSRec_SetRMCDate>Second :%d\r\n",RMCInfo.Second);
    //debug_msg("<GPSRec_SetRMCDate>Latitude :%f\r\n",RMCInfo.Latitude);
    //debug_msg("<GPSRec_SetRMCDate>Longitude :%f\r\n",RMCInfo.Longitude);
    //debug_msg("<GPSRec_SetRMCDate>Speed :%f\r\n",RMCInfo.Speed);
    //debug_msg("<GPSRec_SetRMCDate>Angle :%f\r\n",RMCInfo.Angle);
}
#endif

BOOL GPSRec_GetUTCDate(UINT32 *Year, UINT32 *Month, UINT32 *Day)
{
    if (RMCInfo.Status == 'A')  //Data valid
    {
        *Year   = RMCInfo.Year;
        *Month  = RMCInfo.Month;
        *Day    = RMCInfo.Day;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL GPSRec_GetUTCTime(UINT32 *Hour, UINT32 *Minute, UINT32 *Second)
{
    if (RMCInfo.Status == 'A')   //Data valid
    {
        *Hour   = RMCInfo.Hour;
        *Minute = RMCInfo.Minute;
        *Second = RMCInfo.Second;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL GPSRec_GetGGADate(GGAINFO *pGGAInfo)
{
    memcpy(pGGAInfo,&GGAInfo,sizeof(GGAINFO));
    if (PosFixInd == 1)  //Data valid
    {
        return TRUE;
    }
    else
        return FALSE;
}

// miotone_2018802 zejia add
BOOL GPSRec_GetESF(ESFINFO *pESFInfo)
{
    memcpy(pESFInfo,&ESFInfo,sizeof(ESFINFO));
    return TRUE;
}

BOOL GPSRec_GetGSV(GSVINFO *pGSVInfo)
{
    memcpy(pGSVInfo,&GSVInfo,sizeof(GSVINFO));
    return TRUE;
}

BOOL GPSRec_GetGPSLocalInfo(GPSLOCALINFO *pGPSLocalInfo)
{
    memcpy(pGPSLocalInfo,&GPSLocalInfo,sizeof(GPSLOCALINFO));
    return TRUE;
}

BOOL GPSRec_SetGPSLocalInfo(GPSLOCALINFO pGPSLocalInfo)
{
	memcpy(&GPSLocalInfo,&pGPSLocalInfo,sizeof(GPSLOCALINFO));
    return TRUE;
}

#if 1
BOOL GPSRec_GetESFData(ESFINFODATATEST *pGPSESFDataInfo)
{
    memcpy(pGPSESFDataInfo,&GPSESFDataInfo,sizeof(ESFINFODATATEST));
    return TRUE;
}
#endif

void GPSRec_ClearESF(void)
{
	memset(&ESFInfo,0,sizeof(ESFINFO));
}

void GPSRec_ClearGSV(void)
{
	memset(&GSVInfo,0,sizeof(GSVINFO));
}

void GPSRec_ClearGGA(void)
{
	memset(&GGAInfo,0,sizeof(GGAINFO));
}

void GPSRec_ClearGNRMC(void)
{
	memset(&RMCInfo,0,sizeof(GNRMCINFO));
}

void GPSRec_ClearAllGPSData(void)
{
	GPSRec_ClearESF();
	GPSRec_ClearGSV();
	GPSRec_ClearGGA();
	GPSRec_ClearGNRMC();
}

//end
FLOAT GPSRec_GetGPS_Speed(void)
{
    static FLOAT CurSpeed = 0;

    CurSpeed = RMCInfo.Speed * 1.852;

    return CurSpeed;
}

//#NT#2015/10/02#KCHong -begin
//#NT#Add GPS heartbeat to detect GPS status

BOOL GPSRec_GetSpeed(FLOAT *Speed)
{
    if (RMCInfo.Status == 'A')   //Data valid
    {
        *Speed = RMCInfo.Speed * 1.852;
        return TRUE;
    }
    else
    {
        *Speed = 0;//90;//
        return FALSE;

    }
}

UINT32 GetGPSHeartBeat(void)
{
    return GPSHeartBeat;
}

CHAR GPSRec_GetGpsStatus(void)
{
   /* UINT32 uRetGpsStatus = 0;

    uRetGpsStatus = uGpsCount ;
	return uRetGpsStatus;

    */
    return RMCInfo.Status;

}

// miotone_2018725 zejia add,G600_CHANGE_DEBUG
void GPSRec_FrameParser(UINT32 nFrame)
{
	CHAR* p = (CHAR*)nFrame;
    UINT32  uiLen;
    CHAR    szSentence[NMEA_SENTENCE_SIZE] = {0};
	CHAR    *pSentenceData;
    UINT16  CheckSentenceType1, CheckSentenceType2;
    UINT32  CheckSentenceType3;
    UINT16  ESFSentenceTotal,ESFSentenceId, ESFSentenceId1;
        //int i;
	
	if( NULL == p ) 
	{ 
	  return ; 
	}

	memcpy(szSentence,p,NMEA_SENTENCE_SIZE);
#if 1//def G600_CHANGE_DEBUG
    if (szSentence[0] == '$')
    {
        pSentenceData = szSentence;
#if 0
        if(strncmp(szSentence, "$GNGGA", 6) && strncmp(szSentence, "$GNRMC", 6) && strncmp(szSentence, "$INSSTAT", 6)){
            debug_msg("%s",pSentenceData);
        }
        else{
            debug_msg("%s",pSentenceData);
        }
#endif
        if (GPS_NMEA_Print == 1)
        {
            debug_msg("%s",pSentenceData);
        }
    }
    else if  (szSentence[0] == 0xB5)
    {
        pSentenceData = szSentence;// miotone_2018925 zejia add
        //for (i = 0;i < ESF_MEAS_LEN;i ++)
        //{
            //NTPrintBuf("=====>ESF_MEAS", szSentence, ESF_MEAS_LEN);
        //}
    }
    else
    {

    }
	#ifdef GPS_POWER_CTRL
	if  (szSentence[0] == 0xB5)
	{
		SystemSet_CheckESFData_Status(TRUE);
	}
	else
	{
		SystemSet_CheckESFData_Status(FALSE);
	}
	#endif
#else
	pSentenceData = szSentence;
	debug_msg("%s",pSentenceData);
#endif    
    //Each NMEA sentence begins with '$' (0x24)
    if (pSentenceData[0] == 0x24) //'$'
    {
    	uiLen = strlen(pSentenceData);
        //MY_MSG("=====>uiLen %d \r\n",uiLen);
        GPSHeartBeat ++;        //#NT#2015/10/02#KCHong#Add GPS heartbeat to detect GPS status
        CheckSentenceType1 = *(UINT16 *)(pSentenceData + 2);
        CheckSentenceType2 = *(UINT16 *)(pSentenceData + 4);
        CheckSentenceType3 = ((UINT)CheckSentenceType2 << 16) | (UINT)CheckSentenceType1;

		//DBG_ERR("CheckSentenceType3 : 0x%x\r\n",CheckSentenceType3);
        switch(CheckSentenceType3)
        {
        case GSA_SENTENCE:
            NMEASentence.GSA = pSentenceData;
            GPSRec_NMEAParser(pSentenceData, uiLen, GSA);
            break;

        case RMC_SENTENCE:
			//DBG_ERR("GNRMC data: %s\r\n",pSentenceData);				
			memcpy(RmcSentence,pSentenceData,NMEA_SENTENCE_SIZE);
			//MY_MSG("%s",RmcSentence);
            NMEASentence.RMC = pSentenceData;
            GPSRec_NMEAParser(pSentenceData, uiLen, RMC);
            break;
        case NRMC_SENTENCE:// miotone_2018801 zejia add
            memcpy(RmcSentence,pSentenceData,NMEA_SENTENCE_SIZE);
            NMEASentence.RMC = pSentenceData;
            GPSRec_NMEAParser(pSentenceData, uiLen, NRMC);
            break;
        //case GGA_SENTENCE:
        case NGGA_SENTENCE:// miotone_2018801 zejia change
            NMEASentence.GGA = pSentenceData;
            GPSRec_NMEAParser(pSentenceData, uiLen, GGA);
            break;

        case GSV_SENTENCE:
            NMEASentence.GSV = pSentenceData;
            GPSRec_NMEAParser(pSentenceData, uiLen, GSV);
            break;

        case GLL_SENTENCE:
            NMEASentence.GLL = pSentenceData;
            GPSRec_NMEAParser(pSentenceData, uiLen, GLL);
            break;

        case VTG_SENTENCE:
            NMEASentence.VTG = pSentenceData;
            GPSRec_NMEAParser(pSentenceData, uiLen, VTG);
            break;
        case INSST_SENTENCE:
            NMEASentence.INS = pSentenceData;
            GPSRec_NMEAParser(pSentenceData, uiLen, INSSTAT);
            break;
        case PDTIN_SENTENCE:
            NMEASentence.PDT = pSentenceData;
            GPSRec_NMEAParser(pSentenceData, uiLen, PDTINFO);
            break;
        default:
            //debug_msg("__________________________%X:%s________________________\r\n", CheckSentenceType3, (char *)&CheckSentenceType3);
            //DBG_ERR("GPS: Can't identify this sentence 0x%X \r\n",CheckSentenceType3);
            break;
        }

    }
    else if (pSentenceData[0] == 0xB5)
    {
    	uiLen = ESF_MEAS_LEN;
        //MY_MSG("=====>uiLen %d \r\n",uiLen);
        GPSHeartBeat ++;
        CheckSentenceType1 = *(UINT16 *)(pSentenceData);
        CheckSentenceType2 = *(UINT16 *)(pSentenceData + 2);
        CheckSentenceType3 = ((UINT)CheckSentenceType2 << 16) | (UINT)CheckSentenceType1;
        ESFSentenceTotal = *(UINT16 *)(pSentenceData + 4);
        //MY_MSG("=====>ESFSentenceTotal %d",ESFSentenceTotal);
        ESFSentenceId =  (UINT16)(ESFSentenceTotal - 8)/4;
        //MY_MSG("=====>ESFSentenceId %d",ESFSentenceId);
        ESFSentenceId1 = *(UINT16 *)(pSentenceData + 12);
        //MY_MSG("=====>ESFSentenceId1 %d",ESFSentenceId1);
        if (ESFSentenceId == ESFSentenceId1)
        {
            //MY_MSG("=====>CheckSentenceType3 %d",CheckSentenceType3);
            switch(CheckSentenceType3)
            {
            case ESF_SENTENCE:
                //DBG_ERR("GNRMC data: %s\r\n",pSentenceData);				
                //memcpy(RmcSentence,pSentenceData,NMEA_SENTENCE_SIZE);
                //MY_MSG("%s",RmcSentence);
                ESFSentence.ESF = pSentenceData;
                GPSRec_ESFParser(pSentenceData, uiLen, ESF);
                break;


            default:
                //DBG_ERR("GPS: Can't identify this sentence 0x%X \r\n",CheckSentenceType3);
                break;
            }
        }
        
    }
    else
    {
        ;
    }

}
void GPSRec_ParseAllFrame(void)
{
    UINT8 i;
	BOOL static GPSAllFramePro = TRUE;

	if (!GPSAllFramePro)
	{
		MY_TRACE_LINE_COLOR;
	}
	if (GPSAllFramePro)
	{
		GPSAllFramePro = FALSE;
		s_nFrameProCnt = s_nFrameCnt;
		for(i = 0; i < s_nFrameProCnt; i++ )
		{
			memcpy(GpsFrameProBuf[i],GpsFrameBuf[i],NMEA_SENTENCE_SIZE);
		}
		for(i = 0; i < s_nFrameProCnt; i++ )
		{
		    GPSRec_FrameParser((UINT32)GpsFrameProBuf[i]);
			memset(GpsFrameProBuf[i],0,NMEA_SENTENCE_SIZE);
		}
		s_nFrameCnt = 0;
		s_nFrameProCnt = 0;
		GPSAllFramePro = TRUE;
	}
}
//end
//#NT#2015/10/02#KCHong -end
//!just for driving behavior

#if 0
//!just for queue process
//!driving behavior
#define DRIVING_BEHAVIOR_SIZE 25
DataQueue_t stDrivBhGryoQueue;

//!real data process
#define REAL_DATA_SIZE 5
DataQueue_t stRlDataGryoQueue;
DataQueue_t stRlDataGPSQueue;

//!log data process
#define LOG_RECORD_SIZE 5
DataQueue_t stRecordGryoQueue;
DataQueue_t stRecordGPSQueue;

int Init_GyroDataQueue(void)
{
    int rval = 0;
    //!gyro info
    rval = QH_DataQueueCreate(&stDrivBhGryoQueue, NULL, sizeof(ElemGyroData), DRIVING_BEHAVIOR_SIZE);
    if(rval){
        debug_msg("Failed to create the driving behavior data queue\r\n");
        return -1;
    }
    rval = QH_DataQueueCreate(&stRlDataGryoQueue, NULL, sizeof(ElemGyroData), REAL_DATA_SIZE);
    if(rval){
        debug_msg("Failed to create the real data queue\r\n");
        return -1;
    }
    rval = QH_DataQueueCreate(&stRecordGryoQueue, NULL, sizeof(ElemGyroData), LOG_RECORD_SIZE);
    if(rval){
        debug_msg("Failed to create the log record data queue\r\n");
        return -1;
    }
    //!gps info
    rval = QH_DataQueueCreate(&stRlDataGPSQueue, NULL, sizeof(RMCINFO_ext), REAL_DATA_SIZE);
    if(rval){
        debug_msg("Failed to create the real data queue\r\n");
        return -1;
    }
    rval = QH_DataQueueCreate(&stRecordGPSQueue, NULL, sizeof(RMCINFO_ext), LOG_RECORD_SIZE);
    if(rval){
        debug_msg("Failed to create the log record data queue\r\n");
        return -1;
    }
    return 0;
}

int DeInit_GyroDataQueue(void)
{
    QH_DataQueueDelete(&stDrivBhGryoQueue);
    QH_DataQueueDelete(&stRlDataGryoQueue);
    QH_DataQueueDelete(&stRecordGryoQueue);
    QH_DataQueueDelete(&stRlDataGPSQueue);
    QH_DataQueueDelete(&stRecordGPSQueue);
    return 0;
}
#endif


//----------------------------------------------------------------------------
//!uart data queue
static UartDataQueue UartQueue, *pUartQueue = NULL;

#if 0
static int isFullUartData(void)
{
    if((pUartQueue->rear+1)%pUartQueue->size == pUartQueue->front){
        return TRUE;
    }
    return FALSE;
}
#endif

static int isEmpityUartData(void)
{
    if( pUartQueue->front == pUartQueue->rear ){
        return TRUE;
    }
    return FALSE;
}
static int getUartDataQueueFree(void)
{
    if(isEmpityUartData() == TRUE)
        return pUartQueue->size;
    else
        return (pUartQueue->size - (pUartQueue->rear - pUartQueue->front))%pUartQueue->size;
}

int getUartDataQueueLen(void)
{
    return ((pUartQueue->rear - pUartQueue->front) + pUartQueue->size)%pUartQueue->size;
}

void UartDataQueue_Init(void){
    static int inited = 0;
    if(inited)return ;
    UartQueue.size = UART_DATA_SIZE;
    UartQueue.front = UartQueue.rear = 0;
    pUartQueue = &UartQueue;
    inited = 1;
}

int putUartData(char *pData, int iLen)
{
    int offset = 0;
    if(getUartDataQueueFree() <= iLen)
    {
        return FALSE;
    }
    //add the data
    if(pUartQueue->rear+iLen <= pUartQueue->size){
        memcpy(&pUartQueue->pBase[pUartQueue->rear], pData, iLen);
    }
    else{
        offset = pUartQueue->size - pUartQueue->rear;
        memcpy(&pUartQueue->pBase[pUartQueue->rear], pData, offset);
        memcpy(&pUartQueue->pBase[0], pData+offset, iLen-offset);
    }
    pUartQueue->rear = (pUartQueue->rear+iLen) % pUartQueue->size;
    return TRUE;
}
int getUartData(char *pData, int *pLen)
{
    int size = *pLen;
    int rdlen = getUartDataQueueLen();
    if(isEmpityUartData() == TRUE)
    {
        *pLen = 0;
        return FALSE;
    }
    if(rdlen > size){
        rdlen = size;
    }
    if(pUartQueue->front+rdlen >= pUartQueue->size){
        rdlen =  pUartQueue->size - pUartQueue->front;
    }
    *pLen = rdlen;
    memcpy(pData, &pUartQueue->pBase[pUartQueue->front], rdlen);
    pUartQueue->front = (pUartQueue->front+rdlen) % pUartQueue->size;
    return TRUE;
}

