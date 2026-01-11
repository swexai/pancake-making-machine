# Documentation Index

- **PRD:** [specs/PRD.md](specs/PRD.md)
- **SRS:** [specs/SRS.md](specs/SRS.md)

## Architecture
- System Overview  
  ![System Overview](docs/architecture/png/system_overview.png)
**What it shows:** End-to-end blocks: HMI → STM32 → (MAX31865/PT100, SSR+Heater+Plate, TB6600+Motor, Pump→Manifold→Nozzles) with the hardware safety cut.  
**Key signals:** UART, SPI (RTD), SSR_CTRL (10 Hz), STEP/DIR/EN, PWM, safety hard-cut of theta-EN & SSR.  
**Why it matters:** One glance view for new contributors and vendors; anchors all detailed diagrams.

- Power & Safety  
  ![Power Single-line](docs/architecture/png/power_singleline.png)
   Mains → EMI filter → MCB → RCD.  
   Two branches: (a) 24 V SMPS for controls; (b) AC heater through SSR → thermal cutoff → heater.  
   Hardware safety chain (E-STOP CH1/CH2 + cover NC) hard-cuts theta-EN and SSR control regardless of firmware.  
   _See:_ `architecture/png/power_singleline.png` (src: `architecture/power_singleline.puml`)
- Nozzle Arm & Rings  
  ![Nozzle & Rings](docs/architecture/png/nozzle_and_rings.png)

## Control Design (modular)


# System Flow (End-to-End)

1. **Power & safety comes first**  
   Mains → EMI filter → MCB → RCD.  
   Two branches: (a) 24 V SMPS for controls; (b) AC heater through SSR → thermal cutoff → heater.  
   Hardware safety chain (E-STOP CH1/CH2 + cover NC) hard-cuts theta-EN and SSR control regardless of firmware.  
   _See:_ `architecture/png/power_singleline.png` (src: `architecture/power_singleline.puml`)
  ![Safety](docs/architecture/png/control_safety_chain.png)

2. **Boot & supervision**  
   STM32 boots FreeRTOS tasks: Safety, TempCtrl (PID), ThetaCtrl, PumpCtrl, HMI/Logger.  
   HMI provides setpoints (temp, RPM, flow/CIP), shows alarms; Logger records 1 Hz data.  
   _See:_ `architecture/png/system_overview.png`, `architecture/png/control_overview_modules.png`
  ![Overview](docs/architecture/png/control_overview_modules.png)

3. **Warm-up / temperature loop**  
   PT100 → MAX31865 (SPI) → TempCtrl (PID) computes duty.  
   MCU drives SSR with 10 Hz time-proportioning; heater warms plate; sensor closes loop.  
   Target: ±3 °C regulation at load steps.  
   _See:_ `architecture/png/control_temp_loop.png`
  ![Temp Loop](docs/architecture/png/control_temp_loop.png)

4. **Homing & theta-axis control**  
   ThetaCtrl homes on index sensor, then runs 1.0 rev/s with jerk-limited ramps.  
   MCU outputs STEP/DIR/EN → TB6600 → NEMA23.  
   Safety chain can disable EN in hardware at any time.  
   _See:_ `architecture/png/control_theta_motion.png`
  ![Theta Motion](docs/architecture/png/control_theta_motion.png)

5. **Dispense / pump path**  
   MCU sets pump PWM (and RUN↔CIP valve if fitted).  
   Single pump → 7-way manifold → per-branch trim → check valve → equal-length tube → nozzle tip.  
   Per-ring flows follow radius-proportional targets so outer rings get more mL/s.  
   _See:_ `architecture/png/pump_overview_horizontal.png`, `architecture/png/control_pump_dispense.png`  
   _Pattern geometry:_ `architecture/png/nozzle_and_rings.png`
  ![Pump](docs/architecture/png/control_pump_dispense.png)

6. **Bake cycle**  
   While theta = 1.0 rev/s, PumpCtrl dispenses ≈28.3 mL/s for 5 s (~141 mL total).  
   Plate at setpoint; Safety monitors E-STOP/cover; any trip → heater off + theta disabled (hardware), alarms logged.

7. **Finish, unload, repeat**  
   Pump stops; theta may coast/stop per recipe; operator removes injera.  
   Logger keeps temp/RPM/duty/faults/cycle count at 1 Hz.

8. **CIP mode**  
   HMI selects CIP: pump runs rinse/detergent/sanitize via 3-way valve; heater off; safety chain still active.

---

## Diagram Index

- **System Overview** — `architecture/png/system_overview.png` (src: `architecture/system_overview.puml`)  
- **Power & Safety** — `architecture/png/power_singleline.png` (src: `architecture/power_singleline.puml`)  
- **Nozzle & Rings** — `architecture/png/nozzle_and_rings.png` (src: `architecture/nozzle_and_rings.puml`)  
- **Pump Overview (Horizontal)** — `architecture/png/pump_overview_horizontal.png` (src: `architecture/pump_overview_horizontal.puml`)  
- **Control Overview (Modules)** — `architecture/png/control_overview_modules.png` (src: `architecture/control_overview_modules.puml`)  
- **Temperature Control Loop** — `architecture/png/control_temp_loop.png` (src: `architecture/control_temp_loop.puml`)  
- **Theta Motion** — `architecture/png/control_theta_motion.png` (src: `architecture/control_theta_motion.puml`)  
- **Pump / Dispense Control** — `architecture/png/control_pump_dispense.png` (src: `architecture/control_pump_dispense.puml`)  
- **Safety Chain** — `architecture/png/control_safety_chain.png` (src: `architecture/control_safety_chain.puml`)  
- **HMI & Logger** — `architecture/png/control_hmi_logger.png` (src: `architecture/control_hmi_logger.puml`)
