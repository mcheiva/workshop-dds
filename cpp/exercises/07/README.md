# Exercise 7: Let's Play a Game!

Now that you are well‑versed in DDS, let’s test its capabilities together.

The game we will run was quickly prototyped in C++ with no optimizations—perfect for stressing messaging behavior under simple conditions.

It is a small combat game driven entirely by inputs published over DDS.
![alt text](image.png)

Create a service that spawns two publishers on two different topics. Each topic must carry the following payloads. You could repurpose the Publisher solution for this exercise.

* A struct in the namespace `EIVA::Game` named `MoveCommand` containing:
    * int32 player_id
    * float dir_x
    * float dir_y
* A struct in the namespace `EIVA::Game` named `WeaponCommand` containing:
    * int32 player_id
    * WeaponType weapon
* `WeaponType` should be an enum containing:
    * Gun
    * Shield
    * SlowField

`player_id` is the identifier for your player. Use the same `player_id` for both commands.

`dir_x` and `dir_y` form a direction vector (e.g., [1, 1] or [−1, 0]). The game normalizes it to determine movement direction.

`WeaponCommand` fires your weapon. You have three types:
* Gun: Fires a small missile in the direction that your player currently has
* Shield: Shields your player from taking a hit! Be careful however, once you use the shield you are unable to fire any command while the shield is active (and a small time afterwards!)
* SlowField: Places a slow field around your player that affects only opponents. While active (and briefly after), you cannot fire other weapons—use it to set up opponents.

Set up your service to use XML profiles and configure the participant as a `CLIENT` for a discovery server. The locator address and topic names will be provided during the workshop. So

Publisher QoS:
- Reliability: `RELIABLE`
- Durability: `TRANSIENT_LOCAL`
- History: `KEEP_LAST`, depth = 1 (the game does not request past samples, so a larger history is unnecessary).

In general, stick with the same `player_id` to avoid interfering with other players.

Suggested controls and behaviors:
* Accept console input via arrow keys; parse into `dir_x`/`dir_y` movement.
* Use Space to send `WeaponCommand`.
* Use Tab (or similar) to cycle `WeaponType` for `WeaponCommand`.
* Avoid spamming commands. The game uses a single processing thread for all incoming commands; I didn't do any further performance optimizations, so help keep it responsive for everyone.

## IDL Setup (Required Before Building)
Add both command types (`MoveCommand` and `WeaponCommand`) to your local IDL development directory and update the build configuration so your project generates and picks up the new types.

- If you are using the C++ Conan/CMake project:
    - Place two new `.idl` files (or one containing both structs) in `idls/idls/`.
    - Edit `idls/idls/CMakeLists.txt`:
        - Add your files to `IDL_FILES`.
        - Add the corresponding generated sources to `ALL_GENERATED_FILES` (e.g., `<YourIdl>PubSubTypes.cxx`, `<YourIdl>TypeObjectSupport.cxx`).
    - Rebuild so fastddsgen generates the C++ types.

- If you are using the MSVC style project:
    - Place the `.idl` files in the `msvc/IDL/idls/` directory.
    - Run `build_idls.bat` in `msvc/IDL/` to generate and copy the types into the solution.

- If you are using the C# track:
    - Place the `.idl` files in `csharp/IDL/idl/`.
    - Run `build_idls.bat` in `csharp/IDL/` to generate C++ types and the C# bindings (namespaced with `IDL.`).

Once the IDLs are added and the build scripts updated, rebuild so the generated `PubSubTypes` are available to your service. Then implement the publishers using the generated types under the `EIVA::Game` namespace.
