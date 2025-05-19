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

__title__  = "FreeCAD MySimII Solvers document object"
__author__ = "Bernd Hahnebach, Qingfeng Xia, Zhenwen Wan"
__url__    = "http://www.freecadweb.org"

class _MySimIISolvers:
    "The MySimIISolvers object"
    def __init__(self, obj):
        self.Type   = "Fem::FemSolverObjectPython"
        self.Object = obj  # keep a ref to the DocObj for nonGui usage
        obj.Proxy   = self # link between App::DocumentObject to this object

        # solver options
        obj.addProperty("App::PropertyString", "Equation",             "Type parameters",  "Equation name")
        obj.addProperty("App::PropertyString", "LinearSolverType",     "LinearSolverParam","Solving directively or iteratively")
        obj.addProperty("App::PropertyString", "LinearDirectMethod",   "LinearSolverParam","Method of direct linear solver")
        obj.addProperty("App::PropertyString", "LinearIterativeMethod","LinearSolverParam","Method of iterative linear solver")
        obj.addProperty("App::PropertyString", "LinearPreconditioning","LinearSolverParam","Method of preconditioning linear solver")
        obj.addProperty("App::PropertyInteger","BiCGStablDegree",      "LinearSolverParam","Degree of BiCGStabl iterative method")
        obj.addProperty("App::PropertyString", "ExecutionWhenCoupling","Type parameters",  "Execution status when coupling")
        # initializing
        obj.Equation                 = "Heat"
        obj.LinearSolverType         = "Iterative"
        obj.LinearDirectMethod       = "umfpack"
        obj.LinearIterativeMethod    = "BiCGStabl"
        obj.LinearPreconditioning    = "vanka"
        obj.BiCGStablDegree          = 2
        obj.ExecutionWhenCoupling    = "always"
        
        # iteration parameter 
        obj.addProperty("App::PropertyFloat",  "SteadyTolerance",      "SteadyStateParam", "Converg-Criterion of steady state")
        obj.addProperty("App::PropertyInteger","LinearMaxIter",        "LinearSolverParam","Steps of max linear iteration")
        obj.addProperty("App::PropertyFloat",  "LinearTolerance",      "LinearSolverParam","Convengence criterion of linear system")
        obj.addProperty("App::PropertyFloat",  "NonLinearTolerance",   "NonLinearSolverParam","Convengence criterion of nonlinear system")
        obj.addProperty("App::PropertyInteger","NonLinearMaxIter",     "NonLinearSolverParam","Iteration steps of nonlinear system")
        obj.addProperty("App::PropertyFloat",  "NonLinearRelax",       "NonLinearSolverParam","Iteration relax coeff of nonlinear system")
        obj.addProperty("App::PropertyInteger","NewtonStartIter",      "NonLinearSolverParam","Newton starting iteration")
        obj.addProperty("App::PropertyFloat",  "NewtonTolerance",      "NonLinearSolverParam","Convengence criterion of Newton iteration")
        # initializing
        obj.SteadyTolerance          = 0.1e-5
        obj.LinearMaxIter            = 100
        obj.LinearTolerance          = 0.1e-5
        obj.NonLinearTolerance       = 0.1e-4
        obj.NonLinearMaxIter         = 4
        obj.NonLinearRelax           = 0.75
        obj.NewtonStartIter          = 2
        obj.NewtonTolerance          = 0.1e-4

    def execute(self, obj):
        return

    def __getstate__(self):
        return self.Type

    def __setstate__(self, state):
        if state:
            self.Type = state
