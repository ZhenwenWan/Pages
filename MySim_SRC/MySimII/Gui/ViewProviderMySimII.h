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


#ifndef MYSIM_ViewProviderMySimII_H
#define MYSIM_ViewProviderMySimII_H

#include <Gui/ViewProviderDocumentObject.h>
#include <Gui/ViewProviderPythonFeature.h>
#include <QCoreApplication>
//#include <Mod/Part/Gui/Material.h>
namespace MySimIIGui
{

	class MySimIIGuiExport ViewProviderMySimII : public Gui::ViewProviderDocumentObject
	{
		Q_DECLARE_TR_FUNCTIONS(MySimIIGui::ViewProviderViewProviderMySimII)
		PROPERTY_HEADER(MySimIIGui::ViewProviderViewProviderMySimII);

	public:
		/// constructor.
		ViewProviderMySimII();

		/// destructor.
		virtual ~ViewProviderMySimII();

		virtual bool doubleClicked(void);

		//virtual std::vector<App::DocumentObject*> claimChildren(void)const;

		//virtual std::vector<App::DocumentObject*> claimChildren3D(void)const;
		void setupContextMenu(QMenu*, QObject*, const char*);

		virtual bool onDelete(const std::vector<std::string> &);

		// shows solid in the tree
		virtual bool isShow(void) const{ return true; }

		/** @name Drag and drop */
		//@{
		/// Returns true if the view provider generally supports dragging objects
		bool canDragObjects() const;
		/// Check whether the object can be removed from the view provider by drag and drop
		bool canDragObject(App::DocumentObject*) const;
		/// Starts to drag the object
		void dragObject(App::DocumentObject*);
		/// Returns true if the view provider generally accepts dropping of objects
		bool canDropObjects() const;
		/// Check whether the object can be dropped to the view provider by drag and drop
		bool canDropObject(App::DocumentObject*) const;
		/// If the dropped object type is accepted the object will be added as child
		void dropObject(App::DocumentObject*);
		//@}
		virtual void setDisplayMode(const char* ModeName);

		void updateData(const App::Property*);
		std::vector<std::string> getDisplayModes(void) const;

	protected:
	//	virtual bool setEdit(int ModNum);
		virtual void unsetEdit(int ModNum);

	};

	//typedef Gui::ViewProviderPythonFeatureT<ViewProviderFemAnalysis> ViewProviderFemAnalysisPython;

} //namespace EngineGui


#endif // ENGINE_ViewProviderBoundaries_H
