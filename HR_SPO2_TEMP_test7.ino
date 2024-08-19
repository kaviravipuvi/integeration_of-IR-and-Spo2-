#include <Wire.h>                    
#include <Adafruit_GFX.h>            
#include <Adafruit_SSD1306.h>        
#include <SparkFunMLX90614.h>        
#include "MAX30100_PulseOximeter.h"  

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define UPDATE_INTERVAL_MS 1000


#define I2C_SDA D2 // SDA pin for I2C
#define I2C_SCL D1 // SCL pin for I2C

// Create an OLED display object
Adafruit_SSD1306 oledDisplay(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); // The last parameter is reset pin, use -1 if not connected

// Create objects for the temperature sensor and pulse oximeter
IRTherm tempSensor;  // MLX90614 temperature sensor
PulseOximeter pulseSensor; // MAX30100 pulse oximeter

// Variables to store sensor readings
float Temperature;
float heartRateBpm;
float spo2Percentage;

// Time at which the last update occurred
uint32_t lastUpdateTime = 0;

void setup() {
    Serial.begin(115200);

    // Initialize the I2C bus
    Wire.begin(I2C_SDA, I2C_SCL);

    // Initialize the temperature sensor
    tempSensor.begin();
    tempSensor.setUnit(TEMP_C);

    // Initialize the pulse oximeter
    Serial.print("Initializing pulse oximeter..");
    if (!pulseSensor.begin()) {
        Serial.println("FAILED");
        while (1);
    } else {
        Serial.println("SUCCESS");
    }
    pulseSensor.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
    // Remove callback registration if not used
    // pulseSensor.setOnBeatDetectedCallback(onPulseDetected);

    // Initialize the OLED display
    if (!oledDisplay.begin(SSD1306_I2C_ADDRESS, 0x3C)) { // Adjust the I2C address if needed
        Serial.println(F("SSD1306 allocation failed"));
        for (;;);
    }
    oledDisplay.clearDisplay();
    oledDisplay.setTextSize(1);
    oledDisplay.setTextColor(SSD1306_WHITE);

    // Display initial labels
    oledDisplay.setCursor(0, 0);
    oledDisplay.print("HEALTH MONITOR");
    oledDisplay.setCursor(0, 10);
    oledDisplay.print("Heart Rate:");
    oledDisplay.setCursor(0, 20);
    oledDisplay.print("SpO2:");
    oledDisplay.setCursor(0, 30);
    oledDisplay.print("Temp:");
    oledDisplay.display();
}

void loop() {
    // Update pulse oximeter data
    pulseSensor.update();

    // Read temperature from the MLX90614 sensor
    if (tempSensor.read()) {
        Temperature = tempSensor.ambient();
        objectTemperature = tempSensor.object();
    }

    // Grab heart rate and SpO2 from the pulse oximeter
    heartRateBpm = pulseSensor.getHeartRate();
    spo2Percentage = pulseSensor.getSpO2();

    // Check if it's time to update the display
    if (millis() - lastUpdateTime > UPDATE_INTERVAL_MS) {
        // Clear the display
        oledDisplay.clearDisplay();

        // Display heart rate
        oledDisplay.setCursor(0, 10);
        oledDisplay.print("Heart Rate: ");
        oledDisplay.print(heartRateBpm);
        oledDisplay.print(" bpm");

        // Display SpO2
        oledDisplay.setCursor(0, 20);
        oledDisplay.print("SpO2: ");
        oledDisplay.print(spo2Percentage);
        oledDisplay.print(" %");

        // Display temperature
        oledDisplay.setCursor(0, 30);
        oledDisplay.print("Temp: ");
        oledDisplay.print(Temperature);  
        oledDisplay.print("C");

        // Update the display with new data
        oledDisplay.display();

        // Update the last update time
        lastUpdateTime = millis();
    }
}
