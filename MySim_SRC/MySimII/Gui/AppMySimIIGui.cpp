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
#ifndef _PreComp_
# include <Python.h>
#endif

#include <Base/Console.h>
#include <Gui/MainWindow.h>
#include <Gui/MenuManager.h>
#include <Gui/ToolBarManager.h>

#include <Gui/DockWindowManager.h>
#include <Gui/Application.h>
#include <Gui/Action.h>
#include <Gui/Command.h>
#include <Gui/Selection.h>
#include <Gui/ToolBoxManager.h>

#include "Workbench.h"

#include <CXX/Extensions.hxx>
#include <CXX/Objects.hxx>
#include "ViewProviderMySimII.h"
#include "ViewProviderAnalysis.h"
#include "ViewProviderMaterials.h"
#include "ViewProviderInitiatives.h"
#include "ViewProviderBoundaries.h"
#include "ViewProviderForces.h"
#include "ViewProviderMeshNodes.h"
#include "ViewProviderMeshBoundaries.h"
#include "ViewProviderMeshRegions.h"
#include "ViewProviderMeshCreate.h"
#include "ViewProviderSolvers.h"
#include "ViewProviderLocalRun.h"
// use a different name to CreateCommand()
void CreateMySimIICommands(void);


namespace MySimIIGui {
class Module : public Py::ExtensionModule<Module>
{
public:
    Module() : Py::ExtensionModule<Module>("MySimIIGui")
    {
        initialize("This module is the MySimIIGui module."); // register with Python
    }

    virtual ~Module() {}

private:
};
} // namespace MySimIIGui


/* Python entry */
PyMODINIT_FUNC initMySimIIGui()
{
    if (!Gui::Application::Instance) {
        PyErr_SetString(PyExc_ImportError, "Cannot load Gui module in console application.");
        return;
    }

    // instanciating the commands
    CreateMySimIICommands();
    MySimIIGui::Workbench::init();

    // ADD YOUR CODE HERE
    //
    //
	MySimIIGui::ViewProviderMySimII               ::init();
	MySimIIGui::ViewProviderMySimIIAnalysis       ::init();
	MySimIIGui::ViewProviderMySimIIAnalysisPython ::init();

	MySimIIGui::ViewProviderMaterials             ::init();
	MySimIIGui::ViewProviderMaterialsPython       ::init();
	MySimIIGui::ViewProviderForces                ::init();
	MySimIIGui::ViewProviderForcesPython          ::init();
	MySimIIGui::ViewProviderInitiatives           ::init();
	MySimIIGui::ViewProviderInitiativesPython     ::init();
	MySimIIGui::ViewProviderBoundaries            ::init();
	MySimIIGui::ViewProviderBoundariesPython      ::init();
	MySimIIGui::ViewProviderMeshNodes             ::init();
	MySimIIGui::ViewProviderMeshNodesPython       ::init();
	MySimIIGui::ViewProviderMeshBoundaries        ::init();
	MySimIIGui::ViewProviderMeshBoundariesPython  ::init();
	MySimIIGui::ViewProviderMeshRegions           ::init();
	MySimIIGui::ViewProviderMeshRegionsPython     ::init();
	MySimIIGui::ViewProviderMeshCreate            ::init();
	MySimIIGui::ViewProviderMeshCreatePython      ::init();
	MySimIIGui::ViewProviderSolvers               ::init();
	MySimIIGui::ViewProviderSolversPython         ::init();
	MySimIIGui::ViewProviderLocalRun              ::init();
	MySimIIGui::ViewProviderLocalRunPython        ::init();
    new MySimIIGui::Module();
    Base::Console().Log("Loading GUI of MySimII module... done\n");
}
