#ifndef CONFIG_H_
#define CONFIG_H_

namespace config
{

    constexpr bool ENABLE_ENCRYPT {true};

    constexpr uint8_t BIND_KEY[] = {0x23, 0x1d, 0x39, 0xc1, 0xd7, 0xcc, 0x1a, 0xb1,
                                    0xae, 0xe2, 0x24, 0xcd, 0x09, 0x6d, 0xb9, 0x32};

    std::string NAME {"out"};

}; // namespace config
#endif
