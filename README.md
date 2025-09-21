
# 8-Channel PPM Receiver with nRF24L01 module

This repository contains code for an Arduino-based 8-channel PPM (Pulse Position Modulation) receiver, using the nRF24L01 wireless transceiver module. The receiver maps wireless control signals to PPM output, which can be used to control a wide range of RC devices.

## Features

- **8 PPM Channels**: Maps up to 8 wireless input channels into corresponding PPM values.
- **Failsafe Mode**: Automatically resets control signals to default values if the wireless signal is lost for more than 1 second.
- **LED Indicator**: Indicates whether data is being received from the transmitter.
- **Customizable PPM Signal Settings**: Allows for easy configuration of frame length, pulse length, and PPM signal pin.

## Hardware Requirements

- **Arduino Uno (or compatible board)**
- **nRF24L01 Transceiver Module**
- **PPM-Compatible Receiver Device** (or servos if adapted)
- **LED** (optional, for data reception indication)

## PPM Configuration

Modify the following parameters in the code as necessary:
- `channel_number`: Number of PPM channels (default: 8)
- `sigPin`: Arduino pin for PPM signal output (default: 2)
- `PPM_FrLen`: Length of the PPM frame in microseconds (default: 27000 µs)
- `PPM_PulseLen`: Length of each PPM pulse in microseconds (default: 400 µs)

## Circuit Connections

| Arduino Pin | Component                |
|-------------|--------------------------|
| 9           | nRF24L01 CE              |
| 10          | nRF24L01 CSN             |
| 2           | PPM Signal Output        |
| 8           | LED (optional, for data reception indication) |

### PPM Channel Mappings

The code maps received data into the following PPM channels:

| PPM Channel | Data Source |
|-------------|-------------|
| Channel 1   | Roll        |
| Channel 2   | Pitch       |
| Channel 3   | Throttle    |
| Channel 4   | Yaw         |
| Channel 5   | Aux1        |
| Channel 6   | Aux2        |
| Channel 7   | Aux3        |
| Channel 8   | Aux4        |

## Setup and Usage

1. **Install Required Libraries**: Ensure the following libraries are installed in your Arduino IDE:
   - `SPI`
   - `nRF24L01`
   - `RF24`

2. **Upload Code**: Open the code in the Arduino IDE, compile, and upload it to your Arduino board.

3. **Connect Hardware**: Assemble the components according to the Circuit Connections table above.

4. **Power On**: Power on both the receiver and transmitter. The LED should indicate data reception status.

## Code Overview

- **resetData()**: Resets control data to default 'safe' values if no signal is received.
- **setPPMValuesFromData()**: Maps received control data to PPM channel values.
- **recvData()**: Reads data from the nRF24L01 and updates the `lastRecvTime` for signal timeout tracking.
- **loop()**: Monitors connection status, sets PPM values, and handles failsafe conditions if needed.
- **ISR(TIMER1_COMPA_vect)**: Generates the PPM signal using Timer1 interrupt.

## Debugging

Serial output of channel data is available in the `loop()` function. Use the Serial Monitor to display throttle, yaw, pitch, roll, and auxiliary channels.

## License

MIT License

---

Author: Ayush Sharma  
GitHub: [Ayush-Zone](https://github.com/Ayush-Zone)
