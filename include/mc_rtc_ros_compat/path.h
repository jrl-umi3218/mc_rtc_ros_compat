#pragma once
#include <filesystem>
#include <mc_rtc_ros_compat/config.hpp>
#include <string_view>

namespace mc_rtc_ros_compat {

/**
 * Resolves a ROS URI of the form `package://<package_name>/...` to an absolute
 * filesystem path.
 *
 * @return A pair where:
 *   - The first element is true if the input was a `package://` URI, false
 * otherwise.
 *   - The second element is the resolved absolute path (if applicable), or the
 * original path if not prefixed by package://
 *
 * @throw if the path is prefixed by `package://` but cannot be resolved.
 * This happens if mc_rtc does not have ROS support, or the package path is not
 * in ament index.
 *
 * @note: uses ament_index_cpp
 */
MC_RTC_ROS_COMPAT_DLLEXPORT std::pair<bool, std::filesystem::path>
resolve_package_path(std::string_view uri);

}  // namespace mc_rtc_ros_compat
