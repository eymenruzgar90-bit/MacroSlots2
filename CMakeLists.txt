cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if ("${CMAKE_SYSTEM_NAME}" STREQUAL "iOS" OR IOS)
    set(CMAKE_OSX_ARCHITECTURES "arm64")
else()
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
endif()
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(MacroSlots VERSION 1.0.0)

# src klasorundaki tum .cpp dosyalarini derle
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Geode SDK bulunamadi! GEODE_SDK ortam degiskenini Geode klasorune isaretlemelisin.")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

# Modu kur, bagimliliklari ayarla ve Geode'a bagla
setup_geode_mod(${PROJECT_NAME})

# "Klasoru Ac" butonu Windows'ta Explorer'i acmak icin shell32 ister
if (WIN32)
    target_link_libraries(${PROJECT_NAME} shell32)
endif()
