{
  inputs.nixpkgs.url = "github:nixos/nixpkgs";

  outputs = { self, nixpkgs }:
  let
    system = "x86_64-linux";
    pkgs = nixpkgs.legacyPackages.${system};
  in {
    devShells.${system}.default = pkgs.mkShell {
      buildInputs = with pkgs; [
        cmake
        gdb
        qt6.full
        qt6.qtbase
        qtcreator
        qt6.wrapQtAppsHook
        makeWrapper
        bashInteractive
      ];
    };

    packages.${system}.default = pkgs.qt6Packages.callPackage ./build.nix {};
  };
}

