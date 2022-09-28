#ifdef __cplusplus
extern "C" {
#endif

void task_ble_entry(void* params);

#ifdef __cplusplus
}
#endif

#include "blackboard.h"
#include "bthome.h"
#include "esp_bt.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_gatt_defs.h"
#include "esp_gattc_api.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "tasks.h"

#include <stdint.h>
#include <string>

static constexpr uint32_t US_TO_S_FACTOR {1000000};
static constexpr uint32_t SECONDS_PER_MINUTE {60};
static constexpr uint64_t SLEEP_1_MINUTE {SECONDS_PER_MINUTE * US_TO_S_FACTOR};
static constexpr uint64_t SLEEP_5_MINUTES {SLEEP_1_MINUTE * 5};

static esp_ble_adv_params_t ble_adv_params = {
    .adv_int_min       = 0x20,
    .adv_int_max       = 0x40,
    .adv_type          = ADV_TYPE_NONCONN_IND,
    .own_addr_type     = BLE_ADDR_TYPE_PUBLIC,
    .peer_addr_type    = BLE_ADDR_TYPE_PUBLIC,
    .channel_map       = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

static uint8_t advertData[64];

static esp_bt_uuid_t service_uuid = {
    .len = ESP_UUID_LEN_16,
    .uuid =
        {
            .uuid16 = bthome::constants::UNENCRYPTED_SERVICE_UUID,
        },
};

void ble_init(void)
{
    ESP_LOGI("ble_task", "Starting BLE init");
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));

    ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_BLE));

    ESP_ERROR_CHECK(esp_bluedroid_init());
    ESP_ERROR_CHECK(esp_bluedroid_enable());
    // ESP_ERROR_CHECK(esp_ble_gap_set_device_name("weather-station"));

    service_uuid.uuid.uuid128[13] = (service_uuid.uuid.uuid16 >> 8) & 0xff;
    service_uuid.uuid.uuid128[12] = service_uuid.uuid.uuid16 & 0xff;

    ESP_LOGI("ble_task", "BLE init completed");
}

void ble_deinit(void)
{
    ESP_ERROR_CHECK(esp_bluedroid_disable());
    ESP_ERROR_CHECK(esp_bluedroid_deinit());
    ESP_ERROR_CHECK(esp_bt_controller_disable());
}

uint8_t build_data_advert(uint8_t data[])
{
    uint8_t bytes = 0;

    // Flags
    data[0] = 2;
    bytes++;

    data[1] = ESP_BLE_AD_TYPE_FLAG;
    bytes++;

    data[2] = ESP_BLE_ADV_FLAG_BREDR_NOT_SPT | ESP_BLE_ADV_FLAG_GEN_DISC;
    bytes++;

    // Name
    data[3] = 5;
    bytes++;

    data[4] = ESP_BLE_AD_TYPE_NAME_CMPL;
    bytes++;

    data[5] = static_cast<uint8_t>('o');
    bytes++;
    data[6] = static_cast<uint8_t>('u');
    bytes++;
    data[7] = static_cast<uint8_t>('t');
    bytes++;
    data[8] = static_cast<uint8_t>('s');
    bytes++;

    data[9] = 19;
    bytes++;

    data[10] = ESP_BLE_AD_TYPE_SERVICE_DATA;
    bytes++;

    data[11] = 0x1c;
    bytes++;
    data[12] = 0x18;
    bytes++;

    bytes += bthome::encode::packet_id(blackboard.system.bootCount, &data[bytes], 64);

    bytes += bthome::encode::pressure(blackboard.sensors.pressure, &data[bytes], 64 - bytes);

    bytes += bthome::encode::temperature(blackboard.sensors.temperature, &data[bytes], 64 - bytes);

    bytes += bthome::encode::humidity(blackboard.sensors.humidity, &data[bytes], 64 - bytes);

    return bytes;
}

void task_ble_entry(void* params)
{
    ble_init();

    vTaskDelay(200 / portTICK_PERIOD_MS);

    ESP_LOGI("ble_task", "Entering task loop");

    for (;;)
    {
        // Wait for sensor task notify
        // Clear all bits on exit to reset
        if (pdTRUE == xTaskNotifyWait(0, 0xFFFFFFFF, NULL, 1000 / portTICK_PERIOD_MS))
        {

            ESP_LOGI("ble_task", "Sensor data ready");

            ESP_LOGI("ble_task", "Packet #%d", blackboard.system.bootCount);

            // Encode sensor data
            uint8_t const dataLength = build_data_advert(&advertData[0]);

            ESP_LOGI("ble_task", "Advert size: %i bytes", dataLength);

            // Configure advertising data
            ESP_ERROR_CHECK(esp_ble_gap_config_adv_data_raw(&advertData[0], dataLength));

            // Begin advertising
            ESP_ERROR_CHECK(esp_ble_gap_start_advertising(&ble_adv_params));

            // Wait 500ms for a few advertisement to go out
            vTaskDelay(500 / portTICK_PERIOD_MS);

            // Stop advertising data
            ESP_ERROR_CHECK(esp_ble_gap_stop_advertising());

            // De-init all BLE related things
            ble_deinit();

            // Enter deep sleep
            ESP_LOGI("ble_task", "Goodbye!");
            esp_deep_sleep(SLEEP_5_MINUTES);
        }
        else
        {
            // Handle the timeout somehow
        }
    }
}