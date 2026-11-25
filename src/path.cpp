#include <mc_rtc_ros_compat/path.h>
#ifdef MC_RTC_ROS_COMPAT_HAS_ROS_SUPPORT
/**
 * https://github.com/ament/ament_index/pull/104 changed the amend-index-cpp API
 * to use std::filesystem This was backported to jazzy and kilted, but not
 * humble used on ubuntu jammy Thus we need to keep support for both, which is
 * done here by defining MC_RTC_ROS_COMPAT_HAS_GET_PACKAGE_SHARE_DIRECTORY This
 * should be removed when dropping ubuntu jammy support
 */
#if defined(MC_RTC_ROS_COMPAT_HAS_ROS_SUPPORT)
#if defined(__has_include)
#if __has_include(<ament_index_cpp/get_package_share_directory.hpp>)
#include <ament_index_cpp/get_package_share_directory.hpp>
#define MC_RTC_ROS_COMPAT_HAS_GET_PACKAGE_SHARE_DIRECTORY 1
#elif __has_include(<ament_index_cpp/get_package_share_path.hpp>)
#include <ament_index_cpp/get_package_share_path.hpp>
#define MC_RTC_ROS_COMPAT_HAS_GET_PACKAGE_SHARE_DIRECTORY 0
#endif
#else
#include <ament_index_cpp/get_package_share_directory.hpp>
#define MC_RTC_ROS_COMPAT_HAS_GET_PACKAGE_SHARE_DIRECTORY 1
#endif
#endif
#endif

#include <stdexcept>

namespace mc_rtc_ros_compat {

std::pair<bool, std::filesystem::path> resolve_package_path(
    std::string_view uri) {
  namespace fs = std::filesystem;
  const std::string package = "package://";
  if (uri.size() >= package.size() &&
      uri.substr(0, package.size()) ==
          package) {  // this is prefixed by package:// either resolve it or
                      // throw
    size_t split = uri.find('/', package.size());
    std::string pkg(uri.substr(package.size(), split - package.size()));
    auto leaf = fs::path(uri.substr(split + 1));
#ifdef MC_RTC_ROS_COMPAT_HAS_ROS_SUPPORT
    try {
#if MC_RTC_ROS_COMPAT_HAS_GET_PACKAGE_SHARE_DIRECTORY
      auto pkg_path = ament_index_cpp::get_package_share_directory(pkg);
      return {true, fs::path{pkg_path} / leaf};
#else
      auto pkg_path = ament_index_cpp::get_package_share_path(pkg);
      return {true, pkg_path / leaf};
#endif
    } catch (const std::exception& e) {
      throw std::runtime_error(
          "[mc_rtc::resolve_package_path] Ament failed to resolve ROS package "
          "'" +
          pkg + "': " + e.what());
      return {true, fs::path()};
    }
#else
    throw std::runtime_error(
        "[mc_rtc::resolve_package_path] Could not resolve path to ROS package "
        "path package '" +
        pkg + "' in URI '" + std::string(uri) +
        "', "
        "because mc_rtc was compiled without ROS support");
#endif
  }
  // This is not a package://, return the original uri
  return {false, fs::path{uri}};
}

}  // namespace mc_rtc_ros_compat
