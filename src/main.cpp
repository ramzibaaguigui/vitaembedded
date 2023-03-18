#include <BluetoothSerial.h>
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <SoftwareSerial.h>

#define FINGERPRINT_RX 7
#define FINGERPRINT_TX 8

#define MESSAGE_OPENING '{'
#define MESSAGE_CLOSING '}'

#define FINGERPRINT_HEIGHT 100 
#define FINGERPRINT_WIDTH 100

#define STATE_SCANNER_IDLE 0;
#define STATE_SCANNER_READING 1;
#define STATE_SCANNER_FINISHED_READING 2;


// Serial
BluetoothSerial bluetooth_serial;
SoftwareSerial fingerprint(FINGERPRINT_RX, FINGERPRINT_TX);

// Task handles
TaskHandle_t from_bluetooth_handle;
TaskHandle_t from_serial_handle;


// global variable
static char* finger;

void handle_received_from_bluetooth(void* param) {
    while(1) {
        if(bluetooth_serial.available()) {
            byte b = bluetooth_serial.read();
            Serial.print((char) b);
        }
    }
}

void handle_received_from_serial(void* param) {
    while(1) {
        if(Serial.available()) {
            byte b = Serial.read();
            bluetooth_serial.write((char) b);
        }
    }
}

void handle_receiver_from_scan_reader(void* param) {
    int currentIndex = 0;
    int currentState = STATE_SCANNER_IDLE;
    finger = "";

    while(1) {
        if(fingerprint.available()) {
            char current = fingerprint.read();
            // depends on the symbol and on the current state
            if (current == MESSAGE_OPENING) {
                currentState = STATE_SCANNER_READING;
                currentIndex = 0;
                finger = "";
            } else if(current == MESSAGE_CLOSING) {
                currentState = STATE_SCANNER_FINISHED_READING;
            } else {
                // handle the remainging states
            }
        }
    }

}

void print_available_stack_space(void* param) {
    uxTaskGetStackHighWaterMark(NULL);
}


void setup() {
    Serial.begin(9600);
    delay(300);
    Serial.println("connected serial");
    bluetooth_serial.begin();
    delay(300);
    Serial.println("connected bluetooth");

    xTaskCreate(handle_received_from_bluetooth, "bluetooth_to_serial", 1024, NULL, 0, &from_bluetooth_handle);
    xTaskCreate(handle_received_from_serial   , "serial_to_bluetooth", 1024, NULL, 0, &from_serial_handle);
}

void loop() {

}