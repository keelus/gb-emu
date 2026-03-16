{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
	buildInputs = [
		pkgs.valgrind
		pkgs.gcc
		pkgs.gnumake
		pkgs.cmake
		pkgs.pkg-config
		pkgs.glib
		pkgs.SDL2
		pkgs.SDL2_ttf
	];

	shellHook = ''
	'';
}
