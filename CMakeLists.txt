#
# Copyright 2026 CNRS-UM LIRMM, CNRS-AIST JRL
#

cmake_minimum_required(VERSION 3.22)

find_package(jrl-cmakemodules 2.1.0 CONFIG QUIET)

set(PROJECT_VERSION 1.0.2)
project(
    mc_rtc_ros_compat
    VERSION ${PROJECT_VERSION}
    DESCRIPTION "Small library to keep mc_rtc ros agnostic"
    HOMEPAGE_URL "https://github.com/jrl-umi3218/mc_rtc_ros_compat"
    LANGUAGES CXX
)

include(cmake/get-jrl-cmakemodules.cmake)

# Configure default binary and install directories, build type, etc.
jrl_configure_defaults()

# -------
# OPTIONS
# -------
jrl_option(DISABLE_ROS "Build without ROS." OFF)
# Unit tests
jrl_option(BUILD_TESTING "Build unit tests." ON)
# NOTE: some tests depend on human_description external package for package:// test with ament_index_cpp
#       these are only tested in Nix for now
jrl_option(BUILD_TESTS_WITH_ROS_PACKAGES "Build tests that depend human_description" OFF)
jrl_print_options_summary()

# ------------
# DEPENDENCIES
# ------------
if(BUILD_TESTING)
    enable_testing()
    find_package(Catch2 3 QUIET)
    if(NOT Catch2_FOUND)
        include(FetchContent)
        FetchContent_Declare(
            Catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG v3.6.0
        )
        FetchContent_MakeAvailable(Catch2)
    endif()
endif()

if(NOT DISABLE_ROS)
    jrl_find_package(rclcpp QUIET)
    jrl_find_package(ament_index_cpp QUIET)
endif()

add_library(mc_rtc_ros_compat SHARED src/path.cpp)
target_compile_features(mc_rtc_ros_compat PUBLIC cxx_std_17)
if(ament_index_cpp_FOUND AND rclcpp_FOUND)
    message(STATUS "ROS support enabled for mc_rtc_ros_compat")
    # link against ros and add compile definitions
    target_compile_definitions(
        mc_rtc_ros_compat
        PUBLIC MC_RTC_ROS_COMPAT_HAS_ROS_SUPPORT
    )
    target_link_libraries(
        mc_rtc_ros_compat
        PUBLIC ament_index_cpp::ament_index_cpp
    )
endif()
target_include_directories(
    mc_rtc_ros_compat
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
jrl_target_generate_config_header( mc_rtc_ros_compat PUBLIC VERSION "${PROJECT_VERSION}")
set_target_properties(
    mc_rtc_ros_compat
    PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION 1
        COMPILE_FLAGS "-DMC_RTC_ROS_COMPAT_EXPORTS"
)
jrl_target_headers(mc_rtc_ros_compat PUBLIC HEADERS include/mc_rtc_ros_compat/path.h BASE_DIRS include)

jrl_add_export_component(NAME mc_rtc_ros_compat TARGETS mc_rtc_ros_compat)

if(BUILD_TESTING)
    add_executable(test_path test/test_path.cpp)
    target_link_libraries(
        test_path
        PUBLIC mc_rtc_ros_compat Catch2::Catch2WithMain
    )
    add_test(NAME test_path COMMAND test_path)
    # Set LD_LIBRARY_PATH for the test (for Nix)
    set_tests_properties(
        test_path
        PROPERTIES
            ENVIRONMENT
                "LD_LIBRARY_PATH=$<TARGET_FILE_DIR:mc_rtc_ros_compat>:$ENV{LD_LIBRARY_PATH}"
    )
endif()

# Export the package files (headers, targets, config files, etc.)
jrl_export_package()

# Print summaries
jrl_print_dependencies_summary()
