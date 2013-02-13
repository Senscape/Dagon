## DAGON - An Adventure Game Engine

### Description

Dagon is a game engine especially tailored for adventure games and based on open
technologies. Featuring a deceptively simple scripting language, blazing fast performance and high portability supporting Windows, Linux, and Mac OS X, the project aims to become a referendum of the genre.

_**Warning:** The engine is still under heavy development and script compatibility breaking changes might be introduced. Proceed with care!_

### Installation

The source code is first and foremost distributed as an Xcode project for Mac OS X. It should compile right out of the box on 10.6 Snow Leopard or 10.7 Lion as all the
required libraries are included.

A Microsoft Visual C++ 2010 Express solution is available in the Windows folder and is also ready for compilation.

Compiling on Linux is possible provided all the required libraries are available (see Credits in the Documents folder). A Makefile is available for your convenience in the Linux folder.

**Please note:** The Linux port still requires work and has been tested mostly on Ubuntu distributions.

### Running

We recommend using the readily available _Asylum Interactive Teaser_ for testing, which also serves as an advanced and highly in-depth tutorial. It can be downloaded from the Senscape website: http://www.senscape.net/asylum-teaser

Its source code is also available in a GitHub repo: https://github.com/Senscape/Asylum-Teaser

To create your own project, copy the Dagon binary in a folder with a _main.lua_ file or indicate the name of your main script in the _Dagon.cfg_ file (included in the Sample folder).

### Support

Feel free to visit our forums to discuss the engine and request technical support: http://www.senscape.com/forums

### Copyright

Copyright (c) 2012 Senscape s.r.l.
All rights reserved.

Senscape permits you to use, modify, and distribute this file in accordance with the terms of the Common Development and Distribution License (CDDL) agreement. A copy of the License can be found in the Documents folder.
