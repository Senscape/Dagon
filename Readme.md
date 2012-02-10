## DAGON - An Adventure Game Engine

### Description

Dagon is a game engine especially tailored for adventure games and based on open
technologies. Featuring a deceptively simple scripting language, blazing fast performance and high portability supporting Windows, Linux, Mac OS X and iOS devices, the project aims to become a referendum of the genre.

### Installation

The source code is first and foremost distributed as an Xcode project for Mac OS X. It should compile right out of the box on 10.6 Snow Leopard or 10.7 Lion as all the
required libraries are included.

A Microsoft Visual C++ 2010 Express solution is available in the Windows folder and is also ready for compilation.

Compiling on Linux is possible provided all the required libraries are available (see Credits in the Documents folder). Simply create a project and include all the *.c files
in the Dagon and Other Sources folders with the exception of .mm files and
DGSystemWin.cpp.

**Please note:** Linux is not supported as of this writing. A tested module will be
uploaded shortly.

### Running

A sample project is included to try the engine. Either copy the Dagon binary inside the
Sample folder or make sure the working directory is set to this folder when running from
an IDE. The source code is fully available including helpful comments.

To load your own projects, either provide a main.lua file or indicate the name of your
script in a dagon.cfg file.

**Please note:** The assets provided with the sample are part of the _Asylum_ game which
is a commercial project from Senscape and may not be re-used in any way. They are solely
provided for educational purposes.

### Support

Feel free to visit our forums to discuss the engine and request technical support:

http://www.senscape.com/forums/

### Copyright

Copyright (c) 2011 Senscape s.r.l.
All rights reserved.

Senscape permits you to use, modify, and distribute this file in accordance with the terms of the Common Development and Distribution License (CDDL) agreement. A copy of the License
can be found in the Documents folder.
