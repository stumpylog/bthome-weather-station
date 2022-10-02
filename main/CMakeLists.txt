idf_component_register(SRCS "BME280.cpp"
                            "main.cpp"
                            "blackboard.cpp"
                            "task_read_sensors.cpp"
                            "task_ble.cpp"
                            INCLUDE_DIRS ".")

add_subdirectory(bthome)

target_link_libraries(${COMPONENT_LIB} PUBLIC bthome)
