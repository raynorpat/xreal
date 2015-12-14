XreaL Readme - http://sourceforge.net/projects/xreal
====================================================

This file contains the following sections:

GENERAL NOTES
LICENSE
GETTING THE SOURCE CODE AND MEDIA
COMPILING ON WIN32 WITH VISUAL C++ 2008 EXPRESS EDITION
COMPILING ON WIN32 WITH MINGW
COMPILING ON GNU/LINUX
COMPILING ON MAC OS X
USING HTTP/FTP DOWNLOAD SUPPORT (SERVER)
USING HTTP/FTP DOWNLOAD SUPPORT (CLIENT)
MULTIUSER SUPPORT ON WINDOWS SYSTEMS


GENERAL NOTES
=============

A short summary of the file layout:

XreaL/base/		XreaL media directory ( models, textures, sounds, maps, etc. )
XreaL/blender/		Blender plugins for ase, md3, and md5 models
XreaL/code/		XreaL source code ( renderer, game code, OS layer, etc. )
XreaL/code/common	framework source code for command line tools like xmap
XreaL/code/xmap		map compiler ( .map -> .bsp ) (based on q3map)
XreaL/code/xmap2	map compiler ( .map -> .bsp ) (based on q3map2)
XreaL/code/xmass	master server
XreaL/code/gtkradiant	GtkRadiant editor source based off GPL release on 17th February 2006
XreaL/darkradiant/	XreaL configured DarkRadiant editor work dir
XreaL/gtkradiant/	XreaL configured GtkRadiant editor work dir


LICENSE
=======

See COPYING.txt for all the legal stuff.


GETTING THE SOURCE CODE AND MEDIA
=================================

This project's SourceForge.net Subversion repository can be checked out through SVN with the following instruction set: 

svn co https://svn.sourceforge.net/svnroot/xreal/trunk/xreal XreaL


COMPILING ON WIN32 WITH VISUAL C++ 2008 EXPRESS EDITION
=======================================================

1. Download and install the Visual C++ 2008 Express Edition.
2. Download libSDL from http://libsdl.org/release/SDL-devel-1.2.13-VC8.zip
	and extract it to C:\libSDL-1.2.13.
3. Download and install the OpenAL SDK from http://www.openal.org.
4. Download libcURL from http://curl.hoxt.com/download/libcurl-7.15.5-win32-msvc.zip
	and extract it to C:\libcURL
5. Download and install Gtk+ 2.10.11 development environment from http://gladewin32.sourceforge.net/.
6. Download http://xreal.varcache.org/STLport-5.1.5.7z and extract it to XreaL/code/.
7. Download http://oss.sgi.com/projects/ogl-sample/ABI/glext.h and copy it
	to C:\Program Files\Microsoft SDKs\Windows\v6.0A\Include\gl.

8. Add necessary include Directories in VC9 under Tools -> Options... -> Project and Solutions -> VC++ Directories:
	example:
	C:\libSDL-1.2.13\include
	C:\Program Files\OpenAL 1.1 SDK\include
	C:\libcURL\include
	C:\GTK\include
	C:\GTK\include\libxml2
	C:\GTK\include\glib-2.0
	C:\GTK\lib\glib-2.0\include
	C:\GTK\include\gtk-2.0
	C:\GTK\lib\gtk-2.0\include
	C:\GTK\include\cairo
	C:\GTK\include\pango-1.0
	C:\GTK\include\atk-1.0
	C:\GTK\include\gtkglext-1.0
	C:\GTK\lib\gtkglext-1.0\include

9. Add necessary lib Directories in VC9 under Tools -> Options... -> Project and Solutions -> VC++ Directories:
	example:
	C:\libSDL-1.2.13\lib
	C:\Program Files\OpenAL 1.1 SDK\lib\Win32
	C:\libcURL
	C:\GTK\lib

10. Use the VC9 solutions to compile what you need:
	XreaL/code/xreal.sln
	XreaL/code/gtkradiant/GtkRadiant.sln
	XreaL/code/xmap/xmap.sln


COMPILING ON WIN32 WITH MINGW
=============================

NOTE: OUTDATED

1. Download and install MinGW from http://www.mingw.org/.
2. Download http://www.libsdl.org/extras/win32/common/directx-devel.tar.gz
     and untar it into your MinGW directory (usually C:\MinGW).
3. Download http://oss.sgi.com/projects/ogl-sample/ABI/glext.h
	and copy it over the existing C:\MingW\include\GL\glext.h.
4. Download and install Python from http://www.python.org/.
5. Download and install SCons from http://www.scons.org/.
6. Download and install libcURL from http://curl.haxx.se/.
7. Download and install the OpenAL SDK from http://www.openal.org.
8. Add the Python installation directory to the system variable %PATH%
9. Download and install Gtk+ 2.10.7 development environment from http://gladewin32.sourceforge.net
10. Set the system variable: PKG_CONFIG_PATH to %GTK_BASEPATH%\lib\pkgconfig
11. Compile XreaL:
	>scons arch=win32-mingw


COMPILING ON GNU/LINUX
======================

You need the following dependencies in order to compile XreaL with all features:

 * SDL >= 1.2
 * FreeType >= 2.3.5
 * OpenAL >= 0.0.8 (if compiled with scons openal=1)
 * libcURL >= 7.15.5 (if compiled with scons curl=1)
 * GTK+ >= 2.4.0 (if compiled with scons mapping=1, requires glib, atk, pango, iconv, etc)
 * gtkglext >= 1.0.0 (if compiled with scons mapping=1)
 * libxml2 >= 2.0.0 (if compiled with scons mapping=1)
 * zlib >= 1.2.0 (if compiled with scons mapping=1)

Compile XreaL for x86 processers:
	>scons arch=linux-i386
Compile XreaL for x86_64 processers:
	>scons arch=linux-x86_64

Type scons -h for more compile options.


COMPILING ON MAC OS X
=====================

NOTE: OUTDATED

Make sure you have libcURL and the SDL framework installed.

Download http://oss.sgi.com/projects/ogl-sample/ABI/glext.h
	and copy it to XreaL/code/renderer

Use the included XCode project to compile XreaL and friends for both PPC & Intel Macs.
The XCode project is located here:
	>code/unix/MacSupport/XreaL.xcodeprj


USING HTTP/FTP DOWNLOAD SUPPORT (SERVER)
========================================

You can enable redirected downloads on your server by using the 'sets'
command to put the sv_dlURL cvar into your SERVERINFO string and
ensure sv_allowDownloads is set to 1.
 
sv_dlURL is the base of the URL that contains your custom .pk3 files
the client will append both fs_game and the filename to the end of
this value.  For example, if you have sv_dlURL set to
"http://xreal.sourceforge.net/", fs_game is "base", and the client is
missing "test.pk3", it will attempt to download from the URL
"http://xreal.sourceforge.net/base/test.pk3"

sv_allowDownload's value is now a bitmask made up of the following
flags:
    1 - ENABLE
    2 - do not use HTTP/FTP downloads
    4 - do not use UDP downloads
    8 - do not ask the client to disconnect when using HTTP/FTP

Server operators who are concerned about potential "leeching" from their
HTTP servers from other XreaL servers can make use of the HTTP_REFERER
that XreaL sets which is "XreaL://{SERVER_IP}:{SERVER_PORT}".  For,
example, Apache's mod_rewrite can restrict access based on HTTP_REFERER. 


USING HTTP/FTP DOWNLOAD SUPPORT (CLIENT)
========================================

Simply setting cl_allowDownload to 1 will enable HTTP/FTP downloads on 
the clients side assuming XreaL was compiled with USE_CURL=1.
Like sv_allowDownload, cl_allowDownload also uses a bitmask value
supporting the following flags:
    1 - ENABLE
    2 - do not use HTTP/FTP downloads
    4 - do not use UDP downloads


MULTIUSER SUPPORT ON WINDOWS SYSTEMS
====================================

On Windows, all user specific files such as autogenerated configuration,
demos, videos, screenshots, and autodownloaded pk3s are now saved in a
directory specific to the user who is running XreaL.

On NT-based systems such as Windows XP, this is usually a directory named:
  "C:\Documents and Settings\%USERNAME%\Application Data\XreaL\"

On Windows Vista, this would be:
  "C:\Users\%USERNAME%\Application Data\XreaL\"

Windows 95, Windows 98, and Windows ME will use a directory like:
  "C:\Windows\Application Data\XreaL"
in single-user mode, or:
  "C:\Windows\Profiles\%USERNAME%\Application Data\XreaL"
if multiple logins have been enabled.

You can revert to the old single-user behaviour by setting the fs_homepath
cvar to the directory where XreaL is installed.  For example:
  xreal.exe +set fs_homepath "c:\xreal"
Note that this cvar MUST be set as a command line parameter.