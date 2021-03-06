set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

find_package(LibXml2 REQUIRED)
include_directories(${LIBXML2_INCLUDE_DIR})

find_package(GTK2 REQUIRED)
include_directories(${GTK2_INCLUDE_DIRS} ${GTK2_GDK_INCLUDE_DIR} ${GTK2_GLIB_INCLUDE_DIR})

find_package(GtkGLExt REQUIRED)
include_directories(${GTKGLEXT_INCLUDE_DIR})

if(NOT WIN32)
	FIND_PACKAGE ( X11 REQUIRED )
	IF ( X11_FOUND )
      		INCLUDE_DIRECTORIES ( ${X11_INCLUDE_DIR} )
      		LINK_LIBRARIES ( ${X11_LIBRARIES} )
      		MESSAGE ( STATUS " X11_INCLUDE_DIR: " ${X11_INCLUDE_DIR} )
      		MESSAGE ( STATUS " X11_LIBRARIES: " ${X11_LIBRARIES} )
	ENDIF ( X11_FOUND )

	FIND_PACKAGE ( GLX REQUIRED )
	IF ( GLX_FOUND )
      		INCLUDE_DIRECTORIES ( ${GLX_INCLUDE_DIR} )
      		LINK_LIBRARIES ( ${GLX_LIBRARIES} )
	ENDIF ( GLX_FOUND )
else()
    add_definitions(
        -D_WIN32
    )
endif()

include_directories("${CMAKE_SOURCE_DIR}/libs")
include_directories("${CMAKE_SOURCE_DIR}/include")

set(RADIANTLIST
    autosave.cpp
    autosave.h
    brush.cpp
    brush.h
    brush_primit.cpp
    brush_primit.h
    brushmanip.cpp
    brushmanip.h
    brushmodule.cpp
    brushmodule.h
    brushnode.cpp
    brushnode.h
    brushtokens.cpp
    brushtokens.h
    brushxml.cpp
    brushxml.h
    build.cpp
    build.h
    camwindow.cpp
    camwindow.h
    clippertool.cpp
    clippertool.h
    commands.cpp
    commands.h
    console.cpp
    console.h
    csg.cpp
    csg.h
    dialog.cpp
    dialog.h
    eclass.cpp
    eclass.h
    eclass_def.cpp
    eclass_def.h
    eclass_doom3.cpp
    eclass_doom3.h
    eclass_fgd.cpp
    eclass_fgd.h
    eclass_xml.cpp
    eclass_xml.h
    entity.cpp
    entity.h
    entityinspector.cpp
    entityinspector.h
    entitylist.cpp
    entitylist.h
    environment.cpp
    environment.h
    error.cpp
    error.h
    feedback.cpp
    feedback.h
    filetypes.cpp
    filetypes.h
    filters.cpp
    filters.h
    findtexturedialog.cpp
    findtexturedialog.h
    glwidget.cpp
    glwidget.h
    grid.cpp
    grid.h
    groupdialog.cpp
    groupdialog.h
    gtkdlgs.cpp
    gtkdlgs.h
    gtkmisc.cpp
    gtkmisc.h
    help.cpp
    help.h
    image.cpp
    image.h
    main.cpp
    main.h
    mainframe.cpp
    mainframe.h
    map.cpp
    map.h
    mru.cpp
    mru.h
    multimon.cpp
    multimon.h
    nullmodel.cpp
    nullmodel.h
    parse.cpp
    parse.h
    patch.cpp
    patch.h
    patchdialog.cpp
    patchdialog.h
    patchmanip.cpp
    patchmanip.h
    patchmodule.cpp
    patchmodule.h
    plugin.cpp
    plugin.h
    pluginapi.cpp
    pluginapi.h
    pluginmanager.cpp
    pluginmanager.h
    pluginmenu.cpp
    pluginmenu.h
    plugintoolbar.cpp
    plugintoolbar.h
    points.cpp
    points.h
    preferencedictionary.cpp
    preferencedictionary.h
    preferences.cpp
    preferences.h
    qe3.cpp
    qe3.h
    qgl.cpp
    qgl.h
    referencecache.cpp
    referencecache.h
    renderer.cpp
    renderer.h
    renderstate.cpp
    renderstate.h
    resource.h
    scenegraph.cpp
    scenegraph.h
    select.cpp
    select.h
    selection.cpp
    selection.h
    server.cpp
    server.h
    shaders.cpp
    shaders.h
    sockets.cpp
    sockets.h
    stacktrace.cpp
    stacktrace.h
    surfacedialog.cpp
    surfacedialog.h
    texmanip.cpp
    texmanip.h
    textureentry.cpp
    textureentry.h
    textures.cpp
    textures.h
    texwindow.cpp
    texwindow.h
    timer.cpp
    timer.h
    treemodel.cpp
    treemodel.h
    undo.cpp
    undo.h
    url.cpp
    url.h
    view.cpp
    view.h
    watchbsp.cpp
    watchbsp.h
    winding.cpp
    winding.h
    windowobservers.cpp
    windowobservers.h
    xmlstuff.cpp
    xmlstuff.h
    xywindow.cpp
    xywindow.h
)

add_executable(radiant WIN32 radiant.rc ${RADIANTLIST})
target_link_libraries(radiant
    ${CMAKE_DL_LIBS}
    ${LIBXML2_LIBRARIES}
    ${GTK2_LIBRARIES}
    ${OPENGL_gl_LIBRARY}
    cmdlib
    container
    ddslib
    debugging
    gtkutil
    ${GTKGLEXT_LIBRARIES}
    l_net
#   math
    mathlib
    memory
    modulesystem
    os
    picomodel
    profile
    script
    signal
    stream
    string
    xmllib
)