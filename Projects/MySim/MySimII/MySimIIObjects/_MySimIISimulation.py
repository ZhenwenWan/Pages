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

__title__  = "FreeCAD MySimII Simulation document object"
__author__ = "Bernd Hahnebach, Qingfeng Xia, Zhenwen Wan"
__url__    = "http://www.freecadweb.org"

class _MySimIISimulation:
    "The MySimIISimulation object"
    def __init__(self, obj):
        self.Type   = "Fem::FemSolverObjectPython"
        self.Object = obj  # keep a ref to the DocObj for nonGui usage
        obj.Proxy   = self # link between App::DocumentObject to this object

        obj.addProperty("App::PropertyString", "SimulationType",          "Simulation","SimuType(steady/transient/scanning)")
        obj.addProperty("App::PropertyFloat",  "Start",                   "Simulation","the starting value")
        obj.addProperty("App::PropertyFloat",  "End",                     "Simulation","the ending value")
        obj.addProperty("App::PropertyFloat",  "Step",                    "Simulation","length of solving step")
        obj.addProperty("App::PropertyInteger","I_output",                "Simulation","steps to output")
        obj.addProperty("App::PropertyInteger","SteadyStateMaxIterations","Simulation","steady state max iterations")
        obj.addProperty("App::PropertyInteger","SteadyStateMinIterations","Simulation","steady state min iterations")
        obj.addProperty("App::PropertyString", "TimeSteppingMethod",      "Simulation","TimeStepping(BDF/Runge-Kutta/Implicit Euler/newmark)")
        obj.addProperty("App::PropertyInteger","BDF_Order",               "Simulation","BDF order degree")
        obj.addProperty("App::PropertyFloat",  "Scal_fact",               "Simulation","scaling factor")
        obj.addProperty("App::PropertyString", "DataDir",                 "DataDir.",  "Directory documenting this app")

        obj.SimulationType           = "Steady"
        obj.Start                    = 0.0
        obj.End                      = 0.0
        obj.Step                     = 1.0
        obj.I_output                 = 1
        obj.SteadyStateMinIterations = 1
        obj.SteadyStateMaxIterations = 1
        obj.TimeSteppingMethod       = "BDF"
        obj.BDF_Order                = 2
        obj.Scal_fact                = 0.001
        obj.DataDir                  = "../data/Mesh_dir"

    def execute(self, obj):
        return

    def __getstate__(self):
        return self.Type

    def __setstate__(self, state):
        if state:
            self.Type = state
