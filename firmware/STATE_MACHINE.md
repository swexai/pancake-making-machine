# State Machine and Control Flow

## System State Machine Diagram

```
                    ┌─────────────────────────────────┐
                    │           START / IDLE           │
                    │  - All subsystems powered off    │
                    │  - Waiting for operator command  │
                    └────────────────┬──────────────────┘
                                     │
                    ┌────────────────┴──────────────────┐
                    │  Operator Command: T (warmup)    │
                    └─────────────────┬──────────────────┘
                                      │
                    ┌─────────────────▼──────────────────┐
                    │            WARMUP                  │
                    │  - SSR heater enabled              │
                    │  - Temperature rising              │
                    │  - PID control active              │
                    │  - Wait for setpoint ± 3°C         │
                    └─────────────────┬──────────────────┘
                                      │
                            [T ≥ Tset - 3°C]
                                      │
                    ┌─────────────────▼──────────────────┐
                    │            READY                   │
                    │  - Plate at stable temperature    │
                    │  - Awaiting dispense command      │
                    │  - SSR maintaining setpoint       │
                    │  - Motor homed                    │
                    └─────────────────┬──────────────────┘
                                      │
          ┌───────────────────────────┼───────────────────┐
          │                           │                   │
    [DISPENSE Cmd]          [CIP Cmd]  │            [Idle Loop]
          │                     │      │                   │
    ┌─────▼──────┐      ┌──────▼────┐ │    ┌─────────────▼─────┐
    │  DISPENSE  │      │   CIP     │ │    │    (stay READY)   │
    │            │      │           │ │    └───────────────────┘
    │ Θ = 1 rev/s│      │ Pump on   │ │
    │ Pump on    │      │ No heat   │ │
    │ T=5.0 sec  │      │ Water in  │ │
    │            │      │ CIP time  │ │
    └─────┬──────┘      └──────┬────┘ │
          │                     │      │
    [5 sec elapsed]    [Manual stop]  │
          │                     │      │
    ┌─────▼──────┐      ┌──────▼────┐ │
    │ COOLDOWN   │      │  READY    │◄┘
    │            │      └───────────┘
    │ Pump off   │
    │ Motor off  │
    │ SSR on (maintain)
    │ Cool to <100°C
    └─────┬──────┘
          │
    [T < 100°C]
          │
    ┌─────▼──────┐
    │   IDLE     │
    └────────────┘
          ▲
          │
    ┌─────┴──────────────────────────────────┐
    │                                         │
┌───▼────────┐                      ┌────────▼────┐
│   ERROR    │                      │    ESTOP    │
│            │                      │             │
│ Fault      │                      │ Emergency   │
│ occurred   │                      │ Stop active │
│            │                      │             │
│ (manual    │                      │ (requires   │
│  reset)    │                      │  reset)     │
└────────────┘                      └─────────────┘
```

## State Descriptions

### IDLE
- **Entry Conditions:** System startup OR cooldown complete
- **Active Actions:** None (all subsystems idle)
- **Transitions:**
  - → WARMUP: When `requested_mode = MODE_WARMUP` (T command)
  - → CIP: When `requested_mode = MODE_CIP` (C command)
  - → ERROR: Safety fault with auto-acknowledge
- **Exit Conditions:** Valid state transition command

### WARMUP
- **Entry Conditions:** Transition from IDLE with warmup request
- **Active Actions:**
  - Enable SSR heater (TIM1 PWM CH1)
  - Read PT100 temperature at 25 Hz
  - Apply PID control: $u = K_p e + K_i \int e \, dt + K_d \frac{de}{dt}$
  - Update SSR duty at 10 Hz
- **Transitions:**
  - → READY: When $|T_{measured} - T_{setpoint}| \leq 3°C$ for stable reading
  - → ERROR: If RTD sensor fails or timeout (>120 seconds)
  - → ESTOP: If safety fault detected
- **Temperature Control Loop:**
  ```
  Sample Rate: 25 Hz (40 ms)
  SSR Update: 10 Hz (100 ms = every 3rd sample)
  Anti-windup: Integral clamped to [0, 100]
  Hysteresis: ±3°C band
  ```

### READY
- **Entry Conditions:** Temperature stable within ±3°C
- **Active Actions:**
  - Maintain SSR duty to hold setpoint
  - PID continues running
  - Monitor for operator commands
- **Transitions:**
  - → DISPENSE: When S (start) command received
  - → CIP: When C (CIP) command received
  - → IDLE: When idle via timeout (future feature)
  - → ESTOP: If E-stop pressed or cover opened
- **Duration:** Indefinite until operator action

### DISPENSE
- **Entry Conditions:** S command received while READY, and θ axis homed
- **Active Actions:**
  1. **t = 0-5 seconds:**
     - Enable stepper motor (EN_THETA = 1)
     - Ramp motor to 1.0 rev/s (60 RPM)
     - Enable pump PWM (duty = 85%)
     - Maintain plate temperature with PID
  2. **t = 5.0 seconds:**
     - Disable pump (PWM = 0%)
     - Disable motor (EN_THETA = 0)
     - Continue temperature control
- **Transitions:**
  - → COOLDOWN: At t = 5.0 sec (cycle complete)
  - → ERROR: If motor stalls before 5 sec
  - → ESTOP: If safety fault
- **Cycle Timing:**
  ```
  Starting plate rotation (0-1 sec):
    Smooth acceleration ramp: a_max = 2 rev/s²
    
  Steady rotation (1-5 sec):
    θ = 1.0 rev/s (constant)
    ψ(t) = t - 0.5 sec (position in revolutions)
    
  Dispense exit (at 5 sec):
    Ramp-down over 0.5 sec if implemented
  ```
- **Flow Profile:**
  ```
  t=0-5s:   Pump running at 85% duty ≈ 28.3 mL/s
  Total:    141.5 mL (diameter ≈ 300 mm, thickness ≈ 2 mm)
  ```

### COOLDOWN
- **Entry Conditions:** Dispense cycle complete (5 seconds elapsed)
- **Active Actions:**
  - Keep SSR running but reduce duty (PID controlling to lower temp)
  - OR: Turn off SSR and coast
  - Monitor temperature decline
- **Transitions:**
  - → IDLE: When $T_{measured} < 100°C$ (safe for handling)
  - → READY: If operator starts new cycle during cool (T override)
  - → ESTOP: If safety fault
- **Expected Cooling Rate:** Depends on insulation
  ```
  From 220°C to 100°C: ~5-10 minutes (passive)
  With fan assist: ~2-3 minutes
  ```

### CIP (Cleaning-in-Place)
- **Entry Conditions:** C command from READY or IDLE
- **Active Actions:**
  - Enable pump at 100% duty
  - Disable heater (SSR = 0%)
  - Run water/cleaning fluid
  - No motor rotation (optional)
- **Transitions:**
  - → READY: Manual stop via E or idle timeout
  - → IDLE: If ESTOP or safety fault
- **Duration:** Operator-controlled (~30 minutes typical)

### ERROR
- **Entry Conditions:** Fault detected with auto-recovery capability
- **Active Actions:**
  - Disable all outputs (SSR, motor, pump)
  - Display error description on HMI
  - Log fault to circular buffer
- **Transitions:**
  - → IDLE: Manual acknowledge (future: auto timeout)
  - → ESTOP: If fault escalates (e.g., repeated failures)
- **Fault Types:**
  ```
  - Sensor loss (RTD not responding)
  - Motor stall (current > threshold)
  - Pump failure (pressure loss)
  - System timeout (watchdog)
  ```

### ESTOP
- **Entry Conditions:** 
  - E command received
  - Hardware E-stop button pressed
  - Cover interlock opened
  - Thermal cutoff triggered (T > 250°C)
- **Active Actions:**
  - Disable ALL outputs immediately (< 50 ms)
  - Set system mode flag
  - Log emergency condition
- **Transitions:**
  - → ERROR: After manual acknowledge
  - → IDLE: System power cycle or watchdog reset
- **Hardware Interlock:**
  ```
  Safety chain (hardwired):
  
  230VAC → RCD → Main contactor
                  │
       ┌──────────┴──────────┐
       │                     │
    (a) 24V PSU          (b) AC Heater
       │                    │
    SSR ← Safety relay ← E-STOP NC1
    Motor EN ← Safety relay ← E-STOP NC2 + COVER NC
    
  Firmware monitors:
  - GPIO (ESTOP_Pin, NC_Switch_Pin)
  - Thermal via MAX31865
  - Issues second shutdown command if needed
  ```

## Task Synchronization

### Mutex-Protected Critical Sections

Current implementation uses **no explicit synchronization** (safe due to simple data structures):

```c
// Global state access (read-only in most tasks)
float current_temp = g_system_state.thermal.current_c;  // Safe

// Writes coordinated by single task
thermal_pid_update();  // Only writes thermal state from thermal task
motion_update();       // Only writes motion state from motion task
```

**If extended to shared write access, add:**
```c
SemaphoreHandle_t state_mutex;
xSemaphoreTake(state_mutex, pdMS_TO_TICKS(10));
g_system_state.thermal.current_c = new_value;
xSemaphoreGive(state_mutex);
```

## Timing Diagram

```
Time:     0ms         100ms        200ms        300ms        400ms        500ms
          ├───────────┼───────────┼───────────┼───────────┼───────────┤

SAFETY    |●|●|●|●|●|●|●|●|●|●|●|●|●|●|●|●|●|●|●|●|●|●|●|●|●|●|●|●|●|●|●|
Monitor   └─ 1 kHz (1 ms period) ─────────────────────────────────────────────

THERMAL   |═══════│   |═══════│   |═══════│   |═══════│   |═══════│   |═══
Control   └────── 40ms ──────────────── 40ms (25 Hz) ────────────── 40ms
(25 Hz)

MOTION    |═══════│   |═══════│   |═══════│   |═══════│   |═══════│   |═══
Control   └────── 40ms ──────────────── 40ms (25 Hz) ────────────── 40ms
(25 Hz)

PUMP      |═══════════════│   |═══════════════│   |═══════════════│   |════
Control   └────────────── 100ms ──────────────────── 100ms (10 Hz)
(10 Hz)

HMI       |═══════════════════────────────│   |═══════════════════────────
(2 Hz)    └──────────── 500ms ──────────────────────── 500ms ────────────

LOGGING   |═══════════════════════════════════════════════════════════════
(1 Hz)    └──────────────────────── 1000ms (1 Hz) ──────────────────────
```

## Control Loop Performance Analysis

### Thermal Control Loop Stability

**Bode Plot Approximation (first-order system):**
```
Open-loop transfer function:
G(s) = Kp(1 + Ki/s + Kd·s) / (τ·s + 1)

With typical values (Kp=50, Ki=15, Kd=5, τ≈10s):
- Crossover frequency: ~0.1 Hz (slow thermal system)
- Phase margin: ~45-60° (stable)
- Gain margin: >10 dB
- Settling time: ~30-60 seconds
```

**Step Response to Temperature Command:**
```
T_set = 220°C

T_measure
  ↑
225│     ╱╲
   │    ╱  ╲___
220├───╱       ╲____    ← Target ±3°C band
   │  ╱             ╲___
215│ ╱                   ╲____
   │╱                         ╲___
 25└──────────────────────────────────→ time
   0s     20s      40s      60s      80s
   
Warmup time: ~40-60 seconds (platform dependent)
Overshoot: ~±5-8°C (tunable)
Settling: ~30 seconds
```

### Motion Control Loop

**Stepper Acceleration Profile:**
```
RPM
 ↑  
60├─────────────────────────── Target (1.0 rev/s = 60 RPM)
  │     ╱─────────────────────
45├────╱
  │   ╱  (a_max = 2 rev/s²)
30├──╱
  │ ╱
15├╱
  │
  0└────────────────────────→ time (sec)
  0    5    10    15    20   25
  
Ramp time: ~15-20 seconds from 0 → 60 RPM
Profile: Trapezoidal (ramp-up, hold, ramp-down if needed)
```

### Dispense Cycle Timing

```
Event Timeline (5-second dispense):

Time    Motor             Pump              Plate Temp
(sec)   (EN, RPM)         (PWM)             Control
────────────────────────────────────────────────────────
0.0     Start ramp        OFF               PID active
        (0 → 60 RPM)      

1.0     Steady 60 RPM     ON (85%)          ±3°C band
        (full rotation)   Flow: 28.3 mL/s   

4.0     Steady 60 RPM     ON (85%)          Stable
        (4 revolutions)   @28.3 mL/s        ±3°C

5.0     OFF (EN = 0)      OFF (PWM = 0)     Continue heating
        (coast)           (stop)            to setpoint

5.5     Stopped           OFF               Cooling phase
        (locked)          OFF               or maintain

Total fluid: 28.3 mL/s × 5 s = 141.5 mL
Plate rotations: ~1.0 revolutions (assume slow start)
```

## Error Handling and Recovery

### Thermal Sensor Failure

```
Scenario: MAX31865 stops responding

Detection:
  - thermal_read_rtd_spi() returns false
  - Thermal task: SSR disabled immediately
  - Safety task flags FAULT_SENSOR_LOSS

Recovery:
  1. Manual acknowledge (ARP command - future)
  2. Retry sensor read (up to 3 times)
  3. If persistent: stay in ERROR mode
  4. Operator must power-cycle or skip heating
```

### Motor Stall Detection

```
Scenario: Cannot reach target RPM within timeout

Detection:
  - motor_get_rpm() stuck below 55 RPM after 10 seconds
  - Motion task: disable motor, set EN=0
  - Safety task flags FAULT_MOTOR_STALL

Recovery:
  1. Manual home sequence (H command)
  2. Check for mechanical jam
  3. Retry dispense from READY state
```

### E-Stop Debouncing

```
Hardware E-stop (active low):

0ms   ├─────────────────────────────┐
      │   Physical contact closed   │
      │   (noise & contact bounce)  │
      │                             │
50ms  ├─ Debounce threshold ────────┤
      │ First valid reading of LOW  │
      │                             │
      │   GPIO reads LOW            │
      └─────────────────────────────┘
      
      → Safety latches ESTOP fault
      → All outputs disabled
      → System → ESTOP mode
```

## Power Sequencing

```
Power-up sequence (from rest):

1. 230VAC applied
2. RCD detects ground reference (if needed)
3. Main contactor closes (if energized)
4. 24V PSU startup:
   a. 24V output ramps to 24V (< 1 sec)
   b. STM32F446 powered from 24V buck converter
   c. STM32 boots from flash

5. STM32 MCU startup:
   a. System clock configured (180 MHz)
   b. GPIO initialized
   c. SPI, TIM, UART peripherals initialized
   d. HAL_Init() completes

6. Control system initialization:
   a. safety_init() → Monitor E-stop/cover
   b. thermal_init() → SSR off, setpoint=220°C
   c. motion_init() → Motor disabled (EN=0)
   d. pump_init() → PWM off
   e. hmi_init() → UART ready, send banner

7. FreeRTOS scheduler starts:
   a. All 6 tasks created (suspended)
   b. vTaskStartScheduler()
   c. SysTick configured for 1 kHz
   d. Tasks begin executing

8. Normal operation:
   → IDLE state, waiting for command
   → E-stop monitored continuously
   → Ready for operator input
```

## Shutdown Sequence

### Normal Shutdown (Planned)

```
Operator sends ESTOP or cycles power:

1. GPIO detects E-stop pressed (active low)
2. Safety monitor (1 kHz) immediately:
   a. SSR → OFF (TIM1 PWM = 0)
   b. Motor EN → LOW
   c. Pump PWM → OFF
   d. g_system_state → MODE_ESTOP

3. Active cooling:
   a. If plate hot, optional fan assist
   b. Otherwise passive cooling

4. Monitoring continues until power off:
   a. Temperature logged
   b. Safety chain remains open
   c. No motor/pump operation possible
```

### Emergency Shutdown (Hardwired)

```
Hardware safety chain breaks (independent of firmware):

230VAC Main → RCD → Contactor ← Safety Relay
                                    ↑
                                 E-Stop CH1 (NC)
                                 E-Stop CH2 (NC)
                                 Cover Interlock (NC)

If ANY safety switch opens:
  1. Safety relay de-energizes
  2. Contactor opens within < 50 ms
  3. Main 24V distribution lost
  4. SSR current → zero (heater OFF)
  5. Motor stepper driver → lose power

Firmware still running (backup rail):
  1. Detects voltage loss on power sense
  2. Logs ESTOP event
  3. Enters infinite loop (watchdog reset after ~10 sec)
  4. Would be caught by external watchdog circuit
```

---

**Document Version:** 1.0  
**Last Updated:** 2026-03-22  
**References:** Requirements in `docs/specs/SRS.md`
