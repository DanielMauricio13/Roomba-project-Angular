# CyBot Autonomous Navigation Platform

## Introduction
CyBot is a semester-long embedded systems project that equips an iRobot Create 2 platform with custom TM4C123G-based firmware, a Python Socket.IO bridge, and a modern Angular dashboard for semi-autonomous navigation. The original project proposal outlines the vision, milestones, and evaluation metrics; keep it handy for additional diagrams and background context.

- 📄 Project proposal: [288.project.proposal.1.pdf](https://github.com/user-attachments/files/18512734/288.project.proposal.1.pdf)

## Project Overview
The repository contains everything needed to plan missions, communicate with the robot, and visualize sensor feedback:

- `z_final_proj/`: TM4C123G firmware that performs sensor fusion, motion control, and message formatting for the Create 2 base.
- `Python-Socket/`: Socket.IO bridge that proxies commands between the web UI and the robot over Wi-Fi or a serial tunnel.
- `Angular-UI/`: Angular 14 single-page application that renders the mission canvas, obstacle map, and robot telemetry.
- `UI/User-Interface/`: Legacy Swing prototype retained for historical reference.

## Key Features
- Interactive mission canvas that lets operators click-to-drive the robot or right-click to reset its state via Socket.IO. 
- Obstacle, cliff, and bump detection rendered in real time using laser range, ultrasonic ping, and bumper telemetry from the TM4C firmware.
- Python eventlet-based bridge that forwards Socket.IO messages to the robot (or a mock simulator) and relays `START ... END` telemetry packets back to the UI.
- Firmware routines for sweeping the servo, building obstacle profiles, auto-correcting wheel drift, and safely reacting to cliffs or bump sensors before reversing to safety.
- Modular codebase that supports both live hardware and offline simulation for regression testing and UI development.

## System Architecture
```mermaid
flowchart LR
  UI[Angular Web UI\n(Angular-UI)]
  Bridge[Python Socket.IO Bridge\n(Python-Socket)]
  Firmware[TM4C123G Firmware\n(z_final_proj)]
  Create2[iRobot Create 2 Base]

  UI -- Socket.IO --> Bridge
  Bridge -- TCP 288 --> Firmware
  Firmware -- Serial Sensors --> Create2
  Firmware -- Telemetry Packets --> Bridge
  Bridge -- Events --> UI
```

### Component Responsibilities
| Layer | Responsibilities | Key Technologies |
| --- | --- | --- |
| Angular UI | Canvas visualization, mission planning, user interactions, Socket.IO client | Angular 14, RxJS, HTML5 Canvas |
| Python Bridge | Socket.IO server, hardware proxy, message queueing, optional mock interface | Flask-SocketIO, Eventlet, Gevent |
| Firmware | Sensor calibration, motion control, obstacle mapping, command parser | C, TivaWare, TM4C123GH6PM |

## Installation
### 1. Hardware & Firmware
1. Provision an iRobot Create 2 with the TM4C123GXL LaunchPad, HC-SR04 ping sensor, TOF laser (ADC based), and servo (PB5 PWM).
2. Install Code Composer Studio (CCS) or an equivalent ARM GCC toolchain with TivaWare drivers.
3. Open `z_final_proj` in CCS, ensure the provided `targetConfigs/Tiva TM4C123GH6PM.ccxml` matches your debugger, and build/flash the project to the LaunchPad.
4. Verify UART1 is wired to your Wi-Fi module or USB serial adapter at 115200 bps.

### 2. Python Socket Bridge
```bash
cd Python-Socket
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
```
Set `useCybot` in `SockioServer.py` to `False` for offline testing or update the IP/port pair for your robot when going live.

### 3. Angular Mission Control UI
```bash
cd Angular-UI
npm install
npm run start
```
Navigate to `http://localhost:4200` to load the dashboard. The UI expects the Socket.IO bridge on `ws://127.0.0.1:5000/`.

## Usage
1. Power the Create 2 and LaunchPad, ensuring the firmware starts broadcasting `START READY END` packets.
2. Launch the Python bridge: `python SockioServer.py`. Confirm that it connects to the robot (or the mock stream) and begins relaying messages.
3. Start the Angular UI and open the canvas. Left-click to send a `(angle,distance)q` tuple for point-to-point navigation, or right-click to free the sensors.
4. Monitor the obstacle list, movement confirmations, and safety alerts in the browser console or UI overlays.

### Telemetry & Command Grammar
| Packet | Description | Example |
| --- | --- | --- |
| `START READY END` | Robot is idle and ready for the next command. | `START READY END` |
| `START turn <deg> END` | Firmware just completed a turn and reports the measured angle. | `START turn 45 END` |
| `START move <cm> END` | Firmware completed a translation and reports distance travelled (positive forward, negative backward). | `START move 60 END` |
| `START obj <angle>,<distance>,<width> END` | Detected a tall obstacle during a sweep; UI renders it on the canvas. | `START obj 30, 80, 25 END` |
| `START bump <l|r|b> END` | Bumper contact on left/right/both triggered an emergency stop; the UI marks a short obstacle. | `START bump l END` |
| `START cliff <ll|ml|mr|rr> END` | Cliff sensor detected a drop-off; the UI paints a cliff warning. | `START cliff mr END` |

## Architecture Notes
- Servo sweeps between 45° and 135° while driving to watch for frontal obstacles; hits pause forward motion when the laser ADC trips a near-threshold value.
- Wheel speed is dynamically trimmed based on encoder feedback to maintain a straight trajectory.
- Upon any stop flag, the firmware reverses 15 cm before resuming mission planning and plays a tone to notify the operator.

## Testing & Verification
- **Angular UI**: `npm test` executes Karma/Jasmine unit tests for services and components.
- **Python Bridge**: Use `pytest` (not yet configured) or run `SockioServer.py` with `useCybot = False` and issue manual inputs to validate message routing.
- **Firmware**: Exercise command sequences over UART using a serial terminal to confirm obstacle and cliff responses before field deployment.

## Future Enhancements
- Add automated end-to-end tests that spin up the Socket.IO bridge with a mock CyBot to drive the Angular UI during CI.
- Expand the mission planner with waypoint queues and area coverage patterns.
- Overlay sensor confidence heatmaps and allow data export for analytics.
- Containerize the Python bridge and Angular UI for easier deployment on a base station.

## Team & Contact
This project was developed by CPRE 288 teammates including Coby Konkol, Daniel Vergara, Nate Trotter, and Murali Patibandla. Refer to the project proposal for instructor contacts and evaluation rubric, or open a GitHub issue for repository-related questions.

## References
- Primary documentation and diagrams: [Project Proposal (PDF)](https://github.com/user-attachments/files/18512734/288.project.proposal.1.pdf)
- [Angular CLI documentation](https://angular.io/cli) for front-end tooling support.
