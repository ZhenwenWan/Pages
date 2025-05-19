if(MSVC)
    add_definitions(-DHAVE_ACOSH -DHAVE_ATANH -DHAVE_ASINH)
else(MSVC)
    add_definitions(-DHAVE_LIMITS_H -DHAVE_CONFIG_H)
endif(MSVC)

if (BUILD_SMESH)
    add_definitions(-DHAVE_SMESH -DHAVE_MEFISTO)
endif(BUILD_SMESH)

if(BUILD_FEM_NETGEN)
    add_definitions(-DHAVE_NETGEN)
endif(BUILD_FEM_NETGEN)

include_directories(
    ${CMAKE_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}
    ${Boost_INCLUDE_DIRS}
    ${OCC_INCLUDE_DIR}
    ${COIN3D_INCLUDE_DIRS}
    ${QT_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIR}
    ${PYTHON_INCLUDE_DIRS}
    ${XercesC_INCLUDE_DIRS}
	{VTK_DIR}
)
link_directories(${OCC_LIBRARY_DIR})

set(MySimIIGui_LIBS
    Part
    MySimII
    FreeCADGui
	${VTK_LIBRARIES}
)

if(BUILD_QT5)
    qt5_add_resources(MySimIIResource_SRCS Resources/MySimII.qrc)
else()
    qt4_add_resources(MySimIIResource_SRCS Resources/MySimII.qrc)
endif()
SOURCE_GROUP("Resources" FILES ${MySimIIResource_SRCS})

set(MySimIIGui_MOC_HDRS
   InitialMesh.h
   SolutionSetup.h
   FrequencySweep.h
   MeshView.h
   Boundaries.h
   ViewProviderMySimII.h
   ViewProviderMaterials.h
   ViewProviderInitiatives.h
   ViewProviderForces.h
   ViewProviderBoundaries.h
   ViewProviderMeshNodes.h
   ViewProviderMeshBoundaries.h
   ViewProviderMeshRegions.h
   ViewProviderMeshCreate.h
   ViewProviderSolvers.h
   RotateXYZ.h
   SelectFrequency.h
   SolverSettings.h
   Contours.h
   Glyphs.h
   Animat.h
	ProjectView.h
	#Waveportmodes.h
	#Waveportpost.h
)
fc_wrap_cpp(MySimIIGui_MOC_SRCS ${MySimIIGui_MOC_HDRS})
SOURCE_GROUP("Moc" FILES ${MySimIIGui_MOC_SRCS})



set(MySimIIGui_UIC_SRCS
    	InitialMesh.ui
		SolutionSetup.ui
		FrequencySweep.ui
		MeshView.ui
		Boundaries.ui
		RotateXYZ.ui
		SelectFrequency.ui
		SolverSettings.ui
	        Contours.ui
	        Glyphs.ui
	        Animat.ui
		#Waveport.ui
		#Waveportmodes.ui
		#Waveportpost.ui
)
qt4_wrap_ui(MySimIIGui_UIC_HDRS ${MySimIIGui_UIC_SRCS})

qt4_add_resources(MySimII_QRC_SRCS Resources/MySimII.qrc)
SET(MySimIIGui_SRCS
    ${MySimIIGui_QRC_SRCS}
    ${MySimIIResource_SRCS}
	${MySimIIGui_UIC_HDRS}
      ${MySimII_QRC_SRCS}
    AppMySimIIGui.cpp
    AppMySimIIGuiPy.cpp
    Resources/MySimII.qrc
	InitialMesh.ui
	InitialMesh.cpp
	InitialMesh.h
    PreCompiled.cpp
    PreCompiled.h
    ProjectView.h
    ProjectView.cpp
    Workbench.cpp
    Workbench.h
	SolutionSetup.h
	SolutionSetup.cpp
	SolutionSetup.ui
	FrequencySweep.h
	FrequencySweep.cpp
	FrequencySweep.ui
	MeshView.h
	MeshView.cpp
	MeshView.ui
	Boundaries.h
	Boundaries.cpp
	Boundaries.ui
	CommonFunction.h
	CommonFunction.cpp
	Geometry.h
	Geometry.cpp
	SIF.h
	SIF.cpp
	ViewProviderMySimII.h
	ViewProviderMySimII.cpp
	ViewProviderAnalysis.h
	ViewProviderAnalysis.cpp
        ViewProviderMaterials.h
        ViewProviderMaterials.cpp
	ViewProviderInitiatives.h
	ViewProviderInitiatives.cpp
	ViewProviderForces.h
	ViewProviderForces.cpp
        ViewProviderBoundaries.h
        ViewProviderBoundaries.cpp
	ViewProviderMeshNodes.h
	ViewProviderMeshNodes.cpp
	ViewProviderMeshBoundaries.h
	ViewProviderMeshBoundaries.cpp
	ViewProviderMeshRegions.h
	ViewProviderMeshRegions.cpp
	ViewProviderMeshCreate.h
	ViewProviderMeshCreate.cpp
	ViewProviderSolvers.h
	ViewProviderSolvers.cpp
	ViewProviderLocalRun.h
	ViewProviderLocalRun.cpp
	RotateXYZ.h
	RotateXYZ.cpp
	RotateXYZ.ui
	SelectFrequency.h
	SelectFrequency.cpp
	SelectFrequency.ui
	SolverSettings.h
	SolverSettings.ui
	SolverSettings.cpp
	Contours.h
	Contours.cpp
	Contours.ui
	Glyphs.h
	Glyphs.cpp
	Glyphs.ui
	Animat.h
	Animat.cpp
	Animat.ui
        Command.cpp	
)


SET(MySimIIGuiIcon_SVG Resources/icons/MySimIIWorkbench.svg)

add_library(MySimIIGui SHARED ${MySimIIGui_SRCS} ${MySimIIGuiIcon_SVG})
target_link_libraries(MySimIIGui ${MySimIIGui_LIBS})

fc_copy_sources(MySimIIGui "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_DATADIR}/Mod/MySimII" ${MySimIIGuiIcon_SVG})
INSTALL(FILES ${MySimIIGuiIcon_SVG} DESTINATION "${CMAKE_INSTALL_DATADIR}/Mod/MySimII/Resources/icons")

SET_BIN_DIR(MySimIIGui MySimIIGui /Mod/MySimII)
SET_PYTHON_PREFIX_SUFFIX(MySimIIGui)

INSTALL(TARGETS MySimIIGui DESTINATION ${CMAKE_INSTALL_LIBDIR})

# Python modules ui files, they are copied as they are, thus the need not to be added to MySimII.qrc
# see https://forum.freecadweb.org/viewtopic.php?f=10&t=25833
SET(MySimIIGuiPythonUI_SRCS
    Resources/ui/PhysicalMaterials.ui
    Resources/ui/Boundaries.ui
    Resources/ui/Forces.ui
    Resources/ui/Initiatives.ui
    Resources/ui/Materials.ui
    Resources/ui/Materials01.ui
    Resources/ui/MeshBoundaries.ui
    Resources/ui/MeshCreate.ui
    Resources/ui/MeshCreate0.ui
    Resources/ui/MeshNodes.ui
    Resources/ui/MeshRegions.ui
    Resources/ui/PhysicalMaterials.ui
    Resources/ui/PhysicalProperties.ui
    Resources/ui/Properties01.ui
    Resources/ui/Solvers.ui
    Resources/ui/LocalRun.ui
)

ADD_CUSTOM_TARGET(MySimIIPythonUi ALL
    SOURCES ${MySimIIGuiPythonUI_SRCS}
)

fc_copy_sources(MySimIIPythonUi "${CMAKE_BINARY_DIR}/Mod/MySimII" ${MySimIIGuiPythonUI_SRCS})

INSTALL(FILES ${MySimIIGuiPythonUI_SRCS} DESTINATION Mod/MySimII/Resources/ui)

