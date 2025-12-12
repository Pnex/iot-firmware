# Pnex IoT Firmware

[![MIT License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)
[![PlatformIO](https://img.shields.io/badge/PlatformIO-ESP-blue.svg)](https://platformio.org/)
[![Docker](https://img.shields.io/badge/Docker-Build%20Environment-blue.svg)](https://www.docker.com/)

Open-source IoT firmware collection for ESP-based devices, designed to work with the [Pnex.io](https://pnex.io) platform (Platform Nexus). This project provides production-ready firmware for various IoT devices used in environmental monitoring and control applications.

## 📋 Table of Contents
- [Overview](#overview)
- [Available Firmware](#available-firmware)
- [Hardware Requirements](#hardware-requirements)
- [Quick Start](#quick-start)
- [Development Setup](#development-setup)
- [Building and Flashing](#building-and-flashing)
- [Configuration](#configuration)
- [Communication Protocol](#communication-protocol)
- [Contributing](#contributing)
- [License](#license)
- [Support](#support)

## 📖 Overview

This repository contains firmware for IoT devices that connect to the Pnex.io platform. All firmware is written in C++ using the Arduino framework and targets ESP microcontrollers. The project follows modern development practices with Docker support, PlatformIO, and automated task management.

**Key Features:**
- 🚀 Production-ready firmware for real-world IoT applications
- 🔄 WebSocket-based communication with Pnex.io server
- 📊 OLED display support for local status visualization
- 🔧 Configurable via environment variables
- 🐳 Docker-based build environment
- 📦 PlatformIO project management
- 🔒 Protocol Buffers for efficient binary communication

## 🛠️ Available Firmware

### 1. **4-Channel Relay Actuator** (`4_chan_relay/`)
Intelligent 4-channel relay control with autonomous decision-making capabilities.

**Features:**
- Formal state machine with hysteresis and threshold logic
- Protocol Buffers (Nanopb) for efficient binary communication
- 10-second timeout protection with configurable safe mode
- OLED display for status visualization
- Binary and PWM control modes
- Safe mode options (OFF, ON, KEEP)

**Hardware:** ESP8266 (NodeMCU v2) + 4-channel relay module + OLED display

### 2. **Soil Sensor** (`soil_sensor/`)
Soil moisture and temperature monitoring device.

**Features:**
- Soil moisture reading via analog pin A0
- DS18B20 temperature sensor support
- WebSocket communication to Pnex.io server
- OLED display for real-time readings
- Configurable sampling intervals

**Hardware:** ESP8266 (NodeMCU v2) + soil moisture sensor + DS18B20 + OLED display

### 3. **TFT Development** (`tft_dev/`)
Development and testing firmware for TFT displays.

**Features:**
- DisplayManager demonstration
- Loading animations and connection status display
- Minimal functionality for development and testing

**Hardware:** ESP8266 (ESP12E board) + TFT display

## 🖥️ Hardware Requirements

### Common Requirements (all projects):
- ESP8266 microcontroller (NodeMCU v2 recommended)
- USB-to-Serial programmer
- OLED display (SSD1306, 128x64)
- 3.3V power supply

### Project-Specific Requirements:
- **4-Channel Relay**: 4-channel relay module, external 5V power supply for relays
- **Soil Sensor**: Soil moisture sensor (analog), DS18B20 temperature sensor, 4.7kΩ resistor
- **TFT Development**: TFT display compatible with ESP8266

## 🚀 Quick Start

### Prerequisites
- [Docker](https://docs.docker.com/get-docker/) or [PlatformIO CLI](https://docs.platformio.org/en/latest/core/installation.html)
- Python 3.8+ (if not using Docker)
- Serial port access to flash ESP8266

### Using Docker (Recommended)
```bash
# Clone the repository
git clone https://github.com/pnex/iot-firmware.git
cd iot-firmware

# Build the Docker image
docker build -t pnex-firmware .

# Run build for a specific project
docker run --rm -v $(pwd):/workspace pnex-firmware pio run -d 4_chan_relay
```

### Using PlatformIO Directly
```bash
# Install PlatformIO
pip install platformio

# Navigate to project directory
cd 4_chan_relay

# Build the firmware
pio run

# Flash to device
pio run --target upload

# Monitor serial output
pio device monitor
```

## 🛠️ Development Setup

### Environment Variables
Create a `.env` file in the project directory (use `.env.dist` as template):

```bash
# WiFi Configuration
WIFI_SSID=your_wifi_ssid
WIFI_PASSWORD=your_wifi_password

# Pnex.io Server Configuration
HOST=wss://pnex.io
TOKEN=your_device_token
DEVICE_ID=your_device_id

# Build Configuration (optional)
BUILD_FLAGS=-DWIFI_SSID=\"${WIFI_SSID}\" -DWIFI_PASSWORD=\"${WIFI_PASSWORD}\" -DHOST=\"${HOST}\" -DTOKEN=\"${TOKEN}\" -DDEVICE_ID=\"${DEVICE_ID}\"
```

### Task Automation
Each project includes a `Taskfile.yaml` for common operations:

```bash
# List available tasks
task --list

# Flash firmware
task flash-firmware

# Monitor serial output
task dev-monitor

# Clean build
task clean
```

## 🔧 Building and Flashing

### 1. Configure Your Device
Copy the `.env.dist` file to `.env` and update with your credentials:

```bash
cd 4_chan_relay
cp .env.dist .env
# Edit .env with your settings
```

### 2. Build the Firmware
```bash
# Using PlatformIO
pio run

# Using Docker
docker run --rm -v $(pwd):/workspace -v $(pwd)/.env:/workspace/.env pnex-firmware pio run
```

### 3. Flash to Device
```bash
# Using PlatformIO
pio run --target upload

# Using task automation
task flash-firmware
```

### 4. Monitor Output
```bash
# Using PlatformIO
pio device monitor

# Using task automation
task dev-monitor
```

## ⚙️ Configuration

### Compile-Time Configuration
Configuration is set via compile-time defines in `common_libs/config/config.h`. Values can be overridden using environment variables passed as build flags.

**Key Configuration Options:**
- `WIFI_SSID`, `WIFI_PASSWORD`: WiFi network credentials
- `HOST`: Pnex.io WebSocket server URL (default: `wss://pnex.io`)
- `TOKEN`: Device authentication token
- `DEVICE_ID`: Unique device identifier

### Runtime Configuration
Some devices support runtime configuration via Protocol Buffers messages from the server:
- Relay thresholds and hysteresis
- Sensor sampling intervals
- Display settings

## 📡 Communication Protocol

### WebSocket Connection
All devices establish a WebSocket connection to the Pnex.io server:
- Connection URL: `wss://pnex.io/ws`
- Authentication via token in connection headers
- Heartbeat mechanism for connection monitoring

### Protocol Buffers (4-Channel Relay)
The 4-channel relay uses Protocol Buffers for efficient binary communication:

**Message Types:**
- `ActuatorConfig`: Channel configuration from server
- `SensorData`: Aggregated sensor values from server
- `ActuatorState`: Device state reporting to server

**Protocol Features:**
- Binary mode with configurable thresholds and hysteresis
- PWM mode with value mapping tables
- Safe mode fallback options
- State reason codes for debugging

### JSON Messages (Soil Sensor)
The soil sensor uses JSON for simpler data transmission:
```json
{
  "device_id": "sensor_001",
  "moisture": 65.5,
  "temperature": 22.3,
  "timestamp": 1678901234
}
```

## 🤝 Contributing

We welcome contributions from the community! Here's how you can help:

### Ways to Contribute
1. **Report Bugs**: Open an issue with detailed reproduction steps
2. **Suggest Features**: Propose new firmware or improvements
3. **Submit Pull Requests**: Fix bugs or add new functionality
4. **Improve Documentation**: Help make the project more accessible
5. **Create New Firmware**: Add support for new IoT devices

### Development Workflow

1. **Fork the Repository**
   ```bash
   fork https://github.com/pnex/iot-firmware
   ```

2. **Create a Feature Branch**
   ```bash
   git checkout -b feature/amazing-feature
   ```

3. **Make Your Changes**
   - Follow the existing code style
   - Add tests if applicable
   - Update documentation

4. **Commit Your Changes**
   ```bash
   git commit -m "Add amazing feature"
   ```

5. **Push to Your Fork**
   ```bash
   git push origin feature/amazing-feature
   ```

6. **Open a Pull Request**
   - Provide a clear description of changes
   - Reference any related issues
   - Include test results if applicable

### Code Style Guidelines
- **C++**: Follow Arduino/PlatformIO conventions
- **Indentation**: 2 spaces (no tabs)
- **Naming**: camelCase for variables/functions, PascalCase for classes
- **Comments**: Document complex logic and public APIs
- **Headers**: Include guards with `#pragma once`

### Testing
- Test hardware functionality before submitting
- Verify serial output matches expected behavior
- Ensure compatibility with Pnex.io platform
- Test with different ESP8266 boards if possible

### Pull Request Checklist
- [ ] Code follows project style guidelines
- [ ] Self-review completed
- [ ] Documentation updated (if needed)
- [ ] Tests pass (if applicable)
- [ ] Changes are backward compatible
- [ ] Commit messages are clear and descriptive

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.


[//]: # (## 📞 Support)

[//]: # (### Community Support)

[//]: # (- **GitHub Issues**: [Report bugs or request features]&#40;https://github.com/pnex/iot-firmware/issues&#41;)

[//]: # (- **Discussions**: [Join the community]&#40;https://github.com/pnex/iot-firmware/discussions&#41;)

[//]: # (### Pnex.io Platform)

[//]: # (This firmware is designed to work with the [Pnex.io]&#40;https://pnex.io&#41; platform:)

[//]: # (- **Website**: https://pnex.io)

[//]: # (- **Documentation**: Platform integration guides)

[//]: # (- **Source Code**: [Pnex.io platform repository]&#40;https://github.com/pnex/platform&#41; &#40;MIT licensed&#41;)

[//]: # (### Related Projects)

[//]: # (- [Pnex Platform]&#40;https://github.com/pnex/platform&#41;: Main platform server)

[//]: # (- [Pnex Mobile App]&#40;https://github.com/pnex/mobile&#41;: Mobile client application)

[//]: # (- [Pnex Web Dashboard]&#40;https://github.com/pnex/web&#41;: Web-based management interface)


---

**Note**: This firmware is specifically designed for use with the Pnex.io platform. While the code is open-source and can be modified for other purposes, full functionality requires integration with the Pnex.io server infrastructure.

*Last updated: December 2025*