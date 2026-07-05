# Ethanol Fuel Sensor - Detailed Design Documentation

## Table of Contents

1. [Introduction](#introduction)
2. [Sensor Type & Operating Principle](#sensor-type--operating-principle)
3. [Circuit Design](#circuit-design)
4. [Component Selection](#component-selection)
5. [PCB Layout Considerations](#pcb-layout-considerations)
6. [Calibration Methodology](#calibration-methodology)
7. [Testing & Validation](#testing--validation)

---

## Introduction

This document details the design of a **capacitive ethanol fuel mixture sensor** for automotive applications. The sensor detects the ethanol content (E0-E100) in fuel and outputs an analog voltage signal proportional to the ethanol percentage.

### Design Goals

- ✅ Accurate ethanol detection (±2% accuracy)
- ✅ Simple interface (single analog output)
- ✅ Fast response time (<100ms)
- ✅ Temperature compensated
- ✅ Cost-effective
- ✅ Automotive-grade reliability

---

## Sensor Type & Operating Principle

### Why Capacitive?

Capacitive sensors are ideal for fuel composition measurement because:

1. **Different Dielectric Constants**
   - Gasoline (E0): εr ≈ 2.0
   - Ethanol (E100): εr ≈ 6.5
   - This difference creates a measurable capacitance change

2. **Non-Invasive**
   - No moving parts
   - No wear
   - High reliability

3. **Temperature Stability**
   - Dielectric constant varies less with temperature than conductivity
   - Easier to compensate

### Sensor Operating Principle

```
Capacitor Configuration:
┌─────────────────────────────┐
│   Outer Conductor (Shielded)│
│  ┌───────────────────────┐  │
│  │  Inner Conductor     │  │
│  │                       │  │
│  │   Fuel Dielectric    │  │  ← Ethanol content changes
│  │   (E0 to E100)       │  │     capacitance
│  │                       │  │
│  └───────────────────────┘  │
└─────────────────────────────┘

C = ε₀ × εᵣ × A / d

where:
  ε₀ = permittivity of free space
  εᵣ = relative dielectric constant (depends on ethanol %)
  A = electrode area
  d = distance between electrodes
```

### Output Relationship

```
Ethanol % = f(Capacitance) = f(Output Voltage)

Example calibration curve:
  0% ethanol  → 0.5V
  25% ethanol → 1.6V
  50% ethanol → 2.7V
  75% ethanol → 3.8V
  100% ethanol→ 4.9V

(Linear approximation: V = 0.044 × E% + 0.5, where E% = 0-100)
```

---

## Circuit Design

### Block Diagram

```
        ┌─────────────────┐
        │ Fuel Tank Sensor│ (Capacitive probe)
        │   C_sensor      │
        └────────┬────────┘
                 │
                 ▼
        ┌─────────────────┐
        │ Oscillator      │ (FC output varies with C)
        │ Circuit         │ Typical: 1-10 MHz
        └────────┬────────┘
                 │
                 ▼
        ┌─────────────────┐
        │ Demodulator &   │
        │ Low-Pass Filter │ (Convert frequency to DC voltage)
        └────────┬────────┘
                 │
                 ▼
        ┌─────────────────┐
        │ Amplifier       │ (Gain adjustment)
        │ (0-5V output)   │
        └────────┬────────┘
                 │
                 ▼
        ┌─────────────────┐
        │ Microcontroller │ (ADC input)
        │ (Arduino/ESP32) │
        └────────┬────────┘
                 │
                 ▼
        ┌─────────────────┐
        │ Display/Gauge   │ (Show ethanol %)
        └─────────────────┘
```

### Detailed Circuit Schematic

#### Stage 1: Capacitive Sensor Interface

```
Fuel Tank Sensor (Capacitive Probe)
       │
       ├─────R1 (100Ω)───┬───────┐
       │                 │       │
       │            C_block      │
       │            (100nF)      │
       │                 │       │
       └─────────────────┴───────┼──→ To Oscillator

- C_sensor = fuel tank probe (1-10pF range)
- R1 = series resistor for impedance matching
- C_block = DC blocking capacitor (if using AC coupling)
```

#### Stage 2: Oscillator (Capacitance-to-Frequency Converter)

```
Option A: 555 Timer Circuit (Simple, cost-effective)

        +5V
         │
         ├──────────────────┐
         │                  │
        R1                  ├─────→ Frequency output
        R2                  │
         │          ┌──────┴──────┐
      C_var ┤─────┤ 555 Timer    │
         │          │              │
         │          ├──────────────┴──→ To demodulator
        GND         │
                    └──────────────────

F = 1.44 / ((R1 + 2×R2) × C_var)

Typical values:
  R1 = 10k, R2 = 47k
  C_var = 1pF to 10pF (fuel sensor)
  Output frequency: 100kHz to 1MHz
```

#### Stage 3: Demodulator & Low-Pass Filter

```
Frequency Input
      │
      ├──────R3 (10k)──┬──────────────┐
      │                │              │
      └────|>|─────────┤              │
      (Diode D1)       R4(100k)      |
                       │             C2(10µF)
                       │              │
                      GND ────────────┤
                                      │
                                      ▼
                             Demodulated DC Voltage
                             (proportional to ethanol %)

Transfer function:
V_out = V_pk × (1 - e^(-t/τ))
where τ = R4 × C2 = time constant (controls filtering)
```

#### Stage 4: Amplifier (Op-Amp, 0-5V Output)

```
        Demod Input
             │
             ├──────────────┤+\
             │              │  \ Gain = 1 + Rf/Ri
            R_in            │ /
             │              │/
             │         ┌────┴────┐
             │         │ Op-Amp  │
        Ref V ┤─────┤- │ (LM358) │────→ Output (0-5V)
             │         └─────────┘
             │              │
             │             Rf
             │              │
             └──────────────┤

Typical gain: 5-10x
Used to scale demodulated signal to 0-5V range
```

### Recommended Sensor Chip: CC2531A or TDC7001

**Alternative: Integrated Capacitive-to-Digital Converter**

```
Fuel Tank Probe (Capacitive)
      │
      └──┤ CDC Circuit ├──┐
         │ (e.g., TDC) │  │
         └─────────────┘  │
                          │ SPI/I2C
                          │
                      Microcontroller ADC
                      or Digital Interface
```

**Advantages:**
- Factory calibrated
- Digital output (easier for MCU)
- Temperature compensation included
- Smaller form factor

---

## Component Selection

### Critical Components

| Component | Purpose | Recommended Part | Notes |
|-----------|---------|------------------|-------|
| Sensor Probe | Capacitive detection | Custom or capacitive fuel probe | 1-10pF range |
| Oscillator IC | Frequency conversion | LM555 or CD40106 | 555 is simpler |
| Op-Amp | Signal amplification | LM358, TL072, OPA2134 | Rail-to-rail preferred |
| Microcontroller | Data processing | Arduino, ESP32, STM32 | 10-bit or 12-bit ADC |
| Display | User interface | 16x2 LCD or 128x64 OLED | SPI or I2C interface |
| Filters | Noise rejection | 100nF, 10µF ceramic caps | Low ESR preferred |
| Voltage Regulator | Power supply | AMS1117 (3.3V/5V) | 1A capable |

### Bill of Materials Structure

See `/bom/BOM.csv` for:
- Part numbers from DigiKey, Mouser, AliExpress
- Quantity per unit
- Unit cost
- Sourcing links

---

## PCB Layout Considerations

### Design Rules

1. **Layer Stack-up**
   - Layer 1: Top copper (components, traces)
   - Layer 2: Ground plane
   - Layer 3: Power plane (optional)
   - Layer 4: Bottom copper (return paths)

2. **Trace Routing**
   - Signal traces: minimum 8mil width
   - Power traces: minimum 12mil width
   - Keep sensor signal traces short (<2cm)
   - Use via stitching on ground plane

3. **Grounding**
   - Star-ground point near oscillator
   - Separate analog and digital grounds
   - Multiple ground vias near op-amp

4. **Noise Reduction**
   - Shielded enclosure around sensor probe
   - Ferrite beads on signal lines
   - Bypass capacitors (100nF) near ICs
   - Bulk capacitors (10µF) near power inputs

5. **Thermal Management**
   - Copper pour around high-current areas
   - Heat sink pads if needed
   - Adequate creepage distances (isolation)

### Sensor Probe Considerations

```
Probe Design (cylindrical capacitor):

    Outer shielding (metal tube, connected to GND)
    │
    ├─── Inner conductor (signal, sensing electrode)
    │
    └─── Dielectric: fuel between electrodes

Probe dimensions:
  Diameter: 10-15mm
  Length: 30-50mm
  Sealed end with epoxy or ceramic
```

---

## Calibration Methodology

### 5-Point Calibration Procedure

**Equipment Needed:**
- Fuel samples: E0, E25, E50, E75, E100
- Test container (non-conductive)
- Multimeter (voltmeter)
- Thermometer
- PC with serial monitor

**Steps:**

1. **Zero-Point (E0 - Pure Gasoline)**
   - Place sensor probe in 100mL E0 fuel
   - Wait 2 minutes for stabilization
   - Record ADC value: `adc_0 = XXX`
   - Corresponding voltage: `V_0 = adc_0 × (5.0 / 1023)`

2. **25% Ethanol (E25)**
   - Replace fuel with E25 mixture
   - Wait 2 minutes
   - Record: `adc_25 = XXX`

3. **50% Ethanol (E50)**
   - Record: `adc_50 = XXX`

4. **75% Ethanol (E75)**
   - Record: `adc_75 = XXX`

5. **100% Ethanol (E100)**
   - Record: `adc_100 = XXX`

### Calibration Curve Calculation

```
Linear regression:

Ethanol% = m × ADC_value + b

where:
  m = slope
  b = y-intercept

Calculation:
  Points: (adc_0, 0), (adc_25, 25), (adc_50, 50), (adc_75, 75), (adc_100, 100)
  
  m = (100 - 0) / (adc_100 - adc_0)
  b = 0 - m × adc_0
```

### Temperature Compensation

```
If operating range is -40°C to +125°C:

Corrected_Ethanol% = Ethanol% × Temp_factor

where Temp_factor accounts for:
  - Dielectric constant change with temperature
  - Component drift (capacitors, resistors)
  - Oscillator frequency drift

Simple approximation:
  Temp_factor ≈ 1 + k × (T - 25°C)
  k ≈ 0.001 per °C (adjust based on testing)
```

### Stored Calibration Values

Store in microcontroller EEPROM:
```c
struct CalibrationData {
  uint16_t adc_0;      // E0 reference
  uint16_t adc_100;    // E100 reference
  float m;             // Calibration slope
  float b;             // Calibration intercept
  uint16_t crc;        // Checksum
};
```

---

## Testing & Validation

### Unit Tests

1. **Power Supply**
   - ✓ Voltage regulation: ±5% tolerance
   - ✓ Load testing: current draw <500mA
   - ✓ Stability: no ripple >50mV

2. **Sensor Circuit**
   - ✓ Oscillator frequency: 100kHz-1MHz
   - ✓ Frequency modulation: 5-15% swing with fuel change
   - ✓ Response time: <100ms after fuel change

3. **Signal Conditioning**
   - ✓ Output voltage: 0-5V (full scale)
   - ✓ Linearity: R² > 0.99 for calibration curve
   - ✓ Noise floor: <50mV RMS

4. **Microcontroller Interface**
   - ✓ ADC accuracy: ±2 LSB
   - ✓ Serial communication: error-free
   - ✓ Display update: <200ms refresh rate

### System Integration Tests

1. **Accuracy Test**
   - Test with fuel samples E0, E25, E50, E75, E100
   - Expected accuracy: ±2% ethanol content
   - Temperature range: -10°C to +60°C

2. **Environmental Tests**
   - Vibration immunity (automotive)
   - EMI/RFI immunity (fuel pump EMI)
   - Temperature extremes: -40°C to +125°C

3. **Reliability Tests**
   - 1000-hour continuous operation
   - 500 thermal cycles (-40°C to +85°C)
   - Fuel compatibility: ethanol, gasoline, biofuels

---

## Safety Warnings

⚠️ **Fuel Safety:**
- Ethanol vapors are flammable; use only in ventilated areas
- Avoid sparks or open flames
- Follow automotive fuel handling safety procedures
- Dispose of fuel samples properly

---

## References

1. Automotive Fuel Sensors - SAE J1930 Standards
2. Capacitive Sensor Theory - Application notes from sensor manufacturers
3. KiCad PCB Design Guide - https://docs.kicad.org/
4. Microcontroller Datasheets - See `/datasheets/` folder

---

**Document Version:** 1.0  
**Last Updated:** July 2026
