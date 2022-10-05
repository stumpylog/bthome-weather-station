add_library(sensors STATIC
            "i2cBus.cpp"
            "BME280.cpp"
            )

target_include_directories(sensors PUBLIC ".")

target_compile_options(sensors PRIVATE -Wall -Wextra)
