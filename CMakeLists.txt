cmake_minimum_required(VERSION 3.22)
project(imgv VERSION 0.0.2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

find_package(docopt CONFIG REQUIRED)
find_package(spdlog REQUIRED)
find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_image CONFIG REQUIRED)

add_executable(imgv)
target_sources(imgv PRIVATE
        src/animation.h
        src/any_of.h
        src/application.cpp
        src/application.h
        src/main.cpp
        src/settings.h
)

get_target_property(BINARY_NAME imgv NAME)
configure_file(src/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h @ONLY)
configure_file(settings.conf ${CMAKE_CURRENT_BINARY_DIR}/settings.conf COPYONLY)

target_include_directories(imgv
        PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
        PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src
)

target_link_libraries(imgv PRIVATE docopt)
target_link_libraries(imgv PRIVATE SDL3::SDL3)
target_link_libraries(imgv PRIVATE $<IF:$<TARGET_EXISTS:SDL3_image::SDL3_image-shared>,SDL3_image::SDL3_image-shared,SDL3_image::SDL3_image-static>)
target_link_libraries(imgv PRIVATE spdlog::spdlog spdlog::spdlog_header_only)
