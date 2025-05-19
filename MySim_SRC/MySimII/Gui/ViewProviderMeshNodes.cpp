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

#include "ViewProviderMeshNodes.h"
#include <Gui/Command.h>
#include <Gui/Document.h>
#include <Gui/Control.h>


using namespace MySimIIGui;



PROPERTY_SOURCE(MySimIIGui::ViewProviderMeshNodes, Gui::ViewProviderDocumentObject)


ViewProviderMeshNodes::ViewProviderMeshNodes()
{
    sPixmap = "MySimII-MeshNodes";
}

ViewProviderMeshNodes::~ViewProviderMeshNodes()
{

}

std::vector<std::string> ViewProviderMeshNodes::getDisplayModes(void) const
{
    return { "MeshNodes" };
}



// Python feature -----------------------------------------------------------------------

namespace Gui {
/// @cond DOXERR
PROPERTY_SOURCE_TEMPLATE(MySimIIGui::ViewProviderMeshNodesPython, MySimIIGui::ViewProviderMeshNodes)
/// @endcond

// explicit template instantiation
template class MySimIIGuiExport ViewProviderPythonFeatureT<ViewProviderMeshNodes>;
}
