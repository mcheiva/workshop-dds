# Exercise 7: Lets play a game!

Now that you are well versed in the world of DDS lets test the capabilitties of DDS!

The game that we will attempt to play together is one i personally vibe coded in an evening written in C++ with ZERO optimizations, so lets see how it goes.

It is a small combat game that resolves around shooting eachother based on inputs providing over DDS!
![alt text](image.png)

Create a service that spawns two publishers on two different topics, the topics are required to have the following information

* A struct in the namespace EIVA::Game with the name of MoveCommand containing the following fields:
    * int32 player_id
    * float dir_x
    * float dir_y
* A struct in the namespace EIVA::Game with the name of WeaponCommand
    * player_id
    * WeaponType weapon
* The WepaonType is should be an enum containing the following enum fields:
    * Gun
    * Shield
    * SlowField

Here the player_id corresponds to some value (you should use same player_id for both commands) that you would like your player to have.

the dir_x and dir_y values is a direction vector, so could be e.g.: [1,1] or [-1,0] etc, that is normalized by the game to provide what direction you wish your player to move. 

Secondly the WeaponCommand fires your weapon, you have three different kinds to choose from,
* Gun: Fires a small missile in the direction that your player currently has
* Shield: Shields your player from taking a hit! Be careful however, once you use the shield you are unable to fire any command while the shield is active (and a small time afterwards!)
* SlowField: Does what the name suggests, puts down a SlowField around your player that slows down only your opponents! Once agian, you will be unable to fire another weapons why your SlowField is active, so use it wisely and setup your opponents to get killed!

You should setup your service to use XML profiles and setup your participant to be a CLIENT to a discovery server, the locator address will be provided at the workshop!

The publishers should use realiability RELIEALBE, DURABILITY TRANSIENT_LOCAL, History settings using KEEP_LAST with depth 1 is fine. The game will connect but will not request past samples so do not bother keeping a large history in your writer.

In general try to stick with the same player_id to not interfere too much with other players.

You should/could:
* Implement the service to accept console commands via arrow-keys and use those to parse into dir_x and dir_y movement
* Use e.g: Space to send the WeponCommand
* Use e.g: Tab or similar to switch which WeaponType is being sent on the WeaponCommand
* Try not to spam too much, the game is only using a single processing thread to process ALL incoming commands so be nice!
