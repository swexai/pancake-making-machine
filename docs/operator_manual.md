# Operator Manual: Injera-Making Machine v1.0

## Table of Contents
1. [Introduction](#1-introduction)
2. [Safety Instructions](#2-safety-instructions)
3. [Installation and Setup](#3-installation-and-setup)
4. [Operation Procedures](#4-operation-procedures)
5. [Maintenance and Cleaning](#5-maintenance-and-cleaning)
6. [Troubleshooting](#6-troubleshooting)
7. [Technical Specifications](#7-technical-specifications)
8. [Warranty and Support](#8-warranty-and-support)

---

## 1. Introduction

### 1.1 Overview
The Injera-Making Machine is an automated food production system designed for commercial kitchens and bakeries. It produces consistent, high-quality Ø30 cm injera (Ethiopian flatbread) with uniform thickness and traditional "eyes" pattern.

### 1.2 Key Features
- **Automated Production**: Single-operator continuous production
- **Precise Control**: Temperature regulation to ±3°C, flow control for uniform batter distribution
- **Safety-First Design**: Dual-channel E-stop, cover interlock, and thermal cutoff
- **Easy Operation**: Simple HMI interface with status indicators
- **Sanitation**: Built-in CIP (Cleaning-in-Place) system
- **Reliability**: Designed for ≥95% uptime during 8-hour shifts

### 1.3 Intended Use
This machine is intended for use in commercial food production environments by trained operators. It is not suitable for domestic use.

---

## 2. Safety Instructions

**WARNING: Read all safety instructions before operating the machine. Failure to follow these instructions may result in serious injury or death.**

### 2.1 General Safety
- **Electrical Hazard**: Machine operates on 230 VAC. Ensure proper grounding and qualified electrical installation.
- **Thermal Hazard**: Baking plate reaches 230°C. Never touch hot surfaces. Allow adequate cooling time.
- **Mechanical Hazard**: Moving parts include rotating plate and pump mechanisms. Keep hands clear during operation.
- **Food Safety**: Use only food-grade materials. Follow sanitation procedures.

### 2.2 Safety Features
- **Emergency Stop (E-Stop)**: Red mushroom button on control panel. Press immediately to stop all operations.
- **Cover Interlock**: Machine stops automatically if safety cover is opened.
- **Thermal Cutoff**: Automatic shutdown if temperature exceeds safe limits.
- **Safety Chain**: Hardware-based safety system independent of software.

### 2.3 Operating Precautions
- Always wear appropriate PPE: heat-resistant gloves, apron, and safety glasses.
- Keep work area clear of obstructions and flammable materials.
- Never operate with safety guards removed.
- Do not modify or bypass safety systems.
- Supervise operation at all times.

### 2.4 Emergency Procedures
1. **Power Failure**: Machine will stop safely. Restart after power restoration.
2. **Overheat**: Thermal cutoff will activate. Allow cooling before investigation.
3. **Mechanical Failure**: Press E-stop. Contact service technician.
4. **Fire**: Disconnect power, use appropriate fire extinguisher (Class C for electrical).

---

## 3. Installation and Setup

### 3.1 Unpacking and Inspection
1. Inspect packaging for damage upon delivery.
2. Verify all components are present:
   - Main machine unit
   - Power cable (230 VAC)
   - Control panel
   - Nozzle arm assembly
   - Documentation package
3. Report any damage to supplier immediately.

### 3.2 Location Requirements
- **Space**: Minimum 1m clearance on all sides for ventilation and access
- **Surface**: Level, stable surface capable of supporting 50kg
- **Environment**: Indoor use only, 15-35°C ambient temperature, 20-80% RH non-condensing
- **Ventilation**: Adequate exhaust for heat and steam
- **Power**: Dedicated 230 VAC circuit, 10A minimum, properly grounded

### 3.3 Electrical Connection
**WARNING: Electrical work must be performed by qualified electrician only.**

1. Verify power supply matches machine requirements (230 VAC, 50/60 Hz)
2. Install appropriate circuit breaker and RCD (Residual Current Device)
3. Connect power cable to dedicated outlet
4. Test grounding continuity

### 3.4 Initial Setup
1. **Mechanical Setup**:
   - Position machine on stable surface
   - Install nozzle arm assembly
   - Verify all tubing connections are secure
   - Check trim valves are in neutral position

2. **Power-On Sequence**:
   - Connect power cable
   - Turn on main power switch
   - Verify control panel illuminates
   - Check for any error indicators

3. **Calibration**:
   - Home the θ-axis (rotational axis)
   - Set initial temperature setpoint
   - Perform flow calibration if needed

### 3.5 Axis Homing Procedure

**IMPORTANT: The θ-axis must be homed before each production session to ensure accurate plate positioning.**

The homing procedure establishes the reference position for the rotating baking plate:

1. **Initiate Homing**:
   - Press "Home Axis" button on control panel, or
   - Send 'H' command via serial interface

2. **Homing Process**:
   - Motor ramps to slow speed (30 RPM)
   - Plate rotates until home sensor detects reference position
   - System sets position to zero and confirms homing complete
   - Motor ramps to operating speed (60 RPM = 1.0 rev/s)

3. **Verification**:
   - HMI displays "HOMED" status
   - System allows production cycles to begin
   - Homing takes approximately 5-10 seconds

**Note**: If homing fails (timeout after 10 seconds), check for mechanical obstructions and retry. Contact service if problem persists.

---

## 4. Operation Procedures

### 4.1 Daily Startup
1. **Pre-Operation Check**:
   - Verify power connection and grounding
   - Check safety cover is closed
   - Inspect nozzle arm for obstructions
   - Ensure batter reservoir is clean and filled

2. **Power On**:
   - Turn on main power switch
   - Wait for system initialization (approximately 30 seconds)
   - Verify all status indicators are normal

3. **Warm-Up**:
   - Set temperature setpoint (default: 220°C)
   - Allow 15-20 minutes for plate to reach operating temperature
   - Monitor temperature via HMI display

4. **Axis Homing**:
   - Perform θ-axis homing procedure (see Section 3.5)
   - Verify "HOMED" status before proceeding
   - Do not start production cycles until homing is complete

### 4.2 Normal Operation
1. **Load Batter**:
   - Prepare batter according to recipe specifications
   - Fill reservoir to recommended level
   - Ensure batter viscosity is within specifications

2. **Start Production Cycle**:
   - Press "Start Cycle" button or send 'S' command via serial interface
   - Machine will:
     - Rotate plate at 1.0 rev/s
     - Dispense batter through 7 nozzles for 5 seconds
     - Maintain temperature regulation
   - Cycle completes in ≤5.0 seconds

3. **Remove Product**:
   - Wait for cycle completion indicator
   - Use heat-resistant tools to remove injera
   - Allow cooling if necessary

4. **Continuous Operation**:
   - Repeat cycle as needed
   - Monitor system status regularly
   - Maintain batter level in reservoir

### 4.3 Recipe Adjustments
- **Temperature**: Adjust via HMI (210-230°C range)
- **Flow Rate**: Use trim valves for per-nozzle adjustment
- **Cycle Time**: Fixed at 5 seconds (not user-adjustable)

### 4.4 Shutdown Procedure
1. **End of Production**:
   - Complete current cycle if in progress
   - Allow plate to cool to safe temperature (<50°C)
   - Turn off main power switch

2. **Extended Shutdown**:
   - Perform CIP cleaning cycle
   - Drain and clean batter system
   - Store in clean, dry environment

---

## 5. Maintenance and Cleaning

### 5.1 Daily Maintenance
- **Visual Inspection**: Check for leaks, damage, or unusual wear
- **Clean Exterior**: Wipe down with food-safe sanitizer
- **Check Connections**: Verify all tubing and electrical connections
- **Log Operation**: Record cycle count and any issues

### 5.2 Cleaning-in-Place (CIP)
The machine includes an automated CIP system for thorough cleaning:

1. **Prepare CIP Solution**: Mix appropriate food-safe cleaning solution
2. **Initiate CIP Mode**: Press CIP button or send 'C' command
3. **Monitor Process**: System circulates cleaning solution for 15-20 minutes
4. **Rinse**: System automatically rinses with clean water
5. **Complete**: Process takes ≤30 minutes total

### 5.3 Weekly Maintenance
- **Deep Clean Nozzles**: Remove and soak in cleaning solution
- **Check Flow Rates**: Verify per-nozzle flow calibration
- **Inspect Heater**: Check for carbon buildup or damage
- **Lubricate Moving Parts**: Apply food-grade lubricant as needed

### 5.4 Monthly Maintenance
- **Calibration Check**: Verify temperature and flow accuracy
- **Safety System Test**: Test E-stop and interlocks
- **Filter Replacement**: Replace any inline filters
- **Software Update**: Check for firmware updates
- **Homing System Check**: Verify home sensor operation and alignment

### 5.5 Spare Parts
Keep these parts on hand:
- Nozzle tips (set of 7)
- Tubing sections
- Trim valves
- Temperature sensor (PT100)
- SSR (Solid State Relay)

---

## 6. Troubleshooting

### 6.1 Common Issues and Solutions

| Symptom | Possible Cause | Solution |
|---------|---------------|----------|
| Machine won't start | Power not connected | Check power cable and outlet |
| | E-stop engaged | Reset E-stop button |
| | Safety cover open | Close safety cover |
| Temperature not reaching setpoint | Heater failure | Check SSR and heating element |
| | Sensor malfunction | Calibrate or replace PT100 |
| Uneven batter distribution | Clogged nozzles | Clean or replace nozzles |
| | Flow imbalance | Adjust trim valves |
| Plate not rotating | Motor failure | Check stepper driver and motor |
| | Homing failure | Re-home θ-axis |
| Error codes displayed | See error code table | Follow specific troubleshooting |

### 6.2 Error Codes

| Code | Description | Action |
|------|-------------|--------|
| E-01 | Temperature sensor fault | Check PT100 connection |
| E-02 | Motor driver fault | Verify stepper driver power |
| E-03 | Pump fault | Check pump wiring and power |
| E-04 | Safety chain open | Inspect E-stop and cover switch |
| E-05 | Over-temperature | Allow cooling, check thermal cutoff |
| E-06 | Communication fault | Reset system or check serial connection |
| E-07 | Homing timeout | Check home sensor, clear obstructions, retry homing |

### 6.3 Homing-Specific Troubleshooting

**Homing is critical for accurate plate positioning during batter dispensing.**

**Symptoms of Homing Issues:**
- "NOT HOMED" status displayed
- Plate rotation erratic or incorrect speed
- Production cycles fail to start
- E-07 error code

**Common Homing Problems:**

1. **Home Sensor Not Detected**:
   - Check home sensor (hub switch) connection
   - Verify sensor alignment with reference mark on plate
   - Clean sensor if dirty or obstructed

2. **Mechanical Obstruction**:
   - Inspect plate rotation path for debris
   - Check motor coupling and belt tension
   - Ensure plate is properly seated

3. **Motor or Driver Issues**:
   - Verify stepper motor power (24V supply)
   - Check TB6600 driver connections
   - Listen for unusual motor sounds during homing

4. **Timeout During Homing**:
   - Allow full 10-second timeout period
   - Check if plate can rotate freely by hand
   - Reset system and retry homing

**Homing Recovery Procedure:**
1. Press E-stop to ensure safe state
2. Clear any mechanical obstructions
3. Power cycle the system
4. Retry homing procedure
5. If persistent, contact service technician
Contact authorized service technician for:
- Electrical faults
- Mechanical failures
- Persistent error codes
- Performance degradation
- Safety system malfunctions

---

## 7. Technical Specifications

### 7.1 Performance
- **Cycle Time**: ≤5.0 seconds per injera
- **Product Size**: Ø30 cm diameter
- **Thickness**: 2.0 mm ± 0.3 mm
- **Temperature Range**: 210-230°C (±3°C regulation)
- **Flow Rate**: 28.3 mL/s total (±5%)
- **Rotation Speed**: 1.0 rev/s (±1%)

### 7.2 Electrical
- **Power Supply**: 230 VAC, 50/60 Hz, 10A
- **Control Voltage**: 24 VDC
- **Power Consumption**: 150-240 W operating

### 7.3 Mechanical
- **Dimensions**: 600mm W × 600mm D × 400mm H
- **Weight**: 45 kg
- **Plate Diameter**: 450 mm
- **Nozzles**: 7 concentric ring outlets

### 7.4 Environmental
- **Operating Temperature**: 15-35°C
- **Humidity**: 20-80% RH non-condensing
- **Storage Temperature**: 5-40°C

### 7.5 Safety Standards
- Dual-channel E-stop (2NC contacts)
- Cover interlock safety system
- Thermal cutoff protection
- CE marking ready (EMC/LVD compliant)

---

## 8. Warranty and Support

### 8.1 Warranty
- **Duration**: 1 year from date of installation
- **Coverage**: Manufacturing defects in materials and workmanship
- **Exclusions**: Consumable parts, misuse, unauthorized modifications
- **Service**: On-site repair or replacement at manufacturer's discretion

### 8.2 Support
- **Technical Support**: Contact manufacturer for assistance
- **Training**: Operator training available upon request
- **Documentation**: Updates available at manufacturer's website
- **Service Contracts**: Extended warranty and maintenance contracts available

### 8.3 Contact Information
- **Manufacturer**: Maya Africa Tours
- **Location**: Gothenburg, Sweden
- **Email**: support@injera-machine.com
- **Phone**: +46-XX-XXX-XXXX

---

**Document Version**: 1.0  
**Date**: April 14, 2026  
**Revision History**: Initial release</content>
<parameter name="filePath">/workspaces/pancake-making-machine/docs/operator_manual.md