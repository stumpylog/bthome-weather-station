idf_component_register(SRCS "i2cBus.cpp"
                       INCLUDE_DIRS ".")

target_compile_options(${COMPONENT_LIB} PRIVATE -Wall -Wextra)
