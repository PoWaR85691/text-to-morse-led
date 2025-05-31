cmake_minimum_required(VERSION 4.0)

project(led-daemon)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core DBus)
qt_standard_project_setup()

include_directories(
    ${CMAKE_BINARY_DIR}/${PROJECT_NAME}
    ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}
)

set(SOURCES
    main.cpp
    led.h led.cpp
)
qt_add_dbus_adaptor(SOURCES su.powar.LedDaemon.xml led.h Led)

add_definitions(-DPROJECT_NAME=\"${PROJECT_NAME}\")

add_executable(${PROJECT_NAME}
    ${SOURCES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core Qt6::DBus
)

install(FILES su.powar.LedDaemon.conf
        DESTINATION /usr/share/dbus-1/system.d)