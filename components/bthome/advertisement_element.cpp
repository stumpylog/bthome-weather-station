#include "advertisement.h"

#include <cstring>

namespace bthome
{

    AdvertisementElement::AdvertisementElement(uint8_t const type)
    {
        // Length begins at 2 for the length and the element type
        this->data[0] = 2;
        // Type is always second
        this->data[1] = type;
    }
    AdvertisementElement::~AdvertisementElement() { }

    inline void AdvertisementElement::writeByte(uint8_t const data)
    {
        if (this->data[0] < constants::BLE_ADVERT_MAX_LEN)
        {
            this->data[this->data[0]] = data;
            this->data[0]++;
        }
    }

    inline void AdvertisementElement::writeBytes(uint8_t const data[], uint8_t const length)
    {
        if (this->data[0] + length < constants::BLE_ADVERT_MAX_LEN)
        {
            std::memcpy(&this->data[this->data[0]], data, length);
            this->data[0] += length;
        }
    }

    FlagsElement::FlagsElement(void) : AdvertisementElement(constants::BLE_ADVERT_DATA_TYPE::TYPE)
    {
        this->writeByte(constants::BLE_FLAGS_DATA_TYPE::BREDR_NOT_SUPPORTED |
                        constants::BLE_FLAGS_DATA_TYPE::GENERAL_DISCOVERY);
    }
    FlagsElement::~FlagsElement() { }

    LocalNameElement::LocalNameElement(std::string const& name)
        : AdvertisementElement(constants::BLE_ADVERT_DATA_TYPE::COMPLETE_NAME)
    {
        // Write the name bytes
        for (uint8_t idx = 0; idx < name.size(); idx++)
        {
            this->writeByte(name[idx]);
        }
    }

    ServiceDataElement::ServiceDataElement(uint16_t const uuid)
        : AdvertisementElement(constants::BLE_ADVERT_DATA_TYPE::SERVICE_DATA)
    {
        this->writeByte(constants::SERVICE_UUID_BYTES[0]);
        this->writeByte(constants::SERVICE_UUID_BYTES[1]);
    }

}; // namespace bthome
