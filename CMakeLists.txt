cmake_minimum_required(VERSION 3.16)
project(uefiSetTool VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

set(HEADERS
    include/bootentry.h
    include/ibootmanager.h
    include/mainwindow.h
)

set(SOURCES
    main.cpp
    src/mainwindow.cpp
)

if(WIN32)
    list(APPEND HEADERS include/winbootmanager.h)
    list(APPEND SOURCES src/winbootmanager.cpp)
elseif(UNIX AND NOT APPLE)
    list(APPEND HEADERS include/linuxbootmanager.h)
    list(APPEND SOURCES src/linuxbootmanager.cpp)
endif()

if(WIN32)
    add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${HEADERS})
else()
    add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE advapi32 shell32)
endif()
