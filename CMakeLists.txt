cmake_minimum_required(VERSION 3.27)

project(slingbound)

set(CMAKE_CXX_STANDARD 23)
set(SOURCE_DIR src)
set(VENDOR_DIR vendor)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(EMSCRIPTEN)
    add_compile_definitions(PLATFORM_WEB)
    set(BUILD_PLATFORM "PLATFORM=Web")
else()
    add_compile_definitions(PLATFORM_DESKTOP)
    set(BUILD_PLATFORM "PLATFORM=Desktop")
endif()

add_subdirectory("vendor/raylib")
add_subdirectory("vendor/entt")

file(GLOB_RECURSE SOURCE_FILES 
    "${SOURCE_DIR}/**.cpp" 
    "${SOURCE_DIR}/**.h"
)

set(INCLUDE_DIRECTORIES
    "${SOURCE_DIR}"
    "${VENDOR_DIR}/raylib/src"
    "${VENDOR_DIR}/entt/src"
)

add_executable(slingbound "${SOURCE_FILES}")

target_include_directories(slingbound PUBLIC ${INCLUDE_DIRECTORIES})
target_link_libraries(slingbound PUBLIC raylib EnTT::EnTT)

if(EMSCRIPTEN)
    set_target_properties(slingbound PROPERTIES 
        LINK_FLAGS "-s USE_GLFW=3 -s INITIAL_MEMORY=16777216 -s ASYNCIFY=1 -s ASYNCIFY_IMPORTS=['emscripten_sleep'] --shell-file ${CMAKE_SOURCE_DIR}/index.html --preload-file ${CMAKE_SOURCE_DIR}/resources@resources"
    )
    file(COPY ${CMAKE_SOURCE_DIR}/index.html DESTINATION ${CMAKE_BINARY_DIR})
    file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_BINARY_DIR})
endif()