# XreaL
![XreaL](https://github.com/raynorpat/xreal/raw/master/docs/xreal_scrnshot.jpg)

**The project is hosted at:** https://github.com/raynorpat

**Report bugs here:** https://github.com/raynorpat/xreal/issues

Uses code from the ioquake3 project ![ioq3](https://github.com/raynorpat/xreal/raw/master/docs/ioquake3_logo.jpg)


## GENERAL NOTES

A short summary of the file layout:

Directory                     | Description
:---------------------------- | :------------------------------------------------
XreaL/base/                   | XreaL media directory ( models, textures, sounds, maps, etc. )
XreaL/code/                   | XreaL source code ( renderer, game code, OS layer, etc. )
XreaL/code/tools/xmap         | map compiler ( .map -> .bsp ) (based on q3map)
XreaL/code/tools/xmap2        | map compiler ( .map -> .bsp ) (based on q3map2)
XreaL/code/tools/xmaster	  | master server
XreaL/code/tools/gtkradiant   | GtkRadiant map editor
XreaL/tools/gtkradiant/       | XreaL configured GtkRadiant editor work dir
XreaL/tools/blender/          | Blender plugins for ase, md3, and md5 models


## LICENSE

See docs/COPYING.txt for all the legal stuff.


## GETTING THE SOURCE CODE AND MEDIA

This project's git repository can be cloned with the following instruction set: 

`
git clone https://github.com/raynorpat/xreal.git
`


## COMPILING ON WINDOWS

NOTE: THIS IS REALLY OUTDATED......

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


## COMPILING ON GNU/LINUX

You need the following dependencies in order to compile XreaL with all features:
 * SDL >= 1.2
 * FreeType >= 2.3.5
 * OpenAL >= 0.0.8 
 * libcURL >= 7.15.5 
 * GTK+ >= 2.4.0 
 * gtkglext >= 1.0.0
 * libxml2 >= 2.0.0
 * zlib >= 1.2.0
 * cmake >= 2.8

`
mkdir build && cd build && cmake .. && make
`


## COMPILING ON MAC OS X

- Install MacPorts
- Install XQuartz
- Install dependencies with MacPorts:
    `
    sudo port install dylibbundler pkgconfig gtkglext cmake libsdl
    `
- Use cmake to produce XCode project files


## USING HTTP/FTP DOWNLOAD SUPPORT (SERVER)

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
   - 1 : ENABLE
   - 2 : do not use HTTP/FTP downloads
   - 4 : do not use UDP downloads
   - 8 : do not ask the client to disconnect when using HTTP/FTP

Server operators who are concerned about potential "leeching" from their
HTTP servers from other XreaL servers can make use of the HTTP_REFERER
that XreaL sets which is "XreaL://{SERVER_IP}:{SERVER_PORT}".  For,
example, Apache's mod_rewrite can restrict access based on HTTP_REFERER. 


## USING HTTP/FTP DOWNLOAD SUPPORT (CLIENT)

Simply setting cl_allowDownload to 1 will enable HTTP/FTP downloads on 
the clients side assuming XreaL was compiled with USE_CURL=1.
Like sv_allowDownload, cl_allowDownload also uses a bitmask value
supporting the following flags:
   - 1 : ENABLE
   - 2 : do not use HTTP/FTP downloads
   - 4 : do not use UDP downloads


## MULTIUSER SUPPORT ON WINDOWS SYSTEMS

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