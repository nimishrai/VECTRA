# VECTRA Hardware Documentation

## Complete Hardware Specifications & Wiring Guide

---

## Table of Contents
1. [Component List](#component-list)
2. [Pin Configuration](#pin-configuration)
3. [Wiring Diagram](#wiring diagram)
4. [Power Requirements](#power-requirements)
5. [Assembly Instructions](#assembly-instructions)
6. [Troubleshooting](#troubleshooting)

---

## Component List

### Core Components

| Component | Specification | Quantity | Notes |
|-----------|--------------|----------|-------|
| **ESP32 Development Board** | ESP32-WROOM-32 | 1 | Dual-core, WiFi + Bluetooth |
| **MPU6500 IMU Sensor** | 6-axis (3-accel, 3-gyro) | 1 | I2C interface |
| **GPS Module** | GY-NEO6MV2 | 1 | UART interface, 9600 baud |
| **L298N Motor Driver** | Dual H-bridge | 1 | Up to 2A per channel |
| **DC Motors** | 12V, 300RPM | 2 | With wheels |
| **Li-ion Battery** | 18650, 3.7V, 2500mAh | 2-3 | In series for 12V |
| **Battery Holder** | 3-cell 18650 | 1 | With protection circuit |
| **PS5 DualSense Controller** | Bluetooth | 1 | For wireless control |
| **Chassis** | Acrylic/Metal | 1 | 2-wheel or 4-wheel drive |
| **Jumper Wires** | Male-to-Male, Male-to-Female | 40+ | Various lengths |
| **Breadboard/PCB** | Prototype board | 1 | For circuit assembly |
| **Switch** | SPST Rocker | 1 | Power control |
| **Capacitors** | 100µF, 0.1µF | 2 | Motor noise filtering |

---

## Pin Configuration

### ESP32 Pin Assignments

```
┌─────────────────────────────────────────────────────────────┐
│                    ESP32 PIN MAPPING                         │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  MOTOR CONTROL (L298N Driver)                                │
│  ┌──────────────────────────────────────────────────────┐  │
│  │ GPIO 22 → ENA (Right Motor Enable)                    │  │
│  │ GPIO 27 → IN1 (Right Motor Direction 1)               │  │
│  │ GPIO 26 → IN2 (Right Motor Direction 2)               │  │
│  │ GPIO 23 → ENB (Left Motor Enable)                      │  │
│  │ GPIO 18 → IN3 (Left Motor Direction 1)                 │  │
│  │ GPIO 19 → IN4 (Left Motor Direction 2)                 │  │
│  └──────────────────────────────────────────────────────┘  │
│                                                              │
│  I2C BUS (MPU6500 IMU)                                       │
│  ┌──────────────────────────────────────────────────────┐  │
│  │ GPIO 21 → SDA (I2C Data)                              │  │
│  │ GPIO  TX → SCL (I2C Clock)                            │  │
│  └──────────────────────────────────────────────────────┘  │
│                                                              │
│  UART2 (GPS Module)                                          │
│  ┌──────────────────────────────────────────────────────┐  │
│  │ GPIO 16 → RX2 (GPS TX)                                 │  │
│  │ GPIO 17 → TX2 (GPS RX)                                 │  │
│  └──────────────────────────────────────────────────────┘  │
│                                                              │
│  POWER                                                       │
│  ┌──────────────────────────────────────────────────────┐  │
│  │ VIN   → 12V from battery (via L298N)                  │  │
│  │ 3.3V  → MPU6500 VCC                                   │  │
│  │ GND   → Common ground (all components)                │  │
│  └──────────────────────────────────────────────────────┘  │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

### L298N Motor Driver Pinout

```
┌─────────────────────────────────────────────────────────────┐
│                    L298N PINOUT                              │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  POWER CONNECTIONS                                            │
│  ┌──────────────────────────────────────────────────────┐  │
│  │ +12V  → Battery positive (12V)                       │  │
│  │ GND   → Battery negative + ESP32 GND                 │  │
│  │ +5V   → ESP32 VIN (can power ESP32)                  │  │
│  └──────────────────────────────────────────────────────┘  │
│                                                              │
│  RIGHT MOTOR CHANNEL                                         │
│  ┌──────────────────────────────────────────────────────┐  │
│  │ ENA   → ESP32 GPIO 22 (PWM speed control)            │  │
│  │ IN1   → ESP32 GPIO 27 (direction)                    │  │
│  │ IN2   → ESP32 GPIO 26 (direction)                    │  │
│  │ OUT1  → Right Motor +                                │  │
│  │ OUT2  → Right Motor -                                │  │
│  └──────────────────────────────────────────────────────┘  │
│                                                              │
│  LEFT MOTOR CHANNEL                                          │
│  ┌──────────────────────────────────────────────────────┐  │
│  │ ENB   → ESP32 GPIO 23 (PWM speed control)            │  │
│  │ IN3   → ESP32 GPIO 18 (direction)                    │  │
│  │ IN4   → ESP32 GPIO 19 (direction)                    │  │
│  │ OUT3  → Left Motor +                                 │  │
│  │ OUT4  → Left Motor -                                 │  │
│  └──────────────────────────────────────────────────────┘  │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

### MPU6500 IMU Connections

```
┌─────────────────────────────────────────────────────────────┐
│                  MPU6500 PINOUT                              │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  ┌──────────────────────────────────────────────────────┐  │
│  │ VCC   → ESP32 3.3V                                   │  │
│  │ GND   → ESP32 GND                                    │  │
│  │ SDA   → ESP32 GPIO 21 (I2C SDA)                      │  │
│  │ SCL   → ESP32 GPIO 25 (I2C SCL)                      │  │
│  │ XDA   → Not connected (I2C pass-through)            │  │
│  │ XCL   → Not connected (I2C pass-through)            │  │
│  │ AD0   → GND (I2C address 0x68)                       │  │
│  │ INT   → Not connected (optional interrupt)           │  │
│  └──────────────────────────────────────────────────────┘  │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

### GPS Module Connections (GY-NEO6MV2)

```
┌─────────────────────────────────────────────────────────────┐
│                 GPS MODULE PINOUT                           │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  ┌──────────────────────────────────────────────────────┐  │
│  │ VCC   → ESP32 3.3V (or 5V if module supports)       │  │
│  │ GND   → ESP32 GND                                    │  │
│  │ TX    → ESP32 GPIO 16 (RX2)                          │  │
│  │ RX    → ESP32 GPIO 17 (TX2)                          │  │
│  │ PPS   → Not connected (pulse per second)             │  │
│  └──────────────────────────────────────────────────────┘  │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

---

## Wiring Diagram

### System Overview

```
                    BATTERY PACK (12V)
                          │
                          ▼
                    ┌──────────┐
                    │  SWITCH  │
                    └────┬─────┘
                         │
        ┌────────────────┼────────────────┐
        │                │                │
        ▼                ▼                ▼
   ┌─────────┐      ┌─────────┐      ┌─────────┐
   │  L298N  │      │ ESP32   │      │ ESP32   │
   │ Driver  │      │  VIN    │      │  3.3V   │
   └────┬────┘      └────┬────┘      └────┬────┘
        │                │                │
        │                │                │
        ▼                │                ▼
   ┌─────────┐           │          ┌─────────┐
   │ RIGHT   │◄──────────┤          │ MPU6500 │
   │ MOTOR   │  GPIO     │          │   IMU   │
   └─────────┘  22,27,26 │          └────┬────┘
                           │               │
        ┌──────────────────┼───────────────┘
        │                  │
        ▼                  │
   ┌─────────┐             │
   │ LEFT    │◄────────────┘
   │ MOTOR   │    GPIO
   └─────────┘   23,18,19
                           │
                           ▼
                    ┌─────────┐
                    │  GPS    │
                    │ MODULE  │
                    └────┬────┘
                         │
                         ▼
                    (ANTENNA)
```

### Detailed Wiring Steps

#### Step 1: Power Distribution
1. Connect battery pack positive terminal to switch input
2. Connect switch output to L298N +12V terminal
3. Connect L298N +5V to ESP32 VIN (to power ESP32)
4. Connect all GND terminals together (battery, L298N, ESP32, sensors)

#### Step 2: Motor Driver Connections
1. **Right Motor**:
   - L298N ENA → ESP32 GPIO 22
   - L298N IN1 → ESP32 GPIO 27
   - L298N IN2 → ESP32 GPIO 26
   - L298N OUT1 → Right Motor positive
   - L298N OUT2 → Right Motor negative

2. **Left Motor**:
   - L298N ENB → ESP32 GPIO 23
   - L298N IN3 → ESP32 GPIO 18
   - L298N IN4 → ESP32 GPIO 19
   - L298N OUT3 → Left Motor positive
   - L298N OUT4 → Left Motor negative

#### Step 3: IMU Sensor Connections
1. MPU6500 VCC → ESP32 3.3V
2. MPU6500 GND → ESP32 GND
3. MPU6500 SDA → ESP32 GPIO 21
4. MPU6500 SCL → ESP32 GPIO 25
5. MPU6500 AD0 → GND (sets I2C address to 0x68)

#### Step 4: GPS Module Connections
1. GPS VCC → ESP32 3.3V
2. GPS GND → ESP32 GND
3. GPS TX → ESP32 GPIO 16 (RX2)
4. GPS RX → ESP32 GPIO 17 (TX2)

#### Step 5: Noise Filtering (Optional but Recommended)
1. Add 100µF capacitor across motor power terminals
2. Add 0.1µF capacitor near ESP32 power pins
3. Use twisted pair wires for motor connections
4. Keep sensor wires away from motor wires

---

## Power Requirements

### Power Consumption

| Component | Voltage | Current | Power |
|-----------|---------|---------|-------|
| ESP32 | 3.3V | 240mA | 0.8W |
| MPU6500 | 3.3V | 4mA | 0.01W |
| GPS Module | 3.3V | 45mA | 0.15W |
| L298N (idle) | 5V | 50mA | 0.25W |
| DC Motors (each) | 12V | 500mA (avg) | 6W |
| **Total (motors running)** | - | - | **~13W** |
| **Total (idle)** | - | - | **~1.2W** |

### Battery Recommendations

- **Minimum**: 2x 18650 Li-ion cells (7.4V, 2500mAh each)
- **Recommended**: 3x 18650 Li-ion cells (11.1V, 2500mAh each)
- **Runtime**: 2-3 hours continuous use
- **Charging**: Use dedicated Li-ion charger with protection circuit

### Power Safety

1. **Always use a protection circuit** for Li-ion batteries
2. **Never exceed 12V** on L298N input
3. **Add a fuse** (2A) on battery positive line
4. **Monitor battery voltage** during operation
5. **Disconnect battery** when not in use

---

## Assembly Instructions

### Phase 1: Mechanical Assembly

1. **Chassis Preparation**
   - Mount motors to chassis using provided brackets
   - Attach wheels to motor shafts
   - Install battery holder in chassis center
   - Mount switch in accessible location
   - Install ESP32 and L298N on mounting standoffs

2. **Sensor Placement**
   - Mount MPU6500 on chassis center (near center of gravity)
   - Keep MPU6500 flat and level
   - Mount GPS module on top surface with clear sky view
   - Route GPS antenna away from metal components

### Phase 2: Electrical Assembly

1. **Power Wiring**
   - Solder battery leads to switch
   - Connect switch to L298N power input
   - Add fuse in positive line
   - Connect all ground wires together

2. **Motor Wiring**
   - Solder motor wires to L298N outputs
   - Use appropriate gauge wire (22-24 AWG)
   - Add strain relief near motor connections
   - Test motor direction before final assembly

3. **Sensor Wiring**
   - Use jumper wires for sensor connections
   - Keep sensor wires short (<15cm)
   - Add connectors for easy sensor removal
   - Label all connections

4. **ESP32 Wiring**
   - Connect all control wires to ESP32
   - Double-check pin assignments
   - Use breadboard or custom PCB for neat layout
   - Secure all connections with hot glue or tape

### Phase 3: Testing

1. **Power-On Test**
   - Disconnect motors
   - Apply power and check for smoke/sparks
   - Verify ESP32 powers on (LED indicator)
   - Check sensor voltages with multimeter

2. **Motor Test**
   - Reconnect motors
   - Test each motor individually
   - Verify direction control
   - Check for unusual noise/vibration

3. **Sensor Test**
   - Upload test code to ESP32
   - Verify MPU6500 I2C communication
   - Check GPS data output in serial monitor
   - Confirm sensor readings are reasonable

4. **Integration Test**
   - Upload full VECTRA firmware
   - Test PS5 controller connection
   - Verify WiFi access point creation
   - Test web dashboard connectivity

---

## Troubleshooting

### Common Issues

#### ESP32 Won't Power On
- **Symptoms**: No LED, no serial output
- **Causes**: 
  - Insufficient voltage (<3.3V)
  - Short circuit on power lines
  - Faulty USB cable or power supply
- **Solutions**:
  - Check voltage with multimeter
  - Disconnect all peripherals, test ESP32 alone
  - Try different USB cable/power supply

#### Motors Not Spinning
- **Symptoms**: No motor movement despite commands
- **Causes**:
  - Incorrect pin wiring
  - Insufficient motor power
  - L298N enable pins not set
  - PWM not configured
- **Solutions**:
  - Verify pin assignments match code
  - Check 12V supply to L298N
  - Test with simple motor test sketch
  - Check L298N jumper settings

#### MPU6500 Not Detected
- **Symptoms**: "MPU6500 NOT Found!" in serial monitor
- **Causes**:
  - Incorrect I2C wiring
  - Wrong I2C address (AD0 pin state)
  - Loose connections
  - Faulty sensor
- **Solutions**:
  - Check SDA/SCL connections
  - Verify AD0 connected to GND
  - Use I2C scanner sketch to detect address
  - Try different MPU6500 sensor

#### GPS No Fix
- **Symptoms**: "Waiting for GPS Fix..." indefinitely
- **Causes**:
  - Indoor testing (no satellite visibility)
  - Poor antenna placement
  - Insufficient satellites in view
  - Faulty GPS module
- **Solutions**:
  - Test outdoors with clear sky view
  - Ensure antenna has metal ground plane
  - Wait 2-5 minutes for initial fix
  - Check GPS module with u-blox uCenter

#### WiFi Not Connecting
- **Symptoms**: Cannot see "RC_CAR_ESP32" network
- **Causes**:
  - WiFi antenna not connected
  - Interference from other networks
  - ESP32 WiFi module fault
- **Solutions**:
  - Check ESP32 antenna connection
  - Try different WiFi channel
  - Scan for networks with phone
  - Test ESP32 with WiFi scan sketch

#### Dashboard Not Loading
- **Symptoms**: Cannot connect to 192.168.4.1
- **Causes**:
  - Not connected to correct WiFi network
  - Wrong IP address in browser
  - WebSocket server not running
  - Browser compatibility issues
- **Solutions**:
  - Verify connected to "RC_CAR_ESP32"
  - Check serial monitor for ESP32 IP
  - Try different browser (Chrome/Firefox)
  - Clear browser cache

#### PS5 Controller Not Connecting
- **Symptoms**: "PS5 controller not found" in serial monitor
- **Causes**:
  - Wrong MAC address in code
  - Controller not in pairing mode
  - Bluetooth interference
  - ESP32 Bluetooth module issue
- **Solutions**:
  - Update MAC address with correct controller address
  - Hold PS + Share buttons to enter pairing mode
  - Move away from other Bluetooth devices
  - Test with simple PS5 controller sketch

### Debugging Tools

#### Serial Monitor
- Use Arduino IDE Serial Monitor
- Baud rate: 115200
- Monitor for initialization messages
- Check for error messages

#### I2C Scanner
```cpp
#include <Wire.h>
void setup() {
  Wire.begin();
  Serial.begin(115200);
  Serial.println("I2C Scanner");
}
void loop() {
  byte error, address;
  for(address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at 0x");
      Serial.println(address, HEX);
    }
  }
  delay(5000);
}
```

#### Multimeter Checks
- Voltage levels at each component
- Continuity of ground connections
- Current draw of motors
- Battery voltage under load

---

## Additional Resources

### Datasheets
- [ESP32 Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf)
- [MPU6500 Datasheet](https://invensense.tdk.com/wp-content/uploads/2015/02/PS-MPU-6500A-01-v1.1.pdf)
- [L298N Datasheet](https://www.st.com/resource/en/datasheet/l298.pdf)
- [NEO-6M Datasheet](https://www.u-blox.com/sites/default/files/products/documents/NEO-6_DataSheet_(GPS.G6-HW-09005).pdf)

### Libraries
- [ps5Controller](https://github.com/robtillart/arduino-PS5Controller)
- [TinyGPSPlus](https://github.com/mikalhart/TinyGPSPlus)
- [MPU6050](https://github.com/jrowberg/i2cdevlib)

### Community
- [ESP32 Forum](https://www.esp32.com/)
- [Arduino Forum](https://forum.arduino.cc/)
- [Stack Overflow](https://stackoverflow.com/questions/tagged/esp32)

---

## Revision History

| Version | Date | Changes |
|---------|------|---------|
| 1.0 | 2026-07-21 | Initial hardware documentation |

---

**Document Version**: 1.0  
**Last Updated**: July 21, 2026  
**Authors**: VECTRA Team
