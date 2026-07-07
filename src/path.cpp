#include <mc_rtc_ros_compat/path.h>
#ifdef MC_RTC_ROS_COMPAT_HAS_ROS_SUPPORT
#include <ament_index_cpp/get_package_share_directory.hpp>
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
#ifdef __EMSCRIPTEN__
    throw std::runtime_error(
        "[mc_rtc::resolve_package_path] Could not resolve path to ROS package "
        "path package '" +
        pkg +
        "' in "
        "URI '" +
        std::string(uri) + "', because mc_rtc was compiled with Emscripten");
#endif
#ifdef MC_RTC_ROS_COMPAT_WITH_ROS_SUPPORT
    try {
      auto pkg_path = ament_index_cpp::get_package_share_directory(pkg);
      return {true, fs::path(pkg_path) / leaf};
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
