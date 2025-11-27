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

and login with your EIVA credentials. This will install the remote for our own Artifactory server found at: https://artifactory.eiva.com and allow you to use build profiles for visual Studio such as:

```bash
  Visual-Studio-2022-v143-x64-RelWithDebInfo
  Visual-Studio-2022-v143-x64-Debug
  Visual-Studio-2022-v143-x64-Release
```

Once setup you ready to proceed to the exercises!

## Choose Your Track
Pick your preferred language to complete the exercises, there is a right choice, and a wrong one - so choose wisely!

- [C++ Exercises](cpp/README.md)
- [C# Exercises](csharp/README.md)
- [Rust Exercises](https://streamable.com/lf027o)
