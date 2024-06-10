# :confetti_ball: SMB2 Workshop Mod

An extension of ComplexPlane's [Super Monkey Ball 2 mod, ApeSphere](htps://github.com/ComplexPlane/ApeSphere), adding
features useful to custom pack creators. Includes the original practice mod, as well as quality-of-life and
feature-adding patches, and easier options for tweaking in-game variables.

Features include:

* Easy-to-modify configuration file for tweaking and enabling or disabling patches
* All REL patches included in the [smb2-rel-hacks repo](https://github.com/TheBombSquad/smb2-rel-hacks/)
* Patch extending reflective stage objects
* Patch adding SMB1 camera toggle
* Patch adding the missing 'w' to the story mode data select
* Patch that automatically skips story mode cutscenes
* And more miscellaneous patches!

Original ApeSphere features included:

* Console savestates
* An RTA timer that tracks time spent paused
* An individual-world practice mode
* Frame advance
* [Jump mod](https://www.youtube.com/watch?v=kWAunK6Av-Q)

...and more to come!

Planned upcoming features:

* Difficulty rearrangement in configuration file
* Lighting modification in configuration file
* Double 'Stage Select' REL patch
* Ultimate mode patch
* Custom party game configuration (lap timer, course names, etc)
* Custom stage timers
* Custom UI element names (replacing Stage with Floor, etc)

# Setup and Usage

Instructions for setup and usage of this mod are located in
the [relevant section](https://docs.google.com/document/d/194QZxrimkjHEzSSMKbafs86PnmiYmFBZUnoaEnks4es/edit#heading=h.t4unx1ftb63d)
of the Super Monkey Ball Custom Level Guide.

# Original ApeSphere Credits

Thank you to:

**Zephiles** for their numerous code contributions. These include:

* The initial [TTYD-Practice-Codes](https://github.com/Zephiles/TTYD-Practice-Codes) framework of which this mod is
  based on
* The current REL loader Gecko code
* A custom heap implementation for the mod
* A revised REL loader which serves as both a Gecko and ISO loader (not yet included)
* A Super Monkey Ball 1 port of the REL loader (not yet included)

**PistonMiner** for their [ttyd-tools](https://github.com/PistonMiner/ttyd-tools) framework, which provides everything
you need to turn C++ into a .GCI savefile which can be loaded into a Gamecube game at runtime. In addition, thanks very
much for the detailed answers to my romhacking-related questions.

**CraftedCart** for their substantial contributions to the Monkey Ball 2 decompile project, as well as the many large
contributions to the Monkey Ball hacking and custom level scene on the whole.

**Yoshimaster96** for labelling hundreds of functions and data in Super Monkey Ball 1, many of which were ported to
Super Monkey Ball 2.

**TheBombSquad** for their decompile contributions as well, and for writing the current version of the ISO REL loader.

**Alist** and **PetresInc** for providing insightful feedback on a pre-release alpha needed before publishing an initial
beta version of the mod.

**The Monkey Ball Speedrunning Scene** for inspiring this mod! Thank you for being so fun to be a part of.
