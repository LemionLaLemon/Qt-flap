# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/qtflap_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/qtflap_autogen.dir/ParseCache.txt"
  "qtflap_autogen"
  )
endif()
