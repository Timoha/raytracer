cmake_minimum_required(VERSION 2.8.4)
project(Raytracer1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories ("${PROJECT_SOURCE_DIR}/eigen")

IF(OSX)
   INCLUDE_DIRECTORIES ( /System/Library/Frameworks )
   FIND_LIBRARY(GLUT_LIBRARY GLUT )
   FIND_LIBRARY(OpenGL_LIBRARY OpenGL )
   MARK_AS_ADVANCED (GLUT_LIBRARY
                     OpenGL_LIBRARY)
   SET(EXTRA_LIBS ${GLUT_LIBRARY} ${OpenGL_LIBRARY})
ENDIF (OSX)



set(SOURCE_FILES main.cpp)
add_executable(Raytracer1 ${SOURCE_FILES})
target_link_libraries(Raytracer1 ${EXTRA_LIBS})