<!-- # PRD — outline
# PRD & SRS — Injera‑Making Machine v1.0 (Draft)

**Project:** Industrial Injera Maker (single-station)
**Company:** Maya Africa Tours (Gothenburg build)
**Author:** Nesredin Mahmud (Founder, Control Engineer)
**Date:** 2026‑01‑10
**Scope of this doc:** Item **1) PRD** and **2) SRS** from the development artifacts list.

--- -->

## 1) PRD — Product Requirements Document

### 1.1 Problem & Goal

Restaurants and bakeries need consistent, high‑throughput injera without relying on expert operators. The machine must deposit batter in evenly spaced concentric rings and bake a Ø30 cm injera in ≤5 s with uniform “eyes” and repeatable thickness.

### 1.2 Users & Use Cases

* **Primary user:** Line cook/operator in a commercial kitchen.
* **Secondary user:** Production manager/owner.
* **Tertiary user:** Service technician.

**Core use cases**

1. Start of day warm‑up and sanitation check.
2. Continuous production (single operator): load batter, initiate cycle, remove baked injera.
3. Quick recipe switch (temperature/flow tweaks).
4. Cleaning‑in‑place (CIP) at shift end.

### 1.3 Product Scope

* Single heated plate (θ‑axis only, no radial motion).
* One pump feeding **7 outlets** on a rigid nozzle arm (concentric rings).
* Closed‑loop plate temperature control using PT100 + MAX31865 + SSR.
* Safety: dual‑channel E‑stop (2NC), cover interlock, thermal cutoff.
* HMI: basic status/alarms and setpoints; optional serial console.

### 1.4 Key Performance Indicators (KPIs)

* **Cycle time:** ≤ **5.0 s** per Ø30 cm injera (steady‑state).
* **Thickness:** **2.0 mm ± 0.3 mm**.
* **Temperature regulation:** **±3 °C** around setpoint during cycling.
* **Yield (visual uniformity/eyes):** ≥ **95%** pass rate after warm‑up.
* **Uptime:** ≥ **95%** during an 8‑h shift.
* **Sanitation (CIP):** complete in **≤30 min**.

### 1.5 Non‑Goals (V1)

* Multi‑plate indexing carousel (future).
* Vision‑based quality feedback loop (future).
* Cloud telemetry/OTA (future).

### 1.6 Constraints & Assumptions

* **Geometry:** Ø30 cm, 2 mm thickness; batter ≈ **141 mL** per injera.
* **Flow:** total ≈ **28.3 mL/s** for 5 s; per‑nozzle targets proportional to ring radius.
* **Power:** 230 VAC mains; 24 VDC controls (150–240 W).
* **Materials:** SS304/316 for food‑contact; equal‑length tubing.
* **Environment:** Indoor food production, ambient 15–35 °C.

### 1.7 Success Criteria (acceptance at pilot)

* Produce **10 consecutive injeras** within cycle time and thickness spec.
* Maintain setpoint temperature within **±3 °C** while cycling.
* Pass safety tests: E‑stop dual‑channel cut, cover interlock, thermal cutoff.
* CIP completed ≤30 min with clean swab results.

### 1.8 Competitive/Reference Products

* Hwayihgin HY‑910‑CL; TaiwanTrade automatic injera machines; springrollsolution injera maker (reference only).

### 1.9 High‑Level UX

* **HMI home:** status, setpoint temp, cycle button, alarm banner.
* **Recipe screen:** temp, cycle time, pump trim wizard.
* **Maintenance:** sensor status, counters, CIP checklist.

---
**End of PRD v1.0 Draft**
