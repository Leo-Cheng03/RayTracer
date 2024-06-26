CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)
PROJECT(COURSEWORK CXX)

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Release)
ENDIF()

ADD_SUBDIRECTORY(deps/vecmath)

SET(CW_SOURCES
        src/image.cpp
        src/main.cpp
        src/integrator.cpp
        src/mesh.cpp
        src/scene_parser.cpp
        src/bvh.cpp)

SET(CW_INCLUDES
        include/camera.hpp
        include/group.hpp
        include/hit.hpp
        include/image.hpp
        include/light.hpp
        include/material.hpp
        include/mesh.hpp
        include/object3d.hpp
        include/plane.hpp
        include/ray.hpp
        include/scene_parser.hpp
        include/sphere.hpp
        include/transform.hpp
        include/triangle.hpp
        include/integrator.hpp
        include/bvh.hpp
        )

set(CMAKE_CXX_STANDARD 20)

ADD_EXECUTABLE(${PROJECT_NAME} ${CW_SOURCES} ${CW_INCLUDES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} vecmath)
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE include)
