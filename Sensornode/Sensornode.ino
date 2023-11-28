#define DEVICE_ID 1

#include <esp_now.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include "temp_humi.h"
#include "soil.h"
#include "light.h"
#include "global.h"
#include "ultrasonic.h"
#include "lcd.h"
#include "rgb.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include <HardwareSerial.h>


uint8_t pump = 0;
unsigned long previousMillis = 0UL;
unsigned long interval = 700UL;

QueueHandle_t gatewayQueue;
QueueHandle_t lcdQueue;

HardwareSerial SerialPort(2);

void setup()
{
  gatewayQueue = xQueueCreate(20, sizeof(message));
  lcdQueue = xQueueCreate(20, sizeof(message));

  xTaskCreate(blink, "blink", 1024, NULL, 2, NULL);
  xTaskCreate(sampling, "sampling", 4098, NULL, 2, NULL);
  xTaskCreate(send_to_gateway, "send_to_gateway", 4098, NULL, 2, NULL);
  // xTaskCreate(render_lcd, "render_lcd", 4098, NULL, 2, NULL);
  
  // // Init Serial Monitor

  Serial.begin(115200);
  Serial.println("Hello world");
  SerialPort.begin (115200, SERIAL_8N1, RX_pin, TX_pin);
  
  pinMode(pump_pin, OUTPUT);
  // pinMode(rgb_R_pin, OUTPUT);
  // pinMode(rgb_G_pin, OUTPUT);
  // pinMode(rgb_B_pin, OUTPUT);
  // pinMode(ultrasonic_trig_pin, OUTPUT);
  // pinMode(ultrasonic_echo_pin, INPUT);

  // setup_lcd();
}

void loop()
{
}

void sampling(void* arg) {
  begin_temp_humi();
  message msg;
  while (1) {
    msg.temp = get_temp();
    msg.humi = get_humi();
    msg.soil = get_moisture_soil();
    msg.light = get_light();
    xQueueSendToBack(gatewayQueue, &msg, (TickType_t)10);
    xQueueSendToBack(lcdQueue, &msg, (TickType_t)10);
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

void send_to_gateway(void* arg) {
  message msg;
  char buffer[100];
  while (1) {
    if (xQueuePeek(gatewayQueue, &msg, (TickType_t)10) == pdPASS) {
      xQueueReceive(gatewayQueue, &msg, (TickType_t)10);
      sprintf(buffer, "!TEMP:%d#!HUMI:%d#!LIGHT:%d#!SOIL:%d#", msg.temp, msg.humi, msg.light, msg.soil);
      SerialPort.print(buffer);
    }
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

void render_lcd(void* arg) {
  message msg;
  while (1) {
    if (xQueuePeek(lcdQueue, &msg, (TickType_t)10) == pdPASS) {
      xQueueReceive(lcdQueue, &msg, (TickType_t)10);
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void blink(void* arg) {
  while (1) {
    digitalWrite(pump_pin, !digitalRead(pump_pin));
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}