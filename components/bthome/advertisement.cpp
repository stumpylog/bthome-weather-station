
#include "advertisement.h"

#include "esp_log.h"
#include "esp_mac.h"
#include "mbedtls/ccm.h"

#include <cstring>
#include <string>

#define _ADVERT_LOG_NAME "advert"

namespace bthome
{

    Advertisement::Advertisement(void) : m_nextIdx(0), m_serviceDataLengthIdx(0), m_serviceDataStartIdx(0)
    {
        this->writeFlagsElement();
        this->writeServiceUUID();
        this->writeBthomeInfoByte();
    }

    Advertisement::Advertisement(std::string const& name)
    {
        this->writeFlagsElement();
        this->writeLocalNameElement(name);
        this->writeServiceUUID();
        this->writeBthomeInfoByte();
    }

    Advertisement::~Advertisement(void) { }

    void Advertisement::writeFlagsElement(void)
    {
        this->m_data[this->m_nextIdx] = 2;
        this->m_nextIdx++;
        this->m_data[this->m_nextIdx] = constants::BLE_ADVERT_DATA_TYPE::TYPE;
        this->m_nextIdx++;
        this->m_data[this->m_nextIdx] =
            (constants::BLE_FLAGS_DATA_TYPE::BREDR_NOT_SUPPORTED | constants::BLE_FLAGS_DATA_TYPE::GENERAL_DISCOVERY);
        this->m_nextIdx++;
    }

    void Advertisement::writeLocalNameElement(std::string const& name)
    {
        uint8_t const lenIdx          = this->m_nextIdx;
        this->m_data[this->m_nextIdx] = 2;
        this->m_nextIdx++;
        this->m_data[this->m_nextIdx] = constants::BLE_ADVERT_DATA_TYPE::COMPLETE_NAME;
        this->m_nextIdx++;
        // Write the name bytes
        for (uint8_t idx = 0; idx < name.size(); idx++)
        {
            this->m_data[this->m_nextIdx] = name[idx];
            this->m_nextIdx++;
        }
        this->m_data[lenIdx] += name.size();
        this->m_nextIdx++;
    }

    void Advertisement::writeServiceUUID(void)
    {
        this->m_data[this->m_nextIdx] = 4;
        this->m_serviceDataLengthIdx  = this->m_nextIdx;
        this->m_nextIdx++;

        this->m_data[this->m_nextIdx] = constants::BLE_ADVERT_DATA_TYPE::SERVICE_DATA;
        this->m_nextIdx++;

        this->m_data[this->m_nextIdx] = constants::SERVICE_UUID_BYTES[1];
        this->m_nextIdx++;

        this->m_data[this->m_nextIdx] = constants::SERVICE_UUID_BYTES[0];
        this->m_nextIdx++;
    }

    void Advertisement::writeBthomeInfoByte(void)
    {
        this->m_data[this->m_nextIdx] = (0 << constants::BTHOME_DEVICE_INFO_SHIFTS::ENCRYPTED) |
                                        (constants::BTHOME_V2 << constants::BTHOME_DEVICE_INFO_SHIFTS::VERSION);
        this->m_data[this->m_serviceDataLengthIdx] += 1;

        this->m_nextIdx++;
        this->m_serviceDataStartIdx = this->m_nextIdx;
    }

    bool Advertisement::addMeasurement(Measurement const& measurement)
    {
        if ((this->getPayloadSize() + measurement.getPayloadSize()) > constants::BLE_ADVERT_MAX_LEN)
        {
            ESP_LOGE(_ADVERT_LOG_NAME, "Unable to add sensor");
            return false;
        }

        std::memcpy(&this->m_data[this->m_nextIdx], measurement.getPayload(), measurement.getPayloadSize());
        this->m_data[this->m_serviceDataLengthIdx] += measurement.getPayloadSize();

        this->m_nextIdx += measurement.getPayloadSize();

        return true;
    }

    const uint8_t* Advertisement::getPayload(void) const
    {
        return &this->m_data[0];
    }

    uint32_t Advertisement::getPayloadSize(void) const
    {
        return this->m_nextIdx;
    }

    void Advertisement::finalize(void) { }

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

    EncryptedAdvertisement::EncryptedAdvertisement(uint32_t const countId,
                                                   uint8_t const bindKey[constants::BIND_KEY_LEN])
        : Advertisement()
    {
        std::memcpy(&this->bindKey[0], bindKey, constants::BIND_KEY_LEN);
        this->initEncryption(countId);
    }

    EncryptedAdvertisement::EncryptedAdvertisement(std::string const& name, uint32_t const countId,
                                                   uint8_t const bindKey[constants::BIND_KEY_LEN])
        : Advertisement(name)
    {
        std::memcpy(&this->bindKey[0], bindKey, constants::BIND_KEY_LEN);
        this->initEncryption(countId);
    }

    EncryptedAdvertisement::~EncryptedAdvertisement()
    {
        mbedtls_ccm_free(&this->context);
    }

    void EncryptedAdvertisement::writeBthomeInfoByte(void)
    {
        this->m_data[this->m_nextIdx] = (1 << constants::BTHOME_DEVICE_INFO_SHIFTS::ENCRYPTED) |
                                        (constants::BTHOME_V2 << constants::BTHOME_DEVICE_INFO_SHIFTS::VERSION);
        this->m_data[this->m_serviceDataLengthIdx] += 1;

        this->m_nextIdx++;
        this->m_serviceDataStartIdx = this->m_nextIdx;
    }

    void EncryptedAdvertisement::initEncryption(uint32_t const count)
    {

        // First, do some basic init of the encryption context
        mbedtls_ccm_init(&this->context);
        mbedtls_ccm_setkey(&this->context,
                           // Using AES
                           MBEDTLS_CIPHER_ID_AES, &this->bindKey[0],
                           // Key size is in bits
                           constants::BIND_KEY_LEN * 8);

        // Second, build the nonce (aka initialization vector?)

        // Read the Bluetooth MAC address
        uint8_t macAddr[6];
        ESP_ERROR_CHECK(esp_read_mac(&macAddr[0], ESP_MAC_BT));

        // Build the noncethis->buildNonce(countId);
        memcpy(&this->nonce[0], &macAddr[0], 6);
        this->nonce[6] = constants::SERVICE_UUID_BYTES[0];
        this->nonce[7] = constants::SERVICE_UUID_BYTES[1];
        this->nonce[8] = (1 << constants::BTHOME_DEVICE_INFO_SHIFTS::ENCRYPTED) |
                         (constants::BTHOME_V2 << constants::BTHOME_DEVICE_INFO_SHIFTS::VERSION);
        this->nonce[9]  = count & 0xff;
        this->nonce[10] = (count >> 8) & 0xff;
        this->nonce[11] = (count >> 16) & 0xff;
        this->nonce[12] = (count >> 24) & 0xff;
    }

    void EncryptedAdvertisement::finalize(void)
    {
        // -1 length
        // -2 UUID
        // -1 BTHome info

        uint8_t const cipherTextLen = this->m_data[this->m_serviceDataLengthIdx] - 4;
        uint8_t ciphertext[cipherTextLen];
        uint8_t mic[4];
        mbedtls_ccm_encrypt_and_tag(&this->context,
                                    // Length of the input in bytes
                                    cipherTextLen,
                                    // The nonce of IV
                                    &this->nonce[0], constants::NONCE_LEN,
                                    // No additional data
                                    nullptr, 0,
                                    // Input is only service data
                                    &this->m_data[this->m_serviceDataStartIdx],
                                    // Output
                                    &ciphertext[0],
                                    // 4 byte Message Integrity Check
                                    &mic[0], 4);
        // Replace service data with ciphertext
        std::memcpy(&this->m_data[this->m_serviceDataStartIdx], &ciphertext[0], cipherTextLen);
        // Append the count
        std::memcpy(&this->m_data[this->m_nextIdx], &this->nonce[9], sizeof(uint32_t));
        this->m_nextIdx += sizeof(uint32_t);
        this->m_data[this->m_serviceDataLengthIdx] += sizeof(uint32_t);
        // Append the MIC
        std::memcpy(&this->m_data[this->m_nextIdx], &mic[0], sizeof(uint32_t));
        this->m_nextIdx += sizeof(uint32_t);
        this->m_data[this->m_serviceDataLengthIdx] += sizeof(uint32_t);
    }

    /*const uint8_t* EncryptedAdvertisement::getPayload(void)
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
