cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME AntSimulator)
project(${PROJECT_NAME} VERSION 1.0.0 LANGUAGES CXX)

set(SFML_DIR "" CACHE PATH "SFML lib path")
set(SFML_LIB_DIR "${SFML_DIR}/lib")
set(SFML_INC_DIR "${SFML_DIR}/include")

find_package(OpenGL)

set(SFML_LIBS "${SFML_LIB_DIR}/sfml-graphics-s.lib"
    "${SFML_LIB_DIR}/sfml-window-s.lib"
    "${SFML_LIB_DIR}/sfml-system-s.lib"
    "${SFML_LIB_DIR}/freetype.lib"
    "${OPENGL_LIBRARIES}"
    "winmm.lib"
)

file(GLOB source_files
	"src/*.cpp"
)

set(SOURCES ${source_files})

add_executable(${PROJECT_NAME} ${SOURCES})
add_definitions(-DSFML_STATIC)
target_include_directories(${PROJECT_NAME} PRIVATE "${SFML_INC_DIR}" "include")
target_link_libraries(${PROJECT_NAME} ${SFML_LIBS})