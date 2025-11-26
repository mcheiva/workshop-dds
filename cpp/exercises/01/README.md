# Exercise 1: Getting Started

In C++ you have two options: either solve the exercise using a CMake‑based project (recommended) under the `conan/` subfolder, or a classic C++ Visual Studio project under the `msvc/` folder.

For both options you must have the Conan dependency manager set up; it is required for building local IDLs regardless of project type.

The following sections describe getting started with both project types — pick your approach.

## 1. Conan Project

The `conan/` folder contains a `conanfile.py` describing all required dependencies (Fast DDS and helper interfaces). Conan resolves and fetches prebuilt binaries from Artifactory; if a package for your profile is missing, `--build=missing` triggers a local build automatically.

The next two subsections briefly explain how Conan works. You should ideally run the `conan build` command in a terminal with your desired configuration. Note: you can install more than once.
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

Important: Run the build command below before opening the `conan/` folder in Visual Studio so CMake configuration and generated artifacts are present.

```bash
  conan build . -pr:a=<desired-conan-profile> --build=missing
```

Example (Windows, Visual Studio, `RelWithDebInfo`):

```bash
  conan build . -pr:a=Visual-Studio-2022-v143-x64-RelWithDebInfo --build=missing
```

_Voila! Dependencies resolved and the project built in the `build` directory._

## 1.3 Open `conan/` Folder in Visual Studio

Only after successfully running the Conan build, open the `conan/` folder in Visual Studio:
- In Explorer: Right‑click the folder → `Open With Visual Studio`.
- Or in Visual Studio: File → Open → Folder… and select `cpp/conan/`.
- Visual Studio detects `CMakeLists.txt` and configures CMake automatically.

If everything went well, you should see a CMake configuration such as `conan-RelWithDebInfo` in the drop‑down (matching your chosen profile).

Now that everything is set up, you no longer need to run Conan commands manually; you can build directly from within Visual Studio.

You can now skip to Section 3 to learn how to do local IDL development.

## 2. MSVC Project

Inside the `msvc/` folder you will find three key elements: a `build_idls.bat` file that runs fastddsgen to generate C++ from the IDL files; an `idls/` folder that contains a CMake project used by `build_idls.bat` to generate the C++ types via fastddsgen; and the `Solution/` project that references the generated IDLs. The IDLs used in the `Solution` project are defined within the `idls/` folder.

- `Solution/` — the codebase where you complete the exercises. Open `Solution.sln` and ensure you are connected to VPN (if you are not at HQ) so required NuGet packages can be restored from the self‑hosted feed.

Before continuing, you must run `build_idls.bat` to compile the C++ model layer required for the exercises.

## 3. Local IDL development in C++

The base `Awesome.idl` located in `idls/idls/` should remain unchanged for now. To add your own structures:
1. Place additional `.idl` files into `idls/idls/`.
2. Edit `idls/idls/CMakeLists.txt` and add your IDL to the CMake project so fastddsgen picks it up:
```cpp
set (IDL_FILES
    idls/Awesome.idl
    idls/<SubFolderPath>/<NameOfIdl>.idl // ADD YOUR OWN IDL
)

set(ALL_GENERATED_FILES
    ${GENERATED_TYPE_SUPPORT_DIR}/idls/AwesomePubSubTypes.cxx
	${GENERATED_TYPE_SUPPORT_DIR}/idls/AwesomeTypeObjectSupport.cxx
    ${GENERATED_TYPE_SUPPORT_DIR}/idls/<SubFolderPath>/<NameOfIdl>PubSubTypes.cxx // YOUR OWN IDL
	${GENERATED_TYPE_SUPPORT_DIR}/idls/<SubFolderPath>/<NameOfIdl>TypeObjectSupport.cxx // YOUR OWN IDL
)
```
Important: add your IDL to `IDL_FILES` and the generated support files to `ALL_GENERATED_FILES`. This ensures the build triggers fastddsgen to generate the files automatically.

If you are using the Conan CMake project, you can build directly from within Visual Studio — fastddsgen is integrated with CMake. If you use the classic C++ MSVC project, run `build_idls.bat` again so updated data structures are generated and present.