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
  DS13072_Handler_t Handler;
  // set the date and time
  DS13072_DateTime_t DateTime =
  {
    .Second   = 0,
    .Minute   = 52,
    .Hour     = 20,
    .WeekDay  = 1,
    .Day      = 06,
    .Month    = 10,
    .Year     = 25,
    .HourMode = 0, // 0 = 24-hour mode, 1 = 12-hour mode
    .isPM     = 1  // 1 = PM , 0 = AM
  };

  DS13072_Platform_Init(&Handler);
  DS13072_Init(&Handler);
  if(DS13072_SetDateTime(&Handler, &DateTime) != ESP_OK){
     ESP_LOGI(TAG, "Failed to set date and time");
  }
  DS13072_SetOutWave(&Handler, DS13072_OutWave_1Hz);

  while (1)
  {
    if(DS13072_GetDateTime(&Handler, &DateTime) == DS13072_OK){

      if(DateTime.HourMode == 1){
        // Convert isPM to string for printing
        const char *ampm = (DateTime.isPM) ? "PM" : "AM";
        ESP_LOGI(TAG, "Date: %02u/%02u/%02u", DateTime.Day, DateTime.Month, DateTime.Year);
        ESP_LOGI(TAG, "Time: %02u:%02u:%02u %s", DateTime.Hour, DateTime.Minute, DateTime.Second, ampm);
        ESP_LOGI(TAG, "WeekDay: %u", DateTime.WeekDay);
      }
      else{
        ESP_LOGI(TAG, "Date: %02u/%02u/%02u", DateTime.Day, DateTime.Month, DateTime.Year);
        ESP_LOGI(TAG, "Time: %02u:%02u:%02u", DateTime.Hour, DateTime.Minute, DateTime.Second);
        ESP_LOGI(TAG, "WeekDay: %u", DateTime.WeekDay);
      }

    }else{
      ESP_LOGI(TAG, "RTC not detected!! check RTC connected or not");
    }

    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }

  DS13072_DeInit(&Handler);
}