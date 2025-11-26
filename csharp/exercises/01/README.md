# Exercise 1: Getting Started

The C# project consists of two main directories:
- `IDL/` — contains:
	- `build_idls.bat`: runs FastDDSGen to generate C++ types from IDL, then converts them to C# using our custom FastDDSGen build (C# support to be upstreamed to eProsima soon™).
	- `idl/`: the actual `.idl` structure definitions.
- `Solution/` — the codebase where you complete the exercises. Open `Solution.sln` and ensure you are connected to VPN so required NuGet packages can be restored from the self‑hosted feed.

## Local IDL Development
The base `Awesome.idl` located in `IDL/idl/` should remain unchanged for now. To add your own structures:
1. Place additional `.idl` files into `IDL/idl/`.
2. Run `build_idls.bat` in the parent `IDL/` directory.

Prerequisites: Conan must be configured with EIVA build profiles/remotes before running the script.

After the script completes, generated C# types appear in the `IDL.Net` project within `Solution`. Namespace note: if you declare `EIVA.Demo.SomeIdlType` in IDL, FastDDSGen prefixes with `IDL.` — access it in C# as `IDL.EIVA.Demo.SomeIdlType`.

## Mandatory Step Before Proceeding
Run `build_idls.bat` in the `IDL/` directory now. Do not start Exercise 2 until it succeeds; this ensures all models are present in the `Solution` projects.