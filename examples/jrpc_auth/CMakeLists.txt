cmake_minimum_required(VERSION 3.14)
project(jrpc_auth)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS
        Core
        Network
        Sql
        REQUIRED)

find_package(QJSonRPC REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(cpp-jwt REQUIRED)

add_executable(${PROJECT_NAME}
        main.cpp
        src/auth_service.cpp

        inc/auth_service.h
)
target_include_directories(${PROJECT_NAME} PUBLIC
        ${QJSONRPC_INCLUDE_DIR}
        inc
)
target_link_libraries(${PROJECT_NAME}
        Qt::Core
        Qt::Network
        cpp-jwt::cpp-jwt
        common
        ${QJSONRPC_LIBRARIES}
)
