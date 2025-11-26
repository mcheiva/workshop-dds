# DDS Workshop

Welcome to the EIVA DDS Workshop. This repository contains hands‑on exercises that guide you through building DDS applications, tuning QoS, and understanding discovery behavior using eProsima Fast DDS.

## Objectives
- Build a minimal Publisher and Subscriber using generated IDL types.
- Configure QoS policies (reliability, durability, history, resource limits).
- Explore late‑join behavior and QoS compatibility.
- Use XML profiles to externalize configuration.
- Inspect domain discovery and set up a Discovery Server.

## Prerequisites
- Windows with Visual Studio.
- CMake + Conan installed for C++ builds.
- .NET SDK for C# exercises (if applicable).
- Fast DDS and fastddsgen available via the provided build scripts/profiles.

## Installation and setup
All tracks (even C#) requires the use of Conan one way or another. This section walks you through setting up Conan in your build environment. I've also included steps for installing Python and CMake if they're not already set up.

If you already have a working Conan setup that is configured with our build profiles, you can proceed to the next section

**Prerequisites**:

- C++ compiler installed (e.g., gcc, clang, MSVC)

### Step-by-Step Instructions:

#### 1. Install Python (if not already installed)

Download and install Python. Make sure Python is added to your system's PATH during the installation.

#### 2. Install Conan and CMake using pip

Open your terminal or command prompt and run:

```bash
  pip install conan "cmake<4"
```

After installing, verify Conan and CMake are correctly added to your PATH by running:

```bash
  conan --version
  cmake --version
```

If conan is not recognized by your commandline, run

```bash
  pip show conan
```

and ensure that location folder is in your path environment variable

#### 3. Configure Conan

We use our self-hosted Artifactory server for managing Conan packages. This setup helps us maintain consistent build profiles and easily share pre-built binaries across Linux, Windows, and other platforms.

Configure Conan by running:

```bash
  conan config install https://github.com/eivacom/conan-configuration --type git
```

and login with your EIVA credentials. This will install the remote for our own Artifactory server found at: https://artifactory.eiva.local and allow you to use build profiles for visual Studio such as:

```bash
  Visual-Studio-2022-v143-x64-RelWithDebInfo
  Visual-Studio-2022-v143-x64-Debug
  Visual-Studio-2022-v143-x64-Release
```

#### 4. Conanfile Setup

The local directory (i.e. the cpp tutorial directory) already contains a conanfile.py, which defines all the necessary dependencies including Fast DDS and other helper interfaces we commonly use. When running conan install, Conan automatically fetches prebuilt binaries from our Artifactory server. Using --build=missing is important because if the requested dependency is not prebuilt for your specific platform or profile, Conan will automatically compile it locally for you without extra configuration.

#### 5. Install Dependencies with Conan

Run this command from the project's root directory to install dependencies from the local conanfile.py:

```bash
  conan install . -pr:b=<desired-conan-profile> -pr:h=<desired-conan-profile> --build=missing
```

An example for a windows build using Visual Studio compiling it in RelWithDebInfo mode would look like:

```bash
  conan install . -pr:b=Visual-Studio-2022-v143-x64-RelWithDebInfo -pr:h=Visual-Studio-2022-v143-x64-RelWithDebInfo --build=missing
```

#### 6. Build Project With Conan

Conan can directly handle the build process. The following command internally executes the conan install step before running the actual build with CMake. While the previous step is technically optional, it's included above for clarity. For general workflow, running the below command will be enough for setting up the full dependency tree and compling the desired library/executables.

```bash
  conan build . -pr:b=<desired-conan-profile> -pr:h=<desired-conan-profile> --build=missing
```

An example for a windows build using Visual Studio compiling it in RelWithDebInfo mode would look like:

```bash
  conan build . -pr:b=Visual-Studio-2022-v143-x64-RelWithDebInfo -pr:h=Visual-Studio-2022-v143-x64-RelWithDebInfo --build=missing
```

_Voila! You have now acquired all dependencies and the project has been built in the build directory!_

## Choose Your Track
Pick your preferred language to complete the exercises, there is a right choice, and a wrong one - so choose wisely!

- [C++ Exercises](cpp/README.md)
- [C# Exercises](csharp/README.md)
- [Rust Exercises](https://streamable.com/lf027o)
