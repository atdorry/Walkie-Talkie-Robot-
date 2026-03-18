/*
 * remote_unit.ino  (Hand-held remote unit)
 * ------------------------------------------
 * nRF24L01 wireless remote — sends single-character commands to the robot.
 * Also listens for distance readings sent back by the robot.
 *
 * Commands (type in Serial Monitor and press Enter):
 *   F — forward
 *   B — backward
 *   L — left
 *   R — right
 *   S — stop
 *   U — request ultrasonic distance from robot
 *
 * Authors: Abdelrahman Tamer & Sama Adel
 *
 * Wiring:
 *   nRF24L01  CE  → pin 9
 *   nRF24L01  CSN → pin 8
 *   nRF24L01  MOSI/MISO/SCK → SPI pins (11/12/13)
 *
 * Serial monitor: 9600 baud
 * Both units MUST use the same ADDR, channel, and data rate.
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

// ── Send a command ─────────────────────────────────────────────────────────────

void sendCommand(char cmd)
{
    startTransmitting();
    bool ok = radio.write(&cmd, sizeof(cmd));

    Serial.print("[Remote] sent '");
    Serial.print(cmd);
    Serial.print("' → ");
    Serial.println(ok ? "OK" : "FAILED");

    // Return to listening so we can receive distance replies
    startListening();
}

// ── Setup ──────────────────────────────────────────────────────────────────────

void setup()
{
    Serial.begin(9600);

    radio.begin();
    radio.setPALevel(RF24_PA_MIN);
    radio.setDataRate(RF24_250KBPS);
    radio.setChannel(100);
    startListening();

    Serial.println("[Remote] Ready.");
    Serial.println("Commands: F=forward  B=backward  L=left  R=right  S=stop  U=ultrasonic");
}

// ── Loop ──────────────────────────────────────────────────────────────────────

void loop()
{
    // ── 1. Read incoming data from robot (e.g. distance reply) ───────────
    if (radio.available())
    {
        char buf[32] = {0};
        radio.read(buf, sizeof(buf) - 1);
        Serial.print("[Remote] received: ");
        Serial.println(buf);
    }

    // ── 2. Read command from Serial Monitor ──────────────────────────────
    if (Serial.available())
    {
        char cmd = (char)Serial.read();

        // Flush rest of line (newline chars from Serial Monitor)
        while (Serial.available()) Serial.read();

        // Validate command
        const char VALID[] = "FBLRSUfblrsu";
        bool valid = false;
        for (int i = 0; VALID[i]; i++)
            if (cmd == VALID[i]) { valid = true; break; }

        if (valid)
            sendCommand((char)toupper(cmd));
        else if (cmd != '\n' && cmd != '\r')
        {
            Serial.print("[Remote] unknown: '");
            Serial.print(cmd);
            Serial.println("'  (F/B/L/R/S/U)");
        }
    }
}
