# MySimII gui init module
# (c) 2009 Juergen Riegel
#
# Gathering all the information to start FreeCAD
# This is the second one of three init scripts, the third one
# runs when the gui is up

# ***************************************************************************
# *   (c) Juergen Riegel (juergen.riegel@web.de) 2009                       *
# *                                                                         *
# *   This file is part of the FreeCAD CAx development system.              *
# *                                                                         *
# *   This program is free software; you can redistribute it and/or modify  *
# *   it under the terms of the GNU Lesser General Public License (LGPL)    *
# *   as published by the Free Software Foundation; either version 2 of     *
# *   the License, or (at your option) any later version.                   *
# *   for detail see the LICENCE text file.                                 *
# *                                                                         *
# *   FreeCAD is distributed in the hope that it will be useful,            *
# *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
# *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
# *   GNU Lesser General Public License for more details.                   *
# *                                                                         *
# *   You should have received a copy of the GNU Library General Public     *
# *   License along with FreeCAD; if not, write to the Free Software        *
# *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
# *   USA                                                                   *
# *                                                                         *
# *   Juergen Riegel 2002                                                   *
# ***************************************************************************/

import FreeCAD
import FreeCADGui


class MySimIIWorkbench(Workbench):
    "MySimII workbench object"

    def __init__(self):
        self.__class__.Icon = FreeCAD.getResourceDir() + "Mod/MySimII/Resources/icons/MySimIIWorkbench.svg"
        self.__class__.MenuText = "MySimII"
        self.__class__.ToolTip = "MySimII workbench"

    def Initialize(self):
        # load the module
        import MySimII
        import MySimIIGui
        import MySimIIcommands.commands
        import PartGui
        import BasicShapes.Shapes
        import CompoundTools._CommandCompoundFilter
        import CompoundTools._CommandExplodeCompound
        bop = __import__("BOPTools")
        bop.importAll()
        bop.addCommands()
        PartGui.BOPTools = bop

    def GetClassName(self):
        return "MySimIIGui::Workbench"


FreeCADGui.addWorkbench(MySimIIWorkbench())
