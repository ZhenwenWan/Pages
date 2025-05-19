/***************************************************************************
*   Copyright (c) 2013 J¨¹rgen Riegel (FreeCAD@juergen-riegel.net)         *
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
# include <boost/bind.hpp>
# include <QAction>
# include <QMenu>
#endif

#include "ViewProviderMySimII.h"
#include <Gui/Command.h>
#include <Gui/Document.h>
#include <Gui/Control.h>
#include <Gui/ActionFunction.h>
#include <Base/Console.h>
//#include <Mod/Fem/App/FemAnalysis.h>
//#include <Mod/Fem/App/FemSolverObject.h>
//#include <Mod/Fem/App/FemMeshObject.h>
//#include <Mod/Fem/App/FemSetObject.h>
//#include <Mod/Fem/App/FemConstraint.h>
#include <App/MaterialObject.h>
//#include "TaskDlgAnalysis.h"
/*******ini********/
#include <QtCore/QCoreApplication>
#include <QSettings>
#include <QStringList>
#include <QList>
#include <QListIterator>
#include <string>
#include "Boundaries.h"
#include "SolutionSetup.h"
#include "FrequencySweep.h"
#include "SolverSettings.h"
#include "InitialMesh.h"
//#include<Mod/MySimII/Gui/Boundaries.h>
//#include <Mod/Part/Gui/Material.h>

using namespace MySimIIGui;


/* TRANSLATOR FemGui::ViewProviderFemAnalysis */

PROPERTY_SOURCE(MySimIIGui::ViewProviderMySimII, Gui::ViewProviderDocumentObject)


ViewProviderMySimII::ViewProviderMySimII()
{
	sPixmap = "engine-boundaries";
}

ViewProviderMySimII::~ViewProviderMySimII()
{

}

bool ViewProviderMySimII::doubleClicked(void)
{

	return true;
}



void ViewProviderMySimII::setupContextMenu(QMenu* menu, QObject* receiver, const char* member)
{
	Gui::ActionFunction* func = new Gui::ActionFunction(menu);
	QAction* act = menu->addAction(tr("Assign"));
	//func->trigger(act, boost::bind(&ViewProviderFemAnalysis::doubleClicked, this));
}

void ViewProviderMySimII::setDisplayMode(const char* ModeName)
{
	
	
	QString str = QString(QLatin1String(ModeName));
	QString fourteen=str.left(14);
	QByteArray ba = str.toLatin1();
	
	if ((str.left(8) == tr("BSetting") == 1)){
		MySimIIGui::Boundaries::reviseinit(ba.data());
	}

	if ((str == tr("EM") == 1)){
		MySimIIGui::SolverSettings::reviseinit(ba.data());
	}
	if ((str == tr("CF") == 1)){
		MySimIIGui::SolverSettings::reviseinit(ba.data());
	}
	if ((str == tr("Heat") == 1)){
		MySimIIGui::SolverSettings::reviseinit(ba.data());
	}
	if ((str == tr("EM+Heat") == 1)){
		MySimIIGui::SolverSettings::reviseinit(ba.data());
	}
	if ((str == tr("CF+Heat") == 1)){
		MySimIIGui::SolverSettings::reviseinit(ba.data());
	}
	if ((str == tr("EM+CF+Heat") == 1)){
		MySimIIGui::SolverSettings::reviseinit(ba.data());
	}
	
	if ((str == tr("Boundaries") == 1)){
		
		static MySimIIGui::Boundaries p;
	    p.show();
	}
	
	if ((str == tr("MeshSetting") == 1)){
		
		static MySimIIGui::InitialMesh p;
	    p.show();
	}
	
}

void ViewProviderMySimII::unsetEdit(int ModNum)
{
	if (ModNum == ViewProvider::Default) {
		// when pressing ESC make sure to close the dialog
		Gui::Control().closeDialog();
	}
	else {
		Gui::ViewProviderDocumentObject::unsetEdit(ModNum);
	}
}

bool ViewProviderMySimII::onDelete(const std::vector<std::string> &)
{
	//// get the support and Sketch
	//PartDesign::Pad* pcPad = static_cast<PartDesign::Pad*>(getObject()); 
	//Sketcher::SketchObject *pcSketch = 0;
	//App::DocumentObject  *pcSupport = 0;
	//if (pcPad->Sketch.getValue()){
	//    pcSketch = static_cast<Sketcher::SketchObject*>(pcPad->Sketch.getValue()); 
	//    pcSupport = pcSketch->Support.getValue();
	//}

	//// if abort command deleted the object the support is visible again
	//if (pcSketch && Gui::Application::Instance->getViewProvider(pcSketch))
	//    Gui::Application::Instance->getViewProvider(pcSketch)->show();
	//if (pcSupport && Gui::Application::Instance->getViewProvider(pcSupport))
	//    Gui::Application::Instance->getViewProvider(pcSupport)->show();

	return true;
}

bool ViewProviderMySimII::canDragObjects() const
{
	return true;
}

bool ViewProviderMySimII::canDragObject(App::DocumentObject* obj) const
{
	if (!obj)
		return false;
	//if (obj->getTypeId().isDerivedFrom(Fem::FemMeshObject::getClassTypeId()))
	//	return true;
	//else if (obj->getTypeId().isDerivedFrom(Fem::FemSolverObject::getClassTypeId()))
	//	return true;
	//else if (obj->getTypeId().isDerivedFrom(Fem::Constraint::getClassTypeId()))
	//	return true;
	//else if (obj->getTypeId().isDerivedFrom(Fem::FemSetObject::getClassTypeId()))
	//	return true;
	//else if (obj->getTypeId().isDerivedFrom(Base::Type::fromName("Fem::FeaturePython")))
	//	return true;
	//else if (obj->getTypeId().isDerivedFrom(App::MaterialObject::getClassTypeId()))
	//	return true;
	//else
		return false;
}

void ViewProviderMySimII::dragObject(App::DocumentObject* obj)
{
	//Fem::FemAnalysis* analyze = static_cast<Fem::FemAnalysis*>(getObject());
	//std::vector<App::DocumentObject*> fem = analyze->Member.getValues();
	//for (std::vector<App::DocumentObject*>::iterator it = fem.begin(); it != fem.end(); ++it) {
	//	if (*it == obj) {
	//		fem.erase(it);
	//		analyze->Member.setValues(fem);
	//		break;
	//	}
	//}
}

bool ViewProviderMySimII::canDropObjects() const
{
	return true;
}

bool ViewProviderMySimII::canDropObject(App::DocumentObject* obj) const
{
	return canDragObject(obj);
}

void ViewProviderMySimII::dropObject(App::DocumentObject* obj)
{
	//Fem::FemAnalysis* analyze = static_cast<Fem::FemAnalysis*>(getObject());
	//std::vector<App::DocumentObject*> fem = analyze->Member.getValues();
	//fem.push_back(obj);
	//analyze->Member.setValues(fem);
}

void ViewProviderMySimII::updateData(const App::Property* p)
{

}

std::vector<std::string> ViewProviderMySimII::getDisplayModes(void)  const
{
	std::vector<std::string> ret;
	return ret;

}

// Python feature -----------------------------------------------------------------------

//namespace Gui {
//	/// @cond DOXERR
//	PROPERTY_SOURCE_TEMPLATE(FemGui::ViewProviderFemAnalysisPython, FemGui::ViewProviderFemAnalysis)
//		/// @endcond
//
//		// explicit template instantiation
//		template class FemGuiExport ViewProviderPythonFeatureT<ViewProviderFemAnalysis>;
//}
