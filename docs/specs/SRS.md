
## 2) SRS — System Requirements Specification

### 2.1 Definitions

* **θ‑axis:** rotational axis of baking plate.
* **Trim valve:** per‑branch flow adjuster downstream of the manifold.
* **CIP:** cleaning‑in‑place routine for wetted parts.

### 2.2 Functional Requirements (Fxx)

**F‑01** The system shall rotate the plate at **1.0 rev/s** (±1%) during dispense.
**F‑02** The system shall deposit batter via **7 nozzles** forming concentric rings.
**F‑03** The system shall control the pump to deliver **28.3 mL/s** (±5%) total during the 5 s dispense.
**F‑04** The system shall provide per‑nozzle **trim adjustment** to achieve target **mL/s** proportional to ring radius.
**F‑05** The system shall regulate plate temperature to **210–230 °C** with **±3 °C** steady‑state error.
**F‑06** The system shall home the θ‑axis using a hub sensor and prevent motion if homing fails.
**F‑07** The system shall provide an HMI to set temperature, start/stop cycle, and display alarms/status.
**F‑08** The system shall log temperature, cycle count, and alarms once per second (min.).
**F‑09** The system shall support **CIP mode** (pump rinse sequence without heat).
**F‑10** The system shall expose a serial/USB debug interface for service.

### 2.3 Safety Requirements (Sxx)

**S‑01** The system shall implement an **E‑stop** with **two NC channels** that **hard‑disable** θ‑enable and SSR control.
**S‑02** The system shall implement a **cover interlock** (NC) in series with the safety chain.
**S‑03** The system shall include a **thermal cutoff** device in series with the heater.
**S‑04** The MCU shall mirror safety states but shall **not** be required to remove power.

### 2.4 Performance Requirements (Pxx)

**P‑01** Cycle time per injera: **≤5.0 s**; jitter **≤0.2 s** after warm‑up.
**P‑02** Plate temperature stability: **±3 °C** at load steps of ±25% flow.
**P‑03** Flow accuracy: total **±5%**; per‑ring **±10%** after trim.
**P‑04** Repeatability: diameter **±2 mm**, thickness **±0.3 mm**.
**P‑05** Noise: < **75 dBA** at 1 m during steady operation (goal).

### 2.5 Environmental & Regulatory (Exx)

**E‑01** Operating: **15–35 °C**, 20–80% RH non‑condensing.
**E‑02** Materials: food‑contact **SS304/316**, food‑grade tubing.
**E‑03** Electrical: earthing per applicable standards; segregated AC/24 V wiring.
**E‑04** EMC/LVD readiness for CE marking (documentation in technical file).

### 2.6 Interfaces (Ixx)

**I‑01 Mechanical**: Plate Ø450 mm, nozzle arm with **7 outlets**, manifold with **7 equal‑length branches**.
**I‑02 Electrical**: 230 VAC mains; 24 VDC bus for controls; terminal blocks on NS35 rail.
**I‑03 Sensors/Actuators**: PT100 (3‑wire) to MAX31865 (SPI), SSR output, TB6600 STEP/DIR/EN, θ home switch, LEDs, buzzer, E‑stop, cover switch.
**I‑04 HMI**: 24 V panel or UART3 (115200 8N1); minimal Modbus/serial protocol optional.

### 2.7 Data & Logging (Dxx)

**D‑01** Log at 1 Hz: plate temp, pump duty, θ RPM, E‑stop/cover states, cycle count, faults.
**D‑02** Store last **1,000 cycles** in non‑volatile memory or export via UART.

### 2.8 Reliability & Maintainability (Rxx)

**R‑01** MTBF target for electronics: **>20,000 h** (bench‑top estimate).
**R‑02** Replaceable parts: SSR, RTD probe, nozzles, tubing, stepper driver, NEMA23 motor, E‑stop head.
**R‑03** CIP time: **≤30 min**; tool‑less nozzle removal preferred.

### 2.9 Verification (Vxx) — Testable Criteria

* **V‑01:** Demonstrate 10 consecutive cycles at **≤5.0 s** with thickness in spec.
* **V‑02:** Step change ±25% load; hold **±3 °C** at plate.
* **V‑03:** Measure per‑nozzle flow vs. targets; each within **±10%** after trimming.
* **V‑04:** E‑stop opens both NC channels; θ‑EN and SSR_CTRL low within **<50 ms**.
* **V‑05:** Cover interlock halts motion/heat; alarm on HMI.
* **V‑06:** Thermal cutoff opens at rated temperature.
* **V‑07:** EMC pre‑scan passes (no functional upset); L/N leakage within limits.

### 2.10 Dependencies & Risks

* Batter viscosity variability → mitigated by trim valves and recipe calibration.
* Thermal gradients across plate → mitigated by sensor placement and insulation.
* Pump wear/tubing set → mitigated with PM schedule and spare kits.

### 2.11 Acceptance

This SRS is accepted when the verification items **V‑01..V‑07** are met on a pilot machine, and KPIs in the PRD section **1.4** reach targets during a full 8‑hour run.

---
**End of PRD v1.0 Draft**
