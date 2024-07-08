let
  pkgs = import <nixpkgs> {};
  git = pkgs.git.overrideAttrs (o: {
    buildInputs = o.buildInputs ++ [ pkgs.sqlite ];
  });
in git
