#ifndef _COARSE_ATTI_H_
#define _COARSE_ATTI_H_


//!Config:5HZ
typedef struct _AttCtrlParam{
	double attDT;   		//积分integral,0.2,200ms
	double alpha;			//alpha：增益控制（用于性能调节）0.02
	double gbias[3];		//陀螺零偏:0
	double abias[3];		//加表零偏:0
	double MisABias[3];		//安装角误差，需存储弧度表示:degree * M_PI / 180;
}AttCtrlParam;
typedef struct _CoarseAttParam{
	double raw_acce[3];		//200ms的平均加速度和,原始数据
	double raw_angu[3]; 	//200ms角速度,原始数据
	double acce[3];			//200ms的平均加速度和,
	double angu[3]; 		//200ms角速度
	//!用于标记是否进入组合导行
	double RMCATT[3];//从NMEA获取的姿态,当前获取的姿态值，用于更正。
	int    RMCState;		//0,未进入组合导航.3,进入组合导航
	int    tick;			//0,整秒.1,非整秒
}CoarseAttiParam;


extern void CoarseAttInit(AttCtrlParam *pCtrlParam);
extern int Process_Gyro_Atti(AttCtrlParam *pCtrlParam, CoarseAttiParam *pAttiParam, float *pOutAtti);
extern int Simply_Process_Gyro_Atti(float gx, float gy, float gz, float ax, float ay, float az, float outAtt[3]);


#endif
