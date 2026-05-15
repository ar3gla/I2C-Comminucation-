/*
 * MPU6050.h
 *
 * Course number: CECS 447
 * Term: Spring 2026
 * Project number: Project 2
 * Project description:  *	Main implementation of functions to interact with
 *													the 6-dof MPU6050 Accelerometer and Gyroscope
 * Team #: 7
 * Team members: Adrian Gustilo, Alfredo Regla, Gerald Calero, Paris Talebi
 *
 */
 
 
#ifndef MPU6050_H_
#define MPU6050_H_

#include <stdint.h>



//NOTE: There will be no self-test regs

/*
IMPORTANT: BEFORE FILLING MACROS READ THE COMMENT BELOW

NOT ALL MACROS NEED TO BE FILLED OUT. REFERENCE TCS34727.c
TO FIGURE OUT WHAT'S NECESSARY (Filling it all out will not
incure any penelties but will be a waste of time)
*/

/* List of MPU6050 Register Macros */

//#define USE_HIGH

/**********************************************************/
#ifndef USE_HIGH
#define MPU6050_ADDR_AD0_LOW		(0x68)
#else
//Only use if AD0 is pulled high
#define MPU6050_ADDR_AD0_HIGH		()
#endif

/*************Sampling Rate Register*************/
#define SMPLRT_DIV							(0x19)
//	#define SMPLRT_DIV_1					()
//	#define SMPLRT_DIV_2					()
//	#define SMPLRT_DIV_3					()
//	#define SMPLRT_DIV_4					()
//	#define SMPLRT_DIV_5					()
//	#define SMPLRT_DIV_6					()
//	#define SMPLRT_DIV_7					()
	#define SMPLRT_DIV_8					(0x07)

/****************Config Register****************/
#define CONFIG									(0x1A)
	#define CONFIG_DFPL_0					(0x01)//(0x00)
//	#define CONFIG_DFPL_1					()
//	#define CONFIG_DFPL_2					()
//	#define CONFIG_DFPL_3					()
//	#define CONFIG_DFPL_4					()
//	#define CONFIG_DFPL_5					()
//	#define CONFIG_DFPL_6					()

/*************Gyro Config Register*************/
#define GYRO_CONFIG							(0x1B)
	#define GYRO_FS_SEL_0					(0x01)	//(0x00)
	#define GYRO_FS_SEL_1					(GYRO_FS_SEL_0 + 0x08)
	#define GYRO_FS_SEL_2					(GYRO_FS_SEL_0 + 0x10)
	#define GYRO_FS_SEL_3					(GYRO_FS_SEL_0 + 0x18)

/*************Accel Config Register************/
#define ACCEL_CONFIG						(0x1C)
	#define ACCEL_AFS_SEL_0				(0x01)			//(0x00)
	#define ACCEL_AFS_SEL_1				(ACCEL_AFS_SEL_0 + 0x08)
	#define ACCEL_AFS_SEL_2				(ACCEL_AFS_SEL_0 + 0x10)
	#define ACCEL_AFS_SEL_3				(ACCEL_AFS_SEL_0 + 0x18)
/**********************************************************/

#define MOT_THR             		(0x1F)
#define FIFO_EN             		(0x23)
#define I2C_MST_CTRL        		(0x24)
#define I2C_SLV0_ADDR       		(0x25)
#define I2C_SLV0_REG        		(0x26)
#define I2C_SLV0_CTRL       		(0x27)
#define I2C_SLV1_ADDR       		(0x28)
#define I2C_SLV1_REG        		(0x29)
#define I2C_SLV1_CTRL       		(0x2A)
#define I2C_SLV2_ADDR       		(0x2B)
#define I2C_SLV2_REG        		(0x2C)
#define I2C_SLV2_CTRL       		(0x2D)
#define I2C_SLV3_ADDR       		(0x2E)
#define I2C_SLV3_REG        		(0x2F)
#define I2C_SLV3_CTRL       		(0x30)
#define I2C_SLV4_ADDR       		(0x31)
#define I2C_SLV4_REG        		(0x32)
#define I2C_SLV4_DO         		(0x33)
#define I2C_SLV4_CTRL       		(0x34)
#define I2C_SLV4_DI         		(0x35)
#define I2C_MST_STATUS      		(0x36)
#define INT_PIN_CFG         		(0x37)
#define INT_ENABLE          		(0x38)
#define INT_STATUS          		(0x3A)

/**********************************************************/
#define ACCEL_XOUT_H        		(0x3B)
#define ACCEL_XOUT_L        		(0x3C)
#define ACCEL_YOUT_H        		(0x3D)
#define ACCEL_YOUT_L        		(0x3E)
#define ACCEL_ZOUT_H        		(0x3F)
#define ACCEL_ZOUT_L        		(0x40)
//#define TEMP_OUT_H          		(0x41)
//#define TEMP_OUT_L          		(0x42)
#define GYRO_XOUT_H         		(0x43)
#define GYRO_XOUT_L         		(0x44)
#define GYRO_YOUT_H         		(0x45)
#define GYRO_YOUT_L         		(0x46)
#define GYRO_ZOUT_H         		(0x47)
#define GYRO_ZOUT_L         		(0x48)
/**********************************************************/

#define EXT_SENS_DATA_00    		(0x49)
#define EXT_SENS_DATA_01    		(0x4A)
#define EXT_SENS_DATA_02    		(0x4B)
#define EXT_SENS_DATA_03    		(0x4C)
#define EXT_SENS_DATA_04    		(0x4D)
#define EXT_SENS_DATA_05    		(0x4E)
#define EXT_SENS_DATA_06    		(0x4F)
#define EXT_SENS_DATA_07    		(0x50)
#define EXT_SENS_DATA_08    		(0x51)
#define EXT_SENS_DATA_09    		(0x52)
#define EXT_SENS_DATA_10    		(0x53)
#define EXT_SENS_DATA_11    		(0x54)
#define EXT_SENS_DATA_12    		(0x55)
#define EXT_SENS_DATA_13    		(0x56)
#define EXT_SENS_DATA_14    		(0x57)
#define EXT_SENS_DATA_15    		(0x58)
#define EXT_SENS_DATA_16    		(0x59)
#define EXT_SENS_DATA_17    		(0x5A)
#define EXT_SENS_DATA_18    		(0x5B)
#define EXT_SENS_DATA_19    		(0x5C)
#define EXT_SENS_DATA_20    		(0x5D)
#define EXT_SENS_DATA_21    		(0x5E)
#define EXT_SENS_DATA_22    		(0x5F)
#define EXT_SENS_DATA_23    		(0x60)
#define I2C_SLV0_DO         		(0x63)
#define I2C_SLV1_DO         		(0x64)
#define I2C_SLV2_DO         		(0x65)
#define I2C_SLV3_DO         		(0x66)
#define I2C_MST_DELAY_CTRL  		(0x67)
#define SIGNAL_PATH_RESET   		(0x68)
#define MOT_DETECT_CTRL     		(0x69)
#define USER_CTRL           		(0x6A)

/**********Power Management & ID Register**********/
#define PWR_MGMT_1          		(0x6B)
	#define PWR_CLK_SEL_INTERNAL	(0x01)
//	#define PWR_CLK_SEL_PLL_X 		()
//	#define PWR_CLK_SEL_PLL_Y			()
//	#define PWR_CLK_SEL_PLL_Z			()
//	#define PWR_CLK_SEL_EXT_32		()
//	#define PWR_CLK_SEL_EXT_19		()
//	#define PWR_CLK_SEL_STOP			()
//	#define PWR_TEMP_DIS					()
//	#define PWR_CYCLE							()
//	#define PWR_SLEEP							()
	#define PWR_DEVICE_RESET			(0x80)
#define WHO_AM_I            		(0x75) //(0x7E)	
/**********************************************************/

#define PWR_MGMT_2          		(0x6C)
	#define PWR_2_STBY_ZG					(0x01)
	#define PWR_2_STBY_YG					(0x02)
	#define PWR_2_STBY_XG					(0x04)
	#define PWR_2_STBY_ZA					(0x08)
	#define PWR_2_STBY_YA					(0x10)
	#define PWR_2_STBY_XA					(0x20)
	#define PWR_2_WAKE_0					(0x00)
	#define PWR_2_WAKE_1					(0x40)
	#define PWR_2_WAKE_2					(0x80)
	#define PWR_2_WAKE_3					(0xC0)

#define FIFO_COUNTH         		(0x72)
#define FIFO_COUNTL         		(0x73)
#define FIFO_R_W            		(0x74)

#define RAD_TO_DEGREE_CONV			(180/3.1415)

/* Data Struct to store Accelerometer Data*/
typedef struct{
	int16_t Ax_RAW;
	int16_t Ay_RAW;
	int16_t Az_RAW;
	
	float Ax;
	float Ay;
	float Az;
	
} MPU6050_ACCEL_t;

/* Data Struct to store Gyroscope Data*/
typedef struct{
	int16_t Gx_RAW;
	int16_t Gy_RAW;
	int16_t Gz_RAW;
	
	float Gx;
	float Gy;
	float Gz;
	
} MPU6050_GYRO_t;

/* Data Struct to store Tilt Angle Data*/
typedef struct{
	float ArX;
	float ArY;
	float ArZ;
} MPU6050_ANGLE_t;

/*
 *	-------------------MPU6050_Init---------------------
 *	Basic Initialization Function for MPU6050 @ default settings
 *	Input: none
 * 	Output: none
 */
void MPU6050_Init(void);

/*
 *	-----------------MPU6050_Get_Accel------------------
 *	Receive Raw Accelerometer Data and store it in the user struct
 *	Input: MPU6050 Accel User Instance Struct
 * 	Output: none
 */
void MPU6050_Get_Accel(MPU6050_ACCEL_t* Accel_Instance);

/*
 *	-----------------MPU6050_Get_Gyro-------------------
 *	Receive Raw Gyroscope Data and store it in the user struct
 *	Input: MPU6050 Gyro User Instance Struct
 * 	Output: none
 */
void MPU6050_Get_Gyro(MPU6050_GYRO_t* Gyro_Instance);	

/*
 *	---------------MPU6050_Process_Accel----------------
 *	Process Raw Accelerometer Data into usable data and store
 *	it in the user stuct
 *	Input: MPU6050 Accel User Instance Struct
 * 	Output: none
 */
void MPU6050_Process_Accel(MPU6050_ACCEL_t* Accel_Instance);

/*
 *	---------------MPU6050_Process_Gyro----------------
 *	Process Raw Gyroscope Data into usable data and store it in
 *	the user struct
 *	Input: MPU6050 Gyro User Instance Struct
 * 	Output: none
 */
void MPU6050_Process_Gyro(MPU6050_GYRO_t* Gyro_Instance);

/*
 *	-----------------MPU6050_Get_Angle-----------------
 *	Calculate Tilt Angle using processed Accelerometer and
 *	Gyroscope data and it in the user angle struct
 *	Input: MPU6050 Angle User Instance Struct
 * 	Output: none
 */
void MPU6050_Get_Angle(MPU6050_ACCEL_t* Accel_Instance, MPU6050_GYRO_t* Gyro_Instance, MPU6050_ANGLE_t* Angle_Instance);

/* Used for Debugging Purposes */
uint8_t MPU6050_Read_Reg(uint8_t reg);

#endif
