cmake_minimum_required(VERSION 3.16.0)
project(reminecraftpe-core)

set(CMAKE_CXX_STANDARD 17)
# Build
file(GLOB_RECURSE SRC_FILES "*.cpp")
add_library(reminecraftpe-core STATIC
	${SRC_FILES}
)
message(STATUS "Selected Directories: ${SRC_FILES}")
target_include_directories(reminecraftpe-core PUBLIC . ..)

# RakNet
add_subdirectory(../thirdparty/raknet raknet)
target_link_libraries(reminecraftpe-core PUBLIC raknet)

# zlib
add_library(zlib-interface INTERFACE)
if(MCPE_WIN32)
    # Compile Vendored ZLib
    add_subdirectory(../thirdparty/zlib zlib)
    target_link_libraries(zlib-interface INTERFACE zlib)
elseif(EMSCRIPTEN)
    # Use Emscripten's ZLib
    set(ZLIB_FLAG -sUSE_ZLIB=1)
    target_compile_options(zlib-interface INTERFACE "${ZLIB_FLAG}")
    target_link_options(zlib-interface INTERFACE "${ZLIB_FLAG}")
else()
    # Use System ZLib
    find_package(ZLIB REQUIRED)
    target_link_libraries(zlib-interface INTERFACE ZLIB::ZLIB)
endif()
target_link_libraries(reminecraftpe-core PUBLIC zlib-interface)

if(MCPE_WIN32)
    target_sources(reminecraftpe-core PRIVATE ../thirdparty/GL/GLExt.cpp)
endif()

# Android Logging
if(ANDROID)
    target_link_libraries(reminecraftpe-core PUBLIC log)
endif()
