# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\CustomerApp_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\CustomerApp_autogen.dir\\ParseCache.txt"
  "CustomerApp_autogen"
  )
endif()
