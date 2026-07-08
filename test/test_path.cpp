#include <mc_rtc_ros_compat/path.h>

#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <iostream>
#include <string>

TEST_CASE("resolve_package_path handles non-package URIs",
          "[resolve_package_path]") {
  auto [is_package, path] =
      mc_rtc_ros_compat::resolve_package_path("/some/regular/path.txt");
  REQUIRE_FALSE(is_package);
  REQUIRE(path == std::filesystem::path("/some/regular/path.txt"));
}

#if defined(MC_RTC_ROS_COMPAT_HAS_ROS_SUPPORT) && \
    defined(BUILD_TESTS_WITH_ROS_PACKAGES)
TEST_CASE("resolve_package_path resolves package URIs",
          "[resolve_package_path]") {
  // Replace "your_package" and "some/file.txt" with actual values for your test
  // environment
  std::string file_path = "HeadLinkFull.dae";
  std::string uri = "package://human_description/meshes/" + file_path;
  std::cout << "Running ros test - looking for human_description path: " << uri
            << std::endl;
  auto [is_package, path] = mc_rtc_ros_compat::resolve_package_path(uri);
  REQUIRE(is_package);
  // The path should end with file_path
  REQUIRE(path.filename() == file_path);
  REQUIRE(path.parent_path().filename() == "meshes");
  std::cout << "Resolved path: " << path << std::endl;
}
#endif

TEST_CASE("resolve_package_path throws on package URI without ROS support",
          "[resolve_package_path]") {
#ifndef MC_RTC_ROS_COMPAT_HAS_ROS_SUPPORT
  std::string uri = "package://foo/bar.txt";
  REQUIRE_THROWS_AS(mc_rtc_ros_compat::resolve_package_path(uri),
                    std::runtime_error);
#endif
}
