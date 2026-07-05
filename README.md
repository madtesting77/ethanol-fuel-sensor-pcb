# Ethanol Fuel Sensor PCB

An open-source ethanol fuel mixture detection sensor with PCB design and firmware for real-time display of fuel composition percentage on a fuel gauge.

## Overview

This project creates a **capacitive-based ethanol fuel mixture sensor** that:
- Detects E0-E100 ethanol content in fuel
- Displays mixture percentage on a fuel gauge
- Interfaces with microcontroller (Arduino, ESP32, STM32)
- Provides accurate calibration for automotive use

## Features

- ✅ Capacitive sensor design for accurate ethanol detection
- ✅ Signal conditioning circuit with analog filtering
- ✅ Microcontroller interface with ADC input
- ✅ Fuel gauge/display output
- ✅ Calibration methodology included
- ✅ KiCad PCB design files
- ✅ Firmware examples for popular microcontrollers

## Project Structure

```
ethanol-fuel-sensor-pcb/
├── schematic/          # KiCad schematic files
├── pcb/                # PCB layout and Gerber files
├── firmware/           # Microcontroller firmware
├── datasheets/         # Component datasheets
├── docs/               # Design documentation
├── bom/                # Bill of Materials
└── calibration/        # Calibration data and procedures
```

## Quick Start

1. **Review the schematic** in `/schematic/`
2. **Check the Bill of Materials** in `/bom/`
3. **Manufacture PCB** using Gerber files in `/pcb/`
4. **Program microcontroller** with firmware in `/firmware/`
5. **Calibrate** using procedures in `/calibration/`

## Hardware Requirements

- Microcontroller: Arduino Uno, ESP32, STM32, or equivalent
- Capacitive sensor chip
- Op-amp for signal conditioning
- Display: LCD/OLED or analog gauge
- Supporting components (resistors, capacitors, etc.)

## Design Highlights

### Sensor Circuit
- **Sensor Type:** Capacitive fuel sensor
- **Measurement Range:** 0-100% ethanol content (E0-E100)
- **Output:** Analog voltage (0-5V)
- **Interface:** Single ADC input channel

### Signal Conditioning
- Low-pass filtering (noise reduction)
- Amplification stage
- Protection circuitry
- Reference voltage generation

### Microcontroller Interface
- Single ADC pin input
- I2C/SPI for optional display
- PWM output for analog gauge control
- UART for debugging

## Software

Example firmware for:
- Arduino with LCD display
- ESP32 with WiFi logging
- STM32 with CAN bus integration

## Calibration

Includes:
- 5-point calibration procedure
- Temperature compensation
- Conversion table (sensor voltage → ethanol %)

## Files Included

- `schematic/ethanol-sensor-v1.sch` - KiCad schematic
- `pcb/ethanol-sensor-v1.kicad_pcb` - PCB layout
- `bom/BOM.csv` - Bill of Materials
- `docs/DESIGN.md` - Detailed design documentation
- `firmware/arduino_example/` - Arduino sketch
- `datasheets/` - Component datasheets

## Design Specifications

| Parameter | Value |
|-----------|-------|
| Input Voltage | 5V or 12V |
| Output Voltage | 0-5V (proportional to ethanol %) |
| ADC Resolution | 10-bit (Arduino) or 12-bit (ESP32) |
| Sensor Range | E0 to E100 |
| Accuracy | ±2% ethanol content |
| Response Time | <100ms |
| Operating Temperature | -40°C to +125°C |
| PCB Size | 50mm × 60mm (estimated) |

## How It Works

### Ethanol Detection Principle

Ethanol has a different **dielectric constant** than gasoline:
- **Gasoline (E0):** ~2.0 dielectric constant
- **Ethanol (E100):** ~6.5 dielectric constant

A **capacitive sensor** measures this difference and outputs a proportional voltage that varies with ethanol content.

### Signal Flow

```
Capacitive Sensor → Signal Conditioning → Low-Pass Filter → Op-Amp Amplifier → ADC Input → Microcontroller
                                                                                    ↓
                                                            Display/Gauge Output (PWM or I2C)
```

## Getting Started

### For PCB Design
1. Install [KiCad](https://kicad.org/) (free, open-source)
2. Open files in `/schematic/` and `/pcb/`
3. Review and customize as needed
4. Generate Gerber files for manufacturing

### For Firmware
1. Choose your microcontroller
2. Load example from `/firmware/`
3. Calibrate sensor using `/calibration/` procedure
4. Upload and test

## Bill of Materials (Summary)

See `/bom/BOM.csv` for complete list with:
- Component names
- Part numbers
- Quantities
- Sourcing information

## Safety Considerations

⚠️ **Working with fuel:**
- Ensure proper ventilation
- Use non-conductive tools
- Follow automotive safety standards
- Test in controlled environment first

## Contributing

Contributions welcome! Please:
1. Fork the repository
2. Create a feature branch
3. Make improvements
4. Submit a pull request

## License

Apache License 2.0 - See LICENSE file

## References

- Automotive flex-fuel sensor specifications
- KiCad documentation: https://docs.kicad.org/
- Microcontroller datasheets (links in `/datasheets/`)

## Support

For questions or issues:
1. Check existing issues
2. Review documentation in `/docs/`
3. Create a new issue with details

---

**Status:** 🚧 In Development  
**Last Updated:** July 2026  
**Maintainer:** @madtesting77
