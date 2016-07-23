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

AC_CHECK_LIB(SDL2_image, IMG_LoadTexture, ,
  AC_MSG_ERROR([*** SDL2_image library not found]))

AC_CHECK_LIB(SDL2_ttf, TTF_OpenFont, ,
  AC_MSG_ERROR([*** SDL2_ttf library not found]))

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
  Makefile
  data/Makefile
  src/Makefile
])
AC_OUTPUT
