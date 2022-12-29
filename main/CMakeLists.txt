idf_component_register(SRCS "main.cpp"
                            "blackboard.cpp"
                            "task_read_sensors.cpp"
                            "task_ble.cpp"
                            INCLUDE_DIRS ".")

target_compile_options(${COMPONENT_LIB} PRIVATE -Wall -Wextra)
