# POC Hardware Specification & Bill of Materials

## 1) Purpose
This document defines the proof-of-concept hardware architecture, key specifications, and the Bill of Materials for the pancake/injera-making machine prototype.

## 2) Scope
The POC is intended to demonstrate the machine's core functions:
- heated plate temperature control
- rotational motion of the nozzle arm
- batter dispense through 7 concentric nozzles
- hardware safety chain (E-stop + cover interlock)
- 24 VDC control electronics with STM32-based firmware

## 3) System Overview
The prototype uses a single-axis rotating nozzle arm over a heated plate.
The STM32F446RE microcontroller supervises three subsystems:
- Thermal control: PT100 sensor + MAX31865 + SSR-driven heater
- Motion control: STEP/DIR/EN output → TB6600 driver → NEMA 23 stepper
- Dispense control: PWM-controlled pump feeding a 7-way manifold

Power architecture is split into:
- 230 VAC mains for heater and main power feed
- 24 VDC control power for logic, sensors, drivers, and pump

Safety is implemented as a hardware safety chain that can de-energize both the stepper enable and the heater SSR independent of firmware.

## 4) Key POC Hardware Specifications
- Plate temperature: 210–230 °C, ±3 °C regulation
- Rotation speed: 1.0 rev/s with homing and jerk-limited ramps
- Dispense cycle: 5.0 s total, ≈141 mL total flow (~28.3 mL/s)
- Injection geometry: 7 concentric nozzles, equal-length fluid paths
- Control voltage: 24 VDC
- Mains input: 230 VAC, 50/60 Hz
- Estimated operating power: 150–240 W

## 5) Bill of Materials
### Electrical / Electronic Components
| Category | Item | Quantity | Notes |
|---|---|---|---|
| Control MCU | STM32F446RE development board or custom STM32F446RE PCB | 1 | MCU running FreeRTOS firmware |
| Temperature sensing | PT100 RTD probe | 1 | Food-safe probe or plate-embedded sensor |
| Temperature interface | MAX31865 RTD-to-Digital Converter | 1 | SPI-connected |
| Power switching | 230 VAC SSR, 25 A | 1 | Heater drive, zero-cross or random-phase depending on heating element |
| Thermal protection | Thermal cutoff / thermal fuse | 1 | 250 °C rating or appropriate safety cutoff |
| Motion driver | TB6600 stepper motor driver | 1 | STEP/DIR/EN inputs |
| Motion actuator | NEMA 23 stepper motor | 1 | torque sized to rotate nozzle arm smoothly |
| Power supply | Mean Well HDR-150-24, 24 VDC, 6.5 A | 1 | DIN-rail power supply for control power and pump power |
| Power input | AC input connector / terminal block | 1 | 230 VAC mains connection |
| Status panel | Status LEDs, buzzer, buttons, and/or small display | 1 | operator status and alarm interface |
| Indicators | LEDs & buzzer | 1 set | visual/audio status and alarm feedback |
| Safety I/O | Safety I/O terminal blocks | as required | wired connections for E-stop, cover interlock, and safety chain |
| Communication | UART / USB connector | 1 | firmware debug/log interface |
| Wiring | 24 VDC, signal, and mains wiring | as required | appropriate gauge for current and safety |
| Connectors | Screw terminals / headers | as required | for sensors, safety inputs, motor, pump |
| Grounding | Protective earth wiring and terminals | as required | essential for operator safety |

### Mechanical / Structural Components
| Category | Item | Quantity | Notes |
|---|---|---|---|
| Motion coupling | Motor flange / shaft coupling | 1 | to connect motor to nozzle arm drive |
| Pump | DC or PWM-compatible pump | 1 | meets ~28 mL/s output at operating pressure |
| Fluid control | 7-way dispense manifold | 1 | equal-length branch design preferred |
| Nozzles | Food-grade dispense nozzles | 7 | concentric ring pattern |
| Mechanical | Heated plate assembly | 1 | stainless steel heating surface |
| Mechanical | Rotating nozzle arm assembly | 1 | supports 7 nozzles and pump supply lines |
| Chassis | Enclosure or frame | 1 | supports electrical, mechanical, and safety components |
| Cable management | Cable ties, conduit, cable carriers | as required | keep wiring organized and isolated |

## 6) Mechanical Design
- Heated plate: food-safe stainless steel surface with embedded heater and PT100 sensor pocket.
- Nozzle arm: rigid rotating arm with seven concentric dispense outlets and equal-length flexible tubing paths.
- Motion drive: NEMA23 stepper coupled to arm via a flange/shaft coupling, with bearings or bushings to support smooth rotation.
- Frame: robust chassis that isolates 230 VAC heater wiring from 24 VDC control electronics and provides grounding.
- Enclosure: protective cover with interlock switch and easy access for maintenance, while keeping moving parts shielded.
- Flow manifold: fixed 7-way manifold, ideally with manual trim valves per branch to balance ring flow.
- Service access: front or top panel for status indicators, E-stop, cover opening, and maintenance access.

## 7) Procured Components
The following items are already acquired for the POC:
- TB6600 stepper driver for θ-axis motion control
- NEMA 23 stepper motor for θ-axis drive
- Mean Well HDR-150-24 24 VDC DIN-rail power supply
- Safety I/O terminal blocks for E-stop and cover interlock wiring
- LED status indicators and buzzer for operator feedback
- PT100 RTD temperature probe (food-contact or plate-embedded)

## 8) Notes for POC Implementation
- Use off-the-shelf modules where possible for rapid prototyping: STM32 dev board, TB6600, SSR module, MAX31865 breakout.
- Keep the safety chain separate from firmware. The hardware chain should directly cut the heater SSR and the stepper enable line.
- Build the nozzle manifold with equal-length tubing to minimize flow imbalance across the 7 nozzles.
- Validate the heater control with a thermal cutoff device and double-check sensor wiring before power-up.

## 9) Recommended POC Enhancements
- Add a dedicated 24 VDC isolation barrier for the safety chain.
- Include mechanical flow adjustment/trimming valves on each nozzle branch.
- Use a food-grade pump and tubing rated for the batter viscosity.
- Add a simple status panel showing: power, heat, motion enabled, E-stop, cover open, and fault state.

## 10) Document Revision
- Version: 1.0
- Date: April 26, 2026
- Author: Pancake Making Machine POC team
