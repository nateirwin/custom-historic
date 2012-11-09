dnl
dnl $Id: config.m4
dnl

PHP_ARG_WITH(magickwand, for Magickwand support,
[  --with-magickwand[=DIR] Include Magickwand support], no)

dnl PHP_ARG_WITH(magickwand-gm, for magickwand support,
dnl [  --with-magickwand-gm[=DIR]       Magickwand: Use GraphicsMagick instead of ImageMagick as Backend], no, no)


if test "$PHP_MAGICKWAND" != "no"; then

dnl  if test "$PHP_MAGICKWAND_GM" != "no"; then
dnl     GM_PATH="GraphicsMagick/"
dnl     MAGICK_NAME="GraphicsMagick"
dnl     NEEDED_VERSION=1001003
dnl     NEEDED_VERSION_STRING=1.1.3
dnl     CONFIG_NAME="GraphicsMagick-config"
dnl     LIB_NAME="GraphicsMagick"
dnl     AC_DEFINE(MAGICKWAND_BACKEND, "GraphicsMagick", [ ])
dnl     AC_MSG_RESULT(Building with GraphicsMagick as backend)
dnl  else

     GM_PATH=""
     MAGICK_NAME="ImageMagick"
     NEEDED_VERSION=6001005
     NEEDED_VERSION_STRING=6.1.5
     CONFIG_NAME="Wand-config"
     LIB_NAME="Magick"
     AC_DEFINE(MAGICKWAND_BACKEND, "ImageMagick", [ ])
     AC_MSG_RESULT(Building with ImageMagick as backend)

dnl  fi

  if test -r $PHP_MAGICKWAND/include/${GM_PATH}wand/magick_wand.h; then
    AC_MSG_CHECKING(for $MAGICK_NAME in provided path)
    IMAGEMAGICK_DIR=$PHP_MAGICKWAND
    AC_MSG_RESULT(found in $IMAGEMAGICK_DIR)
  else
    AC_MSG_CHECKING(for $MAGICK_NAME in default path)
    for i in /usr/local /usr; do
      if test -r $i/include/${GM_PATH}wand/magick_wand.h; then
        IMAGEMAGICK_DIR=$i
        AC_MSG_RESULT(found in $i)
      fi
    done

    dnl red hat has some strange locations for their imagemagick files
    dnl so here's for redhat

    if test -z "$IMAGEMAGICK_DIR"; then
      for i in /usr/local/X11R6 /usr/X11R6; do
        if test -r $i/include/X11/${GM_PATH}wand/magick_wand.h; then
          IMAGEMAGICK_DIR=$i
          AC_MSG_RESULT(found in $i)
        fi
      done
    fi
  fi

  if test -z "$IMAGEMAGICK_DIR"; then
    AC_MSG_RESULT(not found)
    AC_MSG_ERROR(Please reinstall the $MAGICK_NAME distribution -
    magick_wand.h should be in <imagemagick-dir>/include/wand/)
  fi
  IMAGEMAGICK_CONFIG="${CONFIG_NAME}"

  if ${IMAGEMAGICK_DIR}/bin/${CONFIG_NAME} --libs  > /dev/null 2>&1; then
    IMAGEMAGICK_CONFIG=${IMAGEMAGICK_DIR}/bin/${CONFIG_NAME}
  else
    if ${IMAGEMAGICK_DIR}/${CONFIG_NAME} --libs  > /dev/null 2>&1; then
       IMAGEMAGICK_CONFIG=${IMAGEMAGICK_DIR}/${CONFIG_NAME}
    fi
  fi
  AC_MSG_CHECKING(for ${MAGICK_NAME} ${NEEDED_VERSION_STRING} or greater with $IMAGEMAGICK_CONFIG)
  imagemagick_version_full=`$IMAGEMAGICK_CONFIG --version`
  imagemagick_version=`echo ${imagemagick_version_full} | awk 'BEGIN { FS = "."; } { printf "%d", ($1 * 1000 + $2) * 1000 + $3;}'`
  AC_MSG_RESULT($imagemagick_version_full)
  if test "$imagemagick_version" -ge ${NEEDED_VERSION}; then
    IMAGEMAGICK_LIBS=`$IMAGEMAGICK_CONFIG --libs`
    IMAGEMAGICK_LDFLAGS=`$IMAGEMAGICK_CONFIG --ldflags`    
  else
    AC_MSG_ERROR(${MAGICK_NAME} ${NEEDED_VERSION_STRING} or later is required to compile php with magickwand support)
  fi

  PHP_ADD_INCLUDE($IMAGEMAGICK_DIR/include/${GM_PATH})
  PHP_EVAL_LIBLINE($IMAGEMAGICK_LIBS, MAGICKWAND_SHARED_LIBADD)
  PHP_EVAL_LIBLINE($IMAGEMAGICK_LDFLAGS, MAGICKWAND_SHARED_LIBADD)

  AC_CHECK_LIB(${LIB_NAME},InitializeMagick, 
  [ 
    AC_DEFINE(HAVE_MAGICKWAND,1,[ ])
  ],[
    AC_MSG_ERROR(There is something wrong. Please check config.log for more information.)
  ],[
    $IMAGEMAGICK_LIBS -L$IMAGEMAGICK_DIR/lib $IMAGEMAGICK_LDFLAGS
  ])

  PHP_EXTENSION(magickwand, $ext_shared)
  PHP_SUBST(MAGICKWAND_SHARED_LIBADD)
fi
