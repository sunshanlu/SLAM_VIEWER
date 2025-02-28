cmake_minimum_required(VERSION 3.5)

project(slam_viewer VERSION 0.0.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if(NOT DEFINED BUILD_EXAMPLES)
    set(BUILD_EXAMPLES ON)
endif()

find_package(Pangolin REQUIRED)
find_package(Sophus REQUIRED)
find_package(PCL REQUIRED)
find_package(TBB REQUIRED)
find_package(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(PACKAGE_INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/include)
set(PACKAGE_LIBS_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/lib)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc)

add_library(slam_viewer SHARED ${SRC_FILES})
target_link_libraries(
    slam_viewer PUBLIC ${Pangolin_LIBRARY} ${PCL_LIBRARIES} Sophus::Sophus
                       TBB::tbb ${OpenCV_LIBS} Eigen3::Eigen)
target_include_directories(
    slam_viewer PUBLIC ${OpenCV_INCLUDE_DIRS} ${PCL_INCLUDE_DIRS}
                       ${PACKAGE_INCLUDE_INSTALL_DIR})

if(${BUILD_EXAMPLES})
    add_subdirectory(examples)
endif()

# 添加安装指令
install(
    TARGETS slam_viewer
    EXPORT ${PROJECT_NAME}Targets
    LIBRARY DESTINATION ${PACKAGE_LIBS_INSTALL_DIR}
    ARCHIVE DESTINATION ${PACKAGE_LIBS_INSTALL_DIR})
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/slam_viewer
        DESTINATION ${PACKAGE_INCLUDE_INSTALL_DIR})

# 安装导出文件
install(
    EXPORT ${PROJECT_NAME}Targets
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake/${PROJECT_NAME})

# 生成并安装Config文件
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

configure_file(Config.cmake.in
               "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake" @ONLY)

install(FILES ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
              ${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake/${PROJECT_NAME})

# 输出配置信息，以查看是否配置正确
message(====================================================================)
message(STATUS ${PROJECT_NAME} Configure:)
message(STATUS CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE})
message(STATUS BUILD_EXAMPLES: ${BUILD_EXAMPLES})
message(STATUS CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX})
message(====================================================================)
