# NV9 Universal Payment Controller

This project is a universal interface designed to connect an industrial NV9 Bank Note Validator to an Arduino microcontroller. The goal was to create a flexible payment controller that can be integrated into any automated system—from massage chairs and coffee machines to phone charging stations or car wash timers.

## Project Scope & Utility

While many payment systems are locked to specific machines, I designed this controller to be modular. It acts as a "bridge" between the payment hardware and the device it controls.

By using a relay module as the output, this system can control virtually any electrical appliance based on the amount of money inserted.

**Potential Use Cases:**
* **Massage Chairs in Malls:** Activates the chair for a set duration when a banknote is inserted.
* **Vending/Coffee Machines:** Enables the dispensing mechanism.
* **Phone Charging Stations:** Turns on the 5V power rail for a specific time.
* **Lighting/Car Wash:** Controls high-voltage solenoids or lights for timed services.

## How It Works (Pricing Logic)

The core logic of the system interprets the data stream from the NV9 validator and converts it into "Time Credits." Instead of a fixed timer, I implemented a tiered system that detects the value of the banknote and assigns the corresponding duration.

Currently, the code is configured for the following logic:

* **1 RON (Tier 1):** Activates the relay for **5 Minutes**.
* **5 RON (Tier 2):** Activates the relay for **30 Minutes**.
* **10 RON (Tier 3):** Activates the relay for **60 Minutes**.

*Note: These values are fully configurable in the code variables (`DURATION_X_MIN`), making it easy to adapt the pricing model for different businesses.*

## Technical Implementation

### Protocol (SIO)
I chose to use the **Simple Serial Input/Output (SIO)** protocol at 9600 baud. This allows the Arduino to receive digital data packets from the NV9 rather than relying on noisy pulse counting. This ensures that the system doesn't accidentally trigger if there is electrical interference on the line.

### Signal Aggregation
To handle the bursty nature of the data transmission, the software uses a "Signal Aggregation" window. It listens for the data stream, buffers the input for a few seconds to ensure the full value is read, and then executes the logic. This prevents the system from triggering twice for the same banknote.

### Non-Blocking Timer
Since this device is meant to control real-world appliances for long periods (up to days on end), I used a non-blocking timer architecture (`millis()`). This allows the microcontroller to keep monitoring the validator for new notes or status updates even while the relay is active.

## Hardware Setup & Documentation

I have included detailed images of the pin layouts and wiring diagrams in the **[`/docs`](./docs)** folder of this repository.

**Quick Connection Guide:**
* **Arduino Pin 3:** Connected to NV9 Transmit (Data Line).
* **Arduino Pin 4:** Connected to the Relay Module (Control Pin).
* **Power:** The system requires a common ground between the 12V power supply (for the NV9) and the Arduino.

## ⚡ Circuit & Power Architecture

The system is designed around a centralized power distribution topology, converting mains voltage (220V AC) into logic-level DC voltages for the control electronics.

### 1. Power Supply Unit (PSU)
The entire logic circuit is powered by a single industrial **AC/DC Switching Power Supply (12V / 5A)**.
* **Input:** 220V AC (Mains).
* **Output:** 12V DC.
* **Role:** Provides sufficient current for the NV9 motors (which can draw peak currents during note stacking) and regulates power for the Arduino.

### 2. Component Wiring Strategy

#### **A. NV9 Validator (12V Rail)**
The validator requires a stable 12V source to operate its internal motors and sensors.
* **V+ (Red):** Connected directly to the PSU 12V output.
* **GND (Black):** Connected to the PSU GND (Common Ground).

#### **B. Arduino Uno (Logic Controller)**
The microcontroller is powered via the `Vin` pin, taking advantage of the onboard voltage regulator.
* **Vin:** Connected to the PSU 12V rail.
* **GND:** Connected to Common Ground.
* *Note: Using a common ground between the NV9 and Arduino is critical for the UART serial communication to function correctly.*

#### **C. Relay Module (The Actuator)**
I used a **5V Active LOW Relay Module** to control the external appliance (e.g., Phone Charger, Massage Chair).
* **Low Voltage Side:**
    * **VCC:** 5V (from Arduino 5V pin).
    * **GND:** Arduino GND.
    * **IN:** Arduino Pin 4.
* **High Voltage Side (220V):** The relay acts as a switch on the **Live (L)** wire of the circuit powering the target device.
    * **COM (Common):** Mains 220V Live input.
    * **NO (Normally Open):** Connected to the Device/Charger Live input.

### ⚠️ Safety & Isolation
The circuit separates the Low Voltage Logic (12V/5V) from the High Voltage Load (220V). The relay module provides **galvanic isolation** via an optocoupler, ensuring that the high-voltage mains current cannot flow back into the microcontroller, protecting the logic board from spikes.

> **Visual Reference:** Please see the `docs/` folder for the complete schematic and pinout diagrams.

The current version supports basic time-based activation. A future update could include an LCD display to show the remaining time to the user and a "cumulative" feature to stack time for the user to see (e.g., inserting two 1 RON notes for 10 minutes).

**Author:** Cioromila Mario Daniel