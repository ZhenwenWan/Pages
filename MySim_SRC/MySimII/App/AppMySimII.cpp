/***************************************************************************
 *   Copyright (c) YEAR YOUR NAME         <Your e-mail address>            *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/


#include "PreCompiled.h"
# include <Python.h>

#include <Base/Console.h>
#include <Base/Interpreter.h>
#include <Base/PyObjectBase.h>
#include <CXX/Extensions.hxx>

#include "MySimIIMaterialsObject.h"
#include "MySimIIInitiativesObject.h"
#include "MySimIIForcesObject.h"
#include "MySimIIBoundariesObject.h"
#include "MySimIIMeshNodesObject.h"
#include "MySimIIMeshBoundariesObject.h"
#include "MySimIIMeshRegionsObject.h"
#include "MySimIIMeshCreateObject.h"
#include "MySimIISolversObject.h"
#include "MySimIILocalRunObject.h"
#include "MySimIIAnalysis.h"


namespace MySimII {
class Module : public Py::ExtensionModule<Module>
{
public:
    Module() : Py::ExtensionModule<Module>("MySimII")
    {
        initialize("This module is the MySimII module."); // register with Python
    }

    virtual ~Module() {}

private:
};

PyObject* initModule()
{
    return (new Module)->module().ptr();
}
} // namespace MySimII


/* Python entry */
PyMODINIT_FUNC initMySimII() {

    // load dependent module
    try {
        Base::Interpreter().loadModule("Part");
    }
    catch(const Base::Exception& e) {
        PyErr_SetString(PyExc_ImportError, e.what());
        PyMOD_Return(0);
    }
    
    PyObject* MySimIIModule = MySimII::initModule();

    Base::Console().Log("Loading MySimII module... done\n");

    MySimII::DocumentObject                    ::init();
    MySimII::FeaturePython                     ::init();
    MySimII::MySimIIAnalysis                   ::init();
    MySimII::MySimIIAnalysisPython             ::init();
    MySimII::MySimIIMaterialsObject            ::init();
    MySimII::MySimIIMaterialsObjectPython      ::init();
    MySimII::MySimIIInitiativesObject          ::init();
    MySimII::MySimIIInitiativesObjectPython    ::init();
    MySimII::MySimIIForcesObject               ::init();
    MySimII::MySimIIForcesObjectPython         ::init();
    MySimII::MySimIIBoundariesObject           ::init();
    MySimII::MySimIIBoundariesObjectPython     ::init();
    MySimII::MySimIIMeshNodesObject            ::init();
    MySimII::MySimIIMeshNodesObjectPython      ::init();
    MySimII::MySimIIMeshBoundariesObject       ::init();
    MySimII::MySimIIMeshBoundariesObjectPython ::init();
    MySimII::MySimIIMeshRegionsObject          ::init();
    MySimII::MySimIIMeshRegionsObjectPython    ::init();
    MySimII::MySimIIMeshCreateObject           ::init();
    MySimII::MySimIIMeshCreateObjectPython     ::init();
    MySimII::MySimIISolversObject              ::init();
    MySimII::MySimIISolversObjectPython        ::init();
    MySimII::MySimIILocalRunObject             ::init();
    MySimII::MySimIILocalRunObjectPython       ::init();

    PyMOD_Return(MySimIIModule);

}
