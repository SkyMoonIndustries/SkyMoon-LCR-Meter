# DIY Precision LCR Meter ⚡📏

> **Historical Note:** *This project was born entirely out of necessity. Needing to measure specific inductance values for hardware prototyping but lacking the budget for professional LCR equipment, I engineered this measurement tool from scratch in a single 7-hour sitting using fundamental electronics physics and an Arduino Nano.*

This repository contains the source code and hardware architecture for a custom-built, OLED-equipped LCR (Inductance, Capacitance, Resistance) Meter. 

## 🧠 Engineering Concept & Use Case

When sourcing components for custom analog audio circuits, like distortion or compressor pedals, verifying exact capacitance and inductance values is critical before committing to a PCB layout. Off-the-shelf components often have wide tolerances that can drastically alter signal processing.

Instead of relying on nominal values, this tool uses fundamental time-constant and resonance mathematics to actively measure components. 

## 📐 Mathematical Principles & Architecture

The system utilizes three distinct measurement methodologies depending on the selected mode:
* **Resistance (R):** Utilizes a standard voltage divider circuit referencing a known 9.8kΩ precision resistor.
* **Capacitance (C):** Measures the RC time constant. A 2N2222 transistor is used to actively discharge the capacitor, after which the microcontroller times the charging phase up to a specific ADC threshold.
* **Inductance (L):** The core engineering challenge. The circuit pulses an LC tank (using a known 200nF reference capacitor). An LM339 comparator squares the resulting decaying sine wave. The Arduino measures the frequency of this oscillation using the `pulseIn` function, calculating the unknown inductance via the resonance formula: $L = \frac{1}{C (2\pi f)^2}$

## 🛠️ Hardware Wiring & Schematic

**Microcontroller:** Arduino Nano
**Display:** 0.96" I2C OLED (SDA -> A4, SCL -> A5)

**Resistance & Capacitance Module:**
* `+5V` -> `10kΩ Resistor` -> `A0` (Measurement Point 1)
* **Discharge Transistor (2N2222):** 
  * Emitter -> `GND`
  * Base -> `330Ω Resistor` -> `D7`
  * Collector -> `A0`

**Inductance Module (LM339 Comparator):**
* `LM339 Out 1` -> `+5V` (via `330Ω` pull-up) AND -> `D11` (Measurement Pin)
* `LM339 +In 1` -> Measurement Point 2
* `LM339 +In 1` -> `200nF Ceramic Cap` -> `GND`
* `LM339 +In 1` -> Cathode of `1N4007` (Anode -> `150Ω Resistor` -> `D13` Charge Pin)
* `LM339 -In 1` -> `GND`

**User Interface:**
* Mode Toggle Button -> `D2`
* Measure Execute Button -> `D3`

## 🚀 Operation

1. Power on the device.
2. Use the Mode Button (D2) to cycle between Resistance, Capacitance, and Inductance modes.
3. Place the unknown component across the respective measurement points.
4. Press the Measure Button (D3) to execute the calculation and render the result on the OLED.
