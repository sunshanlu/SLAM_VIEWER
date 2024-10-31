cmake_minimum_required(VERSION 3.5)

project(SLAMViewer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(BUILD_EXAMPLES ON)

find_package(Pangolin REQUIRED)
find_package(Sophus REQUIRED)
find_package(PCL REQUIRED)
find_package(TBB REQUIRED)
find_package(OpenCV 3.4 REQUIRED)
find_package(Eigen3 REQUIRED)

if (POLICY CMP0074)
    cmake_policy(SET CMP0074 NEW)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc)

add_library(slam_viewer SHARED ${SRC_FILES})
target_link_libraries(slam_viewer PUBLIC ${Pangolin_LIBRARY} ${PCL_LIBRARIES}
                                         Sophus::Sophus TBB::tbb ${OpenCV_LIBS})
target_include_directories(slam_viewer PUBLIC ${EIGEN3_INCLUDE_DIRS}
                                              ${PCL_INCLUDE_DIRS})

if(${BUILD_EXAMPLES})
    add_subdirectory(examples)
endif()

install(
    TARGETS slam_viewer
    EXPORT ${PROJECT_NAME}Targets
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/slam_viewer
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include)

install(
    EXPORT ${PROJECT_NAME}Targets
    FILE ${PROJECT_NAME}Targets.cmake
    DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake/${PROJECT_NAME})

# 生成xxxConfig.cmake配置文件
set(INCLUDE_DIRS ${CMAKE_INSTALL_PREFIX}/include)
set(LIBRARIES slam_viewer)
set(LIB_DIR ${CMAKE_INSTALL_PREFIX}/lib)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    VERSION 0.0.1
    COMPATIBILITY SameMajorVersion)

configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/${PROJECT_NAME}Config.cmake.in
    ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake/${PROJECT_NAME}
    PATH_VARS INCLUDE_DIRS LIB_DIR LIBRARIES)

install(FILES ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
              ${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake/${PROJECT_NAME})

# 输出配置信息，以查看是否配置正确
message(
    ========================================================================)
message(STATUS ${PROJECT_NAME} Configure:)
message(STATUS CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE})
message(STATUS CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX})
message(
    ========================================================================)

