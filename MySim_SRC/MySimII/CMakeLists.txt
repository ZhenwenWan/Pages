
include(${VTK_USE_FILE})
add_definitions(-DFC_USE_VTK)

add_subdirectory(App)


if(BUILD_GUI)
    add_subdirectory(Gui)
endif(BUILD_GUI)


# Python non Gui packages and modules
SET(MySimIIScripts_SRCS
    Init.py
    InitGui.py
    ObjectsMySimII.py
)

SET(MySimIICommands_SRCS
    MySimIIcommands/__init__.py
    MySimIIcommands/commands.py
    MySimIIcommands/manager.py
)

SET(MySimIIObjects_SRCS
    MySimIIObjects/__init__.py
    MySimIIObjects/_MySimIIBoundaries.py
    MySimIIObjects/_MySimIIForces.py
    MySimIIObjects/_MySimIIInitiatives.py
    MySimIIObjects/_MySimIIMaterials.py
    MySimIIObjects/_MySimIIMeshBoundaries.py
    MySimIIObjects/_MySimIIMeshCreate.py
    MySimIIObjects/_MySimIIMeshNodes.py
    MySimIIObjects/_MySimIIMeshRegions.py
    MySimIIObjects/_MySimIIProperties.py
)

SET(MySimIIGuiObjects_SRCS
    MySimIIGuiObjects/__init__.py
    MySimIIGuiObjects/_MySimIIMeshRegions.py
    MySimIIGuiObjects/_ViewProviderMySimIIBoundaries.py
    MySimIIGuiObjects/_ViewProviderMySimIIForces.py
    MySimIIGuiObjects/_ViewProviderMySimIIInitiatives.py
    MySimIIGuiObjects/_ViewProviderMySimIIMaterials.py
    MySimIIGuiObjects/_ViewProviderMySimIIMeshBoundaries.py
    MySimIIGuiObjects/_ViewProviderMySimIIMeshCreate.py
    MySimIIGuiObjects/_ViewProviderMySimIIMeshNodes.py
    MySimIIGuiObjects/_ViewProviderMySimIIMeshRegions.py
    MySimIIGuiObjects/_ViewProviderMySimIIProperties.py
    MySimIIGuiObjects/MySimIISelectionWidgets.py
)


SET(MySimIIAllScripts
    ${MySimIIScripts_SRCS}
    ${MySimIICommands_SRCS}
)

ADD_CUSTOM_TARGET(MySimIIScriptsTarget ALL SOURCES 
	          ${MySimIIAllScripts}
)

fc_copy_sources(MySimIIScriptsTarget "${CMAKE_BINARY_DIR}/Mod/MySimII" ${MySimIIAllScripts})

# install Python packages (for make install)
INSTALL(FILES ${MySimIIScripts_SRCS} DESTINATION Mod/MySimII)
INSTALL(FILES ${MySimIICommands_SRCS} DESTINATION Mod/MySimII/MySimIIcommands)
INSTALL(FILES ${MySimIIObjects_SRCS} DESTINATION Mod/MySimII/MySimIIObjects)
INSTALL(FILES ${MySimIIGuiObjects_SRCS} DESTINATION Mod/MySimII/MySimIIGuiObjects)
