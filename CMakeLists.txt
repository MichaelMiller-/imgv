cmake_minimum_required(VERSION 3.1)
project(imgv CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_CXX_STANDARD 17)

set("${PROJECT_NAME}_VERSION_MAJOR" 0)
set("${PROJECT_NAME}_VERSION_MINOR" 1)

configure_file("${PROJECT_SOURCE_DIR}/src/config.h.in" "${PROJECT_BINARY_DIR}/config.h")

## see: https://doc.qt.io/qt-5/cmake-manual.html
##
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)
# Find the QtWidgets library
find_package(Qt5Widgets CONFIG REQUIRED)

# Populate a CMake variable with the sources
set(SOURCES
   src/main.cpp
   src/main_widget.h
   src/main_widget.cpp
)

## TODO: Boost_USE_STATIC_LIBS
find_package(Boost REQUIRED COMPONENTS program_options)
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)

add_executable(${PROJECT_NAME} ${SOURCES})

if (CMAKE_COMPILER_IS_GNUCXX)
   target_link_libraries(${PROJECT_NAME} "-lstdc++fs")
endif()

target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
target_link_libraries(${PROJECT_NAME} ${Boost_PROGRAM_OPTIONS_LIBRARY})

# include(CTest)
# enable_testing()
# add_subdirectory(tests)

## local Windows
# cmake .. "-DCMAKE_TOOLCHAIN_FILE=C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake"
