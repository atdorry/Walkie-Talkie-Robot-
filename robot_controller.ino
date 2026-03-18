/*
 * robot_controller.ino  (Robot / Car unit)
 * -----------------------------------------
 * nRF24L01 wireless robot controller.
 * Receives movement commands from the remote unit and drives motors.
 * Also reads an HC-SR04 ultrasonic sensor and sends distance back.
 *
 * Commands received (single char):
 *   'F' — forward
 *   'B' — backward
 *   'L' — turn left
 *   'R' — turn right
 *   'S' — stop
 *   'U' — measure ultrasonic distance and transmit back
 *
 * Authors: Abdelrahman Tamer & Sama Adel
 *
 * Wiring:
 *   nRF24L01  CE  → pin 9
 *   nRF24L01  CSN → pin 8
 *   nRF24L01  MOSI/MISO/SCK → SPI pins (11/12/13)
 *   Motor driver ENA → pin A0 (PWM)
 *   Motor driver IN1 → pin 4
 *   Motor driver IN2 → pin 5
 *   Motor driver ENB → pin A1 (PWM)
 *   Motor driver IN3 → pin 6
 *   Motor driver IN4 → pin 7
 *   HC-SR04 TRIG    → pin 2
 *   HC-SR04 ECHO    → pin 3
 */

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// ── Radio ──────────────────────────────────────────────────────────────────────
RF24 radio(9, 8);   // CE, CSN
const byte ADDR[6] = "00001";

#define RX_MODE  0x5
#define TX_MODE  0xA
static byte radioMode = 0;

// ── Motor driver pins ──────────────────────────────────────────────────────────
#define ENA   A0
#define IN1    4
#define IN2    5
#define ENB   A1
#define IN3    6
#define IN4    7
#define MOTOR_SPEED 180   // 0–255 PWM speed

// ── Ultrasonic pins ────────────────────────────────────────────────────────────
#define TRIG_PIN  2
#define ECHO_PIN  3

// ── Radio helpers ──────────────────────────────────────────────────────────────

void startListening()
{
    if (radioMode != RX_MODE)
    {
        radio.openReadingPipe(0, ADDR);
        radio.startListening();
        radioMode = RX_MODE;
    }
}

void startTransmitting()
{
    if (radioMode != TX_MODE)
    {
        radio.stopListening();
        radio.openWritingPipe(ADDR);
        radioMode = TX_MODE;
    }
}

// ── Motor control ──────────────────────────────────────────────────────────────

void stopMotors()
{
    analogWrite(ENA, 0);
    analogWrite(ENB, 0);
}

void forward()
{
    digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);  analogWrite(ENA, MOTOR_SPEED);
    digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);  analogWrite(ENB, MOTOR_SPEED);
}

void backward()
{
    digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH); analogWrite(ENA, MOTOR_SPEED);
    digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH); analogWrite(ENB, MOTOR_SPEED);
}

void turnLeft()
{
    digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH); analogWrite(ENA, MOTOR_SPEED);
    digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);  analogWrite(ENB, MOTOR_SPEED);
}

void turnRight()
{
    digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);  analogWrite(ENA, MOTOR_SPEED);
    digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH); analogWrite(ENB, MOTOR_SPEED);
}

// ── Ultrasonic ─────────────────────────────────────────────────────────────────

float measureDistance()
{
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long t = pulseIn(ECHO_PIN, HIGH, 30000);
    if (t == 0) return -1;
    return (t * 0.0343f) / 2.0f;
}

// ── Command handler ────────────────────────────────────────────────────────────

void handleCommand(char cmd)
{
    Serial.print("[Robot] cmd=");
    Serial.println(cmd);

    switch (cmd)
    {
        case 'F': forward();  break;
        case 'B': backward(); break;
        case 'L': turnLeft(); break;
        case 'R': turnRight(); break;
        case 'S': stopMotors(); break;

        case 'U':
        {
            float d = measureDistance();
            char buf[16];
            if (d < 0)
                snprintf(buf, sizeof(buf), "OOR");       // out of range
            else
                snprintf(buf, sizeof(buf), "D:%.1f", d);

            startTransmitting();
            radio.write(buf, strlen(buf) + 1);
            startListening();

            Serial.print("[Robot] distance sent: ");
            Serial.println(buf);
            break;
        }

        default:
            Serial.print("[Robot] unknown command: ");
            Serial.println(cmd);
            break;
    }
}

// ── Setup ──────────────────────────────────────────────────────────────────────

void setup()
{
    Serial.begin(9600);

    // Motor pins
    pinMode(ENA, OUTPUT); pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
    pinMode(ENB, OUTPUT); pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
    stopMotors();

    // Ultrasonic pins
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    // Radio
    radio.begin();
    radio.setPALevel(RF24_PA_MIN);
    radio.setDataRate(RF24_250KBPS);   // more reliable at low PA
    radio.setChannel(100);
    startListening();

    Serial.println("[Robot] Ready — waiting for commands.");
}

// ── Loop ──────────────────────────────────────────────────────────────────────

void loop()
{
    startListening();

    if (radio.available())
    {
        char cmd = 0;
        radio.read(&cmd, sizeof(cmd));
        handleCommand(cmd);
    }
}
