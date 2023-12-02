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
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include <HardwareSerial.h>

#define DELAY_TO_SAMPLING_TEMP  30000
#define DELAY_TO_SAMPLING_HUMI  30000
#define DELAY_TO_SAMPLING_LIGHT 30000
#define DELAY_TO_SAMPLING_SOIL  30000

uint8_t pump = 0;
unsigned long previousMillis = 0UL;
unsigned long interval = 700UL;

QueueHandle_t dataQueue;

HardwareSerial SerialPort(2);

void setup()
{
  begin_temp_humi();
  setup_lcd();

  dataQueue = xQueueCreate(30, sizeof(sensor_data));

  xTaskCreatePinnedToCore(control_led, "control_led", 4098, NULL, 10, NULL, 0);
  xTaskCreatePinnedToCore(send_to_gateway, "send_to_gateway", 4098, NULL, 2, NULL, 0);
  xTaskCreatePinnedToCore(temp_sampling, "temp_sampling", 4098, NULL, 2, NULL, 0);
  xTaskCreatePinnedToCore(humi_sampling, "humi_sampling", 4098, NULL, 2, NULL, 0);
  xTaskCreatePinnedToCore(light_sampling, "light_sampling", 4098, NULL, 2, NULL, 0);
  xTaskCreatePinnedToCore(soil_sampling, "soil_sampling", 4098, NULL, 2, NULL, 0);
  xTaskCreatePinnedToCore(render_lcd, "render_lcd", 4098, NULL, 5, NULL, 1);
  
  // // Init Serial Monitor

  Serial.begin(115200);
  SerialPort.begin (9600, SERIAL_8N1, RX_pin, TX_pin);
  
  pinMode(pump_pin, OUTPUT);
  pinMode(rgb_R_pin, OUTPUT);
  pinMode(rgb_G_pin, OUTPUT);
  pinMode(rgb_B_pin, OUTPUT);

}

void loop()
{
}

void temp_sampling(void* arg) {
  sensor_data data;
  while (1) {
    data.id = TEMP;
    temp_value = get_temp();
    data.value = temp_value;
    xQueueSendToBack(dataQueue, &data, (TickType_t)10);
    vTaskDelay(pdMS_TO_TICKS(DELAY_TO_SAMPLING_TEMP));
  }
}

void humi_sampling(void* arg) {
  sensor_data data;
  while (1) {
    data.id = HUMI;
    humi_value = get_humi();
    data.value = humi_value;
    xQueueSendToBack(dataQueue, &data, (TickType_t)10);
    vTaskDelay(pdMS_TO_TICKS(DELAY_TO_SAMPLING_HUMI));
  }
}

void light_sampling(void* arg) {
  sensor_data data;
  while (1) {
    data.id = LIGHT;
    light_value = get_light();
    data.value = light_value;
    xQueueSendToBack(dataQueue, &data, (TickType_t)10);
    vTaskDelay(pdMS_TO_TICKS(DELAY_TO_SAMPLING_LIGHT));
  }
}

void soil_sampling(void* arg) {
  sensor_data data;
  while (1) {
    data.id = SOIL;
    soil_value = get_moisture_soil();
    data.value = soil_value;
    xQueueSendToBack(dataQueue, &data, (TickType_t)10);
    vTaskDelay(pdMS_TO_TICKS(DELAY_TO_SAMPLING_SOIL));
  }
}

void send_to_gateway(void* arg) {
  sensor_data data;
  char buffer[100];
  while (1) {
    if (xQueuePeek(dataQueue, &data, (TickType_t)10) == pdPASS) {
      switch (data.id) {
        case TEMP:
          Serial.println("Receive temp request");
          Serial.println(data.value);
          xQueueReceive(dataQueue, &data, (TickType_t)10);
          sprintf(buffer, "!%d:TEMP:%d#", DEVICE_ID, data.value);
          break;
        case HUMI:
          Serial.println("Receive humi request");
          Serial.println(data.value);
          xQueueReceive(dataQueue, &data, (TickType_t)10);
          sprintf(buffer, "!%d:HUMI:%d#", DEVICE_ID, data.value);
          break;
        case LIGHT:
          Serial.println("Receive light request");
          Serial.println(data.value);
          xQueueReceive(dataQueue, &data, (TickType_t)10);
          sprintf(buffer, "!%d:LIGHT:%d#", DEVICE_ID, data.value);
          break;
        case SOIL:
          Serial.println("Receive soil request");
          Serial.println(data.value);
          xQueueReceive(dataQueue, &data, (TickType_t)10);
          sprintf(buffer, "!%d:SOIL:%d#", DEVICE_ID, data.value);
          break;
        default:
          break;
      }
      // SerialPort.print(buffer);
      vTaskDelay(pdMS_TO_TICKS(100));
    }
  }
}

void render_lcd(void* arg) {
  while (1) {
    print_lcd();
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void control_led_pump(void* arg) {
  uint8_t buffer[100];
  uint8_t id;
  uint8_t name[20];
  uint8_t value;
  uint8_t i = 0;
  while (1) {
    if (SerialPort.read() == '!') {
      buffer[0] = '!';
      digitalWrite(led_pin, !digitalRead(led_pin));    
      for (i=1; i<100; i++) {
        if (buffer[i] == '#') {
          buffer[i] = '#';
          buffer[i+1] = '\0';
          break;
        }
        buffer[i] = SerialPort.read();
      }
      sscanf((const char *)buffer, "!%d:%[^:]:%d#", &id, name, &value);
      SerialPort.write(buffer, 20);
      // if (strcmp((const char*) name, "button1")==0) {
      //   digitalWrite(led_pin, value);
      // } else if (strcmp((const char*) name, "button2")==0) {
      //   digitalWrite(pump_pin, value);
      // }
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}
