# Exercise 1: Getting Started


#### 4. Conanfile Setup

The local directory (i.e. the cpp tutorial directory) already contains a conanfile.py, which defines all the necessary dependencies including Fast DDS and other helper interfaces we commonly use. When running conan install, Conan automatically fetches prebuilt binaries from our Artifactory server. Using --build=missing is important because if the requested dependency is not prebuilt for your specific platform or profile, Conan will automatically compile it locally for you without extra configuration.

#### 5. Install Dependencies with Conan

Run this command from the project's root directory to install dependencies from the local conanfile.py:

```bash
  conan install . -pr:a=<desired-conan-profile> --build=missing
```

An example for a windows build using Visual Studio compiling it in RelWithDebInfo mode would look like:

```bash
  conan install . -pr:a=Visual-Studio-2022-v143-x64-RelWithDebInfo --build=missing
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