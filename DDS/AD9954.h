#ifndef __AD9954_H
#define __AD9954_H	 
#include "sys.h"
/*
** 函数名称 ：void GPIO_AD9954_Init(void)
** 函数功能 ：AD9954接口IO初始化
**            AD9954_CS----------PF13	 		OUT
**	      		AD9954_SCLK--------PF12	 		OUT
**            AD9954_SDIO--------PF11 	  OUT
**            AD9954_OSK---------PB2  	 	OUT
**          	PS0----------------PB1			OUT
**	      		PS1----------------PB0	 		OUT
**            IOUPDATE-----------PB15	  	OUT

**	      		AD9954_SDO---------PF14	  	IN
**            AD9954_IOSY--------PF15	  	OUT
**            AD9954_RES---------PG0    	OUT
**            AD9954_PWR---------PG1    	OUT
*/



#define AD9954_PWR 	PGout(1)// PE5
#define AD9954_RES 	PGout(0)// PE5	
#define AD9954_IOSY PFout(15)// PE5	
#define AD9954_SDO	PFin(14)
#define AD9954_CS 	PFout(13)// PE5	
#define AD9954_SCLK PFout(12)// PE5	
#define AD9954_SDIO PFout(11)// PE5	
#define AD9954_OSK 	PBout(2)// PE5	
#define PS0 				PBout(1)// PB5
#define PS1 				PBout(0)// PE5	
#define IOUPDATE 		PCout(5)// PE5	

#define CFR1		0X00			
#define CFR2    0X01
#define ASF     0X02
#define ARR     0X03
#define FTW0    0X04
#define POW0    0X05
#define FTW1    0X06
#define NLSCW   0X07
#define PLSCW   0X08
#define RSCW0   0X07
#define RSCW1   0X08
#define RSCW2   0X09
#define RSCW3   0X0A
#define RAM     0X0B

#define No_Dwell 0X80

typedef enum {
DownScan  =   0,
UpScan,
DoubleScan
}ScanMode;

void GPIO_AD9954_Init(void);
void AD9954_Init(void);
void AD9954_SETFRE(double f);//single tone
void Single_Tone(double Freq);
void Linear_Sweep(
    double  MinFreq, double MaxFreq, 
    double  UpStepFreq, u8 UpStepTime,
    double  DownStepFreq, u8 DownStepTime,
    ScanMode      Mode
);
void PS0_WriteBit(BitAction BitVal);
void SetASF(u16 data);
void DirectSwitchFSK(double f1, double f2, double f3, double f4);
void DirectSwitchPSK(float f, float phase1, float phase2, float phase3, float phase4);
void AM_Init(void);
void Write_ASF(u16 factor);  //2ASK
u32 Read_Vau1(void);
u32 Read_Vau2(void);
void LINEARSWEEP(float f1,float f2);//linear sweep mode
void UPDATE_SYNC(void);
#endif

