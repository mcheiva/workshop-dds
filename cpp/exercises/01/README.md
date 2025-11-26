# Exercise 1: Getting Started

In C++ you have two options, either solve the exercise using a CMake project (i would definitely recommend) under the `conan` subfolder or a classic C++ Visual Studio project under the `mscvc` folder.

Commonly for both is that it requires you to have setup conan dependecy manager as it is required for building the local idls reqardless of project type.

In the following two sections is a description on getting started in either of the two project types, pick your poison.

## 1. Conan Project

The `conan` folder contains a `conanfile.py` describing all required dependencies (Fast DDS and helper interfaces). Conan resolves and fetches prebuilt binaries from Artifactory; if a package for your profile is missing, `--build=missing` triggers a local build automatically.

The following two sections explain briefly how Conan works, you should ideally end up running the `build` version in a terminal with a your desired configuration of your choice. Note - you can install more than once!
## 1.1. Install Dependencies with Conan

Run this from the project root to install dependencies declared in `conanfile.py`:

```bash
  conan install . -pr:a=<desired-conan-profile> --build=missing
```

Example (Windows, Visual Studio, `RelWithDebInfo`):

```bash
  conan install . -pr:a=Visual-Studio-2022-v143-x64-RelWithDebInfo --build=missing
```

## 1.2. Build Project With Conan (Mandatory Before Opening in VS)

Conan can also run the full build (it will perform install first, then invoke CMake). For day‑to‑day workflow, this single command sets up dependencies and compiles targets.

Important: Run the build command below before opening the `conan` folder in Visual Studio so CMake configuration and generated artifacts are present.

```bash
  conan build . -pr:a=<desired-conan-profile> --build=missing
```

Example (Windows, Visual Studio, `RelWithDebInfo`):

```bash
  conan build . -pr:a=Visual-Studio-2022-v143-x64-RelWithDebInfo --build=missing
```

_Voila! Dependencies resolved and the project built in the `build` directory._

## 1.3 Open `conan` Folder in Visual Studio

Only after successfully running the Conan build, open the `conan` folder in Visual Studio:
- In Folder: Right Click → Select `Open With Visual Studio`.
- In VS: Select `Open a local folder` → Navigate to `conan/` folder.
- VS detects `CMakeLists.txt` and configures CMake automatically.

If everything went well, you should see a CMake configuration such as `conan-RelWithDebInfo` in the drop‑down (matching your chosen profile).

Now that everything is setup, you no longer have to run any command line conan commands, and you can now build directly from within Visual Studio! 

You can now skip to the third section and understand how to do local IDL development

## 2. MSVC Project

Inside the msvc folder you will find three key aspects, a `build_idls.bat` file that internally will run fastddsgen to generate C++ from the idl files. a `idls` folder that contains a cmake project used by the `build_idls.bat` to generate the c++ types using fastddsgen. The idls used int the `Solution` project are defined until the `idls` folder.

- `Solution/` — the codebase where you complete the exercises. Open `Solution.sln` and ensure you are connected to VPN (if you are not at HQ) so required NuGet packages can be restored from the self‑hosted feed.

Before continueing it is required that you run the `build_idls.bat` file to compile the C++ model layer required for the exercises.

## 3. Local IDL development in C++

The base `Awesome.idl` located in `idls/idls/` should remain unchanged for now. To add your own structures:
1. Place additional `.idl` files into `idls/idls/`.
2. Enter the CMakeLists.txt file inside the idls/idls folder and add your IDL to the CMake project for it to be picked up by fastddsgen:
```cpp
set (IDL_FILES
    idls/Awesome.idl
)

set(ALL_GENERATED_FILES
    ${GENERATED_TYPE_SUPPORT_DIR}/idls/AwesomePubSubTypes.cxx
	${GENERATED_TYPE_SUPPORT_DIR}/idls/AwesomeTypeObjectSupport.cxx
)
```
Prerequisites: Conan must be configured with EIVA build profiles/remotes before running the script.
If you are using the conan cmake project, you can just directly hit build from within Visual Studio as fastddsgen is fully integrated with CMake, if you use the classic C++ msvc style project, you have to go back and run the `build_idls.bat` for the updated datastructures to be present.