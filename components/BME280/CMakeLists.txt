
idf_component_register(SRCS "BME280.cpp"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES i2c)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wall -Wextra)
