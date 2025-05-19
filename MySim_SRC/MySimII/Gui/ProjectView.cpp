/***************************************************************************
 *   Copyright (c) 2012 Jürgen Riegel <juergen.riegel@web.de>              *
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
# include <boost/signals.hpp>
# include <boost/bind.hpp>
# include <QAction>
# include <QActionGroup>
# include <QApplication>
# include <qcursor.h>
# include <qlayout.h>
# include <qstatusbar.h>
# include <QContextMenuEvent>
# include <QMenu>
# include <QPixmap>
# include <QTimer>
#endif

//#include <QDirModel>
#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/Console.h>
#include <Base/Interpreter.h>
#include <App/Application.h>
#include <App/Document.h>
#include <App/DocumentObject.h>
#include <App/DocumentObjectPy.h>

#include <Gui/Document.h>
#include <Gui/BitmapFactory.h>
#include <Gui/ViewProviderDocumentObject.h>
#include <Gui/MenuManager.h>
#include <Gui/Application.h>
#include <Gui/MainWindow.h>
#include <Gui/Application.h>
#include <Gui/Selection.h>
#include <Gui/SelectionFilter.h>
#include <Gui/SelectionObjectPy.h>
#include <Gui/View3DInventor.h>
#include <Gui/DockWindowManager.h>

#include <QtGui/QMainWindow>
#include <qtreewidget.h>
#include <qevent.h>
#include <qmenu.h>
/*******ini********/
#include <QtCore/QCoreApplication>
#include <QSettings>
#include <QStringList>
#include <QList>
#include <QListIterator>
#include <string>
#include <Gui/Command.h>

#include <Base/Exception.h>
#include <Base/Console.h>
#include <Base/Interpreter.h>
#include <App/Application.h>
#include <App/Document.h>
#include <App/DocumentObject.h>
#include <App/DocumentObjectPy.h>
#include <vtkInteractorStyleTrackballCamera.h>

/*******ini********/
#include <QtCore/QCoreApplication>
#include <QSettings>
#include <QStringList>
#include <QList>
#include <QListIterator>
#include <string>
#include <QRegExp>
#include "ProjectView.h"

using namespace Gui;


ProjectWidget::ProjectWidget(QWidget* parent) : QTreeWidget(parent)
{

	this->setDragEnabled(true);
	this->setAcceptDrops(true);
	this->setDropIndicatorShown(false);
	this->setRootIsDecorated(false);
	this->setColumnCount(1);
	
	this->setStyleSheet(QString::fromLatin1("QTreeView::branch:has-siblings:adjoins-item {border - image: url(branch - more.png) 0;}"));

	// Add the first main label
	this->rootItem = new QTreeWidgetItem(this);
	this->rootItem->setText(0, tr("Project name"));
	ProjectObjMap["Project name"] = rootItem;
	this->rootItem->setFlags(Qt::ItemIsEnabled);
	this->setSelectionMode(QAbstractItemView::ExtendedSelection);

	Application::Instance->signalNewProjectTreeObject.connect(boost::bind(&ProjectWidget::slotNewObject, this, _1,_2));
	Application::Instance->signalDeletedObjectTreeObject.connect(boost::bind(&ProjectWidget::slotDeleteObject, this, _1,_2));

	this->header()->setResizeMode(0, QHeaderView::ResizeToContents);
	this->header()->setStretchLastSection(false);

	this->setHeaderHidden(true);

	InsertNewDesign();
	this->expandItem(this->rootItem);
	this->expandAll();

	for (int i = ProjectObjectType::MySimII; i < ProjectObjectType::Nonee; i++){
		nodeTypeNameSet.insert(GetBasicObjecLableName((ProjectObjectType)i));
	}
	

}

void ProjectWidget::InsertNewDesign(void)
{

	QTreeWidgetItem *MySimIIdesign = new QTreeWidgetItem(rootItem,
		QStringList() << QString::fromLatin1("MySimII"), ProjectObjectType::MySimII);
	ProjectObjMap["MySimII"] = MySimIIdesign;
	
	QTreeWidgetItem *Solvers = new QTreeWidgetItem(MySimIIdesign,
		QStringList() << QString::fromLatin1("Solvers"), ProjectObjectType::Solvers);
	ProjectObjMap["Solvers"] = Solvers;
	
	QTreeWidgetItem *Boundaries = new QTreeWidgetItem(MySimIIdesign,
		QStringList() << QString::fromLatin1("Boundaries"), ProjectObjectType::Boundaries);
	ProjectObjMap["Boundaries"] = Boundaries;
	
	QTreeWidgetItem *Meshes = new QTreeWidgetItem(MySimIIdesign,
		QStringList() << QString::fromLatin1("Meshes"), ProjectObjectType::Meshes);
	ProjectObjMap["Meshes"] = Meshes;

	QTreeWidgetItem *Results = new QTreeWidgetItem(MySimIIdesign,
		QStringList() << QString::fromLatin1("Results"), ProjectObjectType::Results);
	ProjectObjMap["Results"] = Results;

}


ProjectObjectType ProjectWidget::GetObjectType(const char* classname)
{
	ProjectObjectType ret = ProjectObjectType::Nonee;

	if (strcmp(classname, "MySimIIGui::Solvers") == 0){
		ret = ProjectObjectType::Solvers;
	}
	else if (strcmp(classname, "MySimIIGui::Boundaries") == 0){
		ret = ProjectObjectType::Boundaries;
	}
	else if (strcmp(classname, "MySimIIGui::Meshes") == 0){
		ret = ProjectObjectType::Meshes;
	}
	else if (strcmp(classname, "MySimIIGui::Results") == 0){
		ret = ProjectObjectType::Results;
	}
	return ret;
}


std::string ProjectWidget::GetObjectViewProvider(ProjectObjectType objType)
{
	std::string ret = "";
	switch (objType)
	{
	case ProjectObjectType::Solvers:
		ret = "MySimIIGui::ViewProviderMySimII";
		break;
	case ProjectObjectType::Boundaries:
		ret = "MySimIIGui::ViewProviderMySimII";
		break;
	case ProjectObjectType::Meshes:
		ret = "MySimIIGui::ViewProviderMySimII";
		break;
	case ProjectObjectType::Results:
		ret = "MySimIIGui::ViewProviderMySimII";
		break;
	}
	return ret;
}

std::string ProjectWidget::GetBasicObjecLableName(ProjectObjectType objType)
{
	std::string ret = "";
	switch (objType)
	{
        case ProjectObjectType::Solvers:
		ret = "Solvers";
		break;
	case ProjectObjectType::Boundaries:
		ret = "Boundaries";
		break;
	case ProjectObjectType::Meshes:
		ret = "Meshes";
		break;	
	case ProjectObjectType::Results:
		ret = "Results";
		break;	
	}
	return ret;
}


void ProjectWidget::slotNewObject(const char* classname, const char* objname)
{
	ProjectObjectType objtype = GetObjectType(classname);

	std::map<std::string, QTreeWidgetItem*>::iterator itt;

	std::string strObjname(objname);
	std::map<std::string, QTreeWidgetItem*>::iterator itobj = ProjectObjMap.find(strObjname);
	if (itobj != ProjectObjMap.end()) {
		return;
	}

	std::map<std::string, QTreeWidgetItem*>::iterator it = ProjectObjMap.find(GetBasicObjecLableName(objtype));
	if (it != ProjectObjMap.end()) {
		QTreeWidgetItem *leaf = new QTreeWidgetItem(it->second, QStringList() << QString::fromLatin1(objname), objtype);
		ProjectObjMap[strObjname] = leaf;
	}
}

void ProjectWidget::slotDeleteObject(const char* classname,const char* objname)
{
	
	ProjectObjectType objtype = GetObjectType(classname);

	std::map<std::string, QTreeWidgetItem*>::iterator it = ProjectObjMap.find(GetBasicObjecLableName(objtype));
	int childCount =it->second->childCount();
	for (int i = 0; i<childCount; i++)
	{
		QList<QTreeWidgetItem*> childItem = it->second->takeChildren();
		for (QList<QTreeWidgetItem*>::iterator jt = childItem.begin(); jt != childItem.end(); ++jt) {
			
			delete *jt;
			
		}
		
	}
	ProjectObjMap.erase(objname);

}


ProjectWidget::~ProjectWidget()
{
}

void ProjectWidget::contextMenuEvent(QContextMenuEvent * e)
{
	MenuItem view;

	QMenu contextMenu;
	QMenu subMenu;
	QMenu editMenu;
	this->editAction = new QAction(this);
	this->editAction->setText(tr("Edit"));
	this->editAction->setStatusTip(tr("Edit"));
	connect(this->editAction, SIGNAL(triggered()),
		this, SLOT(onEdit()));

	this->deleteAction = new QAction(this);
	this->deleteAction->setText(tr("Delete"));
	this->deleteAction->setStatusTip(tr("Delete"));
	connect(this->deleteAction, SIGNAL(triggered()),
		this, SLOT(onDelete()));
	
	this->contextItem = itemAt(e->pos());
	
	QString objName = this->contextItem->text(0);
	
	Gui::Application::Instance->setupContextMenu(objName.toStdString().c_str(), &view);
	MenuManager::getInstance()->setupContextMenu(&view, contextMenu);
	std::set<std::string>::iterator itobj = nodeTypeNameSet.find(objName.toStdString());
	if (!(itobj != nodeTypeNameSet.end()))
	{
		if (objName == tr("Results"))
		{
			contextMenu.addAction(this->deleteAction);
		}
		else{
			contextMenu.addAction(this->editAction);
			contextMenu.addAction(this->deleteAction);
		}
	}

	if (contextMenu.actions().count() > 0) contextMenu.exec(QCursor::pos());
}


void ProjectWidget::onEdit()
{
	QString qs = this->contextItem->text(0);
	Base::BaseClass* base;
	base = static_cast<Base::BaseClass*>(Base::Type::createInstanceByName("MySimIIGui::ViewProviderMySimII", true));
	if (base) {
		ViewProviderDocumentObject *pcProvider = static_cast<ViewProviderDocumentObject*>(base);
		QByteArray ba = qs.toLatin1();
		pcProvider->setDisplayMode(ba.data());
	}

}


void ProjectWidget::onDelete()
{
    
	QTreeWidgetItem* item = this->currentItem();

	QString qs = this->contextItem->text(0);
	ProjectObjMap.erase(qs.toStdString().c_str());

	QSettings settings(QObject::tr("../ini/MySimII-ui.ini"), QSettings::IniFormat);
	if(qs.left(4)==tr("Sol.")){
		settings.remove(QObject::tr("SolverSettings/Solver"));
	}

	delete item;
}

void ProjectWidget::mouseDoubleClickEvent(QMouseEvent * event)
{
	App::Document* docs = App::GetApplication().getActiveDocument();
	SelectionSingleton& rSel = Selection();
	rSel.rmvSelection(docs->getName());

	this->contextItem = itemAt(event->pos());
	
	QString qs = this->contextItem->text(0);
        if(qs==tr("Results")||qs==tr("Boundaries")||qs==tr("Solvers")||qs==tr("Meshes"))
	{
		Base::BaseClass* base = static_cast<Base::BaseClass*>(Base::Type::createInstanceByName("MySimIIGui::ViewProviderMySimII", true));
		ViewProviderDocumentObject *pcProvider = static_cast<ViewProviderDocumentObject*>(base);
		QByteArray ba = qs.toLatin1();
		pcProvider->setDisplayMode(ba.data());
	}

}

void ProjectWidget::Deleteitem()
{
	Gui::Application::Instance->DeleteProjectTreeObject("MySimIIGui::Solvers", "SolverSetup");
	Gui::Application::Instance->DeleteProjectTreeObject("MySimIIGui::Meshes", "MeshSetting");

}

void ProjectWidget::OpenProjecttree()
{
	QSettings settings(QObject::tr("../ini/MySimII-ui.ini"), QSettings::IniFormat);
        QString Solver = settings.value(tr("SolverSettings/SolverName")).toString();
	if( ! Solver.isEmpty() ) {
	   Gui::Application::Instance->AddProjectTreeObject("MySimIIGui::Solvers", Solver.toStdString().c_str());
        }
	int count = settings.value(tr("BSettingHead/Count")).toInt();
	if( count > 0 ) {
	   for (int ib=1; ib<=count; ib++) {
	      QString BSetting = settings.value(QObject::tr("BSettingHead/BSetting%1").arg(ib)).toString();
	      if( !BSetting.isEmpty() ) { 
		  Gui::Application::Instance->AddProjectTreeObject("MySimIIGui::Boundaries", 
		   	                                           BSetting.toStdString().c_str());
	      }
	   }
        }
}

bool ProjectWidget::StrDelete(QString strFile, QString strFlagOne, QString strFlagTwo)
{
	QString strlines = 0;
	QStringList strlist;
	QFile readfile(QObject::tr("../ini/") + strFile + tr("/ItemsVar.txt"));
	if (!readfile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		return false;
	}
	QTextStream readtextstream(&readfile);
	strlines = readtextstream.readAll();
	readfile.close();
	QFile writefile(QObject::tr("../ini/") + strFile + tr("/ItemsVar.txt"));
	if (!writefile.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		return false;
	}
	QTextStream writetextstream(&writefile);
	strlist = strlines.split(tr("\n"));
	for each (QString var in strlist)
	{
		if (var.contains(strFlagOne) || var.isEmpty())
		{
			continue;
		}
		writetextstream << tr("%1").arg(var) << endl;
	}
	writefile.close();
	QString cntstrlines = 0;
	QStringList cntstrlist;
	QFile cntreadfile(QObject::tr("../ini/") + strFile + tr("/ItemsVar.txt"));
	if (!cntreadfile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		return false;
	}
	QTextStream cntreadtextstream(&cntreadfile);
	cntstrlines = cntreadtextstream.readAll();
	cntreadfile.close();
	cntstrlist = cntstrlines.split(tr("\n"));
	int cntstr = 0;
	for each (QString var in cntstrlist)
	{		
		if (var.contains(strFlagTwo))
		{
			cntstr++;
			continue;
		}
	}
	if (cntstr == 0)
	{
		return true;
	}
	return false;
}
void ProjectWidget::StrOpenTree(QString strFile, QString strFlag)
{
	QString strlines = 0;
	QStringList strlist;
	QFile readfile(QObject::tr("../ini/") + strFile + tr("/ItemsVar.txt"));
	if (!readfile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		return;
	}
	QTextStream readtextstream(&readfile);
	strlines = readtextstream.readAll();
	readfile.close();
	strlist = strlines.split(tr("\n"));
	for each (QString var in strlist)
	{
		if (var.isEmpty())
		{
			continue;
		}		
		Gui::Application::Instance->AddProjectTreeObject("EngineGui::" + strFlag.toLatin1(), var.toLatin1());
	}
}


#include "moc_ProjectView.cpp"

