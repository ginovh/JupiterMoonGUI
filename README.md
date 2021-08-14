JupiterMoonGUI
==============

Graphical show Jupiter moons position

Usage:

Dependencies :
Qt 4.8.0 and above 
AA+ v2.33 and above (http://www.naughter.com/aa.html)

AA+ itself implements the algorithms from the book "Astronomical algortihms" by Jean Meeus. (http://www.willbell.com/math/mc1.htm)
You may find comments in the code refering to this book.

To Build:
>./getaaplus.bash
>make distclean
>qmake jupitermoon.pro
>make -j4
>make clean  # remove intermediate files but leaves app

Supported platforms:
- Ubuntu, Mac OS X
- Can be build for iOS, but fonts and GUI are wrong
- Can be build for win32 if you manually build aaplus and change '/' with '\\' in .pro file (not tested)
