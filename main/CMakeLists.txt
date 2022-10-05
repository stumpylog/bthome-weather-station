idf_component_register(SRCS "main.cpp"
                            "blackboard.cpp"
                            "task_read_sensors.cpp"
                            "task_ble.cpp"
                            INCLUDE_DIRS ".")

add_subdirectory(bthome)
add_subdirectory(sensors)

target_link_libraries(${COMPONENT_LIB} PUBLIC bthome sensors)
