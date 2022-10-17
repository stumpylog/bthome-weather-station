#ifdef __cplusplus
extern "C" {
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

#include <cstring>
#include <stdint.h>

static constexpr uint32_t US_TO_S_FACTOR {1000000};
static constexpr uint32_t SECONDS_PER_MINUTE {60};
static constexpr uint64_t SLEEP_1_MINUTE {SECONDS_PER_MINUTE * US_TO_S_FACTOR};
static constexpr uint64_t SLEEP_5_MINUTES {SLEEP_1_MINUTE * 5};

static esp_ble_adv_params_t ble_adv_params = {
    // 0.625ms * 0x40 ~ 40ms
    .adv_int_min = 0x40,
    // 0.625ms * 0x140 ~ 200ms
    .adv_int_max       = 0x140,
    .adv_type          = ADV_TYPE_NONCONN_IND,
    .own_addr_type     = BLE_ADDR_TYPE_PUBLIC,
    .peer_addr_type    = BLE_ADDR_TYPE_PUBLIC,
    .channel_map       = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

static uint8_t advertData[64];

void ble_init(void)
{
    ESP_LOGI(BLE_TASK_NAME, "Starting BLE init");
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));

    ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_BLE));

    ESP_ERROR_CHECK(esp_bluedroid_init());
    ESP_ERROR_CHECK(esp_bluedroid_enable());

    ESP_LOGI(BLE_TASK_NAME, "BLE init completed");
}

void ble_deinit(void)
{
    ESP_ERROR_CHECK(esp_bluedroid_disable());
    ESP_ERROR_CHECK(esp_bluedroid_deinit());
    ESP_ERROR_CHECK(esp_bt_controller_disable());
    ESP_ERROR_CHECK(esp_bt_controller_deinit());
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

    // Service data + UUID length
    data[9] = 3;
    bytes++;

    data[10] = ESP_BLE_AD_TYPE_SERVICE_DATA;
    bytes++;

    data[11] = 0x1c;
    bytes++;
    data[12] = 0x18;
    bytes++;

    bthome::encode::BTHomePacketIdIdSensor packet(static_cast<uint64_t>(blackboard.system.bootCount));
    bthome::encode::BTHomeTemperatureSensor temp(blackboard.sensors.temperature);
    bthome::encode::BTHomeHumiditySensor humid(blackboard.sensors.humidity);
    bthome::encode::BTHomePressureSensor press(blackboard.sensors.pressure);

    memcpy(&data[bytes], packet.getPayload(), packet.getPayloadSize());

    bytes += packet.getPayloadSize();

    memcpy(&data[bytes], temp.getPayload(), temp.getPayloadSize());

    bytes += temp.getPayloadSize();

    memcpy(&data[bytes], humid.getPayload(), humid.getPayloadSize());

    bytes += humid.getPayloadSize();

    memcpy(&data[bytes], press.getPayload(), press.getPayloadSize());

    bytes += press.getPayloadSize();

    // Update length
    data[9] += packet.getPayloadSize() + temp.getPayloadSize() + humid.getPayloadSize() + press.getPayloadSize();

    return bytes;
}

void task_ble_entry(void* params)
{
    ble_init();

    vTaskDelay(200 / portTICK_PERIOD_MS);

    ESP_LOGI(BLE_TASK_NAME, "Entering task loop");

    for (;;)
    {
        // Wait for sensor task notify
        // Clear all bits on exit to reset
        if (pdTRUE == xTaskNotifyWait(0, 0xFFFFFFFF, NULL, 1000 / portTICK_PERIOD_MS))
        {

            ESP_LOGI(BLE_TASK_NAME, "Sensor data ready");

            ESP_LOGI(BLE_TASK_NAME, "Packet #%d", blackboard.system.bootCount);

            // Encode sensor data
            uint8_t const dataLength = build_data_advert(&advertData[0]);

            ESP_LOGI(BLE_TASK_NAME, "Advert size: %i bytes", dataLength);

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
            ESP_LOGI(BLE_TASK_NAME, "Goodbye!");
            esp_deep_sleep(SLEEP_5_MINUTES);
        }
        else
        {
            // Handle the timeout somehow
        }
    }
}

#ifdef __cplusplus
}
#endif
