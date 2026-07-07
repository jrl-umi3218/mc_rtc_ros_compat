#
# Copyright 2026 CNRS-UM LIRMM, CNRS-AIST JRL
#

cmake_minimum_required(VERSION 3.22)

find_package(jrl-cmakemodules 2.1.0 CONFIG QUIET)

project(
    mc_rtc_ros_compat
    VERSION 1.0.0
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
jrl_print_options_summary()

# ------------
# DEPENDENCIES
# ------------
if(NOT DISABLE_ROS)
    jrl_find_package(rclcpp QUIET)
    if(rclcpp_FOUND)
        message(
            STATUS
            "rclcpp found, building mc_rtc_ros_compat with ROS support."
        )
    else()
        message(
            STATUS
            "rclcpp not found, building mc_rtc_ros_compat without ROS support."
        )
    endif()
endif()

add_library(mc_rtc_ros_compat SHARED src/path.cpp)
target_compile_features(mc_rtc_ros_compat PUBLIC cxx_std_17)
if(rclcpp_FOUND)
    # link against ros and add compile definitions
    target_compile_definitions(
        mc_rtc_ros_compat
        PUBLIC MC_RTC_ROS_COMPAT_WITH_ROS
    )
    target_link_libraries(mc_rtc_ros_compat PUBLIC rclcpp::rclcpp)
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
    PROPERTIES COMPILE_FLAGS "-DMC_RTC_ROS_COMPAT_EXPORTS"
)
jrl_target_headers(mc_rtc_ros_compat PUBLIC HEADERS include/mc_rtc_ros_compat/path.h BASE_DIRS include)

jrl_add_export_component(NAME mc_rtc_ros_compat TARGETS mc_rtc_ros_compat)

# Export the package files (headers, targets, config files, etc.)
jrl_export_package()

# Print summaries
jrl_print_dependencies_summary()
