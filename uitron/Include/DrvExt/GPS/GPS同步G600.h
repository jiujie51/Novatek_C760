/**
    @file       GPSParser.h
    @ingroup

    @brief      Sample module brief
                Sample module description

    @note       Nothing (or anything need to be mentioned).

    Copyright   Novatek Microelectronics Corp. 2005.  All rights reserved.
*/

#ifndef _NMEAParser_H
#define _NMEAParser_H
#include "DataQueue.h"
#include "Type.h"
#include "uart.h"
#include "rtc.h"

#define NMEA_BUFFER_OFFSET		(35*1024)   //35KB
#define NMEA_BUFFER_RESERVED	128
#define NMEA_SENTENCE_SIZE		180
#define NMEA_TYPE_SIZE			7
#define ESF_TYPE_SIZE			4// miotone_2018801 zejia add
#define GSA_SENTENCE			0x41534750 //'A''S''G''P'
#define RMC_SENTENCE			0x434D5250 //'C''M''R''P'
#define GGA_SENTENCE			0x41474750 //'A''G''G''P'
#define GSV_SENTENCE			0x56534750 //'V''S''G''P'
#define GLL_SENTENCE			0x4C4C4750 //'L''L''G''P'
#define VTG_SENTENCE			0x47545650 //'G''T''V''P'
#define SNR_SENTENCE			0x4F534E45 //'O''S''N''E'
#define ROS_SENTENCE			0x534F5259 //'S''O''R''Y'

#define NGSA_SENTENCE             0x41534750 //'A''S''G''P'
#define NRMC_SENTENCE             0x434D524E //'C''M''R''N'
#define NGGA_SENTENCE             0x4147474E //'A''G''G''N'
#define NGSV_SENTENCE             0x56534750 //'V''S''G''P'
#define NGLL_SENTENCE             0x4C4C474E //'L''L''G''N'
#define NVTG_SENTENCE             0x4754564E //'G''T''V''N'
#define NSNR_SENTENCE             0x4F534E45 //'O''S''N''E'
#define NROS_SENTENCE             0x534F5259 //'S''O''R''Y'
#define NGSA_SENTENCE_BD             0x41534744 //'A''S''G''D'
#define NGSV_SENTENCE_BD             0x56534744 //'V''S''G''D'
#define TXT_SENTENCE             0x5458544E //'T''X''T''N'
#define ESF_SENTENCE             0x021062B5 //

//!for gyro
#define INSST_SENTENCE          0x5453534E
#define PDTIN_SENTENCE          0x4E495444

#define GPS_FLAG_DATAVALID      0x00000001
#define GPS_FLAG_LOG            0x00000002
#define GPS_FLAG_GETDATA        0x00000004

// miotone_2018727 zejia add
#define THE_FIRST_KEY_GPS "$GNRMC"
#define THE_LAST_KEY_GPS "$BDGSV"
#define ESF_MEAS_STR1 (0xB5)
#define ESF_MEAS_STR2 (0x62)
#define ESF_MEAS_STR3 (0x10)
#define ESF_MEAS_STR4 (0x02)
#define ESF_MEAS_LEN   (40)
#define ESF_MEAS_BUFFER_LEN   (5)
#define ESF_MEAS_DATA_LEN   (24)
#define ESF_MEAS_DATA_TYPE   (6)
#define ESF_MEAS_LEN_2BYTE   (2)//total len 0 ~ 39
#define ESF_MEAS_LEN_13BYTE   (13)
#define ESF_MEAS_LEN_14BYTE   (14)
#define ESF_MEAS_LEN_16BYTE   (16)
#define ESF_MEAS_LEN_38BYTE   (38)

#define ESF_GYROX_TYPE             0x0E
#define ESF_GYROY_TYPE             0x0D
#define ESF_GYROZ_TYPE             0x05
#define ESF_GSENSORX_TYPE       0x10
#define ESF_GSENSORY_TYPE       0x11
#define ESF_GSENSORZ_TYPE        0x12

#define ESF_24BITDATA_GYROX        3
#define ESF_24BITDATA_GYROY        7
#define ESF_24BITDATA_GYROZ        11
#define ESF_24BITDATA_GSENSORX        15
#define ESF_24BITDATA_GSENSORY        19
#define ESF_24BITDATA_GSENSORZ        23

#define ESF_LASTDATA_GYROX        0
#define ESF_LASTDATA_GYROY        1
#define ESF_LASTDATA_GYROZ        2
#define ESF_LASTDATA_GSENSORX        3
#define ESF_LASTDATA_GSENSORY        4
#define ESF_LASTDATA_GSENSORZ        5

#define GPS_TYPE_GNXXX        1
#define GPS_TYPE_ESF             2
//end
typedef struct {
	ER(*open)(void);
	ER(*close)(void);
	void(*init)(UART_BAUDRATE BaudRate, UART_LENGTH Length, UART_PARITY Parity);
	ER(*getString)(CHAR *pcString, UINT32 *pcBufferLen);
    UINT32(*getData)(UINT8 *pcString, UINT32 BufferLen);
} GPS_UART_OBJ, *pGPS_UART_OBJ;

typedef struct {
	CHAR    *pGPSDataStartAddr;
	UINT32  GPSBufUsedPos;
	UINT32  GPSBufSize;
} GPSBuf, *pGPSBuf;

typedef struct {

	CHAR    *GGA;
	CHAR    *RMC;
	CHAR    *GSA;
	CHAR    *GSV;
	CHAR    *GLL;
	CHAR    *VTG;
    CHAR    *INS;
    CHAR    *PDT;
	GPSBuf  GPSDataBuf;

} NMEASENTENCE;
extern NMEASENTENCE NMEASentence;

// miotone_2018801 zejia 
typedef struct {

	CHAR    *ESF;
	CHAR    *RES1;
	CHAR    *RES2;

} ESFSENTENCE;
extern ESFSENTENCE ESFSentence;

//end
typedef enum {
	No_Fix = 1,
	TWO_D_Fix,
	THREE_D_Fix
} FIXMODE;

//GSA, Satellite status
typedef struct {

	CHAR    SelMode;			//'A' auto selection of 2D or 3D fix
								//'M' manual
	UINT32  FixMode;			//'1' no fix
								//'2' 2D fix
								//'3' 3D fix
	UINT32  SatUsed01;			//PRNs of satellites used for fix (space for 12)
	UINT32  SatUsed02;
	UINT32  SatUsed03;
	UINT32  SatUsed04;
	UINT32  SatUsed05;
	UINT32  SatUsed06;
	UINT32  SatUsed07;
	UINT32  SatUsed08;
	UINT32  SatUsed09;
	UINT32  SatUsed10;
	UINT32  SatUsed11;
	UINT32  SatUsed12;
	double  PDOP;				//Position dilution of precision
	double  HDOP;				//Horizontal dilution of precision
	double  VDOP;				//Vertical dilution of precision

} GSAINFO;
extern GSAINFO GSAInfo;

//RMC, Recommended Minimum sentence C
#if (GPS_GNRMC == ENABLE)
typedef struct {
	UINT32    Hour;
	UINT32    Minute;
	UINT32    Second;
	UINT32    Year;
	UINT32    Month;
	UINT32    Day;
	CHAR      Status;			//Status A=active or V=Void
	float      Speed;
	float      Angle;
	UINT32      FusionMode;
	float     Roll;
	float     Pitch;
	float     Yaw;			//Speed over the ground in knots
} GNRMCINFO;
extern GNRMCINFO RMCInfo;

// miotone_2018925 zejia add for test
typedef struct {
	CHAR    Hour;
	CHAR    Minute;
	CHAR    Second;
	CHAR    Year;
	CHAR    Month;
	CHAR    Day;
	CHAR    Status;			//Status A=active or V=Void
} GNRMCINFOTEST;
extern GNRMCINFOTEST RMCInfoTest;

//end
typedef struct {
	UINT32    Hour;
	UINT32    Minute;
	UINT32    Second;
	UINT32    Year;
	UINT32    Month;
	UINT32    Day;
	CHAR      Status;			//Status A=active or V=Void
	CHAR      NSInd;
	CHAR      EWInd;
	CHAR      reservd;
	float     Latitude;
	float     Longitude;
	float     Speed;			//Speed over the ground in knots
	float     Angle;			//Track angle in degrees True
} RMCINFO;
extern RMCINFO RMCInfoStandard;

#else
typedef struct {
	UINT32    Hour;
	UINT32    Minute;
	UINT32    Second;
	UINT32    Year;
	UINT32    Month;
	UINT32    Day;
	CHAR      Status;			//Status A=active or V=Void
	CHAR      NSInd;
	CHAR      EWInd;
	CHAR      reservd;
	float     Latitude;
	float     Longitude;
	float     Speed;			//Speed over the ground in knots
	float     Angle;			//Track angle in degrees True
} RMCINFO;
extern RMCINFO RMCInfo;
#endif

//GGA, Global Positioning System Fix Data
typedef struct {

	UINT32    Hour;
	UINT32    Minute;
	UINT32    Second;
	double    Latitude;
	CHAR      NSInd;
	double    Longitude;
	CHAR      EWInd;
	UINT32    PosFixInd;		//Fix quality: 0 = invalid
								//             1 = GPS fix
	UINT32    SatNumber;		//Number of satellites being tracked
	double    HDOP;				//Horizontal dilution of position
	double    Altitude;			//Altitude, Meters, above mean sea level
	double    MSL;				//Height of geoid (mean sea level) above WGS84
	
    UINT32    GpsSatNum;         //Number of satellites being tracked for GPS
    UINT32    BdSatNum;         //Number of satellites being tracked for BD
} GGAINFO;
extern GGAINFO GGAInfo;

typedef struct {

	UINT32    SatNum;			//Satellite PRN number
	UINT32    Elev;				//Elevation, degrees
	UINT32    Azi;				//Azimuth, degrees
	UINT32    SNR;				//SNR - higher is better

} SATDATA;

//GSV, Satellites in view
typedef struct {

	UINT32    NumOfSen;			//Number of sentences for full data
	UINT32    SenNum;			//Sentence number
	UINT32    SatInView;		//Number of satellites in view
	SATDATA   SAT01;
	SATDATA   SAT02;
	SATDATA   SAT03;
	SATDATA   SAT04;
	SATDATA   SAT05;
	SATDATA   SAT06;
	SATDATA   SAT07;
	SATDATA   SAT08;
	SATDATA   SAT09;
	SATDATA   SAT10;
	SATDATA   SAT11;
	SATDATA   SAT12;
	SATDATA   SAT13;
	SATDATA   SAT14;
	SATDATA   SAT15;
	SATDATA   SAT16;

} GSVINFO;
extern GSVINFO GSVInfo;

// soling_20180801 zejia 
//EST, gsensor gyro data
typedef struct {

	double    GsensorX;			//gsensor x
	double    GsensorY;			//gsensor y
	double    GsensorZ;			//gsensor z
	double    GyroX;			//gyro x
	double    GyroY;			//gyro y
	double    GyroZ;			//gyro z
} ESFINFO;
extern ESFINFO ESFInfo;

typedef struct {
	UINT32    Hour;
	UINT32    Minute;
	UINT32    Second;
	UINT32    Year;
	UINT32    Month;
	UINT32    Day;
	UINT32    LocalTime;
	BOOL		LocalFlag;
} GPSLOCALINFO;
extern GPSLOCALINFO GPSLOCALInfo;

#if 1
typedef struct {
	double    gyro_x[ESF_MEAS_BUFFER_LEN];
	double    gyro_y[ESF_MEAS_BUFFER_LEN];
	double    gyro_z[ESF_MEAS_BUFFER_LEN];
	double    gsensor_x[ESF_MEAS_BUFFER_LEN];
	double    gsensor_y[ESF_MEAS_BUFFER_LEN];
	double    gsensor_z[ESF_MEAS_BUFFER_LEN];
	UINT32   valid_frames;
} ESFINFODATATEST;
extern ESFINFODATATEST ESFInfoDataTest;
#endif

//end
typedef enum {
	GSA,
	RMC,
	ESF,// soling_20180801 zejia add
	GGA,
	GSV,
	GLL,
    VTG,
    NRMC,
    INSSTAT,
    PDTINFO
} NMEATYPE;

//!gyro queue for driving behavior
typedef struct _ElemGyroData{
    int   valid;
    float speed;
	float acce_x;  //gsensor x
	float acce_y;  //gsensor y
	float acce_z;  //gsensor z
	float angu_x;  //gyro x
	float angu_y;  //gyro y
	float angu_z;  //gyro z
}ElemGyroData;

typedef struct {
	RTC_DATE  Date;
    RTC_TIME  Time;
	CHAR      Status;			//Status A=active or V=Void
	CHAR      NSInd;
	CHAR      EWInd;
	unsigned char gyro_ver:3;
    unsigned char fusion_mode:3;
    unsigned char resv:2;
	float     Latitude;
	float     Longitude;
	float     Speed;			//Speed over the ground in knots
	float     Angle;			//Track angle in degrees True
	float     Roll;
	float     Pitch;
	float     Yaw;			//Speed over the ground in knots
} RMCINFO_ext;

#if 0
#define DEV_MODEL_MP4_EXT       "G600V2"
#define DEV_MODEL_MP4           "G600"
#define GYRO_HARDWARE_VER_1ST   (0)
#define GYRO_HARDWARE_VER_AA    (1)
#define GYRO_HARDWARE_VER_BB    (2)

typedef struct _GyroRecordInfo_old
{
    char         szMode[16];
    RMCINFO_ext  stGPSInfo;
    ElemGyroData stElemData[5];
    char         szResv[40];
}GyroRecordInfo_old;

typedef struct _GyroRecordInfo
{
    char            szMode[16];
    RMCINFO_ext     stGPSInfo[5];
    ElemGyroData    stElemData[5];
}GyroRecordInfo;

//!real data process
extern DataQueue_t stDrivBhGryoQueue;
extern DataQueue_t stRlDataGryoQueue;
extern DataQueue_t stRecordGryoQueue;
extern DataQueue_t stRlDataGPSQueue;
extern DataQueue_t stRecordGPSQueue;

extern int Init_GyroDataQueue(void);
extern int DeInit_GyroDataQueue(void);
#endif

//!just for uart data process
#define UART_DATA_SIZE 2048
typedef struct _UartDataQueue
{
    char pBase[UART_DATA_SIZE];    //数组指针
    int  front;              //队头索引
    int  rear;               //队尾索引
    int  size;         //当前分配的最大容量
}UartDataQueue;

extern void UartDataQueue_Init(void);
extern int putUartData(char *pData, int iLen);
extern int getUartData(char *pData, int *pLen);
extern int getUartDataQueueLen(void);



extern void GPS_InstallID(void) _SECTION(".kercfg_text");
extern void GPS_SetUartObj(GPS_UART_OBJ *UartObj);
extern ER GPSRec_Open(UART_BAUDRATE BaudRate);
extern ER GPSRec_Close(void);
extern ER GPSRec_Reset(void);
extern BOOL GPSRec_CheckData(void);
#if (GPS_GNRMC == ENABLE)
extern BOOL GPSRec_GetRMCDate(GNRMCINFO *pRMCInfo);
extern void GPSRec_SetRMCDate(GNRMCINFO *pRMCInfo);
extern BOOL GPSRec_GetRMCDate_Read(GNRMCINFO *pRMCInfo);
#else
extern BOOL GPSRec_GetRMCDate(RMCINFO *pRMCInfo);
extern void GPSRec_SetRMCDate(RMCINFO *pRMCInfo);
#endif
extern BOOL GPSRec_GetGGADate(GGAINFO *pGGAInfo);
// miotone_2018802 zejia add
extern BOOL GPSRec_GetESF(ESFINFO *pESFInfo);
extern BOOL GPSRec_GetGSV(GSVINFO *pGSVInfo);
extern BOOL GPSRec_GetGPSLocalInfo(GPSLOCALINFO *pGPSLocalInfo);
extern BOOL GPSRec_SetGPSLocalInfo(GPSLOCALINFO pGPSLocalInfo);
extern BOOL GPSRec_GetESFData(ESFINFODATATEST *pGPSESFDataInfo);
extern void GPSRec_ClearESF(void);
extern void GPSRec_ClearGSV(void);
extern void GPSRec_ClearGGA(void);
extern void GPSRec_ClearGNRMC(void);
extern void GPSRec_ClearAllGPSData(void);
//end
extern FLOAT GPSRec_GetGPS_Speed(void);
extern BOOL GPSRec_GetRMCOriginalDate(char *RmcStr);// miotone_2018801 zejia add

//#NT#2015/10/02#KCHong -begin
//#NT#Add GPS heartbeat to detect GPS status
extern BOOL GPSRec_GetSpeed(FLOAT *Speed);
extern UINT32 GetGPSHeartBeat(void);
extern CHAR GPSRec_GetGpsStatus(void);
// miotone_2018726 zejia add,G600_CHANGE_DEBUG
extern void GPSRec_FrameParser(UINT32 nFrame);
extern void GPSRec_ParseAllFrame(void);
extern void SetGPSGNXXXPrint(BOOL enable);
extern BOOL GetGPSGNXXXPrint(void);
extern void SetESF_Print(BOOL enable);
extern BOOL GetESF_Print(void);
extern void SetGsensor_Sensitivity(double enable);
extern void SetGsensor_Sensitivity1(double enable);
extern double GetGsensor_Sensitivity(void);
extern double GetGsensor_Sensitivity1(void);
extern void NTPrintBuf(const char *szPrompt, CHAR *pData, UINT16 ulLen);
typedef int (*gps_nmea_set_handler)(UINT32 param);
extern UINT8 app_set_gps_cb(gps_nmea_set_handler gps_get_data);
extern void SetGPSNMEA_Print(BOOL enable);
extern BOOL GetGPSNMEA_Print(void);
//end
//#NT#2015/10/02#KCHong -end

extern UINT32 _SECTION(".kercfg_data") GPSRECEIVE_ID; // GPS Task
extern UINT32 _SECTION(".kercfg_data") GPS_PROCESS_ID; // GPS Task
extern UINT32 _SECTION(".kercfg_data") FLG_ID_GPS; // GPS Flag
#endif
