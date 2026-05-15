/*
 * ModuleTest.c 
 *
 * Course number: CECS 447
 * Term: Spring 2026
 * Project number: Project 2
 * Project description: Testing each module using simple test cases.
 * Team #: 7
 * Team members: Adrian Gustilo, Alfredo Regla, Gerald Calero, Paris Talebi
 *
 *
*/

#include "tm4c123gh6pm.h"
#include "UART0.h"
#include "Servo.h"
#include "util.h"
#include "ButtonLED.h"
#include "I2C.h"
#include "MPU6050.h"
#include "TCS34727.h"
#include "LCD.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

//#define TEST_TIMER
//#define TEST_UART0
//#define TEST_SERVO
//#define TEST_GPIO
//#define TEST_MPU6050
#define TEST_TCS34727
//#define TEST_LCD

#ifdef TEST_TIMER
void Test_Timer(void)
{
    GPIO_PORTF_DATA_R ^= 0x02;
    DELAY_1MS(500);
}
#endif

#ifdef TEST_UART0
void Test_UART0(void){
    UART0_OutString("\r\nUART0 Test 1: if you see this string, PASS");
    UART0_OutString("\r\nUART0 Test 2: type anything and press enter, it will be echoed back");
    char buf[32];
    UART0_InString(buf, 31);
    UART0_OutString("\r\nYou typed: ");
    UART0_OutString(buf);
    UART0_OutString("\r\nUART0 Test 2: PASS");
    UART0_OutString("\r\nUART0 Test 3 (edge): type exactly 31 characters then press enter");
    UART0_InString(buf, 31);
    UART0_OutString("\r\nReceived: ");
    UART0_OutString(buf);
    UART0_OutString("\r\nUART0 Test 3: PASS - no overflow");
}
#endif

#ifdef TEST_SERVO
void Test_Servo(void){
    Drive_Servo(0);   DELAY_1MS(1000);
    Drive_Servo(-45); DELAY_1MS(1000);
    Drive_Servo(0);   DELAY_1MS(1000);
    Drive_Servo(+45); DELAY_1MS(1000);
    Drive_Servo(0);   DELAY_1MS(1000);
    Drive_Servo(-90); DELAY_1MS(1000);
    Drive_Servo(0);   DELAY_1MS(1000);
    Drive_Servo(+90); DELAY_1MS(1000);
    Drive_Servo(0);   DELAY_1MS(1000);
}
#endif

#ifdef TEST_GPIO
void Test_GPIO(void){
    if (!(GPIO_PORTF_DATA_R & SW1_PIN)){
        LEDs = (LEDs & ~0x0E) | RED;
    } else if (!(GPIO_PORTF_DATA_R & SW2_PIN)){
        LEDs = (LEDs & ~0x0E) | BLUE;
    }
}
#endif

#ifdef TEST_MPU6050
void Test_MPU6050(void){
    MPU6050_ACCEL_t myAccel;
    MPU6050_GYRO_t  myGyro;
    MPU6050_ANGLE_t myAngle;

    MPU6050_Get_Accel(&myAccel);
    MPU6050_Get_Gyro(&myGyro);
    MPU6050_Process_Accel(&myAccel);
    MPU6050_Process_Gyro(&myGyro);
    MPU6050_Get_Angle(&myAccel, &myGyro, &myAngle);

    char buf[64];
    sprintf(buf, "X: %3d Y: %3d Z: %3d\r\n", (int)myAngle.ArX, (int)myAngle.ArY, (int)myAngle.ArZ);
    UART0_OutString(buf);
    DELAY_1MS(1000);
}
#endif

#ifdef TEST_TCS34727
void Test_TCS34727(void){
    RGB_COLOR_HANDLE_t myColor;
    myColor.C_RAW = TCS34727_GET_RAW_CLEAR();
    myColor.R_RAW = TCS34727_GET_RAW_RED();
    myColor.G_RAW = TCS34727_GET_RAW_GREEN();
    myColor.B_RAW = TCS34727_GET_RAW_BLUE();
    TCS34727_GET_RGB(&myColor);

    char buf[64];
    sprintf(buf, "R:%3d G:%3d B:%3d\r\n", (int)myColor.R, (int)myColor.G, (int)myColor.B);
    UART0_OutString(buf);
    DELAY_1MS(1000);
}
#endif

#ifdef TEST_LCD
void Test_LCD(void)
{
    LCD_Set_Cursor(ROW1, 0);
    LCD_Print_Str((uint8_t*)"alfredo");
    LCD_Set_Cursor(ROW2, 0);
    LCD_Print_Str((uint8_t*)"regla");
    
    while(1) {} 
}
#endif

void System_Init(void){
    UART0_Init();

    #if defined(TEST_TIMER) || defined(TEST_SERVO) || defined(TEST_MPU6050) || defined(TEST_TCS34727) || defined(TEST_LCD)
    WTIMER0_Init();
    #endif

    #if defined(TEST_MPU6050) || defined(TEST_TCS34727) || defined(TEST_LCD)
    I2C_Init();
    #endif

    #ifdef TEST_MPU6050
    MPU6050_Init();
    #endif

    #ifdef TEST_TCS34727
    TCS34727_Init();
    #endif

    #ifdef TEST_SERVO
    Servo_Init();
    #endif

    #ifdef TEST_LCD
    LCD_Init();
    #endif

    #ifdef TEST_TIMER
    LED_Init();
    PortE_Init();
    #endif

    #ifdef TEST_GPIO
    LED_Init();
    BTN_Init();
    #endif
}

int main(void){
    System_Init();
    while(1){
        #ifdef TEST_TIMER
        Test_Timer();
        #endif
        #ifdef TEST_UART0
        Test_UART0();
        #endif
        #ifdef TEST_SERVO
        Test_Servo();
        #endif
        #ifdef TEST_GPIO
        Test_GPIO();
        #endif
        #ifdef TEST_MPU6050
        Test_MPU6050();
        #endif
        #ifdef TEST_TCS34727
        Test_TCS34727();
        #endif
        #ifdef TEST_LCD
        Test_LCD();
        #endif
    }
    return 0;
}
