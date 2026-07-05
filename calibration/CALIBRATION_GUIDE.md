# Ethanol Fuel Sensor - Calibration Guide

## Overview

This guide walks you through the complete calibration procedure for the ethanol fuel sensor. Proper calibration ensures accurate ethanol content detection.

## Equipment Required

- **Fuel Samples** (5 types):
  - E0: 100% gasoline (pure gasoline)
  - E25: 75% gasoline + 25% ethanol
  - E50: 50% gasoline + 50% ethanol
  - E75: 25% gasoline + 75% ethanol
  - E100: 100% ethanol

- **Test Container**: Non-conductive container (glass, plastic) ~500mL
- **Measuring Equipment**:
  - Multimeter (for voltage measurement)
  - Thermometer (optional, for temperature compensation)
  - PC with serial monitor or Arduino IDE

- **Sensor Equipment**:
  - Assembled ethanol sensor PCB
  - Arduino Uno (or compatible microcontroller)
  - USB cable for programming

## Safety Precautions

⚠️ **IMPORTANT:**

- Work in a **well-ventilated area** (ethanol vapors are flammable)
- **No open flames or sparks** nearby
- Wear **safety glasses and gloves**
- Use **antistatic mat** to protect electronics
- Dispose of fuel samples according to local regulations
- Keep a fire extinguisher nearby

## Pre-Calibration Checks

Before starting calibration:

1. **Visual Inspection**
   - ✓ Check PCB for solder bridges or cold joints
   - ✓ Verify all components are properly installed
   - ✓ Check sensor probe is securely mounted

2. **Power Test**
   - ✓ Supply 5V power
   - ✓ Check LED indicator (if present)
   - ✓ Measure voltages: confirm oscillator is running
   - ✓ Oscillator frequency should be 100kHz - 1MHz

3. **Signal Check**
   - ✓ Oscilloscope view: oscillator output should show frequency variation
   - ✓ Without fuel: baseline frequency ~500kHz (example)
   - ✓ With fuel: frequency should shift 5-15%

## Step-by-Step Calibration

### Step 1: Upload Firmware

```bash
# Use Arduino IDE or CLI to upload the calibration sketch:
# File > Examples > Custom > ethanol_sensor_calibration
# Or use command line:
arduino-cli upload -p /dev/ttyUSB0 --fqbn arduino:avr:uno calibration_sketch.ino
```

**Calibration Firmware Features:**
- Accepts serial commands
- Records ADC values for each fuel sample
- Calculates calibration curve
- Outputs calibration constants

### Step 2: E0 Calibration (Pure Gasoline)

**Setup:**

1. Pour approximately 100mL of E0 fuel into test container
2. Carefully insert sensor probe into fuel (do not submerge electronics)
3. Allow 2 minutes for sensor stabilization
4. Sensor output voltage should stabilize to a baseline value

**Measurement:**

```
Serial Monitor Output:
> Ready for E0 sample? (send 'r' to record)
Type 'r' and press Enter

Recorded ADC value for E0: 156
Voltage: 0.763V
```

**Record:**
```
ADC_E0 = 156
Voltage_E0 = 0.763V
```

### Step 3: E25 Calibration

**Preparation:**

1. Empty test container
2. Rinse with small amount of E25 fuel
3. Fill with approximately 100mL E25 fuel
4. Insert probe and allow 2 minutes stabilization

**Measurement:**

```
Recorded ADC value for E25: 298
Voltage: 1.459V
```

**Record:**
```
ADC_E25 = 298
Voltage_E25 = 1.459V
```

### Step 4: E50 Calibration

**Preparation:**
1. Repeat rinse procedure with E50
2. Fill with 100mL E50
3. Wait 2 minutes

**Measurement:**
```
Recorded ADC value for E50: 440
Voltage: 2.156V
```

**Record:**
```
ADC_E50 = 440
Voltage_E50 = 2.156V
```

### Step 5: E75 Calibration

**Measurement:**
```
Recorded ADC value for E75: 582
Voltage: 2.853V
```

**Record:**
```
ADC_E75 = 582
Voltage_E75 = 2.853V
```

### Step 6: E100 Calibration

**Measurement:**
```
Recorded ADC value for E100: 724
Voltage: 3.550V
```

**Record:**
```
ADC_E100 = 724
Voltage_E100 = 3.550V
```

## Calibration Curve Calculation

### Linear Regression Analysis

**Measured Data Points:**

| Ethanol % | ADC Value | Voltage (V) |
|-----------|-----------|-------------|
| 0 (E0)    | 156       | 0.763       |
| 25 (E25)  | 298       | 1.459       |
| 50 (E50)  | 440       | 2.156       |
| 75 (E75)  | 582       | 2.853       |
| 100 (E100)| 724       | 3.550       |

### Calculate Calibration Constants

**Slope Calculation:**
```
m = (E_max - E_min) / (ADC_max - ADC_min)
m = (100 - 0) / (724 - 156)
m = 100 / 568
m = 0.1761 (ethanol% per ADC unit)
```

**Intercept Calculation:**
```
b = E_min - m × ADC_min
b = 0 - 0.1761 × 156
b = -27.49
```

### Verify Linearity (R² value)

**Using Excel/Python/Online Calculator:**

```python
import numpy as np
from scipy import stats

# Data points
adcs = np.array([156, 298, 440, 582, 724])
ethanols = np.array([0, 25, 50, 75, 100])

# Linear regression
slope, intercept, r_value, p_value, std_err = stats.linregress(adcs, ethanols)

print(f"Slope: {slope:.6f}")
print(f"Intercept: {intercept:.2f}")
print(f"R² value: {r_value**2:.6f}")
print(f"Standard Error: {std_err:.6f}")
```

**Expected Result:**
```
Slope: 0.176056
Intercept: -27.49
R² value: 0.999998  (essentially perfect linearity)
```

### Calibration Equation

```
Ethanol% = 0.1761 × ADC_value - 27.49
```

**Verification:**
- At ADC=156 (E0): Ethanol% = 0.1761(156) - 27.49 = 0.0% ✓
- At ADC=440 (E50): Ethanol% = 0.1761(440) - 27.49 = 50.0% ✓
- At ADC=724 (E100): Ethanol% = 0.1761(724) - 27.49 = 100.0% ✓

## Update Firmware with Calibration Constants

### Modify Arduino Sketch

**File:** `firmware/Arduino/ethanol_sensor_arduino.ino`

**Locate these lines (around line 20):**
```cpp
// Calibration Constants (modify after calibration procedure)
const float CALIB_SLOPE = 0.098;      // OLD VALUE
const float CALIB_INTERCEPT = -3.0;   // OLD VALUE
```

**Replace with your calculated values:**
```cpp
// Calibration Constants (modify after calibration procedure)
const float CALIB_SLOPE = 0.1761;     // NEW VALUE
const float CALIB_INTERCEPT = -27.49; // NEW VALUE
```

### Recompile and Upload

```bash
# In Arduino IDE:
# 1. Edit the constants above
# 2. Verify (Sketch > Verify)
# 3. Upload (Sketch > Upload)
# 4. Test with fuel samples to confirm
```

## Post-Calibration Verification

### Test 1: Accuracy Check

**Procedure:**

1. Test with each fuel sample again (E0, E25, E50, E75, E100)
2. Open Serial Monitor (9600 baud)
3. Check displayed ethanol percentage
4. Compare to expected value

**Expected Results:**

```
E0 Sample:   Displayed: 0-2%      (Expected: 0%)
E25 Sample:  Displayed: 23-27%    (Expected: 25%)
E50 Sample:  Displayed: 48-52%    (Expected: 50%)
E75 Sample:  Displayed: 73-77%    (Expected: 75%)
E100 Sample: Displayed: 98-100%   (Expected: 100%)

Accuracy Goal: ±2% across range
```

### Test 2: Response Time

**Procedure:**

1. Switch fuel samples rapidly
2. Observe display update speed
3. Measure time for reading to stabilize

**Expected Result:**
```
Response time < 100ms (< 5 display updates)
```

### Test 3: Stability Test

**Procedure:**

1. Leave sensor in single fuel sample for 5 minutes
2. Observe ADC readings (Serial Monitor)
3. Check for drift

**Expected Result:**
```
ADC values should remain stable (±2 LSB variation)
No gradual drift over time
```

## Temperature Compensation (Optional)

If operating across wide temperature range:

### Procedure:

1. **Cool test:** Place sensor in ice bath (~0°C)
2. **Room temp:** Test at ~25°C
3. **Heat test:** Use warm water bath (~60°C)
4. **Compare results** for same fuel sample

### Calculate Temperature Factor:

```
Temp_factor = ΔEthanol% / ΔTemperature

Example:
At 0°C: measured E50 = 48%
At 25°C: measured E50 = 50%
At 60°C: measured E50 = 52%

Factor ≈ 2% / 60°C = 0.033% per °C
```

### Apply Compensation in Code:

```cpp
float TEMP_FACTOR = 0.001; // Adjust based on testing

// In calculateEthanolPercentage():
float tempCompensation = 1.0 + TEMP_FACTOR * (currentTemp - 25.0);
float correctedEthanol = ethanol * tempCompensation;
```

## Troubleshooting

### Issue: Non-linear Calibration Curve (R² < 0.95)

**Causes:**
- Noisy sensor signal
- Unstable oscillator frequency
- Air bubbles on sensor probe

**Solutions:**
- Increase low-pass filter cutoff frequency
- Add more bulk capacitors on power supply
- Ensure sensor probe is fully submerged
- Repeat measurements more carefully

### Issue: Readings Stuck at One Value

**Causes:**
- Sensor probe not in fuel
- Oscillator not running
- ADC input not connected

**Solutions:**
- Verify probe is immersed in fuel
- Check oscillator frequency with oscilloscope
- Test ADC with multimeter on pin A0
- Check USB connection to Arduino

### Issue: Erratic/Noisy Readings

**Causes:**
- EMI from fuel pump or ignition system
- Poor ground connections
- Sensor probe shielding inadequate

**Solutions:**
- Add ferrite beads on sensor leads
- Improve ground plane connectivity
- Use shielded cable for sensor probe
- Increase ADC sampling rate (average more readings)

## Calibration Data Sheet Template

**Print and fill in your calibration data:**

```
Ethanol Fuel Sensor Calibration Data
====================================

Date: ___________________
Technician: _____________
Ambient Temperature: _____°C

Calibration Samples Used:
☐ Commercial E0
☐ Commercial E25
☐ Prepared Mix
☐ Other: __________

Measured Values:
┌─────────────────────────────────────┐
│ Fuel  │ ADC Value │ Voltage │ Notes │
├─────────────────────────────────────┤
│ E0    │ ______    │ __.__V  │       │
│ E25   │ ______    │ __.__V  │       │
│ E50   │ ______    │ __.__V  │       │
│ E75   │ ______    │ __.__V  │       │
│ E100  │ ______    │ __.__V  │       │
└─────────────────────────────────────┘

Calculated Constants:
  CALIB_SLOPE = _____________
  CALIB_INTERCEPT = _________
  R² value = ________

Post-Calibration Verification:
  ☐ E0 Sample: Displayed ___% (Expected 0%)
  ☐ E50 Sample: Displayed ___% (Expected 50%)
  ☐ E100 Sample: Displayed ___% (Expected 100%)
  ☐ Accuracy: ±___% (Target: ±2%)
  ☐ Response Time: ___ms (Target: <100ms)
  ☐ Stability: ±___LSB (Target: ±2)

Approval: _________________________ Date: _______
```

## Next Steps

1. ✓ Calibration complete
2. → Deploy sensor in test environment
3. → Monitor performance over time
4. → Adjust if needed based on field data
5. → Document results in project history

---

**Calibration Guide Version:** 1.0  
**Last Updated:** July 2026
