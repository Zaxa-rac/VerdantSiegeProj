cmake_minimum_required(VERSION 3.16)
project(VerdantSiege VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ── SFML 3 uses different component names and targets ─────────────────────────
find_package(SFML 3 COMPONENTS Graphics Window System Audio REQUIRED)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(VerdantSiege ${SOURCES})

target_include_directories(VerdantSiege PRIVATE include)

# ── SFML 3 target names are now namespaced ────────────────────────────────────
target_link_libraries(VerdantSiege
    SFML::Graphics
    SFML::Window
    SFML::System
    SFML::Audio
)

add_custom_command(TARGET VerdantSiege POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/config $<TARGET_FILE_DIR:VerdantSiege>/config
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:VerdantSiege>/assets
)
