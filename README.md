# 🧭 Test.c — Ultrasonic Sensor Trigger Test

**Author:** Luan Phan  
**Course:** CSE 325 – Embedded Systems  
**Board:** NXP FRDM-KL46Z  

---

## 📄 Purpose
`Test.c` is used **solely to test the functionality of the ultrasonic distance sensor**.  
It continuously generates a **10 µs trigger pulse every 100 ms** to confirm that the sensor’s echo pin responds correctly.  
This file should be used **only for testing**, not for distance measurement or robot logic.

---

## ⚙️ Functionality
- Sends a **10 µs HIGH pulse** to the ultrasonic sensor’s **TRIG** pin.  
- Waits **100 ms** before repeating the pulse.  
- Runs indefinitely to help verify correct hardware setup and sensor operation.

---

## 🧩 Key Parameters
| Parameter | Value | Description |
|------------|--------|-------------|
| Trigger Pulse Width | **10 µs** | Standard trigger pulse duration for ultrasonic sensors |
| Trigger Interval | **100 ms** | Ensures echo signal is received before next trigger |
| Purpose | **Hardware testing** | No distance or echo timing computation |

---
> ⚠️ If your MCU operates at 3.3 V and the ultrasonic sensor outputs 5 V on **ECHO**, use a **voltage divider** or **logic-level shifter**.

---

## 🧠 Usage Notes
- Flash `Test.c` to your FRDM-KL46Z board.  
- Connect **TRIG** to the MCU pin defined in the source file.  
- Use an oscilloscope or logic analyzer to observe **ECHO**.  
- You should see a consistent 10 µs trigger pulse every 100 ms.

---

## 🚫 Limitations
- No distance calculation or timing logic.  
- No interrupt handling.  
- For **trigger-signal verification only**.

---

**Written by:** *Luan Phan*  
**Date:** *Oct 2025*
