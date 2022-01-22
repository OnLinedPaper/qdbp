# qdbp
A second attempt at the [GloWings](https://github.com/OnLinedPaper/GloWings) arcade project: more modular, more memory-efficient, and built 100% from the ground up.

### Overview
qdbp is a 2D game built on SDL graphics and a self-made engine, and is both 100% original to me (resources, images, fonts, and sounds included) and 100% open source to you. It is heavily driven towards modularity, with the end goal being ANY user can modify the game, without any prior coding experience. Most of the behavior of the game is driven through XML; this is exposed to the end user and allows them to add new enemies, maps, behaviors, and more, all of which are loaded at runtime. At present, the XML does not have a *friendly* interface, but can still be modified at-will without breaking a pre-built executable.

### How to Play
Oh, so you're just here 'cuz you want to know the controls? That's fine too! There will be plenty more added on, but for now, here's what you can do.
```
WASD    - Move your ship around.
SHIFT   - Boost your engines. Mind their heat!

HJKL    - Fire your weapon. Uses vim's control scheme.
O       - Jump through a portal. 

P       - Pause the game.
ESC     - Quit the game.
,       - Show hitboxes. (Will later be deprecated.)
```

### How to Build
I'm still working on automated dependency checking for SDL, and will put that in the makefile at some point; for now, SDL dependencies can be installed with `sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev`. After install, use `make` to build the game, `make debug` to build with debugging flags, and `make mem` (after `make debug`) to run valgrind and record non-SDL-related memory leaks. 

This project currently does not support building *on* Windows, but might be buildable *for* Windows with `make win`. I haven't worked on this for a while though, so no guarantees.

<br><br>
# Code

## Engine
The game's engine runs an event-driven loop, allowing for keybounce protection and even Xbox controller input!

On init, the engine performs the following tasks:
- Inits the XML parser, then parses all XML and builds a tree from it.
- Inits the renderer, the viewport, the image handler, the timeframe, the map handler, the entity handler, the font handler, and the message handler. Note that several of these inits are not strictly necessary, but serve to keep things organized and can aid in troubleshooting.
- Updates game tick and framerate data.
- Preloads data for weapons.
- Inits SDL.

Each cycle, the engine performs the following tasks:
- Gets all keyboard events since the last tick and processes them.
- Calls for updates from the entity handler and the viewport. 
- Calls the map, entity handler, and any debugging functions to draw themselves, in that order.
- Update the timeframe for both game logic and screen rendering.

The engine makes a point to offload as much of the game logic as possible to other parts of the code. 

<br>

## XML
The XML parser is a regex(!) driven, recursively built-and-searched tree structure. It is fairly limited in scope and ability, having been written to meet the needs of qdbp and no more. The XML code is split into "parser" and "node" logic.

The parser handles the building, checking, and eventual destruction of the xml tress. Given that this is fairly user-facing, there is a good amount of error checking involved, and the parser offers both graceful, informative failure for malformed XML and "safe mode" failure where default values are returned instead. It also handles all file I/O, leaving the nodes to focus on the data.

The nodes handle the searching and insertion logic for the tree, performing both recursively. When given a "tag" to insert, the nodes will recursively add the tag until it reaches its destination; when given a 'tag" to find, a similar recursive search is performed, returning informative errors if the tag is not found. Nodes also support the ability to print a list of all of their children, as well as a recursive print of the entire tree they hold.

<br>

## Renderer, Viewport, and Images
The visual suite of qdbp holds some resemblance still to its predecessor, and may in the future be merged into something more comprehensive. The vast majority of the visual logic happens in Image.
### Renderer
The renderer is a fairly simple section of code that controls the window within which the game operates and the SDL_Renderer that interfaces with said window.
### Viewport
The viewport is a fairly simple section of code that maintains information about what the user can see. It keeps the dimensions that map the game world and its units of game distance to the user's screen and its units of screen distance. This information is mostly used to determine whether or not to render an image, and if so, where to place it on the screen.
### Image
An image is an XML-generated, multi-framed SDL_Texture that is rendered to the screen in any manner of scale, rotation, and color. Images can be created or modified by changing the `resources/imagedata.xml` file. Images are often used to represent the entities within the game, and indeed, anything that inherits from the "drawable" class has an image attached to it. Images are not, however, created for each entity; a single image is created for each unique entry in the XML. When an entity wishes some image drawn, it passes a request with the name of the image to the image handler, along with any other data (rotation, scale, color) it would like. Images, being fairly user-facing, feature robust error-checking, and will provide informative error messages to users if their XML entries are invalid in some way, often also offering visual cues when drawn in-game.
### Image Handler
The image handler is responsible for the creation, storage, and eventual destruction of individual images, as well as the logic of drawing them on-screen. Images are lazy-loaded, and if no such image exists when a request to draw comes in, it will be loaded once and maintained in memory. When a request to draw a specific image is presented, the handler works with the position of the entity and the position of the viewport to determine if said image is on-screen and thus eligible for drawing. It then rotates, colors, scales, and performs any other requested operations on the image, before finally sending it to the renderer to be displayed on the next screen refresh.

<br>

## Timekeeping
Game ticks and screen frames are managed by the timeframe. The game tracks both the time since the last game tick and the time since the last rendering frame; any excess time between expected ticks or frames is recorded as delta time and used in several calculations, most notably entity movement. As the time between ticks scales upwards, so to does the speed at which entities move, giving them the appearance of constant speed when the computer is exhibiting non-constant performance.

**Note:** Recent work in [bor3d](https://github.com/OnLinedPaper/bor3d) seems to indicate the presence of hidden bugs in the timekeeping update logic that could surface when the screen is set to refresh very quickly while game ticks are kept low. Nothing has arisen just yet, but I need to look into this before I try to add significant deviations between game tick rate and screen refresh rate.

<br>

## Environment
The environment of qdbp is broken down into several layers. At the top is the Map Handler, responsible for the generation and management of maps and their resources. At the second level is the Map, a collection of chunks and barriers which will eventually also be responsible for entity spawning. The third and final level is made up of Chunks, sections of 1000x1000 game-unit lengths that moderate player movement and will eventually be responsible for entity pathfinding.
### Map Handler
The map handler is fairly limited in scope at the moment; it handles the coordination of map changes when the player jumps from one to the other, which includes creating the first map when the game loads up.
### Map
A Map is a collection of individual chunks arranged in some grid-aligned format. Maps are completely configurable through XML, and can be created or modified by changing the `resources/mapdata.xml` file - these changes are loaded when qdbp is first started up. Maps will eventually support random or procedural generation, but at present are limited to user-generation. Maps, being user-facing, are fairly robust with error-checking, and will provide the user with information if they offer invalid inputs (such as a zero-width map or a player spawn position that is outside the playable area). Maps also bound the entities moving within them, and will prevent entities from moving outside their bounds. 
Additionally, maps are responsible for the loading and parsing of "spawn rules", which are then assigned to and handled by chunks. 
### Chunk
Chunks are small building blocks of maps. They are 1000x1000 game units in size, and can be created or modified by changing the `resources/chunkdata.xml` file. In the future, they will also include data that allows entities to pathfind through them. Chunks presently monitor their own boundaries, and are able to tell when an entity is attempting to leave them. If the entity would leave into an invalid area, the chunk can "rebuff" said entity, bouncing the entity off of its border no matter what its original speed was. 
Chunks are also responsible for spawning entities via "spawn rules". These collections of information include prerequisites for spawning, such as distance from the player or total entities currently alive on-map, and will likely grow more robust as time goes on; at present, they can spawn entities either upon player arrival or while the player is occupied, and can position and launch them in predefined or random ways. 

<br>

## Entities
An inheritance-heavy set of classes, qdbp's entities (internally, "movers") represent anything that can be displaced in some way, encompassing the player, enemy creatures and ships, the projectiles launched by either party, and any neutral entities that will be created in the future. Entities are XML-generated at runtime and can be created or modified by changing the `resources/gamedata.xml` file. The catergories at the moment (all of which are virtual classes) are movable, drawable, and mortal; from mortal comes the sub-classes weapon and gunner, and from gunner comes player. Future designs include "multi-segmented" entities, though this may be added to mortals instead of existing as a separate entity.
### Movable
The template for any entity, a movable encompasses the many things a 2d entity might need, such as position and velocity. Entities by default have acceleration and deceleration as their mode of movement: holding a direction key will accelerate up to a velocity cap, while releasing all keys will decay the entity's velocity until it stops moving. This gives rise to a <s>bug</s> feature wherein holding two opposite direction keys produces no net motion, but prevents velocity from decaying, allowing indefinite travel in the current direction.
### Drawable
A drawable entity has associated with it an image; it is render-able and is as such a parent class for any entity that the player is meant to see. Drawables also include a "team" - part of qdbp's game logic is its division into 7 teams of different color, though the actual interaction between said teams will be left to a different README.
### Mortal
An entity that can be interacted with, mortals feature shields, health, and of course, collision detection through means of hitboxes. These boxes are attached to XML-specified fixed points on an entity, and rotate around as it does. Of special note is that the hitboxes are "hybrid" boxes, processing collision between both 2D boxes and 1D lines. When an entity moves faster than the size of its own hitboxes, it becomes possible for it to "skip" over a collision it should have registered; to prevent this, any box that, when updated, detects it has moved further than its narrowest dimension, will trail a line from its current center point to its previous center point. Anything that collides with the line is treated as though it collided with the box. 
Mortals can only take damage, and cannot cause it on their own in any way. The only entity with a hurtbox is the weapon class; all others only possess hitboxes.
### Gunner
The ability to generate and launch weapons is a property unique to the gunner and its children. This class keeps track of, at present, one single weapon type, along with its ability to shoot said weapon. The `can_shoot()` function checks a variety of factors and can be overloaded by children to determine whether a shot can be fired; the gunner itself checks the time since the last shot. 
### Weapon
At present, the only entities with the ability to cause damage to others are weapons. These possess a hurtbox, but no hitbox, and as such cannot be destroyed in-flight under normal circumstances. Information included in weapons is how deeply they can pierce into enemy armor, how much damage they cause on impact, their parent, and other such data. Also included in the class as a whole are lookup tables that store their normal firing delay, heat generation, and xml path, on a per-id basis. 
### Player
It's you! The player is controlled by whoever's sitting behind the keyboard, and possesses a unique machanic, "heat". Firing a weapon, boosting to higher speed, and regenerating health create a buildup of "heat" that can cripple the ship if not carefully monitored. Several other gameplay-related features are included in the player, and eventually en inventory system will exist to modify the base stats of the ship. Only one player is expected on the field at any given time, and in the future, the creation of a new player will destroy the old one. 

<br>

## Entity Handler
The entity handler manages any entities that are currently or were previously in the game. It is responsible for the creation, storage, and eventual destruction of said entities, and mediates collisions, calls for active entities to update and draw themselves, and handles deactivated entities that may later be reused. Entities that are deactivated (i.e. projectiles that strike their target / travel too far, hostile ships that are destroyed) are NOT removed from memory, and are instead skipped over during the update and render cycles. When a new entity needs to be created, the handler first checks to see if any existing entities of that type are in a deactvated state. If so, they are teleported to their new position, given new data, and reactivated. Firing thousands of short-lived projectiles over the course of several minutes may only actually create some tens of entities in memory that are constantly recycled. 

<br>

## On-Screen Messages and Text
Printing debugging messages to a terminal is helpful up to a point, but qdbp will eventually feature in-game text and messages. At present, the ability to print messages on-screen is fairly limited in scope, and is restricted to placing a message at specific x,y coordinates with no regard to other extant messages. This functionalty will eventually be improved, but at present is only being used to print fast-updating debugging info. 
