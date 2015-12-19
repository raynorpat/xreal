# use pkg-config to get the directories and then use these values
# in the FIND_PATH() and FIND_LIBRARY() calls
include(UsePkgConfig)

pkgconfig(gtkglext-1.0 _GTKGLExtIncDir _GTKGLExtLinkDir _GTKGLExtLinkFlags _GTKGLExtCflags)

set(GTKGLEXT_DEFINITIONS ${_GTKGLExtCflags})

find_path(GTKGLEXT_INCLUDE_DIR
    NAMES
      gtk/gtkgl.h
    PATHS
      ${_GTKGLExtIncDir}
      /opt/gnome/include/gtkglext-1.0
      /opt/local/include/gtkglext-1.0
      /sw/include/gtkglext-1.0
      /usr/include/gtkglext-1.0
      /usr/local/include/gtkglext-1.0
    PATH_SUFFIXES
      gtkglext
)

find_library(GTKGLEXT_LIBRARY
    NAMES
      gtkglext
    PATHS
      ${_GTKGLExtLinkDir}
      /opt/gnome/lib
      /opt/local/lib
      /sw/lib
      /usr/lib
      /usr/local/lib
)

###################################################################################################

pkgconfig(gdkglext-1.0 _GDKGLExtIncDir _GDKGLExtLinkDir _GDKGLExtLinkFlags _GDKGLExtCflags)

set(GDKGLEXT_DEFINITIONS ${_GDKGLExtCflags})

find_library(GDKGLEXT_LIBRARY
    NAMES
      gdkglext-x11-1.0
    PATHS
      ${_GDKGLExtLinkDir}
      /opt/gnome/lib
      /opt/local/lib
      /sw/lib
      /usr/lib
      /usr/local/lib
)


if (GTKGLEXT_LIBRARY AND GTKGLEXT_INCLUDE_DIR)
	set(GTKGLEXT_FOUND TRUE)
endif (GTKGLEXT_LIBRARY AND GTKGLEXT_INCLUDE_DIR)

if (GTKGLEXT_FOUND)
    set(GTKGLEXT_LIBRARIES ${GTKGLEXT_LIBRARY} ${GDKGLEXT_LIBRARY})
    set(GTKGLEXT_INCLUDE_DIRS ${GTKGLEXT_INCLUDE_DIR})
endif (GTKGLEXT_FOUND)

if (GTKGLEXT_FOUND)
	if (NOT GTKGLEXT_FIND_QUIETLY)
		message(STATUS "Found GTKGLExt: ${GTKGLEXT_LIBRARIES}")
	endif (NOT GTKGLEXT_FIND_QUIETLY)
else (GTKGLEXT_FOUND)
	if (GTKGLEXT_FIND_REQUIRED)
		message(FATAL_ERROR "Could not find GTKGLExt")
	endif (GTKGLEXT_FIND_REQUIRED)
endif (GTKGLEXT_FOUND)

# show the GTKGLEXT_INCLUDE_DIRS and GTKGLEXT_LIBRARIES variables only in the advanced view
mark_as_advanced(GTKGLEXT_INCLUDE_DIRS GTKGLEXT_LIBRARIES GTKGLEXT_PUBLIC_LINK_FLAGS)
