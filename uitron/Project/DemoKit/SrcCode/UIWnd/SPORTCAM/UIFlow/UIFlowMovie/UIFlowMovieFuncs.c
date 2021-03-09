#include "Type.h"
//#include "ImageApp_Movie.h"
#include "UIFramework.h"
#include "UIFrameworkExt.h"
#include "UIFlow.h"
#include "UIFlowMovieIcons.h"
#include "DxOutput.h"
#include "ImageApp_MovieMulti.h"
#if( defined(_NVT_ETHREARCAM_RX_))
#include "EthCamAppSocket.h"
#include "EthCamAppNetwork.h"
#endif
#include "FileDB.h"

#define MOVIE_REC_TIME_MIN  10

#define MOVIE_REC_MIN_CLUSTER_SIZE      (0x8000)    //32KB
#define MOVIE_REC_SD_CLASS              (4)         // class 4

MOV_TASK_DATA   gMovData = { 0 };

static UINT32  g_MovRecMaxTime = 0;
static UINT32  g_MovRecCurrTime = 0;
static UINT32  g_MovCurrRecTotalTime = 0;

static UINT32  g_MovRecSelfTimerSec = 0;
static UINT32  g_MovRecSelfTimerID = NULL_TIMER;
UINT8 FlowMovie_GetMovDataState(void)
{
	return gMovData.State;
}

void FlowMovie_StartRec(void)
{
	if (g_MovRecSelfTimerSec == 0) {
		g_MovRecCurrTime = 0;
#if (USE_FILEDB == DISABLE)
		// update DCF Folder/File name
		FlowDCF_UpdateName();
#endif
		if (System_GetState(SYS_STATE_POWERON) == SYSTEM_POWERON_SAFE) {
			// wait playing sound stop
			GxSound_WaitStop();
		}
		Ux_SendEvent(&CustomMovieObjCtrl, NVTEVT_EXE_MOVIE_REC_START, NULL);

		//#NT#2012/10/23#Philex Lin - begin
		// disable auto power off/USB detect timer
		KeyScan_EnableMisc(FALSE);
		//#NT#2012/10/23#Philex Lin - end
	} else {
		debug_err(("FlowMovie_StartRec: Not yet,g_MovSelfTimerSec=%d\r\n", g_MovRecSelfTimerSec));
	}
		
	LED_TurnOnLED(GPIOMAP_LED_MOVIE);

}

void FlowMovie_StopRec(void)
{
#if (_ADAS_FUNC_ == ENABLE)
	// Fixed icon disappear issue when stop record during ADAS warning window
	UxCtrl_SetShow(&UIFlowWndMovie_Panel_Normal_DisplayCtrl, TRUE);
	UxCtrl_SetShow(&UIFlowWndMovie_ADAS_Alert_DisplayCtrl, FALSE);
#endif  // #if (_ADAS_FUNC_ == ENABLE)

	UxState_SetData(&UIFlowWndMovie_Status_RECCtrl, STATE_CURITEM, UIFlowWndMovie_Status_REC_ICON_REC_TRANSPAENT);
	Ux_SendEvent(&CustomMovieObjCtrl, NVTEVT_EXE_MOVIE_REC_STOP, NULL);
	FlowMovie_DrawLock(FALSE);
	//FlowWifiMovie_DrawLock(FALSE);
	//#NT#2012/10/23#Philex Lin - begin
	// enable auto power off/USB detect timer
	KeyScan_EnableMisc(TRUE);
	//#NT#2016/06/21#Niven Cho -begin
	//#NT#Remove danger code
	//#NT#2016/06/21#Niven Cho -end
	gMovData.State = MOV_ST_VIEW;
	//#NT#2012/10/23#Philex Lin - end
	
	UISound_Play(DEMOSOUND_SOUND_RECORD_STOP_TONE);
	LED_TurnOnLED(GPIOMAP_LED_MOVIE);

}

void FlowMovie_StopRecSelfTimer(void)
{
	if (g_MovRecSelfTimerID != NULL_TIMER) {
		GxTimer_StopTimer(&g_MovRecSelfTimerID);
		g_MovRecSelfTimerID = NULL_TIMER;
	}
	g_MovRecSelfTimerSec = 0;
}

UINT8 FlowMovie_GetDataState(void)
{
	return gMovData.State;
}

void FlowMovie_SetRecMaxTime(UINT32 RecMaxTime)
{
	g_MovRecMaxTime = RecMaxTime;
}

UINT32 FlowMovie_GetRecMaxTime(void)
{
	return g_MovRecMaxTime;
}

void FlowMovie_SetRecCurrTime(UINT32 RecCurrTime)
{
	g_MovRecCurrTime = RecCurrTime;
}

UINT32 FlowMovie_GetRecCurrTime(void)
{
	return g_MovRecCurrTime;
}

void FlowMovie_SetCurrRecTotalTime(BOOL bAdd)
{
    if(bAdd)	
	   g_MovCurrRecTotalTime ++;
	else
	   g_MovCurrRecTotalTime = 0;
}

UINT32 FlowMovie_GetCurrRecTotalTime(void)
{
	return g_MovCurrRecTotalTime;
}

BOOL FlowMovie_IsStorageErr(BOOL IsCheckFull)
{
#if (MOVIE_ENSURE_SD_32KCLUSTER == ENABLE)
	UINT32  uiClusterSize;
#endif
#if (MOVIE_ENSURE_SD_CLASS4 == ENABLE)
	PSDIO_MISC_INFORMATION pMiscInfo;
#endif

	// check card inserted
	if (System_GetState(SYS_STATE_CARD)  == CARD_REMOVED) {
		Ux_OpenWindow(&UIFlowWndWrnMsgCtrl, 2, UIFlowWndWrnMsg_StatusTXT_Msg_STRID_PLEASE_INSERT_SD, FLOWWRNMSG_TIMER_2SEC);
		return TRUE;
	}

	// check storage error
	if (UIStorageCheck(STORAGE_CHECK_ERROR, NULL) == TRUE) {
		UISound_Play(DEMOSOUND_SOUND_SD_ERR_TONE);
		//Ux_OpenWindow(&UIFlowWndWrnMsgCtrl, 2, FLOWWRNMSG_ISSUE_MEM_ERR, FLOWWRNMSG_TIMER_KEEP);
		return TRUE;
	}


	// check storage lock or directory read only
	if (UIStorageCheck(STORAGE_CHECK_LOCKED, NULL) == TRUE ||
		UIStorageCheck(STORAGE_CHECK_DCIM_READONLY, NULL) == TRUE) {
		UISound_Play(DEMOSOUND_SOUND_SD_ERR_TONE);
		//Ux_OpenWindow(&UIFlowWndWrnMsgCtrl, 2, UIFlowWndWrnMsg_StatusTXT_Msg_STRID_CARD_LOCKED, FLOWWRNMSG_TIMER_2SEC);
		return TRUE;
	}

	// check storage full
	if (TRUE == IsCheckFull) {
		g_MovRecMaxTime = MovieExe_GetMaxRecSec();
		if (g_MovRecMaxTime <= MOVIE_REC_TIME_MIN) {
			UISound_Play(DEMOSOUND_SOUND_SD_ERR_TONE);
			g_MovRecMaxTime = 0;
			//Ux_OpenWindow(&UIFlowWndWrnMsgCtrl, 2, UIFlowWndWrnMsg_StatusTXT_Msg_STRID_CARD_FULL, FLOWWRNMSG_TIMER_2SEC);
			return TRUE;
		}
	}

	// check folder full
	if (UIStorageCheck(STORAGE_CHECK_FOLDER_FULL, NULL) == TRUE) {
		UISound_Play(DEMOSOUND_SOUND_SD_ERR_TONE);
		//Ux_OpenWindow(&UIFlowWndWrnMsgCtrl, 2, UIFlowWndWrnMsg_StatusTXT_Msg_STRID_CARD_FULL, FLOWWRNMSG_TIMER_2SEC);
		return TRUE;
	}

	// check serial number full
	if (MovieExe_CheckSNFull()) {
		UISound_Play(DEMOSOUND_SOUND_SD_ERR_TONE);
		//Ux_OpenWindow(&UIFlowWndWrnMsgCtrl, 2, UIFlowWndWrnMsg_StatusTXT_Msg_STRID_CARD_FULL, FLOWWRNMSG_TIMER_2SEC);
		return TRUE;
	}

#if (MOVIE_ENSURE_SD_CLASS4 == ENABLE)
	// check sd card whether faster than class 4
	pMiscInfo = sdio_getMiscInfo();
	if (pMiscInfo->uiWriteRate < MOVIE_REC_SD_CLASS) {
		UISound_Play(DEMOSOUND_SOUND_SD_ERR_TONE);
		//Ux_OpenWindow(&UIFlowWndWrnMsgCtrl, 2, UIFlowWndWrnMsg_StatusTXT_Msg_STRID_SD_CLASS4, FLOWWRNMSG_TIMER_2SEC);
		return TRUE;
	}
#endif

#if (MOVIE_ENSURE_SD_32KCLUSTER == ENABLE)
	// Check cluster size, need to be larger than 32KB
	uiClusterSize = FileSys_GetDiskInfo(FST_INFO_CLUSTER_SIZE);
	if (uiClusterSize < MOVIE_REC_MIN_CLUSTER_SIZE) {
		UISound_Play(DEMOSOUND_SOUND_SD_ERR_TONE);
		//Ux_OpenWindow(&UIFlowWndWrnMsgCtrl, 2, UIFlowWndWrnMsg_StatusTXT_Msg_STRID_CLUSTER_WRONG, FLOWWRNMSG_TIMER_2SEC);
		return TRUE;
	}
#endif

	return FALSE;
}

UINT32 FlowMovie_GetSelfTimerID(void)
{
	return g_MovRecSelfTimerID;
}

#if 0
BOOL FlowMovie_ChkDrawStoreFullFolderFull(void)
{

	if (UIStorageCheck(STORAGE_CHECK_FULL, &(g_MovRecMaxTime)) == TRUE) {
		g_MovRecMaxTime = UIMovRecObj_GetData(RECMOVIE_MAXSECOND);
		if (System_GetState(SYS_STATE_CARD)  == CARD_REMOVED) {
			Ux_OpenWindow(&UIFlowWndWrnMsgCtrl, 2, FLOWWRNMSG_ISSUE_MEM_FULL, FLOWWRNMSG_TIMER_2SEC);
			return TRUE;
		} else {
			Ux_OpenWindow(&UIFlowWndWrnMsgCtrl, 2, FLOWWRNMSG_ISSUE_CARD_FULL, FLOWWRNMSG_TIMER_2SEC);
			return TRUE;
		}
	}

	return FALSE;
}
#endif

void FlowMovie_GetPictureInMovie(void)
{
#if (0)//(_MOVIE_PIM_MODE_ == ENABLE)
	UINT32  sec, unsafe;

	switch (gMovData.State) {
	case MOV_ST_REC:
		sec = MovRec_Disk2Second();
		unsafe = MovRec_CheckUnsavedFileBlocks();

		if ((sec > 3) && (unsafe == 0)) {
			RawEnc_SetCaptureOne();
			//UxCtrl_SetShow(&UIFlowWndMovie_Static_pimCtrl, TRUE);
		} else {
			if (unsafe) {
				debug_err(("unsafe to take picture !\r\n"));
			} else {
				debug_err(("no space to encode RAW !\r\n"));
			}
		}
		break;
	}
#endif
}

void FlowMovie_OnTimer1SecIndex(void)
{

	switch (gMovData.State) {
	case MOV_ST_VIEW:
	case MOV_ST_VIEW | MOV_ST_ZOOM:
	case MOV_ST_REC:
	case MOV_ST_REC | MOV_ST_ZOOM:
		gMovData.SysTimeCount++;
		if (UxCtrl_IsShow(&UIFlowWndMovie_YMD_StaticCtrl)) {
			FlowMovie_IconDrawDateTime();
		}
		FlowMovie_IconDrawWifi(&UIFlowWndMovie_Status_WIFICtrl);
		FlowMovie_DrawSSIDPassWord();
		FlowMovie_DrawCardFreeCapacity();
		FlowMovie_IconDrawMicroPhone(&UIFlowWndMovie_Status_MicroPhoneCtrl);
		break;

	}
}

extern BOOL g_bRecordLock;
void FlowMovie_SetCrash(void)
{CHKPNT;
	UINT32 i, mask, movie_rec_mask;

	movie_rec_mask = Movie_GetMovieRecMask();
	mask = 1;
    UISound_Play(DEMOSOUND_SOUND_RECORD_LOCK_TONE);
	FlowMovie_DrawLock(TRUE);
	g_bRecordLock = TRUE;
	
	#if(defined(_NVT_ETHREARCAM_RX_))
		CHKPNT;
		ImageApp_MovieMulti_SetCrash(_CFG_REC_ID_1, TRUE);
		for (i = 0; i < ETH_REARCAM_CAPS_COUNT; i++) {
			if(socketCliEthData1_IsRecv(ETHCAM_PATH_ID_1 +i)){
				ImageApp_MovieMulti_SetCrash(_CFG_ETHCAM_ID_1+i, TRUE);
			}
		}
		//UISound_Play(DEMOSOUND_SOUND_RECORD_LOCK_TONE);
        FlowMovie_DrawLock(TRUE);
		g_bRecordLock = TRUE;
	#else
		for (i = 0; i < SENSOR_CAPS_COUNT; i++) {
			if (movie_rec_mask & mask) {
				ImageApp_MovieMulti_SetCrash(_CFG_REC_ID_1 + i, TRUE);
			}
			mask <<= 1;
		}
	#endif
}

#if (USE_FILEDB== ENABLE)
/*
static FILEDB_INIT_OBJ gMovieFuncsInitObj={
                         "A:\\",  //rootPath
                         NULL,  //defaultfolder
                         NULL,  //filterfolder
                         TRUE,  //bIsRecursive
                         TRUE,  //bIsCyclic
                         TRUE,  //bIsMoveToLastFile
                         TRUE, //bIsSupportLongName
                         FALSE, //bIsDCFFileOnly
                         TRUE,  //bIsFilterOutSameDCFNumFolder
                         {'N','V','T','I','M'}, //OurDCFDirName[5]
                         {'I','M','A','G'}, //OurDCFFileName[4]
                         FALSE,  //bIsFilterOutSameDCFNumFile
                         FALSE, //bIsChkHasFile
                         60,    //u32MaxFilePathLen
                         10000,  //u32MaxFileNum
                         (FILEDB_FMT_MOV|FILEDB_FMT_MP4),
                         0,     //u32MemAddr
                         0,     //u32MemSize
                         NULL   //fpChkAbort

};*/
void FlowMovie_DeleteFile(UINT32 type)
{
    //UINT32 filenum=0,sec=0;
    UINT32 index = 0;
    UINT32 TotalFileNum;
    //UINT32 i = 0;
    PFILEDB_FILE_ATTR  pfile; 
	UINT32 RET_ER;
    //static FILEDB_HANDLE	 FileDBHandle =0;
    //PFILEDB_INIT_OBJ		 pFDBInitObj = &gMovieFuncsInitObj;
	FileSys_WaitFinish();
	FileDB_Refresh(0);
    FileDB_SortBy(0,FILEDB_SORT_BY_MODDATE,FALSE);
    TotalFileNum = FileDB_GetTotalFileNum(0);
	
	debug_msg("Liwk --- Total File %d\r\n",TotalFileNum);
	if(TotalFileNum == 0)
		return;
	
	switch(type)
	{
	   case DELETE_LAST_UNLOCKED_ONE:
            while (index < TotalFileNum)
            {
               pfile = FileDB_SearhFile2(0,index);
               if(!M_IsReadOnly(pfile->attrib))
               {
                  FileSys_WaitFinish();
			      RET_ER = FileSys_DeleteFile(pfile->filePath);
				  debug_msg("Liwk ------- Delete File %s  ER:%d\r\n",pfile->filePath,RET_ER);
				  if(RET_ER == FST_STA_OK)
				  {
				     break;
				  }
               }
               index++;
            }
			break;
	    case DELETE_ALL_BUT_LOCKED:
			while (index < TotalFileNum)
            {
               pfile = FileDB_SearhFile2(0,index);
               if(!M_IsReadOnly(pfile->attrib))
               {
                  FileSys_WaitFinish();
			      RET_ER = FileSys_DeleteFile(pfile->filePath);
				  debug_msg("Liwk ------- Delete File %s  ER:%d\r\n",pfile->filePath,RET_ER);
				  if(RET_ER != FST_STA_OK)
				  {
				     break;
				  }
               }
               index++;
            }
			break;
		case DELETE_ALL:
			while (index < TotalFileNum)
            {
               pfile = FileDB_SearhFile2(0,index);
               if(M_IsReadOnly(pfile->attrib))
               {
                  FileSys_WaitFinish();
				  FileSys_SetAttrib(pfile->filePath,FS_ATTRIB_READ,FALSE);				  
                  FileSys_WaitFinish();
			      RET_ER = FileSys_DeleteFile(pfile->filePath);
				  debug_msg("Liwk ------- Delete File %s  ER:%d\r\n",pfile->filePath, RET_ER);
               }
			   else
			   {
                  FileSys_WaitFinish();
                  RET_ER = FileSys_DeleteFile(pfile->filePath);
                  debug_msg("Liwk ------- Delete File %s  ER:%d\r\n",pfile->filePath, RET_ER);
			   }
               index++;
            }
			break;
		default:
			break;
	}
}
#endif

