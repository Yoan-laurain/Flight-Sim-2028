@echo off
mkdir build 2>nul
cd build
xcopy /s /e /y ..\res\ .\OpenGLProject\res\
cmake ..