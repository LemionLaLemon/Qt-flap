cmake_minimum_required(VERSION 3.21)

project(QtFlap
    VERSION 0.1.0
    DESCRIPTION "Flappy Bird, Qt6"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Generate build/compile_commands.json so clangd knows the real Qt include paths.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# A game is debugged far more than it is shipped: default to symbols.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# Qt's moc/uic/rcc run automatically; a game with Q_OBJECT classes needs moc.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

add_executable(qtflap
    src/main.cpp
    src/bird.cpp
    src/resources.qrc
)

target_link_libraries(qtflap PRIVATE Qt6::Widgets)

target_compile_options(qtflap PRIVATE -Wall -Wextra -Wpedantic)

install(TARGETS qtflap RUNTIME DESTINATION bin)
