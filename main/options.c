#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <stdint.h>

#include "options.h"


int16_t getOption16(char *name, int16_t defaultValue)
{
  esp_err_t ret;
  nvs_handle_t _options_handle = 0;
  int16_t _value=defaultValue;
  
  ret = nvs_open("options", NVS_READWRITE, &_options_handle);
  if (ret == ESP_OK) {
     ret = nvs_get_i16(_options_handle, name, &_value);
     if(ret == ESP_ERR_NVS_NOT_FOUND) {
        ret = nvs_set_i16(_options_handle, name, _value);
        if(ret==ESP_OK) {
           nvs_commit(_options_handle);
        }
        else {
        }
     }
  }
  nvs_close(_options_handle);
  return _value;
}


int16_t setOption16(char *name, int16_t value)
{
  esp_err_t ret;
  nvs_handle_t _options_handle = 0;

  ret = nvs_open("options", NVS_READWRITE, &_options_handle);
  if (ret == ESP_OK) {
     ret = nvs_set_i16(_options_handle, name, value);
     if(ret==ESP_OK) {
        nvs_commit(_options_handle);
     }
  }
  nvs_close(_options_handle);

  return value;
}
