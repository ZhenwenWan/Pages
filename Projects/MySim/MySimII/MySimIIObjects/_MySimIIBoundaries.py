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

__title__ = "FreeCAD MySimII mesh boundary layer document object"
__author__ = "Bernd Hahnebach, Qingfeng Xia"
__url__ = "http://www.freecadweb.org"

## @package MySimIIBoundaries
#  \ingroup MySimII
#  \brief MySimII mesh boundary layer object

EMList = ["Empty", "PEC", "Rad.", "WavePort", "LumpedPort", "Inc.Wave"]

class _MySimIIBoundaries:
    "The MySimIIBoundaries object"
    def __init__(self, obj):
        self.Type = "MySimII::MySimIIBoundaries"
        self.Object = obj  # keep a ref to the DocObj for nonGui usage
        obj.Proxy = self  # link between App::DocumentObject to this object

        obj.addProperty("App::PropertyString","Dirichlet","General_Boundary","general boundary condition")
        obj.addProperty("App::PropertyString","Neumann","General_Boundary","general boundary condition")
        obj.addProperty("App::PropertyString","RobinCoef","General_Boundary","general boundary condition")
        obj.addProperty("App::PropertyString","RobinExt","General_Boundary","general boundary condition")
        obj.addProperty("App::PropertyBool","NTCoordG","General_Boundary", "Whether Normal-Tangent coordinate")
        obj.Dirichlet = ""
        obj.Neumann = ""
        obj.RobinCoef = ""
        obj.RobinExt = ""
        obj.NTCoordG = False

        obj.addProperty("App::PropertyString","DirichletF","General_TypeF","general BC typeF")
        obj.addProperty("App::PropertyString",  "NeumannF","General_TypeF","general BC typeF")
        obj.addProperty("App::PropertyString","RobinCoefF","General_TypeF","general BC typeF")
        obj.addProperty("App::PropertyString", "RobinExtF","General_TypeF","general BC typeF")
        obj.addProperty("App::PropertyBool","NTCoordF","General_TypeF", "Whether Normal-Tangent coordinate")
        obj.DirichletF = ""
        obj.NeumannF   = ""
        obj.RobinCoefF = ""
        obj.RobinExtF  = ""
        obj.NTCoordF = False

        obj.addProperty(
            "App::PropertyString",
            "Vel",
            "CFD_Boundary",
            "CFD boundary velocity"
        )
        obj.Vel = ""

        obj.addProperty(
            "App::PropertyString",
            "Pressure",
            "CFD_Boundary",
            "Pressure of CFD boundary velocity"
        )
        obj.Pressure = ""

        obj.addProperty(
            "App::PropertyString",
            "SlipCoeff",
            "CFD_Boundary",
            "Slip coefficient of CFD boundary"
        )
        obj.SlipCoeff = ""

        obj.addProperty(
            "App::PropertyString",
            "HeatFlux",
            "Heat_Boundary",
            "Heat flux"
        )
        obj.HeatFlux = ""

        obj.addProperty(
            "App::PropertyString",
            "Temperature",
            "Heat_Boundary",
            "Temperature"
        )
        obj.Temperature = ""

        obj.addProperty(
            "App::PropertyString",
            "IncWave",
            "EM_Boundary",
            "EM field of incident wave"
        )
        obj.IncWave = ""

        obj.addProperty(
            "App::PropertyString",
            "IncProp",
            "EM_Boundary",
            "EM propogation of incident wave"
        )
        obj.IncProp = ""

        obj.addProperty(
            "App::PropertyString",
            "NormalForce",
            "Elastic_Boundary",
            "Normal force"
        )
        obj.NormalForce = ""

        obj.addProperty(
            "App::PropertyString",
            "Force",
            "Elastic_Boundary",
            "Force"
        )
        obj.Force = ""

        obj.addProperty(
            "App::PropertyString",
            "Displacement",
            "Elastic_Boundary",
            "Displacement"
        )
        obj.Displacement = ""

        obj.addProperty(
            "App::PropertyString",
            "MeshUpdate",
            "Elastic_Boundary",
            "MeshUpdate"
        )
        obj.MeshUpdate = ""

        obj.addProperty(
            "App::PropertyBool",
            "NTCoord",
            "CFD_Boundary",
            "Whether to use local Normal-Tangent coordinate"
        )
        obj.NTCoord = False

        obj.addProperty(
            "App::PropertyBool",
            "FSI",
            "CFD_Boundary",
            "Fluid-solid interaction"
        )
        obj.FSI = False

        obj.addProperty(
            "App::PropertyBool",
            "NoSlipWall",
            "CFD_Boundary",
            "No slip wall BC"
        )
        obj.NoSlipWall = False

        obj.addProperty(
            "App::PropertyBool",
            "FreeSurf",
            "CFD_Boundary",
            "Whether to impose free surface"
        )
        obj.NTCoord = False

        obj.addProperty(
            "App::PropertyBool",
            "Wall",
            "CFD_Boundary",
            "Whether to impose a wall"
        )
        obj.Wall = False

        obj.addProperty(
            "App::PropertyStringList",
            "ComB_EMList",
            "EM_Boundary",
            "ItemText list of EM boundary condition"
        )
        obj.ComB_EMList = EMList
    
        obj.addProperty(
            "App::PropertyString",
            "ComB_EM",
            "EM_Boundary",
            "current ItemText of EM boundary condition"
        )
        obj.ComB_EM = obj.ComB_EMList[0]
        
        obj.addProperty(
            "App::PropertyString",
            "SelfDef1",
            "UserDefinition",
            "text to included into Elmer SIF"
        )
        obj.SelfDef1 = ""
        
        obj.addProperty(
            "App::PropertyString",
            "SelfDef2",
            "UserDefinition",
            "text to included into Elmer SIF"
        )
        obj.SelfDef2 = ""
        
        obj.addProperty(
            "App::PropertyString",
            "ConstraintType",
            "Constraints",
            "boundary constraint type"
        )
        obj.ConstraintType = "Empty"
        
        obj.addProperty(
            "App::PropertyLinkSubList",
            "References",
            "BoundariesShapes",
            "List of boundary shape elements"
        )

    def execute(self, obj):
        return

    def __getstate__(self):
        return self.Type

    def __setstate__(self, state):
        if state:
            self.Type = state
