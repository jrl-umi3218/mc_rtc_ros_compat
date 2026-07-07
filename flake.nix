{
  description = "mc-rtc-ros-compat: a small library to keep mc-rtc ros-agnostic";

  inputs.mc-rtc-nix.url = "github:mc-rtc/nixpkgs";
  inputs.jrl-cmakemodules.url = "github:jrl-umi3218/jrl-cmakemodules";

  outputs =
    inputs:
    inputs.mc-rtc-nix.lib.mkFlakoboros inputs (
      { lib, ... }:
      {
        overrideAttrs.jrl-cmakemodules = {
          src = inputs.jrl-cmakemodules;
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
                    with-ros ? true,
                    rclcpp
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
                    ];
                    propagatedBuildInputs = lib.optional with-ros rclcpp;

                    cmakeFlags = [
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
      }
    );
}
