/*
 * main.c
 *
 * Course number: CECS 447
 * Term: Spring 2026
 * Project number: Project 2
 * Project description: Full system integration test.
 * Team #: 7
 * Team members: Adrian Gustilo, Alfredo Regla, Gerald Calero, Paris Talebi
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

void Test_Full_System(void)
{
    static uint8_t  running = 0;
    static uint16_t tick    = 0;

    MPU6050_ACCEL_t    myAccel;
    MPU6050_GYRO_t     myGyro;
    MPU6050_ANGLE_t    myAngle;
    RGB_COLOR_HANDLE_t myColor;
    char buf[64];

    if (!(GPIO_PORTF_DATA_R & SW1_PIN)) {
        DELAY_1MS(20);
        if (!(GPIO_PORTF_DATA_R & SW1_PIN)) {
            running ^= 1;
            while (!(GPIO_PORTF_DATA_R & SW1_PIN)) {}   /* wait for release */
        }
    }
    if (!running) return;

    MPU6050_Get_Accel(&myAccel);
    MPU6050_Get_Gyro(&myGyro);
    MPU6050_Process_Accel(&myAccel);
    MPU6050_Process_Gyro(&myGyro);
    MPU6050_Get_Angle(&myAccel, &myGyro, &myAngle);

    int16_t angle = (int16_t)myAngle.ArX;
    if (angle >  90) angle =  90;
    if (angle < -90) angle = -90;
    Drive_Servo(angle);

    myColor.C_RAW = TCS34727_GET_RAW_CLEAR();
    myColor.R_RAW = TCS34727_GET_RAW_RED();
    myColor.G_RAW = TCS34727_GET_RAW_GREEN();
    myColor.B_RAW = TCS34727_GET_RAW_BLUE();
    TCS34727_GET_RGB(&myColor);
    COLOR_DETECTED color = Detect_Color(&myColor);

    /* LED mirrors detected color */
    if      (color == RED_DETECT)   LEDs = (LEDs & ~0x0E) | RED;
    else if (color == GREEN_DETECT) LEDs = (LEDs & ~0x0E) | GREEN;
    else if (color == BLUE_DETECT)  LEDs = (LEDs & ~0x0E) | BLUE;
    else                            LEDs = (LEDs & ~0x0E) | DARK;

    tick++;
    if (tick >= 10) {
        tick = 0;

        LCD_Set_Cursor(ROW1, 0);
        switch (color) {
            case RED_DETECT:   LCD_Print_Str((uint8_t *)"Color: RED      "); break;
            case GREEN_DETECT: LCD_Print_Str((uint8_t *)"Color: GREEN    "); break;
            case BLUE_DETECT:  LCD_Print_Str((uint8_t *)"Color: BLUE     "); break;
            default:           LCD_Print_Str((uint8_t *)"Color: NONE     "); break;
        }

        LCD_Set_Cursor(ROW2, 0);
        sprintf(buf, "Angle: %4d deg ", angle);
        LCD_Print_Str((uint8_t *)buf);

        sprintf(buf, "R:%3d G:%3d B:%3d | Angle: %4d\r\n",
                (int)myColor.R, (int)myColor.G, (int)myColor.B, angle);
        UART0_OutString(buf);
    }

    /* SW2 (PF0): force immediate LCD + UART update with debounce */
    if (!(GPIO_PORTF_DATA_R & SW2_PIN)) {
        DELAY_1MS(20);
        if (!(GPIO_PORTF_DATA_R & SW2_PIN)) {
            LCD_Set_Cursor(ROW1, 0);
            switch (color) {
                case RED_DETECT:   LCD_Print_Str((uint8_t *)"Color: RED      "); break;
                case GREEN_DETECT: LCD_Print_Str((uint8_t *)"Color: GREEN    "); break;
                case BLUE_DETECT:  LCD_Print_Str((uint8_t *)"Color: BLUE     "); break;
                default:           LCD_Print_Str((uint8_t *)"Color: NONE     "); break;
            }

            LCD_Set_Cursor(ROW2, 0);
            sprintf(buf, "Angle: %4d deg ", angle);
            LCD_Print_Str((uint8_t *)buf);

            sprintf(buf, "R:%3d G:%3d B:%3d | Angle: %4d\r\n",
                    (int)myColor.R, (int)myColor.G, (int)myColor.B, angle);
            UART0_OutString(buf);

            while (!(GPIO_PORTF_DATA_R & SW2_PIN)) {}  /* wait for release */
        }
    }

    DELAY_1MS(100);
}


void System_Init(void)
{
    UART0_Init();
    WTIMER0_Init();
    I2C_Init();
    MPU6050_Init();
    TCS34727_Init();
    Servo_Init();
    LCD_Init();
    LED_Init();
    BTN_Init();
}


int main(void)
{
    System_Init();
    while (1) {
        Test_Full_System();
    }
    return 0;
}
