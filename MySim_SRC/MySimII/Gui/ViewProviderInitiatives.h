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


#ifndef MySimII_ViewProviderInitiatives_H
#define MySimII_ViewProviderInitiatives_H

#include <Gui/ViewProviderGeometryObject.h>
#include <Gui/ViewProviderBuilder.h>
#include <Gui/ViewProviderPythonFeature.h>

class SoCoordinate3;
class SoDrawStyle;
class SoIndexedFaceSet;
class SoIndexedLineSet;
class SoShapeHints;
class SoMaterialBinding;

namespace MySimIIGui
{



class MySimIIGuiExport ViewProviderInitiatives : public Gui::ViewProviderDocumentObject
{
    PROPERTY_HEADER(MySimIIGui::ViewProviderInitiatives);

public:
    /// constructor
    ViewProviderInitiatives();

    /// destructor
    ~ViewProviderInitiatives();

    // shows solid in the tree
    virtual bool isShow(void) const {
        return Visibility.getValue();
    }
    /// A list of all possible display modes
    virtual std::vector<std::string> getDisplayModes(void) const;
};

typedef Gui::ViewProviderPythonFeatureT<ViewProviderInitiatives> ViewProviderInitiativesPython;

} //namespace MySimIIGui


#endif // MySimII_ViewProviderInitiatives_H
