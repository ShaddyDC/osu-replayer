{
  inputs = {
    nixpkgs.url = "nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem
      (system:
        let
          pkgs = nixpkgs.legacyPackages.${system};
          inputs = with pkgs;[
            cmake
            libGLU
            SDL2
            openssl
            ninja
          ];

          # Uses podman to ensure same process as CD
          # Note that this still requires podman to be enabled on the host
          build-emscripten = pkgs.writeShellScriptBin "build-emscripten" ''
            ${pkgs.podman}/bin/podman pull ghcr.io/shaddydc/emscripten-magnum-docker:master
            ${pkgs.podman}/bin/podman run -v "$PWD":/src ghcr.io/shaddydc/emscripten-magnum-docker:master sh -c "
              cd /src && cd build-emscripten &&
              cmake .. -DCMAKE_TOOLCHAIN_FILE="../toolchains/generic/Emscripten-wasm.cmake" -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_INSTALL_PREFIX=./deploy &&
              cmake --build . --target install"
          '';
        in
        with pkgs;
        {
          devShells.default = stdenv.mkDerivation {
            name = "dev-shell";
            buildInputs = inputs ++ [ build-emscripten ];
          };
        }
      );
}
