idf_component_register(SRCS "sensor.cpp" "advertisement.cpp"
                       INCLUDE_DIRS ".")

target_compile_options(${COMPONENT_LIB} PRIVATE -Wall -Wextra)
