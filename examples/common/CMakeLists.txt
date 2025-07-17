cmake_minimum_required(VERSION 3.14)
project(common)

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
find_package(cpp-jwt REQUIRED)

add_library(common STATIC
        src/json_configuration.cpp
        src/mem_auth_storage.cpp
        src/qsql_user_storage.cpp

        inc/auth_configuration/iauth_config.h
        inc/auth_configuration/iuser_config.h
        inc/auth_configuration/iservice_config.h
        inc/auth_configuration/json_configuration.h

        inc/auth_storage/iauth_storage.h
        inc/auth_storage/mem_auth_storage.h

        inc/user_storage/iuser_storage.h
        inc/user_storage/qsql_user_storage.h
)

target_include_directories(common PUBLIC
        inc
)
target_link_libraries(common
        Qt::Core
        Qt::Network
        Qt::Sql
        cpp-jwt::cpp-jwt
)