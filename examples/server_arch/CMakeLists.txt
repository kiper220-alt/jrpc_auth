cmake_minimum_required(VERSION 3.30)
project(jrpc_auth)

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
pkg_check_modules(PAM REQUIRED pam)

add_executable(jrpc_auth
        main.cpp
        src/AuthService.cpp
        src/AuthStorage/MemoryAuthStorage.cpp
        src/UserStorage/CustomUserStorage.cpp
)
target_include_directories(jrpc_auth PUBLIC
        ${QJSONRPC_INCLUDE_DIR}
        ${PAM_INCLUDE_DIRS}
        src
)
target_link_libraries(jrpc_auth
        Qt::Core
        Qt::Network
        ${QJSONRPC_LIBRARIES}
        ${PAM_LIBRARIES}
)
