cmake_minimum_required(VERSION 3.22)
project(imgv VERSION 0.0.2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

find_package(docopt CONFIG REQUIRED)
find_package(spdlog REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()
qt_add_executable(imgv)
target_sources(imgv PRIVATE
        src/any_of.h
        src/main.cpp
        src/main_widget.h
        src/main_widget.cpp
)

get_target_property(BINARY_NAME imgv NAME)
configure_file(src/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h @ONLY)

target_include_directories(imgv
        PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
        PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src
)

target_link_libraries(imgv PRIVATE docopt)
target_link_libraries(imgv PRIVATE Qt6::Widgets)
target_link_libraries(imgv PRIVATE spdlog::spdlog spdlog::spdlog_header_only)

set_target_properties(imgv PROPERTIES
        # Prevent the creation of a console window on Windows.
        WIN32_EXECUTABLE ON
        # Create an application bundle on macOS.
        MACOSX_BUNDLE ON
)