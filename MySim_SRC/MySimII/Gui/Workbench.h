/***************************************************************************
 *   Copyright (c) 2019 Zhenwen Wan         <aw4wzw@gmail.com>             *
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


#ifndef MySimII_WORKBENCH_H
#define MySimII_WORKBENCH_H

#include <Gui/Workbench.h>

#include <QList>
#define QSTRING(x) QString::fromLatin1(x)
extern QList<QString> mods;

//
namespace MySimIIGui {

class Workbench : public Gui::StdWorkbench
{
    TYPESYSTEM_HEADER();

public:
    Workbench();
    virtual ~Workbench();

protected:
    Gui::MenuItem* setupMenuBar() const;
    virtual Gui::ToolBarItem* setupToolBars() const;
    virtual Gui::ToolBarItem* setupCommandBars() const;
};

} // namespace MySimIIGui

#endif // Cfd WORKBENCH_H 
