# Injera Making Machine - System Architecture

**Version:** 1.0
**Status:** Draft
**Target:** Revision A (Standalone Controller)

---

# 1. Purpose

This document defines the overall system architecture of the Injera Making Machine.

It describes:

- Product architecture
- Controller architecture
- Hardware architecture
- Software architecture
- Deployment architecture
- Scalability strategy

This document is the highest-level technical document for the project.

---

# 2. Design Philosophy

The project shall be designed as a configurable automation platform rather than a single-purpose machine.

The same hardware and software platform shall support:

- Standalone machine
- Sequential multi-plate machine
- Parallel multi-plate machine

through configuration rather than redesign.

---

# 3. Product Architecture

```
               Injera Machine

           +------------------+
           |     Controller   |
           +------------------+
                     |
     +-------+-------+-------+-------+
     |       |       |       |       |
  Motion  Heating  Dispense Inputs Outputs
```

The controller coordinates all machine subsystems.

---

# 4. Deployment Modes

## Standalone

One controller operates one baking station.

```
Controller

↓

One Plate
```

---

## Sequential

One controller operates multiple stations sequentially.

A shared dispensing mechanism is transferred between plates.

```
Plate 1

↓

Plate 2

↓

Plate 3
```

Transfer motion is enabled.

---

## Parallel

Each station operates independently.

```
Station 1

Station 2

Station 3
```

Transfer motion is disabled.

Each station has its own dispensing mechanism.

---

# 5. Controller Architecture

The controller consists of the following modules.

```
Power

Digital Inputs

Digital Outputs

Motion Control

Temperature

MCU Interface

Safety
```

Each module has an independent schematic sheet.

---

# 6. Motion Architecture

The controller supports two configurable motion channels.

## Axis A

Purpose:

Dispensing rotation (Theta).

Required in every deployment.

---

## Axis B

Purpose:

Transfer motion.

Examples:

- Shared dispenser movement
- Plate indexing
- Future expansion

Optional.

The firmware determines whether Axis B is used.

---

# 7. MCU Interface

The controller shall not depend on a specific development board.

The MCU Interface is the contract between the controller hardware and the processor.

Revision A:

STM32 NUCLEO-F446RE

Revision B:

STM32F446RE directly mounted on PCB

Future:

Other STM32 families may implement the same interface.

---

# 8. Hardware Layers

```
Application

↓

Machine Functions

↓

MCU Interface

↓

STM32 HAL

↓

Hardware
```

---

# 9. Software Layers

```
Application

↓

Control System

↓

Motion

Thermal

Pump

Safety

↓

MCU Interface

↓

STM32 HAL
```

---

# 10. Safety

Safety shall always be independent from normal machine operation.

Safety functions include:

- Emergency stop
- Cover interlock
- Over-temperature
- Watchdog

Hardware safety shall always have priority over software.

---

# 11. Scalability

The architecture shall support:

Standalone

↓

Sequential

↓

Parallel

↓

Industrial Network

without redesign of the controller PCB.

---

# 12. Revision Strategy

Revision A

- NUCLEO-based
- Single controller
- Standalone validation
- Production-quality controller PCB

Revision B

- STM32 on PCB
- Optimized BOM
- Industrial enclosure

Revision C

- Multi-controller networking
- Industrial deployment
- Factory integration

---

# 13. Guiding Principles

1. One hardware platform
2. One firmware platform
3. Configurable deployment
4. Modular architecture
5. Design for debugging
6. Design for manufacturing
7. Design for scalability