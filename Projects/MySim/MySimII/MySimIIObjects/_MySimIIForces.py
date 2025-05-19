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

__title__ = "FreeCAD MySimII mesh force layer document object"
__author__ = "Bernd Hahnebach, Qingfeng Xia"
__url__ = "http://www.freecadweb.org"



class _MySimIIForces:
    "The MySimIIForces object"
    def __init__(self, obj):
        self.Type = "MySimII::MySimIIForces"
        self.Object = obj  # keep a ref to the DocObj for nonGui usage
        obj.Proxy = self  # link between App::DocumentObject to this object

        obj.addProperty(
            "App::PropertyString",
            "ForceX",
            "CFD_Force",
            "X component of CFD force"
        )
        obj.ForceX = ""

        obj.addProperty(
            "App::PropertyString",
            "ForceY",
            "CFD_Force",
            "Y component of CFD force"
        )
        obj.ForceY = ""

        obj.addProperty(
            "App::PropertyString",
            "ForceZ",
            "CFD_Force",
            "Z component of CFD force"
        )
        obj.ForceZ = ""

        obj.addProperty(
            "App::PropertyBool",
            "Floating",
            "CFD_Force",
            "Whether to allow for floating force"
        )
        obj.Floating = False

        obj.addProperty(
            "App::PropertyString",
            "HeatSource",
            "Heat_Force",
            "Heat input per units of volume and time"
        )
        obj.HeatSource = ""

        obj.addProperty(
            "App::PropertyString",
            "TemperatureLowest",
            "Heat_Force",
            "Smart source/sink to keep lowest temperature"
        )
        obj.TemperatureLowest = ""

        obj.addProperty(
            "App::PropertyString",
            "TemperatureHighest",
            "Heat_Force",
            "Smart source/sink to keep highest temperature"
        )
        obj.TemperatureHighest = ""

        obj.addProperty(
            "App::PropertyBool",
            "SmartHeat",
            "Heat_Force",
            "Whether to activate smart source/sink"
        )
        obj.SmartHeat = False

        obj.addProperty(
            "App::PropertyString",
            "CurrentDensityRealX",
            "EM_Force",
            "X component of Real E current density"
        )
        obj.CurrentDensityRealX = ""

        obj.addProperty(
            "App::PropertyString",
            "CurrentDensityRealY",
            "EM_Force",
            "Y component of Real E current density"
        )
        obj.CurrentDensityRealY = ""

        obj.addProperty(
            "App::PropertyString",
            "CurrentDensityRealZ",
            "EM_Force",
            "Z component of Real E current density"
        )
        obj.CurrentDensityRealZ = ""

        obj.addProperty(
            "App::PropertyString",
            "CurrentDensityImagX",
            "EM_Force",
            "X component of Imag E current density"
        )
        obj.CurrentDensityImagX = ""

        obj.addProperty(
            "App::PropertyString",
            "CurrentDensityImagY",
            "EM_Force",
            "Y component of Imag E current density"
        )
        obj.CurrentDensityImagY = ""

        obj.addProperty(
            "App::PropertyString",
            "CurrentDensityImagZ",
            "EM_Force",
            "Z component of Imag E current density"
        )
        obj.CurrentDensityImagZ = ""

        obj.addProperty(
            "App::PropertyString",
            "MassForce1",
            "ElasticForce",
            "X component of mass force"
        )
        obj.MassForce1 = ""

        obj.addProperty(
            "App::PropertyString",
            "MassForce2",
            "ElasticForce",
            "Y component of mass force"
        )
        obj.MassForce2 = ""

        obj.addProperty(
            "App::PropertyString",
            "MassForce3",
            "ElasticForce",
            "Z component of mass force"
        )
        obj.MassForce3 = ""

        obj.addProperty(
            "App::PropertyString",
            "VolumeForce1",
            "ElasticForce",
            "X component of volume force"
        )
        obj.VolumeForce1 = ""

        obj.addProperty(
            "App::PropertyString",
            "VolumeForce2",
            "ElasticForce",
            "Y component of volume force"
        )
        obj.VolumeForce2 = ""

        obj.addProperty(
            "App::PropertyString",
            "VolumeForce3",
            "ElasticForce",
            "Z component of volume force"
        )
        obj.VolumeForce3 = ""

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
            "App::PropertyLinkList",
            "Subdomains",
            "Subdomain",
            "List of subdomains"
        )

    def execute(self, obj):
        return

    def __getstate__(self):
        return self.Type

    def __setstate__(self, state):
        if state:
            self.Type = state
