cmake_minimum_required (VERSION 3.7)

if(WIN32)
  cmake_policy(SET CMP0091 NEW)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

set(EXECUTABLE_NAME mp4_boxes)

project(${EXECUTABLE_NAME})


set(SOURCE_FILES
    source/main.cpp
    source/mp4box.cpp
    source/xmlreader.cpp
   )

set(CMAKE_CXX_STANDARD 20)

add_executable(${EXECUTABLE_NAME} ${SOURCE_FILES})


if(WIN32)
  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${EXECUTABLE_NAME}) # VS users will be happy with this
  target_compile_options(${EXECUTABLE_NAME} PRIVATE $<$<CONFIG:RELEASE>:/O2> $<$<CONFIG:RELEASE>:/Ob2> $<$<CONFIG:RELEASE>:/Ot> $<$<CONFIG:RELEASE>:/GL>)

  target_compile_options(${EXECUTABLE_NAME} PRIVATE /W4)  # warning level
  target_compile_options(${EXECUTABLE_NAME} PRIVATE /GR-) # disable RTTI
  target_compile_options(${EXECUTABLE_NAME} PRIVATE /permissive-)
  
  add_compile_definitions(_WIN32)
  add_compile_definitions(_WIN64)
  add_compile_definitions(_AMD64_)
  add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    
  target_link_options(${EXECUTABLE_NAME} PRIVATE $<$<CONFIG:RELEASE>:/LTCG>)

elseif(UNIX AND NOT APPLE) # Don't have Mac, so can't tell if it compiles out of the box :-(
  ## 
  ## Linux
  ##
  target_compile_options(${EXECUTABLE_NAME} PRIVATE -Wall)     # always enable all warnings
  target_compile_options(${EXECUTABLE_NAME} PRIVATE -fno-rtti) # always disable RTTI

  #
  # experimenting...
  #
  # target_compile_options(${EXECUTABLE_NAME} PRIVATE $<$<CONFIG:RELEASE>:-O2> $<$<CONFIG:RELEASE>:-falign-functions=16> $<$<CONFIG:RELEASE>:-falign-loops=16> $<$<CONFIG:RELEASE>:-flto>)
  #

endif()
