cmake_minimum_required(VERSION 3.16.0)
project(reminecraftpe-android)

# Native Android Build
target_compile_definitions(reminecraftpe-core PUBLIC USE_NATIVE_ANDROID)

# Build
add_library(reminecraftpe SHARED
    android_native_app_glue.c
    AppPlatform_android.cpp
    main.cpp
    SoundSystemSL.cpp
)

# Core
target_link_libraries(reminecraftpe reminecraftpe-core)

# OpenGL
target_link_libraries(reminecraftpe-core PUBLIC EGL GLESv1_CM)

# stb_image
target_link_libraries(reminecraftpe stb_image)

# Extra Dependencies
target_link_libraries(reminecraftpe android OpenSLES)