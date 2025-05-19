/***************************************************************************
 *   Copyright (c) 2013 Jürgen Riegel (FreeCAD@juergen-riegel.net)         *
 *   Copyright (c) 2015 Qingfeng Xia (FreeCAD@iesensor.com)                *
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


#ifndef MySimII_MySimIIBoundariesObject_H
#define MySimII_MySimIIBoundariesObject_H

#include <App/FeaturePython.h>

namespace MySimII
{
class AppMySimIIExport MySimIIBoundariesObject : public App::DocumentObject
{
    PROPERTY_HEADER(MySimII::MySimIIBoundariesObject);

public:
    /// Constructor
    MySimIIBoundariesObject(void);
    virtual ~MySimIIBoundariesObject();

    // Attributes are implemented in the MySimIIBoundariesObjectPython

    /// returns the type name of the ViewProvider
    virtual const char* getViewProviderName(void) const {
        return "MySimIIGui::ViewProviderBoundaries";
    }
    virtual App::DocumentObjectExecReturn *execute(void) {
        return App::DocumentObject::StdReturn;
    }
    virtual short mustExecute(void) const;
    virtual PyObject *getPyObject(void);

};

typedef App::FeaturePythonT<MySimIIBoundariesObject> MySimIIBoundariesObjectPython;

} //namespace MySimII


#endif // MySimII_MySimIIBoundariesObject_H
