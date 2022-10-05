#!/usr/bin/env bash

find main/ -type f -name "*.cpp" -exec clang-format -i -style=file "{}" \;
find main/ -type f -name "*.h" -exec clang-format -i -style=file "{}" \;
