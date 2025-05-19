/***************************************************************************
 *   Copyright (c) 2019 Zhenwen Wan         <aw4wzw@gmail.com>             *
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
 #include <qobject.h>
#endif

#include "ProjectView.h"
#include "Workbench.h"
#include <Gui/MainWindow.h>
#include <Gui/MenuManager.h>
#include <Gui/ToolBarManager.h>

#include <Gui/DockWindowManager.h>
#include <Gui/Application.h>
#include <Gui/Action.h>
#include <Gui/Command.h>
#include <Gui/Selection.h>
#include <Gui/ToolBoxManager.h>
#include <App/Document.h>
#include <App/DocumentObject.h>

using namespace MySimIIGui;

#if 0 // needed for Qt's lupdate utility
    qApp->translate("Workbench", "Assign");
    //qApp->translate("Workbench", "Boundaries");
    //qApp->translate("Workbench", "IE_Regions");
    //qApp->translate("Workbench", "Excitations");
	//qApp->translate("Workbench", "Mesh_operation");
	//qApp->translate("Workbench", "Analysis_Setup");
    //qApp->translate("Workbench", "Optimetrics_Analysis");
    //qApp->translate("Workbench", "Fields");
    //qApp->translate("Workbench", "Radiation");
    //qApp->translate("Workbench", "Results");
    //qApp->translate("Workbench", "Boundary_Display");
	qApp->translate("Workbench", "&MySimII");
	//qApp->translate("Workbench", "Engine Tools");
	//qApp->translate("Workbench", "&Drafting");
#endif


TYPESYSTEM_SOURCE(MySimIIGui::Workbench, Gui::StdWorkbench)

Workbench::Workbench()
{

}

Workbench::~Workbench()
{
}


Gui::MenuItem* Workbench::setupMenuBar() const
{
    Gui::MenuItem* root = StdWorkbench::setupMenuBar();
    Gui::MenuItem* item = root->findItem("&Tools");

    Gui::MenuItem* prim = new Gui::MenuItem;
    prim->setCommand("Primitives");
    *prim << "Part_Box"
          << "Part_Cylinder"
          << "Part_Sphere"
          << "Part_Cone"
          << "Part_Torus"
          << "Separator"
          << "Part_MakeTube";

    Gui::MenuItem* bop = new Gui::MenuItem;
    bop->setCommand("Boolean");
    *bop << "Part_Boolean"
         << "Part_Cut"
         << "Part_Fuse"
         << "Part_Common";

    Gui::MenuItem* join = new Gui::MenuItem;
    join->setCommand("Join");
    *join << "Part_JoinConnect"
          << "Part_JoinEmbed"
          << "Part_JoinCutout";

    Gui::MenuItem* split = new Gui::MenuItem;
    split->setCommand("Split");
    *split << "Part_BooleanFragments"
           << "Part_SliceApart"
           << "Part_Slice"
           << "Part_XOR";

    Gui::MenuItem* compound = new Gui::MenuItem;
    compound->setCommand("Compound");
    *compound << "Part_Compound"
              << "Part_ExplodeCompound"
              << "Part_CompoundFilter";

    Gui::MenuItem* part = new Gui::MenuItem;
    root->insertItem(item, part);
    part->setCommand("&Geometry");
    *part << "Part_Import"
          << "Part_Export"
          << "Separator"
          << "Part_BoxSelection"
          << "Separator";
    *part << prim
          << "Part_Primitives"
          << "Part_Builder"
          << "Separator"
          //<< "Part_ShapeFromMesh"
          //<< "Part_MakeSolid"
          //<< "Part_ReverseShape"
          //<< "Part_SimpleCopy"
          //<< "Part_RefineShape"
          //<< "Part_CheckGeometry"
          //<< "Part_Defeaturing"
          //<< "Separator"
          << bop << join << split << compound
          //<< "Separator"
          //<< "Part_Section"
          //<< "Part_CrossSections"
          //<< "Part_MakeFace"
          << "Part_Extrude"
          << "Part_Revolve"
          << "Part_Mirror"
          << "Part_Fillet"
          << "Part_Chamfer"
          << "Part_RuledSurface"
          << "Part_Loft"
          << "Part_Sweep";
          //<< "Part_Offset"
          //<< "Part_Offset2D"
          //<< "Part_Thickness"
          //<< "Separator"
          //<< "Part_EditAttachment";


        //Physics
	Gui::MenuItem* Physics = new Gui::MenuItem;
        Physics->setCommand("&Physics");
       *Physics << "MySimII_Materials"
                << "MySimII_Boundaries"
                << "MySimII_Forces"
                << "MySimII_Initiatives";
        root->insertItem( item, Physics );

	//Mesh
	Gui::MenuItem* Mesh = new Gui::MenuItem;
        Mesh->setCommand("&Mesh");
       *Mesh << "MySimII_MeshBoundaries"
	     << "MySimII_MeshRegions"
	     << "MySimII_MeshCreate";
        root->insertItem( item, Mesh );

	//SimulationRun
	Gui::MenuItem* SimulationRun = new Gui::MenuItem;
        SimulationRun->setCommand("&Run");
       *SimulationRun << "MySimII_Simulation"
                      << "MySimII_Solvers"
	              << "MySimII_LocalRun"
	              << "MySimII_RemoteRun";
        root->insertItem( item, SimulationRun );

	//Results
	Gui::MenuItem* Results = new Gui::MenuItem;
	Results->setCommand("&Results");
       *Results << "MySimII_Contours" 
	        << "Separator"
                << "MySimII_Glyphs"
                << "MySimII_Animation"
	        << "Separator"
	        << "MySimII_PlotScript";
        root->insertItem( item, Results );

    return root;
}

Gui::ToolBarItem* Workbench::setupToolBars() const
{
    Gui::ToolBarItem* root = StdWorkbench::setupToolBars();

    Gui::ToolBarItem* solids = new Gui::ToolBarItem(root);
    solids->setCommand("Solids");
    *solids << "Part_Box"
            << "Part_Cylinder"
            << "Part_Sphere"
            << "Part_Cone"
            << "Part_Torus"
            << "Part_Primitives"
            << "Part_Builder";

    Gui::ToolBarItem* tool = new Gui::ToolBarItem(root);
    tool->setCommand("Part tools");
    *tool << "Part_Extrude"
          << "Part_Revolve"
          << "Part_Mirror"
          << "Part_Fillet"
          << "Part_Chamfer"
          << "Part_RuledSurface"
          << "Part_Loft"
          << "Part_Sweep"
          << "Part_CompOffset"
          << "Part_Thickness";

    Gui::ToolBarItem* boolop = new Gui::ToolBarItem(root);
    boolop->setCommand("Boolean");
    *boolop << "Part_CompCompoundTools"
            << "Part_Boolean"
            << "Part_Cut"
            << "Part_Fuse"
            << "Part_Common"
            << "Part_CompJoinFeatures"
            << "Part_CompSplitFeatures"
            << "Part_CheckGeometry"
            << "Part_Defeaturing"
            << "Part_Section"
            << "Part_CrossSections";

    return root;
}

Gui::ToolBarItem* Workbench::setupCommandBars() const
{
    Gui::ToolBarItem* root = new Gui::ToolBarItem;
    Gui::ProjectWidget::OpenProjecttree();
    return root;
}




