{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  buildInputs = [
    pkgs.boost.dev
    pkgs.curl.dev
  ];
}