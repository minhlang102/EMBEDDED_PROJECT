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

#define DELAY_TO_SAMPLING_TEMP  10000
#define DELAY_TO_SAMPLING_LIGHT 10000
#define DELAY_TO_SAMPLING_SOIL  10000

uint8_t pump = 0;
unsigned long previousMillis = 0UL;
unsigned long interval = 700UL;

QueueHandle_t dataQueue;

void setup()
{
  begin_temp_humi();
  setup_lcd();

  dataQueue = xQueueCreate(30, sizeof(sensor_data));

  xTaskCreatePinnedToCore(control_led_pump, "control_led", 4098, NULL, 10, NULL, 0);
  xTaskCreatePinnedToCore(send_to_gateway, "send_to_gateway", 4098, NULL, 2, NULL, 0);
  xTaskCreatePinnedToCore(temp_sampling, "temp_sampling", 4098, NULL, 2, NULL, 0);
  xTaskCreatePinnedToCore(light_sampling, "light_sampling", 4098, NULL, 2, NULL, 0);
  xTaskCreatePinnedToCore(soil_sampling, "soil_sampling", 4098, NULL, 2, NULL, 0);
  xTaskCreatePinnedToCore(render_lcd, "render_lcd", 4098, NULL, 5, NULL, 1);
  
  // // Init Serial Monitor

  Serial.begin(115200);
  Serial2.begin(115200);
  
  pinMode(pump_pin, OUTPUT);
  pinMode(led_pin, OUTPUT);
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
    data.id = HUMI;
    humi_value = get_moisture_soil();
    data.value = humi_value;
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
          Serial.print("Receive temp request: ");
          Serial.println(data.value);
          xQueueReceive(dataQueue, &data, (TickType_t)10);
          sprintf(buffer, "!%d:TEMP:%d#", DEVICE_ID, data.value);
          break;
        case HUMI:
          Serial.print("Receive humi request: ");
          Serial.println(data.value);
          xQueueReceive(dataQueue, &data, (TickType_t)10);
          sprintf(buffer, "!%d:HUMI:%d#", DEVICE_ID, data.value);
          break;
        case LIGHT:
          Serial.print("Receive light request: ");
          Serial.println(data.value);
          xQueueReceive(dataQueue, &data, (TickType_t)10);
          sprintf(buffer, "!%d:LIGHT:%d#", DEVICE_ID, data.value);
          break;
        default:
          break;
      }
      Serial2.print(buffer);
      vTaskDelay(pdMS_TO_TICKS(100));
    }
  }
}

void render_lcd(void* arg) {
  while (1) {
    print_lcd();
    vTaskDelay(pdMS_TO_TICKS(10000));
  }
}

void control_led_pump(void* arg) {
  uint8_t buffer[100];
  uint8_t id;
  char name[20];
  uint8_t value;
  uint8_t i = 0;
  char myData[50];
  while (1) {
    if (Serial2.available() > 0)
    {
      Serial2.read(myData, 20);
      char *token = strtok(myData, ":#!");
      if (token != NULL) {
          id = atoi(token);

          token = strtok(NULL, ":#!");
          if (token != NULL) {
              strcpy(name, token);

              token = strtok(NULL, ":#!");
              if (token != NULL) {
                  value = atoi(token);
              }
          }
      }
      if (strcmp((const char*) name, "button1") == 0) {
        sprintf(myData, "!%d:%s:%d#", id, name, value);
        digitalWrite(led_pin, value);
        Serial2.write(myData, 20);
      } else if (strcmp((const char*) name, "button2") == 0) {
        sprintf(myData, "!%d:%s:%d#", id, name, value);
        digitalWrite(pump_pin, value);
        Serial2.write(myData, 20);
      }
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}
