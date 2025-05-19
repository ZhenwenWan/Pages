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
#endif

#include "MySimIIAnalysis.h"
#include <App/DocumentObjectPy.h>
#include <App/FeaturePythonPyImp.h>
#include <Base/Placement.h>
#include <Base/Uuid.h>

using namespace MySimII;
using namespace App;

PROPERTY_SOURCE(MySimII::MySimIIAnalysis, App::DocumentObjectGroup)


MySimIIAnalysis::MySimIIAnalysis()
{
    Base::Uuid id;
    ADD_PROPERTY_TYPE(Uid,(id),0,App::Prop_None,"UUID of the Analysis");
}

MySimIIAnalysis::~MySimIIAnalysis()
{
}

short MySimIIAnalysis::mustExecute(void) const
{
    return 0;
}

PyObject *MySimIIAnalysis::getPyObject()
{
    return App::DocumentObjectGroup::getPyObject();
}

void MySimIIAnalysis::onChanged(const Property* prop)
{
    App::DocumentObjectGroup::onChanged(prop);
}

void MySimIIAnalysis::handleChangedPropertyName(Base::XMLReader &reader,
                                            const char * TypeName,
                                            const char *PropName)
{
    Base::Type type = Base::Type::fromName(TypeName);
    if (Group.getClassTypeId() == type && strcmp(PropName, "Member") == 0) {
        Group.Restore(reader);
    }
    else {
        App::DocumentObjectGroup::handleChangedPropertyName(reader, TypeName, PropName);
    }
}


// Dummy class 'DocumentObject' in MySimII namespace
PROPERTY_SOURCE_ABSTRACT(MySimII::DocumentObject, App::DocumentObject)

// Python feature ---------------------------------------------------------

namespace App {
/// @cond DOXERR
PROPERTY_SOURCE_TEMPLATE(MySimII::MySimIIAnalysisPython, MySimII::MySimIIAnalysis)
template<> const char* MySimII::MySimIIAnalysisPython::getViewProviderName(void) const {
    return "MySimIIGui::ViewProviderMySimIIAnalysisPython";
}

template<> void MySimII::MySimIIAnalysisPython::Restore(Base::XMLReader& reader) {
    MySimIIAnalysis::Restore(reader);
}
//template<> PyObject* MySimII::MySimIIAnalysisPython::getPyObject(void) {
//    if (PythonObject.is(Py::_None())) {
//        // ref counter is set to 1
//        PythonObject = Py::Object(new App::DocumentObjectPy(this),true);
//    }
//    return Py::new_reference_to(PythonObject);
//}
/// @endcond

// explicit template instantiation
template class AppMySimIIExport FeaturePythonT<MySimII::MySimIIAnalysis>;
}

// ---------------------------------------------------------

namespace App {
/// @cond DOXERR
PROPERTY_SOURCE_TEMPLATE(MySimII::FeaturePython, MySimII::DocumentObject)
template<> const char* MySimII::FeaturePython::getViewProviderName(void) const {
    return "Gui::ViewProviderPythonFeature";
}
template<> PyObject* MySimII::FeaturePython::getPyObject(void) {
    if (PythonObject.is(Py::_None())) {
        // ref counter is set to 1
        PythonObject = Py::Object(new App::FeaturePythonPyT<App::DocumentObjectPy>(this),true);
    }
    return Py::new_reference_to(PythonObject);
}
// explicit template instantiation
template class AppMySimIIExport FeaturePythonT<MySimII::DocumentObject>;
/// @endcond
}
