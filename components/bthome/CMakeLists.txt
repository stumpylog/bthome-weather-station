idf_component_register(SRCS "advertisement_element.cpp" "constants.cpp" "measurement.cpp" "advertisement.cpp"
                       PRIV_REQUIRES mbedtls
                       INCLUDE_DIRS ".")

target_compile_options(${COMPONENT_LIB} PRIVATE -Wall -Wextra)
