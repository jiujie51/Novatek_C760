#include <math.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "CoarseAtti.h"



#define __MODULE__ MDALG
#define __DBGLVL__ 1        //ERROR mode, show err, wrn only
#define __DBGFLT__ "*"      //*=All
#include "DebugModule.h"
/*******************************************/
//    示例，根据实际情况来实现并调整参数
/*******************************************/

/**** Config:5HZ ****/
//double attdT = 0.2;
//double alpha = 0.02;//alpha：增益控制（用于性能调节）

/**** Input ****/
//double lastATT[3];//上次的姿态
//double RMCATT[3];//从NMEA获取的姿态,当前获取的姿态值，用于更正。
/**** Output ****/
//double nextATT[3];//待计算姿态
//static double outATT[3];  //用于输出的姿态

//double acc[3];//200ms的平均加速度和角速度
//double gyro[3];
//unsigned char state = 0;//当前状态 是否进入组合导航
/**** Input  ****/
//double rawdataAcc[3];
//double rawdataGyro[3];
//unsigned char RMCState;//NMEA获得组合导航状态


extern void CoarseAttInit(AttCtrlParam *pCtrlParam);
extern int CoarseAttProc(AttCtrlParam *pCtrlParam, CoarseAttiParam *pAttiParam, float *pMidAtti, double lastAtt[3]);
extern void PreProcess_InPut(AttCtrlParam *pCtrlParam, CoarseAttiParam *pAttiParam, double lastAtt[3]) ;
extern void PostPro_Output(AttCtrlParam *pCtrlParam, CoarseAttiParam *pAttiParam, float *pMidAtti, double *pOutAtti); //输出结果
extern int Process_Gyro_Atti(AttCtrlParam *pCtrlParam, CoarseAttiParam *pAttiParam, float *pOutAtti);

void CoarseAttInit(AttCtrlParam *pCtrlParam)
{
	int i = 0;
	pCtrlParam->attDT = 0.2;
	pCtrlParam->alpha = 0.02;
	for(i=0; i<3; i++)
	{
		pCtrlParam->gbias[i] = 0;
		pCtrlParam->abias[i] = 0;
	}
	pCtrlParam->MisABias[0] = 0;
	pCtrlParam->MisABias[1] = -45*M_PI/180;//
	pCtrlParam->MisABias[2] = 0;
}


/*****  注意：使用的角度均为弧度   ******/
/******* CoarseAtt ********
accIn: 加速度输入，单位m/s2
gyroIn：角速度输入,单位rad/s
state: 惯导状态,0或者3
pattitude：姿态输出指针
lastAtt：上次的姿态
dt：时间差
alpha：增益控制（用于性能调节）
****************************/
int CoarseAttProc(AttCtrlParam *pCtrlParam, CoarseAttiParam *pAttiParam, float *pMidAtti, double lastAtt[3])
{
    double normG;
    double temp_att[2];
    double acc[3];
    double gyro[3];
	double alpha = 0.0;
	int cnt = 0, i= 0;
	for(i = 0; i<3; i++){
		if(pAttiParam->acce[i] != 0.0f){
			cnt++;
		}
		if(pAttiParam->angu[i] != 0.0f){
			cnt++;
		}
	}
	if(cnt != 6){
		debug_msg("_________<%s>:%d,cnt:%d___________\\n", __func__, __LINE__, cnt);
		return -1;
	}
	
    alpha = pCtrlParam->alpha * pCtrlParam->attDT;

    acc[0] = -0.7071*(pAttiParam->acce[1]+pAttiParam->acce[2]);
    acc[1] = pAttiParam->acce[0];
    acc[2] = -0.7071*(pAttiParam->acce[1]-pAttiParam->acce[2]);


    gyro[0] = -0.7071*(pAttiParam->angu[1]+pAttiParam->angu[2]);
    gyro[1] = pAttiParam->angu[0];
    gyro[2] = -0.7071*(pAttiParam->angu[1]-pAttiParam->angu[2]);

    normG = sqrt(acc[0]*acc[0]+acc[1]*acc[1]+acc[2]*acc[2]);
    if(normG == 0.0f){
        return -1;
    }
    temp_att[0] =  atan2(-acc[1],-acc[2]);
    temp_att[1] =  asin(acc[0]/ normG);

	//debug_msg("%f:%f\r\n", temp_att[0]*180/M_PI, temp_att[1]*180/M_PI);
    pMidAtti[0] = temp_att[0]*180/M_PI;
    pMidAtti[1] = temp_att[1]*180/M_PI;
    pMidAtti[2] = 0;
    return 1;

    pMidAtti[0] = lastAtt[0]+gyro[0] * pCtrlParam->attDT;
    pMidAtti[1] = lastAtt[1]+gyro[1] * pCtrlParam->attDT;

    //printf("%f:%f\r\n", pMidAtti[0], pMidAtti[1]);
    pMidAtti[0] = pMidAtti[0]*(1-alpha)+ alpha*temp_att[0] ;
    pMidAtti[1] = pMidAtti[1]*(1-alpha)+ alpha*temp_att[1] ;

    if(pAttiParam->RMCState)
    {
        pMidAtti[2] =  lastAtt[2]+gyro[2] * pCtrlParam->attDT;
    }
    else
    {
        pMidAtti[2] = 0;
    }
    return 1;
}


void PreProcess_InPut(AttCtrlParam *pCtrlParam, CoarseAttiParam *pAttiParam, double lastAtt[3]) 
{
    //state = RMCState;   //输入组合导航状态
    int i = 0;
    for(i=0; i<3; i++)
    {
        pAttiParam->acce[i]= pAttiParam->raw_acce[i] - pCtrlParam->abias[i];  //输入200ms的平均加速度和角速度,去除零偏
        pAttiParam->angu[i]= pAttiParam->raw_angu[i] - pCtrlParam->gbias[i];        
    }

    if(pAttiParam->RMCState && (pAttiParam->tick == 0))  //整秒且为组合导航时，输入组合导航姿态
    {
        lastAtt[2] = pAttiParam->RMCATT[2];
    }
    
    return;
}




/*****  输出的时候需要补偿安装角 MisABias   ******
*
* 当安装角不补偿的时候输出角度有固定的安装误差，
* 方法1：可通过用户按键获取,并存储 MisABias
* 方法2：也可根据组合导航输出的姿态来计算
*************************************************/

/*****  Output ***/


void PostPro_Output(AttCtrlParam *pCtrlParam, CoarseAttiParam *pAttiParam, float *pMidAtti, double *pOutAtti) //输出结果
{    
    if(pAttiParam->RMCState && (pAttiParam->tick == 0))//进入组合导航之后，整秒计算安装角误差
    {
        pCtrlParam->MisABias[0]= 0.95*pCtrlParam->MisABias[0] +(pAttiParam->RMCATT[0]- pMidAtti[0])*0.05;
        pCtrlParam->MisABias[1]= 0.95*pCtrlParam->MisABias[1] +(pAttiParam->RMCATT[1]- pMidAtti[1])*0.05;        
    }
    pOutAtti[0] = pMidAtti[0]+ pCtrlParam->MisABias[0];
    pOutAtti[1] = pMidAtti[1]+ pCtrlParam->MisABias[1];
    pOutAtti[2] = pMidAtti[2];
    return;
}

int Process_Gyro_Atti(AttCtrlParam *pCtrlParam, CoarseAttiParam *pAttiParam, float *pOutAtti)
{
	int rval = 1;
	static double lastATT[3] = {0};//上次的姿态
	//static double nextATT[3] = {0};//待计算姿态
	
	PreProcess_InPut(pCtrlParam, pAttiParam, lastATT);
	//rval = CoarseAttProc(pCtrlParam, pAttiParam, nextATT, lastATT);
    rval = CoarseAttProc(pCtrlParam, pAttiParam, pOutAtti, lastATT);
	if(rval == -1){
		return rval;
	}
	//PostPro_Output(pCtrlParam, pAttiParam, nextATT, pOutAtti);
	//lastATT[0] = nextATT[0]; lastATT[1] = nextATT[1]; lastATT[2] = nextATT[2];
	return rval;
}

int Simply_Process_Gyro_Atti(float gx, float gy, float gz, float ax, float ay, float az, float outAtt[3])
{
    float normG;
    float temp_att[2];
    float acc[3];
    float gyro[3];

    acc[0] = -0.7071*(gy+gz);
    acc[1] = gx;
    acc[2] = -0.7071*(gy-gz);


    gyro[0] = -0.7071*(ay+az);
    gyro[1] = ax;
    gyro[2] = -0.7071*(ay-az);

    normG = sqrt(acc[0]*acc[0]+acc[1]*acc[1]+acc[2]*acc[2]);
    if(normG == 0.0f){
        return -1;
    }
    temp_att[0] =  atan2(-acc[1],-acc[2]);
    temp_att[1] =  asin(acc[0]/ normG);

	//debug_msg("%f:%f\r\n", temp_att[0]*180/M_PI, temp_att[1]*180/M_PI);
    outAtt[0] = temp_att[0]*180/M_PI;
    outAtt[1] = temp_att[1]*180/M_PI;
    outAtt[2] = 0;
    return 0;
}

#if 0
typedef struct _gryo_unit{
	double xacc;
	double yacc;
	double zacc;
	double xangu;
	double yangu;
	double zangu;
	int rval;
}gryo_unit;

extern gryo_unit test_gryo_unit[];
extern int count;
void main()
{  
    AttCtrlParam stAttCtrlPrm;
	CoarseAttiParam stCoarseAttPrm;
	int rval = 0;
	CoarseAttInit(&stAttCtrlPrm);
    for(int tick=0;tick<count;tick++) //5HZ
    {
		stCoarseAttPrm.raw_acce[0] = test_gryo_unit[tick].xacc;
		stCoarseAttPrm.raw_acce[1] = test_gryo_unit[tick].yacc;
		stCoarseAttPrm.raw_acce[2] = test_gryo_unit[tick].zacc;
		stCoarseAttPrm.raw_angu[0] = test_gryo_unit[tick].xangu;
		stCoarseAttPrm.raw_angu[1] = test_gryo_unit[tick].yangu;
		stCoarseAttPrm.raw_angu[2] = test_gryo_unit[tick].zangu;
		stCoarseAttPrm.RMCState = 0;		//0,未进入组合导航.3,进入组合导航
		stCoarseAttPrm.tick = 0;
		#if 0
		for(int i=0; i<3; i++){
			printf("%f:%f:", stCoarseAttPrm.raw_acce[i], stCoarseAttPrm.raw_angu[i]);
		}
		printf("\n");
		#endif
        rval = Process_Gyro_Atti(&stAttCtrlPrm, &stCoarseAttPrm, outATT);//输出结果，计算安装角误差，补偿安装角误差
		if(rval == -1){
			//printf("ERR:%f\t%f\t%f\n", outATT[0], outATT[1], outATT[2]);
		}
		else{
			//printf("%f\t%f\t%f\n", outATT[0], outATT[1], outATT[2]);
		}
		usleep(10000);
    }
}
#endif
