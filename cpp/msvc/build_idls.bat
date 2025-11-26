@echo off

set TARGET_DIR=Solution/IDL/idls/

if exist "%TARGET_DIR%" (
    echo Cleaning C++ files in "%TARGET_DIR%" and subfolders...
    del /q /s "%TARGET_DIR%\*.cxx"
    del /q /s "%TARGET_DIR%\*.hpp"
    del /q /s "%TARGET_DIR%\*.ipp"
) else (
    echo No "%TARGET_DIR%" directory to clean.
)

echo Starting Conan build...
conan build idl/conanfile.py -pr:a=Visual-Studio-2022-v143-x64-Release --build=missing

echo Build finished.