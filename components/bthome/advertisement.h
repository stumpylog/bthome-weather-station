
#ifndef _BTHOME_ADVERTISEMENT_H_
#define _BTHOME_ADVERTISEMENT_H_

#include "constants.h"
#include "mbedtls/ccm.h"
#include "measurement.h"

#include <cstdint>
#include <string>

namespace bthome
{

    class AdvertisementElement
    {
      public:
        AdvertisementElement(uint8_t const type);
        ~AdvertisementElement();
        void writeByte(uint8_t const data);
        void writeBytes(uint8_t const data[], uint8_t const length);
        uint8_t size(void);
        uint8_t data[constants::BLE_ADVERT_MAX_LEN];
    };

    class FlagsElement : public AdvertisementElement
    {
      public:
        FlagsElement(void);
        ~FlagsElement();
    };

    class LocalNameElement : public AdvertisementElement
    {
      public:
        LocalNameElement(std::string const& name);
    };

    class ServiceDataElement : public AdvertisementElement
    {
      public:
        ServiceDataElement(uint16_t const uuid);
    };

    class Advertisement
    {
      public:
        Advertisement(void);
        Advertisement(std::string const& name);
        ~Advertisement();

        bool addMeasurement(Measurement const& measurement);

        const uint8_t* getPayload(void) const;
        uint32_t getPayloadSize(void) const;
        void finalize(void);

      protected:
        void writeFlagsElement(void);
        void writeLocalNameElement(std::string const& name);
        void writeServiceUUID(void);
        void writeBthomeInfoByte(void);
        uint8_t m_nextIdx;
        uint8_t m_serviceDataLengthIdx;
        uint8_t m_serviceDataStartIdx;
        uint8_t m_data[constants::BLE_ADVERT_MAX_LEN];
    };

    class AdvertisementWithId : public Advertisement
    {
      public:
        AdvertisementWithId(uint8_t const packetId);
        AdvertisementWithId(std::string const& name, uint8_t const packetId);
    };

    class EncryptedAdvertisement : public Advertisement
    {
      public:
        EncryptedAdvertisement(uint32_t const countId, uint8_t const bindKey[constants::BIND_KEY_LEN]);
        EncryptedAdvertisement(std::string const& name, uint32_t const countId,
                               uint8_t const bindKey[constants::BIND_KEY_LEN]);
        ~EncryptedAdvertisement();

        void finalize(void);

      protected:
        void writeBthomeInfoByte(void);

      private:
        void initEncryption(uint32_t const count);

        uint8_t nonce[constants::NONCE_LEN];
        uint8_t bindKey[constants::BIND_KEY_LEN];
        mbedtls_ccm_context context;
    };

}; // namespace bthome

#endif
