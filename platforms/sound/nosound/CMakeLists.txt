cmake_minimum_required(VERSION 3.16.0)
project(reminecraftpe-nosound)

# Build
add_library(reminecraftpe-sound STATIC
	NoSoundSystem.cpp
)

# Core
target_link_libraries(reminecraftpe-sound PUBLIC reminecraftpe-core)

# Headers
target_include_directories(reminecraftpe-sound PUBLIC .)
