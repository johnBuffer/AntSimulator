cmake_minimum_required(VERSION 3.10)
set(PROJECT_NAME AntSimulator)
project(${PROJECT_NAME} VERSION 1.0.0 LANGUAGES CXX)
find_package(OpenGL)

file(GLOB source_files
	"src/*.cpp"
)

set(SOURCES ${source_files})

# Detect and add SFML
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
find_package(SFML 2 REQUIRED COMPONENTS network audio graphics window system)

if(WIN32)
   set(WIN32_GUI WIN32)
endif(WIN32)

add_executable(${PROJECT_NAME} ${WIN32_GUI} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE "include" "lib")
target_link_libraries(${PROJECT_NAME} sfml-system sfml-window sfml-graphics)
if (UNIX)
   target_link_libraries(${PROJECT_NAME} pthread)
endif (UNIX)

# copy res dir to the binary directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/res DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
