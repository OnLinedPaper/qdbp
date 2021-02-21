# qdbp
A second attempt at the [GloWings](https://github.com/OnLinedPaper/GloWings) arcade project: more modular, more memory-efficient, and built 100% from the ground up.

### Overview
qdbp is a 2D game built on SDL graphics and a self-made engine. It is heavily driven towards modularity, with the end goal being ANY user can modify the game, without any prior coding experience. Most of the behavior of the game is driven through XML; this is exposed to the end user and allows them to add new enemies, maps, behaviors, and more, all of which are laoded at runtime. At present, the XML does not have a *friendly* interface, but can still be modified at-will without breaking a pre-built executable.

### How to Play
Oh, so you're just here 'cuz you want to know the controls? That's fine too! There will be plenty more added on, but for now, here's what you can do.
```
WASD    - Move the ship around.
SHIFT   - Boost your engines. Mind your heat!

HJKL    - Fire your weapon. Uses vim's control scheme.
O       - Jump through a portal. 

P       - Pause the game.
ESC     - Quit the game.
,       - Show hitboxes. (Will later be deprecated.)
```
<br><br>
# Code

## Engine
The game's engine runs an event-driven loop, allowing for keybounce protection and even Xbox controller input! (That's still a WIP though.) 
On init, the engine performs the following tasks:
- Inits the XML parser, then parses all XML and builds a tree from it.
- Inits the map handler, the renderer, the viewport, the timeframe, the font hadnler, and the message handler.
- Updates game tick and framerate data.
- Preloads data for weapons.
- Inits SDL.

Each cycle, the engine performs the following tasks:
- Get all keyboard events since the last tick and process them.
- Call for updates from the entity handler and the viewport. 
- Call the map, entity handler, and any debugging functions to draw themselves, in that order.
- Update the timeframe for both game logic and screen rendering. (Note that recent work in [bor3d](https://github.com/OnLinedPaper/bor3d) indicates there may be hidden bugs between render time and game time.)

The engine makes a point to offload as much of the game logic as possible to other parts of the code. 

## XML
The XML parser is a regex(!) driven, recursively built-and-searched tree structure. It is fairly limited in scope and ability, having been written to meet the needs of qdbp and no more (YAGNI). The XML code is split into "parser" and "node" logic.

The parser handles the building, checking, and eventual destruction of the xml tress. Given that this is fairly user-facing, there is a good amount of error checking involved, and the parser offers both graceful, informative failure for malformed XML and "safe mode" failure where default values are returned instead. It also handles all file I/O, leaving the nodes to focus on the data.

The nodes handle the searching and insertion logic for the tree, performing both recursively. When given a "tag" to insert, the nodes will recursively add the tag until it reaches its destination; when given a 'tag" to find, a similar recursive search is performed, returning informative errors if the tag is not found. Nodes also support the ability to print a list of all of their children, as well as a recursive print of the entire tree they hold.

## Environment
The environment of qdbp is broken down into several layers. At the top is the Map Handler, responsible for the generation and management of maps and their resources. At the second level is the Map, a collection of chunks and barriers which will eventually also be responsible for entity spawning. The third and final level is made up of Chunks, sections of 1000x1000 game-unit lengths that moderate player movement and will eventually be responsible for entity pathfinding.
### Map Handler
The map handler is fairly limited in scope at the moment, but will eventually be responsible for laoding new maps when the player "jumps" from one area to another. This feature, while planned, is not going to be developed for some time.
### Map
A Map is a collection of individual chunks arranged in some grid-aligned format. Maps are completely configurable through XML, and can be created or modified by changing the `resources/mapdata.xml` file - these chagnes are loaded at runtime when qdbp is first started up. Maps will eventually support random or procedural generation, but at present are limited to user-generation. Maps, being user-facing are fairly robust with error-checking, and will provide the user with information if they offer invalid inputs, such as a zero-width map or a player spawn position that is outside the playable area. Maps also bound the entities moving within them, and will prevent entities from moving outside their bounds. 
### Chunk
Chunks are small building blocks of maps. They are 1000x1000 game units in size, and can be created or modified by changing the `resources/chunkdata.xml` file. In the future, they will also include data that allows entities to pathfind through them. Chunks presently monitor their own boundaries, and are able to tell when an entity is attempting to leave them. If the entity would leave into an invalid area, the chunk can "rebuff" said entity, bouncing the entity off of its border no matter what its original speed was. 
