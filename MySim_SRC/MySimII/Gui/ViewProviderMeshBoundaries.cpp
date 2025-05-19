/***************************************************************************
 *   Copyright (c) 2013 Jürgen Riegel (FreeCAD@juergen-riegel.net)         *
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
# include <Standard_math.hxx>

#endif

#include "ViewProviderMeshBoundaries.h"
#include <Gui/Command.h>
#include <Gui/Document.h>
#include <Gui/Control.h>


using namespace MySimIIGui;



PROPERTY_SOURCE(MySimIIGui::ViewProviderMeshBoundaries, Gui::ViewProviderDocumentObject)


ViewProviderMeshBoundaries::ViewProviderMeshBoundaries()
{
    sPixmap = "MySimII-MeshBoundaries";
}

ViewProviderMeshBoundaries::~ViewProviderMeshBoundaries()
{

}

std::vector<std::string> ViewProviderMeshBoundaries::getDisplayModes(void) const
{
    return { "MeshBoundaries" };
}



// Python feature -----------------------------------------------------------------------

namespace Gui {
/// @cond DOXERR
PROPERTY_SOURCE_TEMPLATE(MySimIIGui::ViewProviderMeshBoundariesPython, MySimIIGui::ViewProviderMeshBoundaries)
/// @endcond

// explicit template instantiation
template class MySimIIGuiExport ViewProviderPythonFeatureT<ViewProviderMeshBoundaries>;
}
