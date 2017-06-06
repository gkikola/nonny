**Nonny is still in development. Binaries are not yet provided. Please
check back later for official releases.**

Nonny
=====

![Nonny](media/nonny.png)

Description
-----------

Play and create nonogram puzzles! Nonograms (also known as Griddlers
or Picross) are logic puzzles in which numeric clues are used to fill
in squares within a grid in order to reveal a picture.

![Puzzle](media/screenshots/puzzle_sm.png)


What Is a Nonogram?
-------------------

Nonograms are logic puzzles originating from Japan. You start with a
blank grid and then fill in the cells according to the clue numbers at
the sides. When you finish the puzzle, you're left with a picture to
look at:

![Solving a puzzle](media/tutorial/solving.png)

For example, a clue of "4" means that you need to fill in four
consecutive blocks somewhere within that line. By considering the
different possible positions you can determine that some of the cells
in the line must be filled:

![Line solving - 4](media/tutorial/clue.png)

Similarly, a clue of "1 2" means that one cell is to be filled,
followed by a gap of at least one blank cell, followed by two
consecutive filled cells:

![Line solving - 1 2](media/tutorial/clue2.png)

By working line by line in this way, you can solve most
puzzles. Harder puzzles may require more advanced techniques, but
well-made puzzles should not require guessing.

Nonny also supports multicolor puzzles. These puzzles work the same way,
but each clue number is colored to indicate which color the cells must
be painted:

![Multicolor puzzle](media/tutorial/color_solving.png)

Adjacent blocks of cells of different colors need not have a gap
between them. For example, a blue 2 followed by a red 2 means that
there are two consecutive blue cells, possibly but not necessarily
followed by a gap, followed by two consecutive red cells:

![Multicolor line solving - 2 2](media/tutorial/color_clue.png)

Again, by considering the different possibilities you can find cells that must be filled in, which gives you more information to solve the other lines in the puzzle.


Development Status
------------------

Nonny is playable but not yet ready for an initial release. Most of
the basic functionality for playing, editing, and analyzing puzzles
has been implemented. However, there are still some minor additions
that need to be made and some general polishing that needs to be done.

At the moment, Nonny only supports two file formats: [Steven Simpson's
.non format](http://www.lancaster.ac.uk/~simpsons/nonogram/) (extended
for multicolor support) and the .g format used by [Mirek Olšák and
Petr Olšák's nonogram
solver](http://www.olsak.net/grid.html#English). Many other popular
formats will be supported in the future.


Installation
------------

Windows binaries will eventually be provided. For now the only way to
install Nonny is to build the game yourself.

To build Nonny, you'll need [CMake](https://cmake.org/) 3.1 or later,
[SDL2](https://www.libsdl.org/),
[SDL2_image](https://www.libsdl.org/projects/SDL_image/), and
[SDL2_ttf](https://www.libsdl.org/projects/SDL_ttf/).

GNU/Linux build and install example:
```
git clone https://github.com/gkikola/nonny.git
cd nonny
mkdir build
cd build
cmake ..
make
sudo make install
```


Copyright
---------

**Nonny** is copyright &copy; 2017 Gregory Kikola. License GPLv3+:
[GNU GPL version 3 or later](http://www.gnu.org/licenses/gpl.html).

**Nonny** is free software: you are free to change and redistribute it. There
is NO WARRANTY, to the extent permitted by law. See the file
[COPYING](COPYING) for more details.
