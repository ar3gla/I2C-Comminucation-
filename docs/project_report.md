
---

<div align="center">

# Project Report

---

<img width="770" height="717" alt="image" src="https://github.com/user-attachments/assets/b6bfdc50-95bf-4ea0-94c4-216e01c7c104" />

California State University, Long Beach
Department of Computer Engineering & Computer Science

**Project Title:** Embedded Systems Integration – Color Detection & IMU Servo Control
**Course:** CECS 447 - Embedded Systems
**Project Number:** I2C
**Team Number:** Group 7
**Instructor:** Min He
**Submission Date:** 5/13/2026
**Team Members:** Alfredo Regla

</div>

---

# Table of Contents
*(Generate automatically if exporting to PDF)*

---

# 1. Introduction

## 1.1 Project Objective

The objective of this project is to design and implement an embedded system on the TM4C123GH6PM microcontroller that integrates eight hardware and software modules: a wide timer, GPIO-based button and LED control, UART serial communication, I2C bus communication, a TCS34725 RGB color sensor, an MPU6050 inertial measurement unit, a 16×2 LCD display with I2C backpack, and a servo motor controlled via hardware PWM. The system samples color and tilt data, displays results on an LCD and UART terminal, drives onboard LEDs to reflect detected color, and positions a servo to track the board's roll angle.

## 1.2 System Overview

At startup, all eight modules are initialized in sequence and each I2C device is confirmed present via its identification register. The main loop runs at two cadences: every 100 ms the color sensor and IMU are sampled, and every 1 second the processed results (color name and tilt angle) are written to the 16×2 LCD and transmitted over UART to a PC terminal. Simultaneously, the onboard RGB LED reflects the currently dominant color, and the servo motor is driven to the roll angle computed from the IMU accelerometer data. Two push buttons provide real-time user control: SW1 starts and stops detection, and SW2 forces an immediate LCD update outside the 1-second cycle.

---

# 2. Requirements & Constraints

## 2.1 Functional Requirements

- RQ-01: WTIMER_Init() initialises 32-bit wide timer (WTIMER1–5, student choice) for 1 ms delay resolution
- RQ-02: WTIMER_Delay(ms): Block for specified milliseconds
- RQ-03: LED Control: Red LED blinks at 2 Hz in timer mode
- RQ-04: Mode Selection: SW1 toggles between UART test mode and WTIMER mode
- RQ-05: LED Cycling: SW2 cycles through onboard LEDs (Red → Green → Blue)
- RQ-06: UART0_Init(): Initialize UART at chosen baud rate (not 57600)
- RQ-07: UART0_SendChar(c): Send single character
- RQ-08: UART0_SendString(s): Send null-terminated string
- RQ-09: UART0_SendInteger(n): Send integer as decimal string
- RQ-10: UART0_SendFloat(f, decimals): Send float with specified precision
- RQ-11: I2C_Init(): Initialize I2C module at 100 kHz
- RQ-12: I2C_Write(addr, reg, data): Write single byte to register
- RQ-13: I2C_Read(addr, reg, count, buffer): Read multiple bytes from register
- RQ-14: TCS34725_Init(): Initialize sensor, detect via ID register
- RQ-15: TCS34725_ReadColor(r, g, b): Read 16-bit RGB values
- RQ-16: MPU6050_Init(): Initialize sensor, detect via WHO_AM_I register
- RQ-17: MPU6050_ReadAccel(ax, ay, az): Read 3-axis acceleration
- RQ-18: LCD_Init(): Initialize LCD via I2C backpack
- RQ-19: PWM_Init(): Configure PWM at 50 Hz frequency
- RQ-20: Servo_SetAngle(angle): Set servo to specified angle (−90° to +90°)

## 2.2 Design & Hardware Constraints

- CN-01: Sensor sampling runs every 100 ms; LCD & UART output runs every 1 s (±100 ms)
- CN-02: Error Handling: Detect timeout/NAK and return status code
- CN-03: UART0 baud rate: 57600 not allowed; team selected an alternate standard baud rate

---

# 3. Design Alternatives & Architecture

## 3.1 Design Alternative A

### Hardware Plan
Use a single shared I2C bus (hardware I2C, one of I2C1–3) with external 4.7 kΩ pull-up resistors on SDA and SCL for all three peripherals (TCS34725, MPU6050, LCD backpack). Hardware PWM (M0PWM1–7 or M1PWM0–7) drives the servo at 50 Hz. A wide timer (WTIMER1–5) handles all delay and timing functions. GPIO pins PF0–PF4 manage onboard LEDs and buttons.

### Software Implementation Method
All modules are written in C with direct register-level access. A polling-based main loop samples sensors every 100 ms using a WTIMER counter and outputs to LCD and UART every 1 second. Compile-time preprocessor flags (`#define TEST_*`) enable isolated module testing. A single app.c coordinates initialization order and main-loop execution.

### Risks & Tradeoffs
I2C bus contention is possible if multiple devices are accessed without proper sequencing, but is mitigated by sequential, blocking transactions. Hardware PWM offloads timing to the peripheral, reducing CPU burden. Polling requires the CPU to remain active during delays but avoids interrupt-driven complexity.

---

## 3.2 Design Alternative B

### Hardware Plan
Use separate I2C buses for each peripheral to eliminate any risk of address conflicts or bus contention. Use software-generated PWM (bit-banging) for servo control. Use busy-wait loops (NOP-based) instead of a hardware wide timer for delays.

### Software Implementation Method
Three separate I2C instances would be initialized and managed independently. Software PWM would be generated by toggling a GPIO pin inside a timed loop. Delays would be approximated by counting clock cycles in a loop rather than using timer hardware.

### Risks & Tradeoffs
Using separate I2C buses consumes significantly more GPIO pins and increases initialization complexity. Software PWM is sensitive to interrupt interference and cannot guarantee a stable 50 Hz signal, which could cause servo jitter. Busy-wait delays are inaccurate and waste CPU cycles, making them unsuitable for a dual-cadence timing scheme. This design is less scalable and less reliable overall.

---

## 3.3 Design Comparison Table

| Criteria | Design A (Shared HW I2C + HW PWM) | Design B (Separate Buses + SW PWM) |
|----------|-----------------------------------|-------------------------------------|
| GPIO Usage | Low – shared SDA/SCL with pull-ups | High – separate pins per bus |
| PWM Accuracy | High – hardware-generated 50 Hz | Low – software-generated, jitter-prone |
| Timing Accuracy | High – WTIMER hardware counter | Low – NOP busy-wait, clock-dependent |
| Implementation Complexity | Moderate – single bus, sequential access | High – multiple bus managers needed |
| CPU Overhead | Low – hardware handles PWM and timing | High – CPU busy during delays and PWM |
| Scalability | Good – additional I2C devices easy to add | Poor – each device may need a new bus |

---

## 3.4 Final Design Selection & Justification

Design A was selected. Using a single hardware I2C bus minimizes GPIO consumption and is consistent with standard embedded systems practice for multi-peripheral designs. The 4.7 kΩ pull-up resistors satisfy Standard Mode (100 kHz) rise-time requirements. Hardware PWM guarantees a stable, CPU-independent 50 Hz waveform required for reliable servo control. The WTIMER-based delay function provides precise 1 ms resolution without busy-waiting, enabling the dual 100 ms / 1 s timing cadence. Design B's software approaches introduce timing inaccuracy and resource waste that are incompatible with the system's real-time constraints.

---

# 4. System Architecture

## 4.1 Block Diagram
*(Insert diagram)*

## 4.2 Module Decomposition

| Module Name | Responsibility | Inputs | Outputs |
|-------------|---------------|--------|---------|
| WTimer | Provides accurate timing functionality using wide timers | System clock, delay value (ms) | Precise blocking delay |
| ButtonLED | Manages user input via buttons and controls LED behavior | SW1 (PF4), SW2 (PF0) | LED state changes, mode selection |
| UART0 | Enables serial communication for debugging and data output | Characters, strings, numeric data | UART transmission output |
| I2C | Facilitates communication with external peripherals over I2C protocol | Device address, register, data | Data read/write transactions |
| MPU6050 | Interfaces with accelerometer and gyroscope sensor | I2C data | Acceleration, gyroscope, and angle values |
| TCS34725 | Interfaces with color sensor to retrieve RGB data | I2C data | Processed RGB values |
| LCD | Controls LCD display via I2C backpack | Text data, cursor position | Visual text output |
| Servo (PWM) | Generates PWM signals to control servo position | Desired angle | PWM waveform |
| ModuleTest | Provides isolated testing framework for each module | Compile-time test flags | Output via UART or LCD |

## 4.3 Call Graph

<img width="680" height="489" alt="image" src="https://github.com/user-attachments/assets/3557cd54-0e59-4af8-a807-b0203e0aa624" />

## 4.4 Data Flow Diagram

<img width="711" height="408" alt="image" src="https://github.com/user-attachments/assets/007a4977-5217-4066-a4a1-8730d98708d2" />

## 4.5 Software Flowchart

<img width="164" height="389" alt="image" src="https://github.com/user-attachments/assets/76e04bcb-18cc-4f3e-aa41-7f4fdddcb6dc" />

---

# 5. Module Implementation & Testing

## 5.1 Module List and Responsibility

- **WTimer (WTimer.c / WTimer.h):** Provides 1 ms delay using a hardware wide timer (WTIMER1–5). Used by all other modules for initialization delays and main-loop timing. *(Alfredo Regla, Paris)*
- **ButtonLED (ButtonLED.c / ButtonLED.h):** Initializes onboard RGB LEDs (PF1/PF2/PF3) and push buttons SW1 (PF4) and SW2 (PF0). Handles mode toggling and LED cycling via direct GPIO reads. *(Gerald, Adrian, Alfredo Regla)*
- **UART0 (UART0.c / UART0.h):** Initializes UART0 on PA0/PA1 at a team-chosen baud rate (not 57600). Provides functions to transmit characters, strings, integers, and floats to a PC terminal. *(Paris, Gerald, Adrian, Alfredo Regla)*
- **I2C (I2C.c / I2C.h):** Initializes hardware I2C at 100 kHz Standard Mode. Manages all read/write transactions to the three external I2C slave devices on the shared SDA/SCL bus. *(Gerald, Adrian, Alfredo Regla)*
- **TCS34725 (TCS34727.c / TCS34727.h):** Initializes the RGB color sensor and verifies its presence via the ID register (0x44). Reads 16-bit RGB and clear channel values and returns the dominant color. *(Paris, Gerald)*
- **MPU6050 (MPU6050.c / MPU6050.h):** Initializes the IMU and confirms its presence via WHO_AM_I (0x68). Reads 3-axis accelerometer data and computes the roll angle used to position the servo. *(Adrian, Alfredo Regla)*
- **LCD (LCD.c / LCD.h):** Initializes the 16×2 LCD via its I2C backpack. Writes color name and angle to the display; updates are throttled to 1 Hz to prevent I2C flickering. *(Paris)*
- **Servo (Servo.c / Servo.h):** Configures hardware PWM at 50 Hz and maps a requested angle (−90° to +90°) to the corresponding pulse width. PWM module selected from M0PWM1–7 or M1PWM0–7 (M0PWM0 not used). *(Gerald, Adrian, Alfredo Regla)*
- **app.c:** Top-level application. Initializes all 8 modules in order, then runs the main loop coordinating the 100 ms sensor sampling and 1 second output cadence. *(Alfredo Regla, Gerald)*

## 5.2 Module Test Strategy

Each module was tested in isolation using compile-time preprocessor flags (`#define TEST_*`) defined in ModuleTest.c. Only one test flag was enabled at a time to prevent resource conflicts. Results were observed via UART terminal output, LCD display, oscilloscope measurement, or direct visual inspection of LED and servo behavior. Evidence for each test is captured as a screenshot, photo, or video file stored in the `/evidence` folder.

## 5.3 Module Test Summary Table

| Module | Test ID | Test Type | Evidence Reference |
|--------|---------|-----------|-------------------|
| WTimer | TC-MOD-001 | Functional – LED toggle timing | evidence/TC-MOD-001_timer_init.png |
| WTimer | TC-MOD-002 | Functional – blocking delay via oscilloscope | evidence/timer_oscilloscope.png |
| ButtonLED | TC-MOD-003 | Functional – Red LED 2 Hz blink | evidence/gpio_led_blink_test.mp4 |
| ButtonLED | TC-MOD-004 | Functional – SW1 mode toggle | evidence/gpio_button_test.mp4 |
| ButtonLED | TC-MOD-005 | Functional – SW2 LED cycle | evidence/gpio_led_cycle_test.mp4 |
| UART0 | TC-MOD-006 | Functional – init & string TX/RX | evidence/uart0_terminal_test.png |
| UART0 | TC-MOD-007 | Functional – single character send | evidence/uart0_terminal_test.png |
| UART0 | TC-MOD-008 | Functional – null-terminated string send | evidence/uart0_terminal_test.png |
| UART0 | TC-MOD-009 | Functional – integer send | evidence/uart0_terminal_test.png |
| UART0 | TC-MOD-010 | Functional – float send with precision | evidence/uart0_terminal_test.png |
| I2C | TC-MOD-011 | Functional – init at 100 kHz | evidence/i2c_init_scope.png |
| I2C | TC-MOD-012 | Functional – write single byte | evidence/i2c_write_test.png |
| I2C | TC-MOD-013 | Functional – read multiple bytes | evidence/i2c_read_test.png |
| TCS34725 | TC-MOD-014 | Functional – init & ID register verify | evidence/tcs34725_terminal_output.png |
| TCS34725 | TC-MOD-015 | Functional – RGB values read | evidence/tcs34725_terminal_output.png |
| MPU6050 | TC-MOD-016 | Functional – init & WHO_AM_I verify | evidence/mpu6050_terminal_output.png |
| MPU6050 | TC-MOD-017 | Functional – 3-axis accel read | evidence/mpu6050_terminal_output.png |
| LCD | TC-MOD-018 | Functional – static text display | evidence/lcd_photo_test.jpg |
| Servo | TC-MOD-019 | Functional – angle sweep & PWM at 50 Hz | evidence/servo_pwm_test.mp4 |
| Servo | TC-MOD-020 | Functional – angle to duty cycle mapping | evidence/servo_pwm_test.mp4 |
| app.c | TC-MOD-021 | Timing – 100 ms sample / 1 s output cadence | evidence/TC-MOD-021_timing.png |

## 5.4 Team Assignment

| Module | Primary Developer |
|--------|-----------------|
| WTimer – WTIMER_Init (RQ-01) | Alfredo Regla |
| WTimer – WTIMER_Delay (RQ-02) | Paris |
| ButtonLED – LED_Blink (RQ-03) | Gerald |
| ButtonLED – Button_Read / SW1 (RQ-04) | Adrian |
| ButtonLED – LED_Cycle / SW2 (RQ-05) | Alfredo Regla |
| UART0 – Init (RQ-06) | Paris |
| UART0 – SendChar (RQ-07) | Gerald |
| UART0 – SendString (RQ-08) | Adrian |
| UART0 – SendInteger (RQ-09) | Alfredo Regla |
| UART0 – SendFloat (RQ-10) | Paris |
| I2C – Init (RQ-11) | Gerald |
| I2C – Write (RQ-12) | Adrian |
| I2C – Read (RQ-13) | Alfredo Regla |
| TCS34725 – Init (RQ-14) | Paris |
| TCS34725 – ReadColor (RQ-15) | Gerald |
| MPU6050 – Init (RQ-16) | Adrian |
| MPU6050 – ReadAccel (RQ-17) | Alfredo Regla |
| LCD – Init (RQ-18) | Paris |
| Servo – PWM_Init (RQ-19) | Gerald |
| Servo – SetAngle (RQ-20) | Adrian |
| Main Loop / Timing (CN-01) | Alfredo Regla |
| I2C Error Handling (CN-02) | Paris |
| UART Baud Verification (CN-03) | Gerald |
| Design Decision – I2C Bus (DES-01) | Adrian |
| PWM Module Selection (DES-02) | Alfredo Regla |
| Full System Integration (SYS-01) | Paris |
| Continuous Sampling Validation (SYS-02) | Gerald |

---

# 6. System Integration & Validation

## 6.1 Integration Order & Rationale

- WTIMER → UART → I2C → MPU6050 → TCS34725 → LCD → Servo

We started with the timer since everything needs delays. Then UART so we could see what was happening during each subsequent integration step. Then I2C since the sensors and LCD all share the bus and depend on it. Then each sensor in turn (MPU6050 first, then TCS34725), then the LCD since it needed confirmed I2C and sensor data to display, and finally the servo last since it requires angle data computed from the IMU.

## 6.2 System-Level Test Matrix

| Requirement | Test Method | Evidence | Pass/Fail |
|-------------|------------|----------|-----------|
| All modules initialize | Checked UART output on boot | Each module printed a confirmation message | Pass |
| SW1 starts/stops detection | Pressed SW1 while watching UART | Output stopped and started correctly | Pass |
| SW2 updates display | Pressed SW2 mid-cycle | LCD updated right away | Pass |
| Servo follows IMU tilt | Tilted board left and right | Servo moved the same direction | Pass |
| LCD shows color and angle | Held colored objects to sensor | LCD showed the right color name and angle | Pass |
| UART updates at 1 Hz | Watched terminal for 10 seconds | One line printed per second | Pass |
| LEDs show detected color | Held red, green, blue objects to sensor | Correct LED turned on each time | Pass |

## 6.3 Team Assignment

| Integration Step | Owner |
|-----------------|-------|
| WTIMER + UART bring-up | Alfredo Regla, Paris |
| I2C bus bring-up | Gerald, Adrian |
| MPU6050 + TCS34725 integration | Adrian, Paris |
| LCD integration | Paris |
| Servo integration | Gerald, Adrian |
| Full system validation | Alfredo Regla, Gerald |

---

# 7. Claim–Evidence–Reasoning (CER)

### Claim 1
**Claim:** All three I2C devices are correctly detected at boot.
- **Evidence:** UART showed WHO_AM_I = 0x68 and ID = 0x44 on boot
- **Reasoning:** The correct identification register values were returned from both the MPU6050 and TCS34725, confirming that I2C communication is functional and all three devices are properly addressed on the shared bus.

### Claim 2
**Claim:** The servo accurately tracks the IMU tilt angle in real time.
- **Evidence:** Tilting the board moved the servo to the same angle shown on UART
- **Reasoning:** The MPU6050 computes a roll angle from accelerometer data, and that value is passed directly to Servo_SetAngle(), producing a one-to-one correspondence between board tilt and servo position.

### Claim 3
**Claim:** Limiting LCD updates to 1 Hz eliminates display flickering.
- **Evidence:** LCD flickered at full speed; flickering stopped after updates were limited to 1 Hz
- **Reasoning:** Writing to the LCD too frequently over I2C caused it to glitch due to transaction overhead at that rate. Throttling to 1 Hz resolved the issue.

### Claim 4
**Claim:** SW2 provides an immediate, timer-independent LCD update.
- **Evidence:** Pressing SW2 updated the LCD right away no matter where the timer was in its 1-second cycle
- **Reasoning:** The SW2 handler reads the most recently sampled values and writes them directly to the LCD without waiting for the next 1-second tick, satisfying the requirement for an on-demand display refresh.

### Claim 5
**Claim:** The onboard RGB LEDs correctly reflect the dominant color detected by the TCS34725.
- **Evidence:** Red, green, and blue LEDs each turned on when the matching color was shown to the sensor
- **Reasoning:** Detect_Color() compares the R, G, and B channel values and returns the highest, and the LED control logic maps that result directly to the corresponding onboard LED pin.

---

# 8. Demonstration Summary

## 8.1 Demo Scenario

The board was powered on and all eight modules were initialized in sequence. UART output confirmed each module's successful initialization. A colored object was placed in front of the TCS34725 sensor while the board was tilted at various angles. The LCD and UART terminal displayed the detected color name and roll angle at 1 Hz. The onboard LED matching the detected color illuminated, and the servo rotated to reflect the current tilt. SW1 was pressed to stop and restart detection, and SW2 was pressed to trigger an immediate LCD refresh mid-cycle.

## 8.2 Robustness Test

- **What was tested:** Nothing in front of TCS34725, just room light
- **Expected behavior:** LCD shows "Color: NONE" and LEDs stay off
- **Actual result:** Passed. No color was detected so NOTHING_DETECT was returned

## 8.3 Observed Behavior Summary

- **What was demonstrated:** Boot with all sensors detected, SW1 start/stop, servo tracking tilt, LCD and UART at 1 Hz, LEDs matching color, SW2 manual update
- **Key results:** All 7 modules worked on their own and together in the integrated system
- **Observations:** The color sensor performed more reliably when ambient room light around the sensor aperture was blocked, reducing false readings from environmental illumination

## 8.4 AI Verification Summary

- **Debugging assistance:** AI identified that the `USE_BTN_INTERRUPT` flag in ButtonLED.h was incompatible with the polling-based button code, causing buttons to behave incorrectly. Commenting out the flag resolved the issue.
- **Verification support:** AI flagged format specifier mismatches where float values were being printed with `%d`. The issue was corrected by casting float values to int before printing.
- **One rejected AI suggestion:** AI suggested making SW2 pause the LCD while held down. This was rejected because the requirement specifies that SW2 should trigger an immediate display update, not pause output.

## 8.5 Demonstration Video
(https://www.youtube.com/shorts/1mhyX8v0Nmg)

---

# 9. Conclusion

## Summary of Technical Achievements

The project successfully integrated eight embedded software modules on the TM4C123GH6PM microcontroller. All three I2C peripherals (TCS34725, MPU6050, and LCD) were initialized and verified on a shared 100 kHz bus. The system maintained a dual-cadence loop with 100 ms sensor sampling and 1 second output to both the LCD and UART terminal. The servo accurately tracked IMU tilt angle, and onboard LEDs correctly reflected detected colors in real time.

## Major Challenges and Solutions

The primary challenge encountered was LCD flickering caused by writing to the display at full loop speed over I2C. This was resolved by throttling LCD writes to 1 Hz using the WTIMER counter. A secondary issue involved button malfunctions traced to the `USE_BTN_INTERRUPT` compile-time flag conflicting with the polling architecture; commenting it out restored correct behavior. Float-to-integer format specifier mismatches in UART output were caught during AI-assisted code review and corrected by explicit casting.

## Key Lessons Learned

Hardware peripherals must be initialized in dependency order — timing before communication, communication before sensors, sensors before display. Throttling I2C writes is essential for stable LCD operation. Compile-time flags that alter hardware behavior must be carefully reviewed when switching between interrupt-driven and polling-based designs. AI tools are effective for catching subtle bugs such as format specifier mismatches, but their suggestions must be evaluated against actual system requirements before acceptance.

---

# 10. References

1. Texas Instruments, *TM4C123GH6PM Microcontroller Data Sheet*, Literature No. SPMS376E. Available: https://www.ti.com/lit/ds/spms376e/spms376e.pdf
2. Texas Instruments, *TivaWare™ Peripheral Driver Library User's Guide*, Literature No. SPMU298D.
3. ams AG, *TCS34725 Color Light-to-Digital Converter with IR Filter Datasheet*, v1.0.
4. InvenSense, *MPU-6050 Product Specification Revision 3.4*.
5. NXP Semiconductors, *UM10204: I2C-Bus Specification and User Manual*, Rev. 7.0, October 2021.
6. PCF8574 Remote 8-Bit I/O Expander for I2C Bus Datasheet, Texas Instruments.
7. J. W. Valvano, *Embedded Systems: Real-Time Interfacing to ARM Cortex-M Microcontrollers*, 5th ed.


---
