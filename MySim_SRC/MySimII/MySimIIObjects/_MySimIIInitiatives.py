# ***************************************************************************
# *                                                                         *
# *   Copyright (c) 2015 Bernd Hahnebach <bernd@bimstatik.org>              *
# *                                                                         *
# *   This program is free software; you can redistribute it and/or modify  *
# *   it under the terms of the GNU Lesser General Public License (LGPL)    *
# *   as published by the Free Software Foundation; either version 2 of     *
# *   the License, or (at your option) any later version.                   *
# *   for detail see the LICENCE text file.                                 *
# *                                                                         *
# *   This program is distributed in the hope that it will be useful,       *
# *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
# *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
# *   GNU Library General Public License for more details.                  *
# *                                                                         *
# *   You should have received a copy of the GNU Library General Public     *
# *   License along with this program; if not, write to the Free Software   *
# *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
# *   USA                                                                   *
# *                                                                         *
# ***************************************************************************

__title__ = "FreeCAD MySimII Properties document object"
__author__ = "Bernd Hahnebach, Zhenwen Wan"
__url__ = "http://www.freecadweb.org"

## @package MySimIIInitiatives
#  \ingroup MySimII
#  \brief FreeCAD MySimII _MySimIIInitiatives

import FreeCAD


class _MySimIIInitiatives():
    """The MySimII::MySimIIInitiatives's Proxy python type, add properties definition
    """

    InitiativeNames = ["Gravity", "EM force", "Coriolis", "Floating"]
    Units = ["N/kg", "V/m", "m/s'2", "N/kg"]

    def __init__(self, obj):
        self.Type = "MySimII::MySimIIInitiatives"
        self.Object = obj  # keep a ref to the DocObj for nonGui usage
        obj.Proxy = self  # link between App::DocumentObject to  this object

        dir_MS = FreeCAD.ParamGet("User parameter:BaseApp/Preferences/Mod/MySimII")

        obj.addProperty(
            "App::PropertyEnumeration",
            "InitiativeName",
            "PhysicalProperty",
            "The name of physical property"
        )
        obj.InitiativeName = _MySimIIInitiatives.InitiativeNames
        obj.InitiativeName = "EM force"

        obj.addProperty(
            "App::PropertyEnumeration",
            "Unit",
            "PhysicalProperty",
            "The unit of physical property"
        )
        obj.Unit = _MySimIIInitiatives.Units
        obj.Unit = "V/m"

        obj.addProperty(
            "App::PropertyLength",
            "MainValue",
            "PhysicalProperty",
            "The value for mainbody"
        )
        obj.MainValue = 0.0

        obj.addProperty(
            "App::PropertyLength",
            "ExtraValue",
            "PhysicalProperty",
            "The value for extra parts"
        )
        obj.ExtraValue = 0.0

        obj.addProperty(
            "App::PropertyLink",
            "Mainbody",
            "Geometry",
            "The main geometrical object -- Model"
        )
        obj.Mainbody = None

        obj.addProperty(
            "App::PropertyLinkList",
            "ExtraParts",
            "Geometry",
            "The extra geometrical objects "
        )
        obj.ExtraParts = []

        obj.addProperty(
            "App::PropertyLinkSubList",
            "References",
            "Geometry",
            "The extra geometrical objects "
        )

    def execute(self, obj):
        return

    def __getstate__(self):
        return self.Type

    def __setstate__(self, state):
        if state:
            self.Type = state
