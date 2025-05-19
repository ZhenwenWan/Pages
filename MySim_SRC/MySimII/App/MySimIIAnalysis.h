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


#ifndef MySimII_MySimIIAnalysis_H
#define MySimII_MySimIIAnalysis_H


#include <App/DocumentObjectGroup.h>
#include <App/PropertyLinks.h>
#include <App/FeaturePython.h>



namespace MySimII
{

class AppMySimIIExport MySimIIAnalysis : public App::DocumentObjectGroup
{
    PROPERTY_HEADER(MySimII::MySimIIAnalysis);

public:
    /// Constructor
    MySimIIAnalysis(void);
    virtual ~MySimIIAnalysis();

    /// returns the type name of the ViewProvider
    virtual const char* getViewProviderName(void) const {
        return "MySimIIGui::ViewProviderMySimIIAnalysis";
    }
    virtual App::DocumentObjectExecReturn *execute(void) {
        return App::DocumentObject::StdReturn;
    }
    virtual short mustExecute(void) const;
    virtual PyObject *getPyObject(void);

    /// unique identifier of the Analysis
    App::PropertyUUID    Uid;


protected:
    /// get called by the container when a property has changed
    virtual void onChanged (const App::Property* prop);
    /// Support of backward compatibility
    virtual void handleChangedPropertyName(Base::XMLReader &reader,
                                           const char * TypeName,
                                           const char *PropName);
};

class AppMySimIIExport DocumentObject : public App::DocumentObject
{
    PROPERTY_HEADER(MySimII::DocumentObject);
};

typedef App::FeaturePythonT<MySimIIAnalysis> MySimIIAnalysisPython;
typedef App::FeaturePythonT<DocumentObject> FeaturePython;


} //namespace MySimII


#endif // MySimII_MySimIIAnalysis_H
