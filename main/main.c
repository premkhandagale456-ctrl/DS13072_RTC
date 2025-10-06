#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"

#include "DS13072.h"
#include "DS13072_platform.h"

static const char *TAG = "RTC";

void app_main(void)
{
  DS1307_Handler_t Handler;
  DS1307_DateTime_t DateTime =
  {
    .Second   = 0,
    .Minute   = 52,
    .Hour     = 4,
    .WeekDay  = 1,
    .Day      = 06,
    .Month    = 10,
    .Year     = 25
  };

  ESP_LOGI(TAG, "DS1307 Driver Example");

  DS1307_Platform_Init(&Handler);
  DS1307_Init(&Handler);
  DS1307_SetDateTime(&Handler, &DateTime);
  DS1307_SetOutWave(&Handler, DS1307_OutWave_1Hz);

  while (1)
  {
    DS1307_GetDateTime(&Handler, &DateTime);
    ESP_LOGI(TAG, "Date: 20%02u/%02u/%02u", DateTime.Year, DateTime.Month, DateTime.Day);
    ESP_LOGI(TAG, "Time: %02u:%02u:%02u", DateTime.Hour, DateTime.Minute, DateTime.Second);
    ESP_LOGI(TAG, "WeekDay: %u", DateTime.WeekDay);

    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }

  DS1307_DeInit(&Handler);
}