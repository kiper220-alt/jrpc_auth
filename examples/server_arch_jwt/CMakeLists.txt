cmake_minimum_required(VERSION 3.14)
project(ServerArchJwt)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS
        Core
        Network
        REQUIRED)
find_package(QJSonRPC REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(cpp-jwt REQUIRED)


set(CMAKE_CXX_STANDARD 17)

add_executable(${CMAKE_PROJECT_NAME} 
    main.cpp
    src/auth_service.cpp
    src/auth_storage/mem_auth_storage.cpp
    src/user_storage/psql_user_storage.cpp
)
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
        ${QJSONRPC_INCLUDE_DIR}
        src
)
target_link_libraries(${CMAKE_PROJECT_NAME}
        Qt::Core
        Qt::Network
        cpp-jwt::cpp-jwt
        ${QJSONRPC_LIBRARIES}
)
