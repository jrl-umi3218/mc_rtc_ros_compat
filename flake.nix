{
  description = "mc-rtc-ros-compat: a small library to keep mc-rtc ros-agnostic";

  inputs = {
    mc-rtc-nix.url = "github:mc-rtc/nixpkgs";
    flake-parts.follows = "mc-rtc-nix/flake-parts";
    jrl-cmakemodules.url = "github:jrl-umi3218/jrl-cmakemodules";
    # A tiny community repository that just yields a true/false value
    # This is used by CI to activate the private overlay while remaining in pure evaluation mode
    # Use as nix build . --override-input private-trigger github:boolean-option/true
    with-ros-trigger.url = "github:boolean-option/true";
  };

  outputs =
    inputs:
    inputs.flake-parts.lib.mkFlake { inherit inputs; } (
      { lib, ... }:
      let
        with-ros-default = inputs.with-ros-trigger.value or true;
      in
      {
        systems = [ "x86_64-linux" ];
        imports = [
          inputs.mc-rtc-nix.flakeModule
          {

            mc-rtc-nix.with-ros = with-ros-default;

            flakoboros = {
              overrideAttrs.jrl-cmakemodules = {
                src = inputs.jrl-cmakemodules;
              };
              overrides.mc-rtc-ros-compat = {
                with-ros = with-ros-default;
              };
              overrideAttrs.mc-rtc-ros-compat = {
                src = lib.cleanSource ./.;
              };

              packages = {
                mc-rtc-ros-compat =
                  {
                    stdenv,
                    lib,
                    cmake,
                    jrl-cmakemodules,
                    catch2_3,
                    with-ros ? true,
                    rclcpp,
                    ament-cmake, # for ament-index-cpp
                    human-description ? null, # for tests
                  }:

                  stdenv.mkDerivation {
                    pname = "mc-rtc-ros-compat";
                    version = "1.0.0";

                    src = lib.cleanSource ./.;

                    buildInputs = [
                      jrl-cmakemodules
                    ];
                    nativeBuildInputs = [
                      cmake
                      catch2_3
                    ]
                    # for tests
                    ++ lib.optional (human-description != null) human-description;
                    propagatedBuildInputs = lib.optionals with-ros [
                      rclcpp
                      ament-cmake
                    ];

                    cmakeFlags = [
                      (lib.cmakeBool "DISABLE_ROS" (!with-ros))
                      (lib.cmakeBool "BUILD_TESTS_WITH_ROS_PACKAGES" (human-description != null))
                    ];

                    doCheck = true;

                    meta = with lib; {
                      description = "mc-rtc-ros-compat: small library to keep mc-rtc ros-agnostic";
                      homepage = "https://github.com/jrl-umi3218/mc_rtc_ros_compat";
                      license = licenses.bsd2;
                      platforms = platforms.all;
                    };
                  };
              };
            };
          }
        ];
      }
    );
}
