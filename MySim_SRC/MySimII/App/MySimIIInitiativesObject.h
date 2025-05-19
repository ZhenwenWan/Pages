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


#ifndef MySimII_MySimIIInitiativesObject_H
#define MySimII_MySimIIInitiativesObject_H

#include <App/FeaturePython.h>

namespace MySimII
{
class AppMySimIIExport MySimIIInitiativesObject : public App::DocumentObject
{
    PROPERTY_HEADER(MySimII::MySimIIInitiativesObject);

public:
    /// Constructor
    MySimIIInitiativesObject(void);
    virtual ~MySimIIInitiativesObject();

    // Attributes are implemented in the MySimIIInitiativesObjectPython

    /// returns the type name of the ViewProvider
    virtual const char* getViewProviderName(void) const {
        return "MySimIIGui::ViewProviderInitiatives";
    }
    virtual App::DocumentObjectExecReturn *execute(void) {
        return App::DocumentObject::StdReturn;
    }
    virtual short mustExecute(void) const;
    virtual PyObject *getPyObject(void);

};

typedef App::FeaturePythonT<MySimIIInitiativesObject> MySimIIInitiativesObjectPython;

} //namespace MySimII


#endif // MySimII_MySimIIInitiativesObject_H
