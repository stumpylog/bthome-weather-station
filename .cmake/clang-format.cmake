# get all project files
file(GLOB_RECURSE ALL_SOURCE_FILES *.cpp *.hpp *.h *.c)

# find the clang-format tool
find_program(CLANG_EXE clang-format REQUIRED)

# Add format target
add_custom_target(
        format
        COMMAND ${CLANG_EXE}
        -style=file
        -i
        ${ALL_SOURCE_FILES}
)
