{ pkgs ? import <nixpkgs> {}}:
let
  lib = pkgs.lib;
  fakelibsources = pkgs.fetchFromGitHub {
    owner = "SCOTT-HAMILTON";
    repo = "FakeLib";
    rev = "master"; # laziness
    sha256 = "1a334w4hifk3b38904w9zyh52axbjj8w90zfn01fz4wnvzg7dkmx";
  };
in
with pkgs; mkShell {
  propagatedBuildInputs = [
    clang-tools
    findutils
    gnugrep
  ];
  shellHook = ''
    cd ..
    format(){
      find . -regextype egrep -regex '.*\.(cpp|h)' | grep -v "subprojects/argparse" | xargs clang-format -i
    }
    tidy(){
      clang-tidy FakeMicWavPlayerLib.cpp -- -std=c++17\
        -I ./include \
        -I ./subprojects/OggPlayer/include \
        -I "${fakelibsources}/include" \
        -I "${lib.getDev libpulseaudio}/include"
    }
  '';
}

