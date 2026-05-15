# Step 3 – Integration & Validation

## 1. Integration Order

Explain:
- WTIMER → UART → I2C → MPU6050 → TCS34727 → LCD → Servo
- We started with the timer since everything needs delays. Then UART so we could see what was happening. Then I2C since the sensors and LCD use it. Then each sensor, then LCD, then servo last since it needs angle data from the IMU.

---

## 2. System Architecture

### 2.1 Final Call Graph
<img width="680" height="489" alt="image" src="https://github.com/user-attachments/assets/cd81f6a1-7ed7-495c-8952-5037abf1c5b4" />


### 2.2 Data Flow Diagram
<img width="711" height="408" alt="image" src="https://github.com/user-attachments/assets/406aae4a-eb0b-49d4-8d92-47c7e2d0cf1d" />


---

## 3. System-Level Validation

| Requirement | Test Method | Evidence | Pass/Fail |
|-------------|------------|----------|-----------|
| All modules initialize | Checked UART output on boot | Each module printed a confirmation message | Pass |
| SW1 starts/stops detection | Pressed SW1 while watching UART | Output stopped and started correctly | Pass |
| SW2 updates display | Pressed SW2 mid-cycle | LCD updated right away | Pass |
| Servo follows IMU tilt | Tilted board left and right | Servo moved the same direction | Pass |
| LCD shows color and angle | Held colored objects to sensor | LCD showed the right color name and angle | Pass |
| UART updates at 1 Hz | Watched terminal for 10 seconds | One line printed per second | Pass |
| LEDs show detected color | Held red, green, blue objects to sensor | Correct LED turned on each time | Pass |

---

## 4. Robustness Test

- **What was tested:** Nothing in front of TCS34727, just room light
- **Expected behavior:** LCD shows "Color: NONE" and LEDs stay off
- **Actual result:** Passed. No color was detected so NOTHING_DETECT was returned

---

## 5. Claim–Evidence–Reasoning (CER)

### Claim 1
- **Claim** I2C communication is working correctly with all three devices
- **Evidence:** UART showed WHO_AM_I = 0x68 and ID = 0x44 on boot
- **Reasoning:** The correct values came back so I2C is working with all three devices

### Claim 2
- **Claim** The IMU roll angle is correctly mapped to servo position in real time
- **Evidence:** Tilting the board moved the servo to the same angle shown on UART
- **Reasoning:** The IMU gives a roll angle and we pass it straight to the servo

### Claim 3
- **Claim** The LCD requires rate-limited I2C writes to display stably
- **Evidence:** LCD flickered at full speed, stopped flickering after we limited updates to 1 Hz
- **Reasoning:** Writing to the LCD too fast over I2C caused it to glitch

### Claim 4
- **Claim** The color sensor accurately identifies red, green, and blue and triggers the correct LED
- **Evidence:** Red, green, and blue LEDs each turned on when the matching color was shown to the sensor
- **Reasoning:** Detect_Color returns which channel is highest and we use that to set the LED

---

## 6. Final Demo Summary

- **What was demonstrated:** Boot with all sensors detected, SW1 start/stop, servo tracking tilt, LCD and UART at 1 Hz, LEDs matching color, SW2 manual update
- **Key results:** All 7 modules worked on their own and together
- **Observations:** The color sensor worked better when we blocked out the room light around it

---

## 7. AI Verification Summary

- **Debugging assistance:** AI found that USE_BTN_INTERRUPT in ButtonLED.h was causing the buttons to not work correctly with our polling code. Commenting it out fixed it.
- **Verification support:** AI caught format warnings where float values were printed with %d. Fixed by casting to int.
- **One rejected AI suggestion:** AI suggested making SW2 pause the LCD while held down. We rejected it because the requirement says SW2 should update the display, not pause it.```
