#!/usr/bin/env bash

for dir in main components; do

  find "${dir}" -type f -name "*.cpp" -exec clang-format -i -style=file "{}" \;
  find "${dir}" -type f -name "*.h" -exec clang-format -i -style=file "{}" \;

done
