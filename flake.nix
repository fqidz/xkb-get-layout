{
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    systems.url = "github:nix-systems/default";
    flake-utils = {
      url = "github:numtide/flake-utils";
      inputs.systems.follows = "systems";
    };
  };

  outputs =
    { nixpkgs, flake-utils, ... }:
    flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
      in
      {
        packages.default = pkgs.clangStdenv.mkDerivation {
          name = "xkb-get-layout";
          src = builtins.path {
            name = "xkb-get-layout";
            path = ./.;
          };
          nativeBuildInputs = [ pkgs.makeWrapper ];

          phases = [
            "buildPhase"
            "postInstall"
          ];
          buildPhase = ''
            mkdir -p $out/bin/
            make -C $src OUTPUT_DIR=$out/bin
          '';
          postInstall = ''
            wrapProgram $out/bin/xkb-get-layout \
              --prefix PATH : ${
                pkgs.lib.makeBinPath [
                  pkgs.nix
                  pkgs.hyprland
                ]
              }
          '';
          outputs = [ "out" ];
          meta = {
            description = "Tiny script to get the language short code of the current keyboard layout in hyprland";
            homepage = "https://github.com/fqidz/xkb-get-layout";
            platforms = pkgs.lib.platforms.linux;
            license = pkgs.lib.licenses.mit;
            mainProgram = "xkb-get-layout";
            maintainers = pkgs.lib.maintainers.fqidz;
          };
        };

        devShells.default = pkgs.mkShell {
          packages = [
            # pkgs.clang-tools should come before pkgs.clang or else clangd can't detect headers
            # https://github.com/NixOS/nixpkgs/issues/76486
            pkgs.clang-tools
            pkgs.clang
            pkgs.gnumake
            pkgs.gdb

            # manpaths dont appear in devshells
            # https://github.com/NixOS/nixpkgs/pull/234367
            # workaround here:
            # https://discourse.nixos.org/t/how-to-get-postgres-man-pages-in-a-devshell/47321/2?u=fqidz
            (pkgs.buildEnv {
              name = "devShell";
              paths = [
                pkgs.man-pages-posix
                pkgs.man-pages
                pkgs.clang-manpages
              ];
            })
          ];
        };
      }
    );
}
