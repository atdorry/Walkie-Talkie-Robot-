# 📡 Wireless Robot Controller — nRF24L01

A two-unit wireless system using nRF24L01 radio modules. The **remote unit** sends single-character movement commands from the Serial Monitor. The **robot unit** receives them and drives motors accordingly. The robot can also measure distance with an HC-SR04 ultrasonic sensor and transmit the reading back to the remote.

---

## ✨ Features

- ✅ Bidirectional nRF24L01 wireless communication
- ✅ Motor control — forward, backward, left, right, stop
- ✅ Ultrasonic distance reading sent back to remote on demand
- ✅ Proper TX/RX mode switching — no missed messages
- ✅ `RF24_250KBPS` data rate + fixed channel for reliable link
- ✅ `radio.write()` success/failure feedback on remote
- ✅ Command validation — unknown commands rejected with message

---

## 📁 Files

| File | Upload To |
|------|-----------|
| `robot_controller/robot_controller.ino` | Robot Arduino (on the car) |
| `remote_unit/remote_unit.ino` | Remote Arduino (handheld) |

> Both units **must** use the same address, channel, and data rate — already matched in both files.

---

## 🔌 Wiring

### Remote Unit

| nRF24L01 | Arduino |
|----------|---------|
| VCC      | 3.3V ⚠️ (NOT 5V) |
| GND      | GND     |
| CE       | Pin 9   |
| CSN      | Pin 8   |
| MOSI     | Pin 11  |
| MISO     | Pin 12  |
| SCK      | Pin 13  |

### Robot Unit

| Component | Arduino Pin |
|-----------|-------------|
| nRF24L01 (same as above) | 3.3V, 9, 8, 11, 12, 13 |
| L298N ENA (PWM)  | A0 |
| L298N IN1        | 4  |
| L298N IN2        | 5  |
| L298N ENB (PWM)  | A1 |
| L298N IN3        | 6  |
| L298N IN4        | 7  |
| HC-SR04 TRIG     | 2  |
| HC-SR04 ECHO     | 3  |

> ⚠️ The nRF24L01 runs at **3.3V** — connecting to 5V will damage the module.

---

## 🎮 Commands

Type these in the **remote unit's** Serial Monitor and press Enter:

| Command | Action |
|---------|--------|
| `F` | Forward |
| `B` | Backward |
| `L` | Turn left |
| `R` | Turn right |
| `S` | Stop |
| `U` | Request ultrasonic distance reading |

Lowercase versions (`f`, `b`, `l`, `r`, `s`, `u`) also accepted.

---

## 📟 Serial Monitor Output

**Remote unit:**
```
[Remote] Ready.
Commands: F=forward  B=backward  L=left  R=right  S=stop  U=ultrasonic
[Remote] sent 'F' → OK
[Remote] sent 'U' → OK
[Remote] received: D:23.4
```

**Robot unit:**
```
[Robot] Ready — waiting for commands.
[Robot] cmd=F
[Robot] cmd=U
[Robot] distance sent: D:23.4
```

---

## ⚙️ Configuration

Both files share these settings (must match on both units):

```cpp
const byte ADDR[6] = "00001";       // pipe address
radio.setDataRate(RF24_250KBPS);    // data rate
radio.setChannel(100);              // RF channel (0–125)
```

Adjust motor speed on the robot:
```cpp
#define MOTOR_SPEED 180   // 0–255 PWM
```

---

## 🚀 Getting Started

1. Wire both units as shown
2. Upload `robot_controller.ino` to the robot Arduino
3. Upload `remote_unit.ino` to the remote Arduino
4. Open Serial Monitor on the **remote** at **9600 baud**
5. Type commands and press Enter

---

## 📄 License

MIT License

---

## 👤 Author

**Abdelrahman Tamer Dorry**
AI Engineering Student — Galala University
[LinkedIn](https://www.linkedin.com/in/abdelrahman-dorry) · [GitHub](https://github.com/atdorry)
