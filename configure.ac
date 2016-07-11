AC_INIT([nonny], [0.1], [gkikola@gmail.com])
AM_INIT_AUTOMAKE([-Wall foreign])
CXXFLAGS="$CXXFLAGS -std=c++0x"
AC_PROG_CXX

SDL_VERSION=2.0
AM_PATH_SDL2($SDL_VERSION,
  :,
  AC_MSG_ERROR([*** SDL version $SDL_VERSION not found!])
)
CFLAGS="$CFLAGS $SDL_CFLAGS"
LIBS="$LIBS $SDL_LIBS"

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
  Makefile
  src/Makefile
])
AC_OUTPUT
