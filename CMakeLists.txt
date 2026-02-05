cmake_minimum_required(VERSION 3.10)
project(bs-blackjack)

file(GLOB_RECURSE BSB_SOURCES CONFIGURE_DEPENDS "src/**.c")
add_executable(${PROJECT_NAME} ${BSB_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE "src")