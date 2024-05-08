@echo off

git submodule init
git submodule update

mkdir build 2>nul
cd build
cmake ..
pause