## JupiterMoonGUI

### Graphical show Jupiter moons position

Calculates next/prev interesting events. It also shows when GRS is in front. I use this tool mainly to find out "interesting" times to picture Jupiter (e.g. shadow transit + GRS in front etc.)

![Screenshot](Screenshot.png)

**Dependencies :**
- Qt 4.8.0 and above 
- AA+ v2.33 and above (http://www.naughter.com/aa.html)

AA+ implements the algorithms from the book "Astronomical algortihms" by Jean Meeus. (http://www.willbell.com/math/mc1.htm)
You may find comments in the code refering to this book.

**Build:**
```
./getaaplus.bash
mkdir build; cd build/
qmake ../jupitermoon.pro
make -j

make distclean # to clean repo artifacts before pushing to github
make clean  # remove intermediate files but leaves app
```

**Supported platforms:**
- Every platform that Qt supports
