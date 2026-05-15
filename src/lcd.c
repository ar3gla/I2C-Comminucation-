/*
 * LCD.c
 *
 * Course number: CECS 447
 * Term: Spring 2026
 * Project number: Project 2
 * Project description: Main implementation of the functions to interact with
 *                      the LCD display through the I2C backpack
 * Team #: 7
 * Team members: Adrian Gustilo, Alfredo Regla, Gerald Calero, Paris Talebi
 *
 */

#include "LCD.h"
#include "tm4c123gh6pm.h"
#include "wtimer.h"
#include "I2C.h"

/*
 * -------------------LCD_Send_CMD------------------
 * Local LCD send commands function
 * Input: Command to send
 * Output: None
 */
static void LCD_Send_CMD(uint8_t cmd) {

    uint8_t cmd_upper, cmd_lower;
    uint8_t cmd_array[4];

    /* Separate Upper and Lower Nibble */
    cmd_upper = cmd & UPPER_NIBBLE_MSK;
    cmd_lower = (cmd << NIBBLE_SHIFT) & UPPER_NIBBLE_MSK;

    /*
     * Each nibble is sent as two bytes:
     *   1) nibble | BACKLIGHT | EN_Pin  (EN high)
     *   2) nibble | BACKLIGHT           (EN low, latches data)
     * RS=0, RW=0 for commands
     */
    cmd_array[0] = cmd_upper | BACKLIGHT | EN_Pin;
    cmd_array[1] = cmd_upper | BACKLIGHT;
    cmd_array[2] = cmd_lower | BACKLIGHT | EN_Pin;
    cmd_array[3] = cmd_lower | BACKLIGHT;

    /* I2C Burst Transmit Command Array to LCD */
    I2C_Burst_Transmit(LCD_WRITE_ADDR, PCF8574A_REG, cmd_array, 4);
}

/*
 * ------------------LCD_Send_Data------------------
 * Local LCD send data function
 * Input: Data to send
 * Output: None
 */
static void LCD_Send_Data(uint8_t data) {

    uint8_t data_upper, data_lower;
    uint8_t data_array[4];

    /* Separate Upper and Lower Nibble */
    data_upper = data & UPPER_NIBBLE_MSK;
    data_lower = (data << NIBBLE_SHIFT) & UPPER_NIBBLE_MSK;

    /*
     * RS=1 (RS_Pin) for data, RW=0, EN toggled high then low
     */
    data_array[0] = data_upper | BACKLIGHT | EN_Pin | RS_Pin;
    data_array[1] = data_upper | BACKLIGHT | RS_Pin;
    data_array[2] = data_lower | BACKLIGHT | EN_Pin | RS_Pin;
    data_array[3] = data_lower | BACKLIGHT | RS_Pin;

    /* I2C Burst Transmit Data Array to LCD */
    I2C_Burst_Transmit(LCD_WRITE_ADDR, PCF8574A_REG, data_array, 4);
}

/*
 * -------------------LCD_Init------------------
 * Basic LCD Initialization Function
 * Input: None
 * Output: None
 */
void LCD_Init(void){
    /* LCD Initialization INIT_REG_CMD and INIT_FUNC_CMD */
    LCD_Send_CMD(INIT_REG_CMD);
    DELAY_1MS(1);
    LCD_Send_CMD(INIT_FUNC_CMD);
    DELAY_1MS(1);
    
    /* 4-Bit Display Mode Initialization */
    //Set Function to 4-Bit, 2 rows, and 5x8 Character
    LCD_Send_CMD(FUNC_MODE|FUNC_4_BIT|FUNC_2_ROW|FUNC_5_7);
    DELAY_1MS(1);
    
    //Turn off Display
    LCD_Send_CMD(DISP_CMD|DISP_OFF);
    DELAY_1MS(1);
    
    //Clear Display 
    LCD_Send_CMD(CLEAR_DISP_CMD);
    DELAY_1MS(2);
    
    //Set Entry Mode
    LCD_Send_CMD(ENTRY_MODE_CMD|ENTRY_INC_CURSOR);
    DELAY_1MS(1);
    
    //Turn on Display with cursor and blink enable
    LCD_Send_CMD(DISP_CMD|DISP_ON|DISP_CURSOR_ON|DISP_BLINK_ON);    
}

/*
 * -------------------LCD_Clear------------------
 * Clear the LCD Display by passing a command
 * Input: None
 * Output: None
 */
void LCD_Clear(void) {
    LCD_Send_CMD(CLEAR_DISP_CMD);
    DELAY_1MS(2);
}

/*
 * ----------------LCD_Set_Cursor----------------
 * Set Cursor to Desired Place
 * Input: Desired Row and Column to place Cursor
 * Output: None
 */
void LCD_Set_Cursor(uint8_t row, uint8_t col) {
    uint8_t addr;
    if (row == ROW1)
        addr = FIRST_ROW_CMD + col;
    else
        addr = SECOND_ROW_CMD + col;
    LCD_Send_CMD(addr);
}

/*
 * ---------------LCD_Reset_Cursor---------------
 * Reset Cursor back to Row 1 and Column 0
 * Input: None
 * Output: None
 */
void LCD_Reset_Cursor(void) {
    LCD_Send_CMD(RETURN_HOME_CMD);
    DELAY_1MS(2);
}

/*
 * ----------------LCD_Print_Char----------------
 * Prints a Character to LCD
 * Input: Character Hex Value
 * Output: None
 */
void LCD_Print_Char(uint8_t data) {
    LCD_Send_Data(data);
}

/*
 * ----------------LCD_Print_Str-----------------
 * Prints a string to LCD
 * Input: Pointer to Character Array
 * Output: None
 */
void LCD_Print_Str(uint8_t* str) {
    while (*str != '\0') {
        LCD_Send_Data(*str);
        str++;
    }
}
