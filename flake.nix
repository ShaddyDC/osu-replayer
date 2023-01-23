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
        in
        with pkgs;
        {
          devShells.default = stdenv.mkDerivation {
            name = "dev-shell";
            buildInputs = inputs;
          };
        }
      );
}
