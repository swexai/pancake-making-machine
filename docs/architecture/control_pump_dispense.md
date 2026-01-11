
@startuml
' Pump / Dispense — PWM + 7 branches (compat-safe)
left to right direction
skinparam shadowing false
skinparam ArrowColor #111
skinparam RoundCorner 8

rectangle "PumpCtrl Task\nFlow_cmd/CIP, trims" as T_PUMP
rectangle "Pump Drive\n(PWM input)" as PDRV
rectangle "3-Way Valve\nRUN↔CIP" as V3
rectangle "7-Way Manifold\n(equal branches)" as MANI

package "Per-branch x7" {
  rectangle "Trim Valve" as TRIM
  rectangle "Check Valve" as CHK
  rectangle "Equal-length Tube" as TUBE
  rectangle "Nozzle Tip" as NOZ
}

' Control to actuation
T_PUMP -[#111]> PDRV : PWM
PDRV -[#111]> V3   : Flow
V3   -[#111]> MANI

' Branch chain (write as separate arrows or on multiple lines)
MANI -[#111]-> TRIM
TRIM -[#111]-> CHK
CHK  -[#111]-> TUBE
TUBE -[#111]-> NOZ

note bottom
• Total ≈28.3 mL/s (~141 mL in 5 s). Outer rings require higher flow.\n
• Keep 7 branch tubes equal length; set per-ring trims after calibration.\n
• RUN/CIP path selected by 3-way valve.
end note
@enduml
