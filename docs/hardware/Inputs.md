# Digital Inputs

The controller contains eight optically isolated
24 V digital inputs.

Each input contains:

- 4.7 kΩ current limiting resistor
- PC817 optocoupler
- 1N4148 reverse protection diode
- 10 kΩ pull-up resistor
- 100 nF noise filter

Recommended footprint assignments for the connected `swexai_ctrl` input sheet:

| Refs | Function | Footprint |
|---|---|---|
| `J2`, `J3`, `J4`, `J5`, `J6`, `J7`, `J8`, `J9` | 24 V field input terminals | `TerminalBlock:TerminalBlock_MaiXu_MX126-5.0-02P_1x02_P5.00mm` |
| `C4`, `C5`, `C6`, `C7`, `C8`, `C9`, `C10`, `C11` | 100 nF input filters | `Capacitor_SMD:C_0805_2012Metric` |

Isolation Voltage

PC817

Input Voltage

24 VDC

Output

3.3 V Logic
