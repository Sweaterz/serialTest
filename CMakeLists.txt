cmake_minimum_required(VERSION 3.5)

project(serialTest LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(Qt5 COMPONENTS Widgets LinguistTools Core REQUIRED)

#set(TS_FILES serialTest_zh_CN.ts)

set(serial_SRCS
    src/serial.cc
    include/serial/serial.h
    include/serial/v8stdint.h
)
if(APPLE)
    # If OSX
    list(APPEND serial_SRCS src/impl/unix.cc)
    list(APPEND serial_SRCS src/impl/list_ports/list_ports_osx.cc)
elseif(UNIX)
    # If unix
    list(APPEND serial_SRCS src/impl/unix.cc)
    list(APPEND serial_SRCS src/impl/list_ports/list_ports_linux.cc)
else()
    # If windows
    list(APPEND serial_SRCS src/impl/win.cc)
    list(APPEND serial_SRCS src/impl/list_ports/list_ports_win.cc)
endif()

## Add serial library
add_library(${PROJECT_NAME} ${serial_SRCS})
if(APPLE)
    target_link_libraries(${PROJECT_NAME} ${FOUNDATION_LIBRARY} ${IOKIT_LIBRARY})
elseif(UNIX)
    target_link_libraries(${PROJECT_NAME} rt pthread)
else()
    target_link_libraries(${PROJECT_NAME} setupapi)
endif()

#Include headers
target_include_directories(${PROJECT_NAME} PUBLIC include)


#Install executable
install(TARGETS ${PROJECT_NAME}
    ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
    LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
    RUNTIME DESTINATION ${CATKIN_GLOBAL_BIN_DESTINATION}
)
# Install headers
install(FILES include/serial/serial.h include/serial/v8stdint.h
    DESTINATION ${CATKIN_GLOBAL_INCLUDE_DESTINATION}/serial)



if(ANDROID)
  add_library(serialTool SHARED
    src/main.cpp
    src/mainwindow.cpp
    include/mainwindow.h
    src/mainwindow.ui
    src/receivethread.cpp
    include/receivethread.h
    include/crc.h
    include/QComboBoxMoreSignal.h
    src/QComboBoxMoreSignal.cpp

    ${TS_FILES}
  )
else()
  add_executable(serialTool
    src/main.cpp
    src/mainwindow.cpp
    include/mainwindow.h
    include/mainwindow.ui
    src/receivethread.cpp
    include/receivethread.h
    include/crc.h
    include/QComboBoxMoreSignal.h
    src/QComboBoxMoreSignal.cpp
    ${TS_FILES}
  )
endif()
add_dependencies(serialTool ${PROJECT_NAME})

target_link_libraries(serialTool PRIVATE ${PROJECT_NAME} Qt5::Widgets Qt5::Core)
include_directories(include)

qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
