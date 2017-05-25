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


Development Status
------------------

Nonny is playable but not yet ready for pre-release. Currently, you
can solve both black and white as well as multicolor nonograms from
anywhere on your hard drive and Nonny will track your completion
status and times. You can even save your progress in the middle of a
puzzle. However, Nonny does not yet allow you to create your own
nonograms.

At the moment, Nonny only supports two file formats: [Steven
Simpson](http://www.lancaster.ac.uk/~simpsons/nonogram/)'s .non format
(extended for multicolor support) and the .g format used by Mirek
Olšák and Petr Olšák's [nonogram
solver](http://www.olsak.net/grid.html#English). Many other popular
formats will be supported in the future.

Finally, Nonny does not yet come prepackaged with a large puzzle
selection. There are a few puzzles included now that are used for
testing but these are not finalized. When Nonny is officially
released, it will include a good assortment of puzzles to play.


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
