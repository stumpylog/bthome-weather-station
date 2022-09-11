#include "blackboard.h"
#include "bthome.h"
#include "esp_bt.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_gatt_defs.h"
#include "esp_gattc_api.h"
#include "esp_log.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "tasks.h"

#include <stdint.h>

static esp_ble_adv_params_t ble_adv_params = {
    .adv_int_min       = 0x20,
    .adv_int_max       = 0x40,
    .adv_type          = ADV_TYPE_NONCONN_IND,
    .own_addr_type     = BLE_ADDR_TYPE_PUBLIC,
    .channel_map       = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

static uint8_t advertData[256];

static const esp_bt_uuid_t service_uuid = {
    .len = ESP_UUID_LEN_16,
    .uuid =
        {
            .uuid16 = UNENCRYPTED_SERVICE_UUID,
        },
};

static esp_ble_adv_data_t ble_adv_data = {.set_scan_rsp        = false,
                                          .include_name        = true,
                                          .include_txpower     = true,
                                          .min_interval        = 0x20,
                                          .max_interval        = 0x40,
                                          .appearance          = 0,
                                          .manufacturer_len    = 0,
                                          .p_manufacturer_data = NULL,
                                          .service_data_len    = 0,
                                          .p_service_data      = &advertData[0],
                                          .service_uuid_len    = service_uuid.len,
                                          .p_service_uuid      = &service_uuid.uuid.uuid16};

void ble_init(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));

    ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_BLE));

    ESP_ERROR_CHECK(esp_bluedroid_init());
    ESP_ERROR_CHECK(esp_bluedroid_enable());
    ESP_ERROR_CHECK(esp_ble_gap_set_device_name("weather-station"));
}

void task_ble_entry(void* params)
{
    ble_init();

    vTaskDelay(200 / portTICK_PERIOD_MS);

    for (;;)
    {
        // Wait for sensor task notify
        // Clear all bits on exit to reset
        if (pdTRUE == xTaskNotifyWait(0, 0xFFFFFFFF, NULL, 500 / portTICK_PERIOD_MS))
        {
            // Encode sensor data
            int32_t bytes = temperature(blackboard.sensors.temperature, &advertData[0], 256);

            bytes += humidity(blackboard.sensors.humidity, &advertData[bytes], 256);

            // Set advertising length
            ble_adv_data.service_data_len = bytes;

            // Configure advertising data
            ESP_ERROR_CHECK(esp_ble_gap_config_adv_data(&ble_adv_data));

            // Begin advertising
            ESP_ERROR_CHECK(esp_ble_gap_start_advertising(&ble_adv_params));

            vTaskDelay(500 / portTICK_PERIOD_MS);
        }
        else
        {
        }
    }
}