/*
 * Ethanol Fuel Sensor - Arduino Firmware
 * Reads capacitive sensor voltage via ADC and displays ethanol percentage
 * 
 * Hardware:
 *   - Arduino Uno
 *   - 16x2 LCD Display (I2C interface)
 *   - Ethanol sensor PCB
 * 
 * Connections:
 *   A0: Sensor analog output (0-5V)
 *   SDA: LCD I2C data (pin 4)
 *   SCL: LCD I2C clock (pin 5)
 *   
 * Author: Ethanol Sensor Project
 * Date: July 2026
 * License: Apache 2.0
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD Configuration (16x2, I2C address 0x27)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Sensor Configuration
#define SENSOR_PIN A0        // Analog input pin
#define NUM_SAMPLES 10       // Number of ADC samples to average
#define SAMPLE_DELAY 50      // Delay between samples (ms)

// Calibration Constants (modify after calibration procedure)
// These should be updated based on actual sensor calibration
const float CALIB_SLOPE = 0.098;  // Slope: (100-0)/(adc_100-adc_0)
const float CALIB_INTERCEPT = -3.0; // Intercept: -slope * adc_0

// Temperature Compensation
#define TEMP_SENSOR_PIN A1   // Optional temperature sensor (LM35)
const float TEMP_FACTOR = 0.001; // Temperature compensation factor per °C

// Display refresh rate
const unsigned long DISPLAY_UPDATE_INTERVAL = 200; // ms

// Global Variables
unsigned long lastDisplayUpdate = 0;
float currentEthanolPercentage = 0;

// Function Prototypes
float readSensorValue();
float calculateEthanolPercentage(float sensorValue);
void displayResults();
float getTemperatureCompensation();
void calibrationMode();

// Setup
void setup() {
  // Initialize serial for debugging
  Serial.begin(9600);
  Serial.println("\n=== Ethanol Fuel Sensor Initialized ===");
  
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.print("Ethanol Sensor");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  
  // Pin configuration
  pinMode(SENSOR_PIN, INPUT);
  pinMode(TEMP_SENSOR_PIN, INPUT);
  
  delay(2000);
  lcd.clear();
  
  Serial.println("System ready. Reading sensor...");
  Serial.println("ADC Value | Raw Voltage | Ethanol %");
  Serial.println("-----------+-------------+-----------");
}

// Main Loop
void loop() {
  // Read sensor value (averaged)
  float sensorValue = readSensorValue();
  
  // Convert to ADC counts (0-1023)
  int adcValue = (int)sensorValue;
  
  // Convert ADC to voltage (0-5V)
  float sensorVoltage = (adcValue / 1023.0) * 5.0;
  
  // Calculate ethanol percentage
  currentEthanolPercentage = calculateEthanolPercentage(sensorValue);
  
  // Apply temperature compensation (optional)
  float tempCompensation = getTemperatureCompensation();
  currentEthanolPercentage *= tempCompensation;
  
  // Clamp to valid range (0-100%)
  if (currentEthanolPercentage < 0) currentEthanolPercentage = 0;
  if (currentEthanolPercentage > 100) currentEthanolPercentage = 100;
  
  // Update display at defined interval
  if (millis() - lastDisplayUpdate >= DISPLAY_UPDATE_INTERVAL) {
    displayResults();
    
    // Debug output to serial
    Serial.print(adcValue);
    Serial.print(" | ");
    Serial.print(sensorVoltage, 2);
    Serial.print("V | ");
    Serial.print(currentEthanolPercentage, 1);
    Serial.println("%");
    
    lastDisplayUpdate = millis();
  }
  
  // Small delay to prevent overwhelming the MCU
  delay(10);
}

/**
 * Read sensor analog value with averaging
 * @return Average ADC value (0-1023)
 */
float readSensorValue() {
  long sum = 0;
  
  for (int i = 0; i < NUM_SAMPLES; i++) {
    sum += analogRead(SENSOR_PIN);
    delay(SAMPLE_DELAY);
  }
  
  return sum / (float)NUM_SAMPLES;
}

/**
 * Calculate ethanol percentage from sensor reading
 * Uses linear calibration: Ethanol% = slope * ADC + intercept
 * 
 * @param sensorValue ADC value (0-1023)
 * @return Ethanol percentage (0-100%)
 */
float calculateEthanolPercentage(float sensorValue) {
  // Linear calibration equation
  // Ethanol% = CALIB_SLOPE * ADC_value + CALIB_INTERCEPT
  float ethanol = (CALIB_SLOPE * sensorValue) + CALIB_INTERCEPT;
  
  return ethanol;
}

/**
 * Get temperature compensation factor
 * Compensates for temperature-dependent dielectric changes
 * 
 * @return Temperature compensation multiplier (typically 0.99 to 1.01)
 */
float getTemperatureCompensation() {
  // Read optional temperature sensor
  int tempRaw = analogRead(TEMP_SENSOR_PIN);
  float tempVoltage = (tempRaw / 1023.0) * 5.0;
  
  // LM35: 10mV per °C, 0.5V at 25°C
  float currentTemp = (tempVoltage - 0.5) / 0.01;
  
  // Compensation factor
  // At 25°C: factor = 1.0
  float factor = 1.0 + TEMP_FACTOR * (currentTemp - 25.0);
  
  return factor;
}

/**
 * Display current readings on LCD
 */
void displayResults() {
  lcd.clear();
  
  // Line 1: Ethanol percentage with progress bar
  lcd.setCursor(0, 0);
  lcd.print("E");
  lcd.print((int)currentEthanolPercentage);
  lcd.print("%  ");
  
  // Simple progress bar (5 characters = 20% increments)
  int barLength = (int)(currentEthanolPercentage / 20);
  for (int i = 0; i < 5; i++) {
    if (i < barLength) {
      lcd.print("|" );
    } else {
      lcd.print(" ");
    }
  }
  
  // Line 2: Status information
  lcd.setCursor(0, 1);
  if (currentEthanolPercentage < 20) {
    lcd.print("Pure Gasoline");
  } else if (currentEthanolPercentage < 40) {
    lcd.print("E10-E25 Blend");
  } else if (currentEthanolPercentage < 70) {
    lcd.print("E50-E85 Blend");
  } else {
    lcd.print("Pure Ethanol");
  }
}

/**
 * CALIBRATION MODE (activate via serial command)
 * Uncomment to run calibration procedure
 */
void calibrationMode() {
  Serial.println("\n=== ENTERING CALIBRATION MODE ===");
  Serial.println("Make sure sensor is immersed in fuel sample");
  
  int adcValues[5] = {0};
  String labels[5] = {"E0", "E25", "E50", "E75", "E100"};
  
  for (int sample = 0; sample < 5; sample++) {
    Serial.print("\nReady for ");
    Serial.print(labels[sample]);
    Serial.println(" sample? (send 'r' to record)");
    
    // Wait for user input
    while (!Serial.available()) {
      delay(100);
    }
    
    if (Serial.read() == 'r') {
      // Read and average 20 samples
      long sum = 0;
      for (int i = 0; i < 20; i++) {
        sum += analogRead(SENSOR_PIN);
        delay(100);
      }
      adcValues[sample] = sum / 20;
      
      Serial.print("Recorded ADC value for ");
      Serial.print(labels[sample]);
      Serial.print(": ");
      Serial.println(adcValues[sample]);
    }
  }
  
  // Calculate calibration constants
  float slope = 100.0 / (float)(adcValues[4] - adcValues[0]);
  float intercept = -slope * adcValues[0];
  
  Serial.println("\n=== CALIBRATION RESULTS ===");
  Serial.print("Slope: ");
  Serial.println(slope, 6);
  Serial.print("Intercept: ");
  Serial.println(intercept, 6);
  Serial.println("\nUpdate CALIB_SLOPE and CALIB_INTERCEPT in code");
}

/*
 * NOTES:
 * 
 * 1. CALIBRATION:
 *    - Run calibration mode to determine CALIB_SLOPE and CALIB_INTERCEPT
 *    - Test with fuel samples: E0, E25, E50, E75, E100
 *    - Update constants in code and recompile
 * 
 * 2. TEMPERATURE COMPENSATION:
 *    - Remove #define TEMP_SENSOR_PIN if not using temperature sensor
 *    - Adjust TEMP_FACTOR based on testing
 * 
 * 3. DISPLAY OPTIONS:
 *    - This code assumes 16x2 LCD with I2C interface
 *    - Modify LiquidCrystal_I2C address if different (0x27 is common)
 *    - For different displays, update displayResults() function
 * 
 * 4. DEBUGGING:
 *    - Open Serial Monitor (9600 baud) to see debug output
 *    - Shows ADC values, voltages, and ethanol percentages
 * 
 * 5. IMPROVEMENTS FOR PRODUCTION:
 *    - Store calibration values in EEPROM
 *    - Add CAN bus interface for vehicle integration
 *    - Implement error checking and fault codes
 *    - Add low-battery warning
 */
