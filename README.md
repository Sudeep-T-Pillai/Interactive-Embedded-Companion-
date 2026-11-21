# Hanki — Interactive Embedded Companion

An expressive, sensor-driven desktop companion robot built on bare-metal embedded C++ (Arduino). Hanki reacts to physical touch and nearby movement in real time using a non-blocking, event-driven state machine and procedural OLED eye animations.

![Status](https://img.shields.io/badge/Status-Active%20Prototype-brightgreen)
![Platform](https://img.shields.io/badge/Platform-Arduino%20Nano-00979D?logo=arduino&logoColor=white)
![Display](https://img.shields.io/badge/Display-0.96%22%20SSD1306%20OLED-blue)
![Firmware](https://img.shields.io/badge/Current%20Sketch-robo__eyes.ino-orange)

---

## Overview

Hanki is designed to bring lifelike personality to a compact desk footprint without relying on heavy blocking loops or pre-rendered video frames. The current prototype (`robo_eyes.ino`) runs on an **Arduino Nano**, combining a **0.96" I2C OLED display**, a **Force-Sensitive Resistor (FSR)** for tactile petting detection, and a **PIR motion sensor** for environmental awareness.

Instead of sequential `delay()` calls that freeze sensor polling during animations, the firmware uses an **asynchronous, priority-based state machine** so eye rendering, blinking, and sensor interrupts run smoothly in parallel.

---

## Current Prototype Features (`robo_eyes.ino`)

- **Procedural Eye Animations:** Powered by `FluxGarage_RoboEyes` over `Adafruit_GFX` and `Adafruit_SSD1306`, supporting smooth eye tracking, natural auto-blinking, and expressive mood transitions.
- **Tactile Interaction (FSR):** Detects gentle petting or pressure on pin `A3`, immediately overriding lower-priority states to trigger happy/affectionate reactions.
- **Presence Awareness (PIR):** Monitors nearby movement on pin `D3` to wake from idle and shift into an alert or curious state.
- **Non-Blocking Event Architecture:** Driven by `millis()` timing intervals so frame updates never block incoming touch or motion events.

### Behavioral Priority Model

| Priority | Trigger Source | Pin | Companion State | Behavior |
| :--- | :--- | :--- | :--- | :--- |
| **1 (Highest)** | FSR Touch Sensor | `A3` | **Happy / Petted** | Overrides idle or curious moods with affectionate eye animations |
| **2** | PIR Motion Sensor | `D3` | **Curious / Alert** | Wakes from resting behavior and reacts to nearby presence |
| **3 (Default)** | Internal Timers | — | **Idle / Ambient** | Natural blinking, wandering gaze, and relaxed resting expressions |

---

## Hardware & Pinout

### Bill of Materials (Current Build)
- **Microcontroller:** Arduino Nano (ATmega328P)
- **Display:** 0.96" Monochrome OLED (SSD1306, 128×64, I2C)
- **Touch Sensor:** Force-Sensitive Resistor (FSR) + 10kΩ pull-down resistor (voltage divider)
- **Motion Sensor:** Passive Infrared (PIR) Sensor Module
- **Prototyping:** Breadboard & jumper wires

### Wiring Diagram

| Component | Module Pin | Arduino Nano Pin | Notes |
| :--- | :--- | :--- | :--- |
| **SSD1306 OLED** | `VCC` | `5V` (or `3.3V`) | Check display regulator tolerance |
| **SSD1306 OLED** | `GND` | `GND` | Common ground |
| **SSD1306 OLED** | `SDA` | `A4` | Hardware I2C Data |
| **SSD1306 OLED** | `SCL` | `A5` | Hardware I2C Clock |
| **FSR Sensor** | Signal (Divider Midpoint) | `A3` | Analog input (`analogRead`) |
| **PIR Sensor** | `OUT` | `D3` | Digital input (`digitalRead`) |

---

## Software Dependencies

Install the following libraries via the Arduino IDE Library Manager or PlatformIO before compiling `robo_eyes.ino`:

1. **Wire** (Built-in I2C library)
2. **Adafruit GFX Library**
3. **Adafruit SSD1306**
4. **FluxGarage_RoboEyes**

---


## 🗺️ Project Roadmap

### ✅ Phase 1 — Core Expressive Prototype (Current)

- [x] Integrate SSD1306 OLED display with `FluxGarage_RoboEyes`
- [x] Implement FSR (`A3`) touch sensing and PIR (`D3`) presence detection
- [x] Develop a non-blocking, priority-driven state machine in `robo_eyes.ino`
- [ ] Fine-tune sensor debouncing, cooldown timers, and mood decay curves
- [ ] Refactor the monolithic `.ino` sketch into modular `.h` and `.cpp` components

### 📡 Phase 2 — Wireless Migration & Hardware Polish

- [ ] Migrate from Arduino Nano to **Ai-M62-32S** (Wi-Fi/BLE MCU) for increased RAM, flash storage, and wireless connectivity
- [ ] Implement low-power sleep modes and stable voltage regulation for standalone operation
- [ ] Design and fabricate a compact desktop enclosure

### 🧠 Phase 3 — Connected Intelligence

- [ ] Integrate live weather data and time-of-day mood modifiers through Wi-Fi APIs
- [ ] Develop a lightweight LLM-backed interaction bridge for contextual responses and persistent companion memory
