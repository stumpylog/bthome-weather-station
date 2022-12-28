
#include "esp_log.h"
// TODO This header isn't found?
// #include "mbedtls/ccm.h"
#include "advertisement.h"

#include <string.h>

#define _ADVERT_LOG_NAME "advert"

namespace bthome
{

    Advertisement::Advertisement(void)
        : m_dataIdx(0), m_serviceDataSizeIdx(0), m_dataStartIdx(0), m_serviceUuid(constants::SERVICE_UUID)
    {
        // Write the common header information
        this->writeHeader();

        // Write the service UUID and setup the size index for the data
        this->writeUuid();

        // Write BTHome device into
        this->writeDeviceInfo();

        // Actual data goes here
        this->m_dataStartIdx = this->m_dataIdx;
    }

    Advertisement::Advertisement(std::string const& name)
        : m_dataIdx(0), m_serviceDataSizeIdx(0), m_dataStartIdx(0), m_serviceUuid(constants::SERVICE_UUID)
    {
        // Write the BLE flags
        this->writeHeader();

        // Track where the length will go
        uint8_t startIdx = this->m_dataIdx;

        // Write the length as 0 for now
        this->writeByte(0);

        // This is a complete name
        this->writeByte(constants::BLE_ADVERT_DATA_TYPE::COMPLETE_NAME);

        // Write the actual bytes now
        for (uint8_t idx = 0; idx < name.size(); idx++)
        {
            this->writeByte(name[idx]);
        }

        // Write the length now
        this->m_data[startIdx] = this->m_dataIdx - startIdx - 1;

        // Follow up the name with the service data UUID (but no data yet)
        this->writeUuid();

        // Write BTHome device into
        this->writeDeviceInfo();

        // Actual data goes here
        this->m_dataStartIdx = this->m_dataIdx;
    }

    Advertisement::~Advertisement(void) { }

    void Advertisement::reset(void)
    {
        this->m_dataIdx = 0;
    }

    void Advertisement::writeHeader(void)
    {
        // Length for this is fixed
        this->writeByte(2);

        this->writeByte(constants::BLE_ADVERT_DATA_TYPE::TYPE);

        this->writeByte(constants::BLE_FLAGS_DATA_TYPE::BREDR_NOT_SUPPORTED |
                        constants::BLE_FLAGS_DATA_TYPE::GENERAL_DISCOVERY);
    }

    void Advertisement::writeUuid(void)
    {
        this->m_serviceDataSizeIdx = this->m_dataIdx;
        // Service data + UUID length is fixed
        this->writeByte(3);

        this->writeByte(constants::BLE_ADVERT_DATA_TYPE::SERVICE_DATA);

        this->writeByte(this->m_serviceUuid & 0xff);
        this->writeByte((this->m_serviceUuid >> 8) & 0xff);
    }

    void Advertisement::writeDeviceInfo(void)
    {
        this->writeByte((0 << constants::BTHOME_DEVICE_INFO_SHIFTS::ENCRYPTED) |
                        (constants::BTHOME_V2 << constants::BTHOME_DEVICE_INFO_SHIFTS::VERSION));

        this->m_data[this->m_serviceDataSizeIdx] += 1;
    }

    inline void Advertisement::writeByte(uint8_t const data)
    {
        if (this->m_dataIdx <= constants::BLE_ADVERT_MAX_LEN)
        {
            this->m_data[this->m_dataIdx] = data;
            this->m_dataIdx++;
        }
        else
        {
            ESP_LOGE(_ADVERT_LOG_NAME, "Discarding data");
        }
    }

    bool Advertisement::addMeasurement(Measurement const& measurement)
    {

        if ((this->m_dataIdx + measurement.getPayloadSize()) > constants::BLE_ADVERT_MAX_LEN)
        {
            ESP_LOGE(_ADVERT_LOG_NAME, "Unable to add sensor");
            return false;
        }

        memcpy(&this->m_data[this->m_dataIdx], measurement.getPayload(), measurement.getPayloadSize());

        // Increment where the next measurement will go
        this->m_dataIdx += measurement.getPayloadSize();

        // Also increase the length in the header
        this->m_data[this->m_serviceDataSizeIdx] += measurement.getPayloadSize();

        return true;
    }

    const uint8_t* Advertisement::getPayload(void) const
    {
        return &this->m_data[0];
    }

    uint32_t Advertisement::getPayloadSize(void) const
    {
        return this->m_dataIdx;
    }

    AdvertisementWithId::AdvertisementWithId(uint8_t const packetId) : Advertisement()
    {
        Measurement packetIdData(constants::ObjectId::PACKET_ID, static_cast<uint64_t>(packetId));
        this->addMeasurement(packetIdData);
    }

    AdvertisementWithId::AdvertisementWithId(std::string const& name, uint8_t const packetId) : Advertisement(name)
    {
        Measurement packetIdData(constants::ObjectId::PACKET_ID, static_cast<uint64_t>(packetId));
        this->addMeasurement(packetIdData);
    }

    /*EncryptedAdvertisement::EncryptedAdvertisement(uint32_t const countId, uint8_t const bindKey[])
        : Advertisement(), m_serviceUuid(constants::ENCRYPTED_SERVICE_UUID)
    {
        this->buildNonce(countId);
        memcpy(&this->bindKey[0], bindKey, constants::BIND_KEY_LEN);
    }

    EncryptedAdvertisement::EncryptedAdvertisement(std::string const& name, uint32_t const countId,
                                                   uint8_t const bindKey[])
        : Advertisement(name), m_serviceUuid(constants::ENCRYPTED_SERVICE_UUID)
    {
        this->buildNonce(countId);
        memcpy(&this->bindKey[0], bindKey, constants::BIND_KEY_LEN);
    }

    void EncryptedAdvertisement::buildNonce(uint32_t const countId)
    {
        uint8_t macAddr[6];
        uint8_t* const uuidPtr  = static_cast<uint8_t*>(&this->m_serviceUuid);
        uint8_t* const countPtr = static_cast<uint8_t*>(&countId);
        ESP_ERROR_CHECK(esp_read_mac(&macAddr[0], ESP_MAC_BT));

        memcpy(&this->nonce[0], &macAddr[0], 6);
        memcpy(&this->nonce[6], uuidPtr, 2);
        memcpy(&this->nonce[8], countPtr, 4);

        // Get MAC address + UUID + count as bytes and store it
    }

    const uint8_t* EncryptedAdvertisement::getPayload(void) const
    {
        uint8_t const randomThing = {0x11};
        uint8_t encryptedData[constants::BLE_ADVERT_MAX_LEN];
        uint8_t encryptionTag[constants::BLE_ADVERT_MAX_LEN];
        uint32_t outputSize = 0;

        mbedtls_ccm_context context;
        mbedtls_ccm_init(&context);

        mbedtls_ccm_setkey(&context, MBEDTLS_CIPHER_ID_AES, &this->bindKey[0], constants::BIND_KEY_LEN * 8);

        mbedtls_ccm_starts(&context, MBEDTLS_CCM_ENCRYPT,
                           // The initialization vector or nonce
                           &this->nonce[0],
                           // IV length
                           12);

        mbedtls_ccm_set_lengths(&context,
                                // No additional data
                                0,
                                // The size of the input (only service data)
                                this->m_data[this->m_serviceDataSizeIdx],
                                // Tag or mac length
                                4);

        mbedtls_ccm_update(&context,
                           // No explanation of why the data is encrypted with an extra 0x11
                           randomThing, 1,
                           // Output
                           &encryptedData[0],
                           // Output max length
                           constants::BLE_ADVERT_MAX_LEN,
                           // The actual written size
                           outputSize);

        mbedtls_ccm_update(&context,
                           // Service data
                           &this->m_data[this->m_serviceDataSizeIdx],
                           // Service data length
                           this->m_data[this->m_serviceDataSizeIdx],
                           // Output
                           &encryptedData[outputSize],
                           // Output max length
                           constants::BLE_ADVERT_MAX_LEN - 1,
                           // The actual written size
                           outputSize);

        //
        mbedtls_ccm_finish(&context, &encryptionTag[0], 4);

        // TODO: Obviously need some combination of encryptedData and encryptionTag at least now
        // to be copied into the data instead
        return &this->m_data[0];
    }*/

}; // namespace bthome
