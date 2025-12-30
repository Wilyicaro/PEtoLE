cmake_minimum_required(VERSION 3.16.0)
project(reminecraftpe-core)

set(CMAKE_CXX_STANDARD 17)
# Build
file(GLOB_RECURSE SRC_FILES "*.cpp")
add_library(reminecraftpe-core STATIC
	${SRC_FILES}
)
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

# Platform Dependencies
if(USE_SDL)
    # SDL
    add_library(SDL INTERFACE)
    if(ANDROID)
        # Use Vendored SDL2 (Only For Android)
        add_subdirectory(../thirdparty/SDL2/src SDL EXCLUDE_FROM_ALL)
        target_link_libraries(SDL INTERFACE SDL2::SDL2)
    elseif(EMSCRIPTEN)
        # Use Emscripten's SDL2
        set(SDL_FLAG -sUSE_SDL=2)
        target_compile_options(SDL INTERFACE "${SDL_FLAG}")
        target_link_options(SDL INTERFACE "${SDL_FLAG}")
    else()
        # Use System SDL2
        find_package(SDL2 REQUIRED)
        target_link_libraries(SDL INTERFACE SDL2::SDL2)
    endif()
    target_link_libraries(reminecraftpe-core PUBLIC SDL)

    # OpenGL
    if(NOT EMSCRIPTEN AND NOT ANDROID)
        option(USE_GLES1_COMPATIBILITY_LAYER "Whether To Enable The GLESv1_CM Compatibility Layer" TRUE)
    else()
        set(USE_GLES1_COMPATIBILITY_LAYER TRUE CACHE BOOL "" FORCE)
    endif()
    if(USE_GLES1_COMPATIBILITY_LAYER)
        set(GLES_COMPATIBILITY_LAYER_USE_SDL TRUE CACHE BOOL "" FORCE)
        set(GLES_COMPATIBILITY_LAYER_DEPENDENCY SDL CACHE STRING "" FORCE)
        set(GLES_COMPATIBILITY_LAYER_USE_ES3 FALSE CACHE BOOL "" FORCE)
        add_subdirectory(../thirdparty/gles-compatibility-layer gles-compatibility-layer)
        target_link_libraries(reminecraftpe-core PUBLIC gles-compatibility-layer)
        target_compile_definitions(reminecraftpe-core PUBLIC USE_GLES1_COMPATIBILITY_LAYER)
        if(EMSCRIPTEN)
            # Use WebGL 2
            target_link_options(reminecraftpe-core PUBLIC -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2)
        endif()
    else()
        find_package(OpenGL REQUIRED)
        target_link_libraries(reminecraftpe-core PUBLIC OpenGL::GL)
    endif()
elseif(USE_NATIVE_ANDROID)
    # OpenGL
    target_link_libraries(reminecraftpe-core PUBLIC EGL GLESv1_CM)
endif()

# Android Logging
if(ANDROID)
    target_link_libraries(reminecraftpe-core PUBLIC log)
endif()

# Sound Data
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../sound_data/sounds.h")
    if(NOT DEFINED ENV{CI})
        message(WARNING "Missing sound data! Did you run tools/grabsounds.py?")
    endif()
    target_compile_definitions(reminecraftpe-core PRIVATE MISSING_SOUND_DATA)
endif()
