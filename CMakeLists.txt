cmake_minimum_required(VERSION 3.16)
project(AntSimulator2 LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(BUILD_SHARED_LIBS OFF)
set(SFML_USE_SYSTEM_DEPS OFF)

include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)

FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.x)
FetchContent_MakeAvailable(SFML)

include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)

# Automatically adds all cpp files contained in the src directory
file(GLOB_RECURSE source_files src/*.cpp)

set(SOURCES ${source_files})

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE "src" "include")
target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

# Copy res dir to the binary directory
add_custom_command(
        TARGET ${PROJECT_NAME}
        COMMENT "Copy Res directory"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:${PROJECT_NAME}>/res
        VERBATIM)
