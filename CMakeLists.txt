cmake_minimum_required(VERSION 3.5)

project(slam_viewer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Release)
set(BUILD_EXAMPLES OFF)

find_package(Pangolin REQUIRED)
find_package(Sophus REQUIRED)
find_package(PCL REQUIRED)
find_package(TBB REQUIRED)
find_package(OpenCV 3.4 REQUIRED)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/core /usr/include/eigen3
    ${PCL_INCLUDE_DIRS})

file(GLOB SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc)

add_library(slam_viewer SHARED ${SRC_FILES})
target_link_libraries(slam_viewer ${Pangolin_LIBRARY} ${PCL_LIBRARIES}
                      Sophus::Sophus TBB::tbb ${OpenCV_LIBS})

if(${BUILD_EXAMPLES})
    add_subdirectory(examples)
endif()
