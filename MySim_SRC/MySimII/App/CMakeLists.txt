
if(MSVC)
    add_definitions(-DFCAppMySimII -DHAVE_ACOSH -DHAVE_ASINH -DHAVE_ATANH)
else(MSVC)
    add_definitions(-DHAVE_LIMITS_H -DHAVE_CONFIG_H)
endif(MSVC)

include_directories(
    ${CMAKE_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_BINARY_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}
    ${Boost_INCLUDE_DIRS}
    ${OCC_INCLUDE_DIR}
    ${PYTHON_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIR}
    ${XercesC_INCLUDE_DIRS}
)

set(MySimII_LIBS
	Part
    FreeCADApp
)

SET(MySimII_SRCS
    AppMySimII.cpp
    PreCompiled.cpp
    PreCompiled.h
    MySimIIAnalysis.h
    MySimIIAnalysis.cpp
    MySimIIMaterialsObject.h
    MySimIIMaterialsObject.cpp
    MySimIIInitiativesObject.h
    MySimIIInitiativesObject.cpp
    MySimIIForcesObject.h
    MySimIIForcesObject.cpp
    MySimIIBoundariesObject.h
    MySimIIBoundariesObject.cpp
    MySimIIMeshNodesObject.h
    MySimIIMeshNodesObject.cpp
    MySimIIMeshBoundariesObject.h
    MySimIIMeshBoundariesObject.cpp
    MySimIIMeshRegionsObject.h
    MySimIIMeshRegionsObject.cpp
    MySimIIMeshCreateObject.h
    MySimIIMeshCreateObject.cpp
    MySimIISolversObject.h
    MySimIISolversObject.cpp
    MySimIILocalRunObject.h
    MySimIILocalRunObject.cpp
)

if(FREECAD_USE_PCH)
    add_definitions(-D_PreComp_)
    GET_MSVC_PRECOMPILED_SOURCE("PreCompiled.cpp" MySimII_CPP_SRCS ${MySimII_SRCS})
    ADD_MSVC_PRECOMPILED_HEADER(MySimII PreCompiled.h PreCompiled.cpp MySimII_CPP_SRCS)
endif(FREECAD_USE_PCH)

add_library(MySimII SHARED ${MySimII_SRCS})
target_link_libraries(MySimII ${MySimII_LIBS})

SET_BIN_DIR(MySimII MySimII /Mod/MySimII)
SET_PYTHON_PREFIX_SUFFIX(MySimII)

INSTALL(TARGETS MySimII DESTINATION ${CMAKE_INSTALL_LIBDIR})
