# I2C Communication – Embedded Systems Project

**Course:** CECS 447 – Embedded Systems  
**Project:** Embedded Systems Integration – Color Detection & IMU Servo Control  
**Team:** Group 7 | California State University, Long Beach

---

## Overview

This project implements I2C communication between a microcontroller and multiple peripherals:

- **MPU-6050** – 6-axis IMU (accelerometer + gyroscope) for motion/orientation sensing
- **TCS34727** – RGB color sensor for color detection
- **LCD Display** – I2C-driven character display for output
- **Servo Motor** – Position control based on sensor data
- **UART** – Serial debug output

---

## Repository Structure

```
├── src/
│   ├── main.c            # Application entry point
│   ├── i2c.c             # I2C driver
│   ├── mpu6050.c         # MPU-6050 IMU driver
│   ├── tcs34727.c        # TCS34727 color sensor driver
│   ├── lcd.c             # LCD display driver
│   ├── servo.c           # Servo motor control
│   ├── uart0.c           # UART serial driver
│   ├── wtimer.c          # Wide timer driver
│   ├── button_led.c      # Button and LED driver
│   └── module_test.c     # Individual module tests
├── include/
│   ├── i2c.h
│   ├── mpu6050.h
│   ├── tcs34727.h
│   ├── lcd.h
│   ├── servo.h
│   ├── uart0.h
│   ├── wtimer.h
│   └── button_led.h
└── docs/
    ├── project_report.md       # Full project report
    ├── integration_report.md   # Step 3 integration report
    └── evidence.md             # Test evidence and photos
```

---

## Team Members

Alfredo
