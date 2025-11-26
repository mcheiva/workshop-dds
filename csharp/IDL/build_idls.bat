@echo off
echo Cleaning build directory...

if exist build-swig (
    rmdir /s /q build-swig
    echo Removed build-swig directory.
) else (
    echo No build-swig directory to remove.
)

if exist build-fastddsgen (
    rmdir /s /q build-fastddsgen
    echo Removed build-fastddsgen directory.
) else (
    echo No build-fastddsgen directory to remove.
)

if exist generated (
    rmdir /s /q generated
    echo Removed generated directory.
) else (
    echo No generated directory to remove.
)

if exist include (
    rmdir /s /q include
    echo Removed include directory.
) else (
    echo No include directory to remove.
)

if not exist Fast-DDS-Gen (
	conan source conanfile_fastddsgen.py
) else (
    echo Fast-DDS-Gen already sourced
)

echo Starting Conan build...

conan build conanfile_fastddsgen.py -pr:a=Visual-Studio-2022-v143-x64-Release --build=missing
conan build conanfile_swig.py -pr:a=Visual-Studio-2022-v143-x64-Release --build=missing

echo Cleaning empty folders inside include...
for /f "delims=" %%d in ('dir "include" /ad /b /s ^| sort /R') do rd "%%d" 2>nul

echo Build finished.