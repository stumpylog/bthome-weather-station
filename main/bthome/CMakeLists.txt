add_library(bthome STATIC
            "bthome.cpp")

target_include_directories(bthome PUBLIC ".")

target_compile_options(bthome PRIVATE -Wall -Wextra)
