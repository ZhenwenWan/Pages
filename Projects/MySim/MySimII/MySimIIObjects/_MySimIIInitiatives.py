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

__title__ = "FreeCAD MySimII mesh initiative layer document object"
__author__ = "Bernd Hahnebach, Qingfeng Xia"
__url__ = "http://www.freecadweb.org"



class _MySimIIInitiatives:
    "The MySimIIInitiatives object"
    def __init__(self, obj):
        self.Type = "MySimII::MySimIIInitiatives"
        self.Object = obj  # keep a ref to the DocObj for nonGui usage
        obj.Proxy = self  # link between App::DocumentObject to this object

        obj.addProperty(
            "App::PropertyString",
            "VelX",
            "CFD_Initiative",
            "X component of CFD initiative velocity"
        )
        obj.VelX = ""

        obj.addProperty(
            "App::PropertyString",
            "VelY",
            "CFD_Initiative",
            "Y component of CFD initiative velocity"
        )
        obj.VelY = ""

        obj.addProperty(
            "App::PropertyString",
            "VelZ",
            "CFD_Initiative",
            "Z component of CFD initiative velocity"
        )
        obj.VelZ = ""

        obj.addProperty(
            "App::PropertyString",
            "Pressure",
            "CFD_Initiative",
            "Pressure of CFD initiative velocity"
        )
        obj.Pressure = ""

        obj.addProperty(
            "App::PropertyString",
            "Temperature",
            "Heat_Initiative",
            "Temperature"
        )
        obj.Temperature = ""

        obj.addProperty(
            "App::PropertyString",
            "E_X",
            "EM_Initiative",
            "X component of E field of EM wave"
        )
        obj.E_X = ""

        obj.addProperty(
            "App::PropertyString",
            "E_Y",
            "EM_Initiative",
            "Y component of E field of EM wave"
        )
        obj.E_Y = ""

        obj.addProperty(
            "App::PropertyString",
            "E_Z",
            "EM_Initiative",
            "Z component of E field of EM wave"
        )
        obj.E_Z = ""

        obj.addProperty(
            "App::PropertyString",
            "M_X",
            "EM_Initiative",
            "X component of M field of EM wave"
        )
        obj.M_X = ""

        obj.addProperty(
            "App::PropertyString",
            "M_Y",
            "EM_Initiative",
            "Y component of M field of EM wave"
        )
        obj.M_Y = ""

        obj.addProperty(
            "App::PropertyString",
            "M_Z",
            "EM_Initiative",
            "Z component of M field of EM wave"
        )
        obj.M_Z = ""

        obj.addProperty(
            "App::PropertyString",
            "Force1",
            "Elastic_Initiative",
            "Force component 1"
        )
        obj.Force1 = ""

        obj.addProperty(
            "App::PropertyString",
            "Force2",
            "Elastic_Initiative",
            "Force component 2"
        )
        obj.Force2 = ""

        obj.addProperty(
            "App::PropertyString",
            "Force3",
            "Elastic_Initiative",
            "Force component 3"
        )
        obj.Force3 = ""

        obj.addProperty(
            "App::PropertyString",
            "Displacement1",
            "Elastic_Initiative",
            "Displacement component 1"
        )
        obj.Displacement1 = ""

        obj.addProperty(
            "App::PropertyString",
            "Displacement2",
            "Elastic_Initiative",
            "Displacement component 2"
        )
        obj.Displacement2 = ""

        obj.addProperty(
            "App::PropertyString",
            "Displacement3",
            "Elastic_Initiative",
            "Displacement component 3"
        )
        obj.Displacement3 = ""

        obj.addProperty(
            "App::PropertyString",
            "MeshUpdate1",
            "Elastic_Initiative",
            "MeshUpdate component 1"
        )
        obj.MeshUpdate1 = ""

        obj.addProperty(
            "App::PropertyString",
            "MeshUpdate2",
            "Elastic_Initiative",
            "MeshUpdate component 2"
        )
        obj.MeshUpdate2 = ""

        obj.addProperty(
            "App::PropertyString",
            "MeshUpdate3",
            "Elastic_Initiative",
            "MeshUpdate component 3"
        )
        obj.MeshUpdate3 = ""

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
