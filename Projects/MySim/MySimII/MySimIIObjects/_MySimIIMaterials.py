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

## @package MySimIIMaterials
#  \ingroup MySimII

class _MySimIIMaterials:
    "The MySimIIMaterials object"
    def __init__(self, obj):
        self.Type = "MySimII::MySimIIMaterial"
        self.Object = obj  # keep a ref to the DocObj for nonGui usage
        obj.Proxy = self  # link between App::DocumentObject to this object

        obj.addProperty("App::PropertyString","CForce","Coriolis_Force","Coriolis force")
        obj.addProperty("App::PropertyString","RForce","Rest_Force","Rest force")
        obj.addProperty("App::PropertyString","TDepth","Topography_Depth","Topography depth")
        obj.addProperty("App::PropertyString","BFriction","Bottom_friction","Bottom friction")
        obj.CForce = ""
        obj.RForce = ""
        obj.TDepth = ""
        obj.BFriction = ""

        obj.addProperty("App::PropertyString","MaterialType","General","Material type")
        obj.MaterialType = "Heat"

        obj.addProperty("App::PropertyString","Density","General","Density")
        obj.Density = ""

        obj.addProperty("App::PropertyString","HeatConduct","General","Heat conductivity")
        obj.HeatConduct = ""

        obj.addProperty("App::PropertyString","HeatExpansion","General","Heat expansion coeff")
        obj.HeatExpansion = ""

        obj.addProperty("App::PropertyString","RefTemperature","Heat","Reference temperature")
        obj.RefTemperature = ""

        obj.addProperty("App::PropertyString","SpecHeatRatio","Heat","Specific heat ratio")
        obj.SpecHeatRatio = ""

        obj.addProperty("App::PropertyString","HeatCapacity","General","Specific heat ratio")
        obj.HeatCapacity = ""

        obj.addProperty("App::PropertyString","Viscosity","CFD","Fluid properties")
        obj.Viscosity = ""

        obj.addProperty("App::PropertyString","RefPressure","CFD","Fluid properties")
        obj.RefPressure = ""

        obj.addProperty("App::PropertyString","CompressModel","CFD","Compressibility model")
        obj.CompressModel = "Empty"

        obj.addProperty("App::PropertyString","RelativePermittivity","EM fields","Relative permittivity")
        obj.RelativePermittivity = ""

        obj.addProperty("App::PropertyString","YoungsModulus","Elastic","Youngs Modulus")
        obj.YoungsModulus = ""

        obj.addProperty("App::PropertyString","PoissonRatio","Elastic","Poisson ratio")
        obj.PoissonRatio = ""

        obj.addProperty("App::PropertyString","MeshElasticModulus","CFD","FSI Youngs Modulus")
        obj.MeshElasticModulus = ""

        obj.addProperty("App::PropertyString","MeshPoissonRatio","CFD","FSI Poisson ratio")
        obj.MeshPoissonRatio = ""

        obj.addProperty(
            "App::PropertyLinkSubList",
            "References",
            "MaterialsShapes",
            "Shapes to impose the same materials"
        )

    def execute(self, obj):
        return

    def __getstate__(self):
        return self.Type

    def __setstate__(self, state):
        if state:
            self.Type = state
