{
	description = "Zirc emulator development flake";
	inputs = {
		nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
	};

	outputs = {
		self,
		nixpkgs,
	}: let
		pkgs = nixpkgs.legacyPackages.x86_64-linux;
	in {
		devShells.x86_64-linux.default =
			pkgs.mkShell {
				buildInputs = with pkgs; [
					valgrind
					gcc
					gnumake
					cmake
					pkg-config
					glib
					SDL2
					SDL2_ttf
					harfbuzz
					gtkmm4
					portaudio
					fontconfig
				];
			};
	};
}
