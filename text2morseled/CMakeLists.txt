cmake_minimum_required(VERSION 4.0)

project(text2morseled)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core DBus)
qt_standard_project_setup()

set(SOURCES
    main.cpp
)
qt_add_dbus_interface(SOURCES ../led-daemon/su.powar.LedDaemon.xml dbusled)

add_executable(${PROJECT_NAME}
    ${SOURCES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core Qt6::DBus
)