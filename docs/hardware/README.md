# Hardware Documentation

## KiCad Footprint Assignments

These assignments apply to the schematic sheets connected from
`hardware/kicad/swexai_ctrl/swexai_ctrl.kicad_sch`:

- `outputs.kicad_sch`
- `stm32_interface.kicad_sch`
- `motioncontrol.kicad_sch`
- `inputs.kicad_sch`
- `power.kicad_sch`

`stm32_interface.kicad_sch` currently has no missing footprint assignments.

| Sheet | Refs | Value | Footprint |
|---|---|---|---|
| `inputs.kicad_sch` | `J2`, `J3`, `J4`, `J5`, `J6`, `J7`, `J8`, `J9` | `DIGITAL_INPUT_1` | `TerminalBlock:TerminalBlock_MaiXu_MX126-5.0-02P_1x02_P5.00mm` |
| `inputs.kicad_sch` | `C4`, `C5`, `C6`, `C7`, `C8`, `C9`, `C10`, `C11` | `100nf` | `Capacitor_SMD:C_0805_2012Metric` |
| `motioncontrol.kicad_sch` | `J1`, `J12` | `J_THETA_DRIVER`, `J_RADIAL_DRIVER` | `TerminalBlock:TerminalBlock_MaiXu_MX126-5.0-06P_1x06_P5.00mm` |
| `motioncontrol.kicad_sch` | `U10` | `ULN2803A` | `Package_DIP:DIP-18_W7.62mm` |
| `outputs.kicad_sch` | `R18`, `R19`, `R20`, `R21`, `R22`, `R23`, `R24`, `R25` | `1k` | `Resistor_SMD:R_0805_2012Metric` |
| `outputs.kicad_sch` | `J10`, `J11` | `FIELD_OUTPUTS_A`, `FIELD_OUTPUTS_B` | `TerminalBlock:TerminalBlock_MaiXu_MX126-5.0-04P_1x04_P5.00mm` |
| `outputs.kicad_sch` | `U9` | `ULN2803A` | `Package_DIP:DIP-18_W7.62mm` |
| `power.kicad_sch` | `J_5V_INPUT1` | `5V_CONTROL_INPUT` | `TerminalBlock:TerminalBlock_MaiXu_MX126-5.0-02P_1x02_P5.00mm` |

Use `Package_SO:SOIC-18W_7.5x11.6mm_P1.27mm` for `U9` and `U10`
instead if the PCB needs the SMD ULN2803A package. Use matching
other `TerminalBlock:*_P5.00mm` footprints if a different exact 5.00 mm
terminal series is selected.
