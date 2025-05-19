# ***************************************************************************
# *                                                                         *
# *   Copyright (c) 2016 Bernd Hahnebach <bernd@bimstatik.org>              *
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

__title__ = "FreeCAD MySimII mesh gmsh document object"
__author__ = "Bernd Hahnebach"
__url__ = "http://www.freecadweb.org"

## @package MySimIILocalRun
#  \ingroup MySimII
#  \brief FreeCAD MySimII _MySimIILocalRun
import FreeCAD

class _MySimIILocalRun():
    """A MySimII::MySimIIMeshObject python type, add Gmsh specific properties
    """

    def __init__(self, obj):
        self.Type = "MySimII::MySimIILocalRun"
        self.Object = obj  # keep a ref to the DocObj for nonGui usage
        obj.Proxy = self  # link between App::DocumentObject to this object

        obj.addProperty(
            "App::PropertyLinkList",
            "BoundaryList",
            "Base",
            "Selecting boundaries"
        )
        obj.BoundaryList = []

        obj.addProperty(
            "App::PropertyLinkList",
            "MaterialList",
            "Base",
            "Selecting materials"
        )
        obj.MaterialList = []

        obj.addProperty(
            "App::PropertyLinkList",
            "ForcingList",
            "Base",
            "Selecting forcing conditions"
        )
        obj.ForcingList = []

        obj.addProperty(
            "App::PropertyLinkList",
            "InitialList",
            "Base",
            "Selecting initial conditions"
        )
        obj.InitialList = []

        obj.addProperty(
            "App::PropertyLinkList",
            "Group",
            "Base",
            "All linker list"
        )
        obj.Group = []

        obj.addProperty(
            "App::PropertyLink",
            "Simulation",
            "Base",
            "Simulation linker"
        )
        obj.Simulation = None
        
        obj.addProperty(
            "App::PropertyLinkList",
            "SolverList",
            "Base",
            "Solvers linker"
        )
        obj.SolverList = []
        
        obj.addProperty(
            "App::PropertyLink",
            "Mesh",
            "Base",
            "Mesh linker"
        )
        obj.Mesh = None

        obj.addProperty(
            "App::PropertyStringList",
            "MappingList",
            "Base",
            "Mapping solvers and materials"
        )
        obj.MappingList = []

    def execute(self, obj):
        return

    def __getstate__(self):
        return self.Type

    def __setstate__(self, state):
        if state:
            self.Type = state
