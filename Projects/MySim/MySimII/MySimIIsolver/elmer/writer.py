# ***************************************************************************
# *   Copyright (c) 2017 Markus Hovorka <m.hovorka@live.de>                 *
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

__title__ = "FreeCAD FEM solver Elmer writer"
__author__ = "Markus Hovorka"
__url__ = "http://www.freecadweb.org"

## \addtogroup FEM
#  @{

import os
import os.path
import subprocess
import tempfile

import FreeCAD
from FreeCAD import Units
from FreeCAD import Console
import Fem
import MySimIItools.femutils as femutils
import MySimIItools.gmshtools_zw as gmshtools
from .. import settings
from . import sifio


_STARTINFO_NAME = "ELMERSOLVER_STARTINFO"
_SIF_NAME = "case.sif"
_ELMERGRID_IFORMAT = "8"
_ELMERGRID_OFORMAT = "2"
_SOLID_PREFIX = "Solid"

EMPTY_FLOAT = 0.43215e-99
EMPTY = ""

UNITS = {
    "L": "m",
    "M": "kg",
    "T": "s",
    "I": "A",
    "O": "K",
    "N": "mol",
    "J": "cd",
}


CONSTS_DEF = {
    "Gravity": "9.82 m/s^2",
    "StefanBoltzmann": "5.67e-8 W/(m^2*K^4)",
    "PermittivityOfVacuum": "8.8542e-12 s^4*A^2/(m*kg)",
    "BoltzmannConstant": "1.3807e-23 J/K",
}


def getFromUi(value, unit, outputDim):
    quantity = Units.Quantity(str(value) + str(unit))
    return convert(quantity, outputDim)


def convert(quantityStr, unit):
    quantity = Units.Quantity(quantityStr)
    for key, setting in UNITS.items():
        unit = unit.replace(key, setting)
    return float(quantity.getValueAs(unit))


def _getAllSubObjects(obj):
    s = ["Solid" + str(i + 1) for i in range(len(obj.Shape.Solids))]
    s.extend(("Face" + str(i + 1) for i in range(len(obj.Shape.Faces))))
    s.extend(("Edge" + str(i + 1) for i in range(len(obj.Shape.Edges))))
    s.extend(("Vertex" + str(i + 1) for i in range(len(obj.Shape.Vertexes))))
    return s


def getConstant(name, dimension):
    return convert(CONSTS_DEF[name], dimension)

def getValue_or_Object(input_str, dict1_str, dict2_str):
    if "equals" in input_str.lower():
        tmp = input_str.split(" ")
        ttt = []
        for sub in tmp:
            if not sub.lower() == "equals":
                ttt.append(sub)
        return sifio.EqualAttr(" ".join(ttt))
    elif "," in input_str:
        tmp = input_str.split(",")
        arr = []
        for it in tmp:
            arr.append(float(it))
        return sifio.FArrayAttr(arr)
    elif "SD" in input_str:
        if dict1_str.isspace() or dict2_str.isspace():
            raise WriteError("No Self-Definings !!!")
        else:
            dict1 = {}
            str_list1 = dict1_str.splitlines()
            for dict_item in str_list1:
                temp = dict_item.split(":")
                dict1[temp[0]] = temp[1]
            dict2 = {}
            str_list2 = dict2_str.splitlines()
            for dict_item in str_list2:
                temp = dict_item.split(":")
                dict2[temp[0]] = temp[1]
            if (input_str in dict1) and (input_str in dict2):
                variable = dict1[input_str]
                function = dict2[input_str]
                return sifio.VariableAttr(variable, function)
            elif (len(dict2) > 0):
                variable = dict1[input_str]
                FunctionList = []
                for key in dict2.keys():
                    if input_str in key:
                        FunctionList.append(dict2[key])
                return sifio.VarArrayAttr(variable, FunctionList)
            else:
                raise WriteError("No Self-Defining connecting to " + input_str)

    try:
        return float(input_str)
    except ValueError:
        if input_str:
            return sifio.VariableAttr("", input_str)
        else:
            return EMPTY_FLOAT

class Writer(object):

    def __init__(self, analysis, directory, testmode=False):
        self.analysis = analysis
        self.mesh = analysis.Mesh
        self.simulation = analysis.Simulation
        self.directory = directory
        self.testmode = testmode
        self._usedVarNames = set()
        self._builder = sifio.Builder()
        self._handledObjects = set()

    def getHandledConstraints(self):
        return self._handledObjects

    def write(self):
        self._handleSimulation()
        self._handleConstants()
        self._handleBndConditions()
        AllBodies = self._getAllBodies()
       
        solverList = [] 
        solverSectionList = [] 
        for text in self.analysis.MappingList:
            tmp = text.split("X")
            i = int(tmp[0]) - 2
            j = int(tmp[1]) - 2
            solver = self.analysis.SolverList[i]
            if solver in solverList:
                k = solverList.index(solver)
                solverSection = solverSectionList[k]
            else:
                solverSection = self._getSolver(solver)
                solverList.append(solver)
                solverSectionList.append(solverSection)
            body = self.analysis.MaterialList[j].Label
            self._addSolver(body, solverSection)
        
        self._handleInitial(AllBodies)
        self._handleBodyForces(AllBodies)
        self._handleMaterial(AllBodies)
        self._addOutputSolver()

        sifPath = os.path.join(self.directory, _SIF_NAME)
        with open(sifPath, "w") as fstream:
            sif = sifio.Sif(self._builder, self.directory)
            sif.write(fstream)
        self._writeMesh()
        self._writeStartinfo()

    def _writeMesh(self):
        mesh = self.analysis.Mesh
        unvPath = os.path.join(self.directory, "mesh.unv")
        mesh.FemMesh.write(unvPath)
        if self.testmode:
            Console.PrintMessage("We are in testmode ElmerGrid may not be installed.\n")
        else:
            binary = FreeCAD.getHomePath() + "bin/Elmer/bin/ElmerGrid.exe"
            if binary is None:
                raise WriteError("Couldn't find ElmerGrid binary.")
            args = [binary,
                    _ELMERGRID_IFORMAT,
                    _ELMERGRID_OFORMAT,
                    unvPath,
                    "-out", self.directory, "-removelowdim"]
            subprocess.call(args, stdout=subprocess.PIPE)

    def _writeStartinfo(self):
        path = os.path.join(self.directory, _STARTINFO_NAME)
        with open(path, "w") as f:
            f.write(_SIF_NAME)

    def _exportToUnv(self, groups, mesh, meshPath):
        unvGmshFd, unvGmshPath = tempfile.mkstemp(suffix=".unv")
        brepFd, brepPath = tempfile.mkstemp(suffix=".brep")
        geoFd, geoPath = tempfile.mkstemp(suffix=".geo")
        os.close(brepFd)
        os.close(geoFd)
        os.close(unvGmshFd)

        tools = gmshtools.GmshTools(mesh)
        tools.group_elements = {g: [g] for g in groups}
        tools.group_nodes_export = False
        tools.ele_length_map = {}
        tools.temp_file_geometry = brepPath
        tools.temp_file_geo = geoPath
        tools.temp_file_mesh = unvGmshPath

        tools.get_dimension()
        tools.get_gmsh_command()
        tools.get_region_data()
        tools.get_boundary_layer_data()
        tools.write_part_file()
        tools.write_geo()
        if self.testmode:
            Console.PrintMessage("We are in testmode, Gmsh may not be installed.\n")
            import shutil
            shutil.copyfile(geoPath, os.path.join(self.directory, "group_mesh.geo"))
        else:
            tools.run_gmsh_with_geo()

            ioMesh = Fem.FemMesh()
            ioMesh.read(unvGmshPath)
            ioMesh.write(meshPath)

        os.remove(brepPath)
        os.remove(geoPath)
        os.remove(unvGmshPath)

    def _handleSimulation(self):
        self._simulation("Coordinate System", "Cartesian " + str(self._getMeshDimension()) + "D" )
        self._simulation("Coordinate Mapping", (1, 2, 3))
        self._simulation("Simulation Type", self.simulation.SimulationType)
        self._simulation("Steady State Min Iterations",self.simulation.SteadyStateMinIterations)
        self._simulation("Steady State Max Iterations",self.simulation.SteadyStateMaxIterations)
        Start     = self.simulation.Start
        End       = self.simulation.End
        Step      = self.simulation.Step
        if Step > 1e-100:
            StepCount = int((End - Start)/Step)
        else:
            StepCount = 1
        self._simulation("Timestep sizes",     Step)
        self._simulation("Timestep Intervals", StepCount)
        self._simulation("Output Intervals",   self.simulation.I_output)
        self._simulation("Coordinate Scaling", self.simulation.Scal_fact)
        self._simulation("Timestepping Method",self.simulation.TimeSteppingMethod)
        self._simulation("BDF Order",          self.simulation.BDF_Order)
        self._simulation("Post File",          sifio.FileAttr("case.vtu"))
        self._simulation("Use Mesh Names",     True)
    
    def _getSolver(self, equation):
        s = self._createNonlinearSolver(equation)
        s["Equation"] = equation.Equation
        if "Heat" in equation.Equation:
            s["Procedure"] = sifio.FileAttr("HeatSolve/HeatSolver")
            s["Variable"]  = self._getUniqueVarName("Temperature")
    
        elif "CFD" in equation.Equation:
            s["Procedure"] = sifio.FileAttr("FlowSolve/FlowSolver")
            s["Variable"]  = "Flow Solution[Velocity:" + str(self._getMeshDimension()) + " Pressure:1]"
            s["Variable DOFs"]  = 1 + self._getMeshDimension()
        elif "Elastic" in equation.Equation:
            s["Procedure"] = sifio.FileAttr("ElasticSolve/ElasticSolver")
            s["Variable"]  = self._getUniqueVarName("Displacement")
            s["Variable DOFs"]  = self._getMeshDimension()
        elif "MeshUpdate" in equation.Equation:
            s["Equation"] = "Mesh Update"
        elif "Flow2D" in equation.Equation:
            s["Procedure"] = sifio.FileAttr("FlowSolveVS/FlowSolver")
            s["Variable"]  = "Flow Solution[Velocity:" + str(self._getMeshDimension()) + " Pressure:1]"
            s["Variable DOFs"]  = 1 + self._getMeshDimension()
        elif "CFT2D" in equation.Equation:
            s["Procedure"] = sifio.FileAttr("CFTSolve/CFTSolver")
            s["Variable"]  = "CompC"
            s["Variable DOFs"]  = self._getMeshDimension()
        elif "EM" in equation.Equation:
            s["Procedure"] = sifio.FileAttr("VectorHelmholtz/VectorHelmholtzSolver")
            s["Variable"]  = "E[E re:1 E im:1]"
            s["Variable DOFs"]  = 2

        s["Exec Solver"] = "Always"
        s["Stabilize"] = True
        s["Bubbles"] = False
        s["Optimize Bandwidth"] = True
        return s

    def _handleConstants(self):
        self._constant(
            "Stefan Boltzmann",
            getConstant("StefanBoltzmann", "M/(O^4*T^3)"))
        gravity = getConstant("Gravity", "L/T^2")
        self._constant("Gravity", (0.0, -1.0, 0.0, gravity))

    def _handleBndConditions(self):
        for obj in self.analysis.Group:
            if not obj.Label[0:8]  == "Boundary":
                continue
            name = obj.Label
            if obj.References:
                temp = getValue_or_Object(obj.Dirichlet, obj.SelfDef1, obj.SelfDef2)
                if not temp == EMPTY_FLOAT:
                    self._boundary(name, "CompC", temp)

                temp = getValue_or_Object(obj.Neumann, obj.SelfDef1, obj.SelfDef2)
                if not temp == EMPTY_FLOAT:
                    self._boundary(name, "FluxC", temp)

                temp = getValue_or_Object(obj.RobinCoef, obj.SelfDef1, obj.SelfDef2)
                if not temp == EMPTY_FLOAT:
                    self._boundary(name, "CoefC", temp)

                temp = getValue_or_Object(obj.RobinExt, obj.SelfDef1, obj.SelfDef2)
                if not temp == EMPTY_FLOAT:
                    self._boundary(name, "ExtC", temp)

                temp = getValue_or_Object(obj.DirichletF, obj.SelfDef1, obj.SelfDef2)
                if not temp == EMPTY_FLOAT:
                    self._boundary(name, "CompF", temp)

                temp = getValue_or_Object(obj.NeumannF, obj.SelfDef1, obj.SelfDef2)
                if not temp == EMPTY_FLOAT:
                    self._boundary(name, "FluxF", temp)

                temp = getValue_or_Object(obj.RobinCoefF, obj.SelfDef1, obj.SelfDef2)
                if not temp == EMPTY_FLOAT:
                    self._boundary(name, "CoefF", temp)

                temp = getValue_or_Object(obj.RobinExtF, obj.SelfDef1, obj.SelfDef2)
                if not temp == EMPTY_FLOAT:
                    self._boundary(name, "ExtF", temp)

                temp = getValue_or_Object(obj.Temperature, obj.SelfDef1, obj.SelfDef2)
                if not temp == EMPTY_FLOAT:
                    self._boundary(name, "Temperature", temp)

                flux = getValue_or_Object(obj.HeatFlux, obj.SelfDef1, obj.SelfDef2)
                if not flux == EMPTY_FLOAT:
                    self._boundary(name, "Heat Flux", flux)

                temp = getValue_or_Object(obj.Vel, obj.SelfDef1, obj.SelfDef2)
                if not temp == EMPTY_FLOAT:
                    self._boundary(name, "Velocity", temp)
                
                temp = getValue_or_Object(obj.Pressure, obj.SelfDef1, obj.SelfDef2)
                if not temp == EMPTY_FLOAT:
                    self._boundary(name, "Pressure", temp)
                
                temp = getValue_or_Object(obj.SlipCoeff, obj.SelfDef1, obj.SelfDef2)
                if not temp == EMPTY_FLOAT:
                    self._boundary(name, "Slip Coefficient", temp)
                
                if obj.NTCoord:
                    self._boundary(name, "Normal-Tangential Velocity", True)
                
                if obj.NTCoordG:
                    self._boundary(name, "Normal-Tangential CompC", True)
                
                if obj.NTCoordF:
                    self._boundary(name, "Normal-Tangential CompF", True)
                
                if obj.NoSlipWall:
                    self._boundary(name, "Noslip wall BC", True)

                if obj.FSI:
                    self._boundary(name, "FSI BC", True)

                temp = getValue_or_Object(obj.NormalForce, obj.SelfDef1, obj.SelfDef2)
                if not temp == EMPTY_FLOAT:
                    self._boundary(name, "Normal force", temp)
                
                temp = getValue_or_Object(obj.Force, obj.SelfDef1, obj.SelfDef2)
                if not temp == EMPTY_FLOAT:
                    self._boundary(name, "Force", temp)
                
                temp = getValue_or_Object(obj.Displacement, obj.SelfDef1, obj.SelfDef2)
                if not temp == EMPTY_FLOAT:
                    self._boundary(name, "Displacement", temp)
                
                temp = getValue_or_Object(obj.MeshUpdate, obj.SelfDef1, obj.SelfDef2)
                if not temp == EMPTY_FLOAT:
                    self._boundary(name, "Mesh Update", temp)
                    
                self._handled(obj)

    def _handleMaterial(self, bodies):
        for obj in self.analysis.MaterialList:
            if not obj.Label in bodies:
                continue
            name = obj.Label
            self._material(name, "Convect", True)
            temp = getValue_or_Object(obj.Density, EMPTY, EMPTY)
            if not temp == EMPTY_FLOAT:
                self._material(name, "Density", temp)
            temp = getValue_or_Object(obj.HeatConduct, "", "")
            if not temp == EMPTY_FLOAT:
                self._material(name, "Heat Conductivity", temp)
            temp = getValue_or_Object(obj.HeatCapacity, "", "")
            if not temp == EMPTY_FLOAT:
                self._material(name, "Heat Capacity", temp)
            temp = getValue_or_Object(obj.YoungsModulus, "", "")
            if not temp == EMPTY_FLOAT:
                self._material(name, "Youngs Modulus", temp)
            temp = getValue_or_Object(obj.PoissonRatio, "", "")
            if not temp == EMPTY_FLOAT:
                self._material(name, "Poisson ratio", temp)
            temp = getValue_or_Object(obj.MeshElasticModulus, "", "")
            if not temp == EMPTY_FLOAT:
                self._material(name, "Youngs Modulus", temp)
            temp = getValue_or_Object(obj.MeshPoissonRatio, "", "")
            if not temp == EMPTY_FLOAT:
                self._material(name, "Poisson ratio", temp)
            temp = getValue_or_Object(obj.Viscosity, "", "")
            if not temp == EMPTY_FLOAT:
                self._material(name, "Viscosity", temp)
            temp = getValue_or_Object(obj.Density, "", "")
            if not temp == EMPTY_FLOAT:
                self._material(name, "Heat expansion Coefficient", temp)
            temp = getValue_or_Object(obj.RefPressure, "", "")
            if not temp == EMPTY_FLOAT:
                self._material(name, "Reference Pressure", temp)
            temp = getValue_or_Object(obj.SpecHeatRatio, "", "")
            if not temp == EMPTY_FLOAT:
                self._material(name, "Specific Heat Ratio", temp)
            if not obj.CompressModel == "Empty":
                self._material(name, "Compressibility Model", obj.CompressModel)
            temp = getValue_or_Object(obj.CForce, "", "")
            if not temp == EMPTY_FLOAT:
                self._material(name, "Coriolis force", temp)
            temp = getValue_or_Object(obj.RForce, "", "")
            if not temp == EMPTY_FLOAT:
                self._material(name, "gross force", temp)
            temp = getValue_or_Object(obj.BFriction, "", "")
            if not temp == EMPTY_FLOAT:
                self._material(name, "Bottom Friction", temp)
            temp = getValue_or_Object(obj.TDepth, "", "")
            if not temp == EMPTY_FLOAT:
                self._material(name, "topography depth", temp)
            self._handled(obj)

    def _handleInitial(self, bodies):
        #Material equal to computing subdomain list
        for obj in self.analysis.InitialList:
            for sub in obj.Subdomains: #refers actually to Material/Body List
                if not sub.Label in bodies:
                    continue
                name = sub.Label #fixed to bodies definition
               
                #Temperature 
                temp = getValue_or_Object(obj.Temperature, obj.SelfDef1, obj.SelfDef2)
                if not temp == EMPTY_FLOAT:
                    self._initial(name, "Temperature", temp)
           
                #Pressure    
                temp = getValue_or_Object(obj.Pressure, obj.SelfDef1, obj.SelfDef2)
                if not temp == EMPTY_FLOAT:
                    self._initial(name, "Pressure", temp)
            
                #Velocity 
                temp = getValue_or_Object(obj.VelX, obj.SelfDef1, obj.SelfDef2)
                if not temp == EMPTY_FLOAT:
                    self._initial(name, "Velocity 1", temp)
                temp = getValue_or_Object(obj.VelY, obj.SelfDef1, obj.SelfDef2)
                if not temp == EMPTY_FLOAT:
                    self._initial(name, "Velocity 2", temp)
                temp = getValue_or_Object(obj.VelZ, obj.SelfDef1, obj.SelfDef2)
                if not temp == EMPTY_FLOAT:
                    self._initial(name, "Velocity 3", temp)
             
                #Force 
                temp = getValue_or_Object(obj.Force1, obj.SelfDef1, obj.SelfDef2)
                if not temp == EMPTY_FLOAT:
                    self._initial(name, "Force 1", temp)
                temp = getValue_or_Object(obj.Force2, obj.SelfDef1, obj.SelfDef2)
                if not temp == EMPTY_FLOAT:
                    self._initial(name, "Force 2", temp)
                temp = getValue_or_Object(obj.Force3, obj.SelfDef1, obj.SelfDef2)
                if not temp == EMPTY_FLOAT:
                    self._initial(name, "Force 3", temp)
              
                #Displacement 
                temp = getValue_or_Object(obj.Displacement1, obj.SelfDef1, obj.SelfDef2)
                if not temp == EMPTY_FLOAT:
                    self._initial(name, "Displacement 1", temp)
                temp = getValue_or_Object(obj.Displacement2, obj.SelfDef1, obj.SelfDef2)
                if not temp == EMPTY_FLOAT:
                    self._initial(name, "Displacement 2", temp)
                temp = getValue_or_Object(obj.Displacement3, obj.SelfDef1, obj.SelfDef2)
                if not temp == EMPTY_FLOAT:
                    self._initial(name, "Displacement 3", temp)
               
                #MeshUpdate 
                temp = getValue_or_Object(obj.MeshUpdate1, obj.SelfDef1, obj.SelfDef2)
                if not temp == EMPTY_FLOAT:
                    self._initial(name, "Mesh Update 1", temp)
                temp = getValue_or_Object(obj.MeshUpdate2, obj.SelfDef1, obj.SelfDef2)
                if not temp == EMPTY_FLOAT:
                    self._initial(name, "Mesh Update 2", temp)
                temp = getValue_or_Object(obj.MeshUpdate3, obj.SelfDef1, obj.SelfDef2)
                if not temp == EMPTY_FLOAT:
                    self._initial(name, "Mesh Update 3", temp)
                
            self._handled(obj)

    def _handleBodyForces(self, bodies):
        #Material equal to computing subdomain list
        for obj in self.analysis.ForcingList:
            for sub in obj.Subdomains: #refers actually to Material/Body List
                if not sub.Label in bodies:
                    continue
                name = sub.Label #fixed to bodies definition
               
                #Heat 
                temp = getValue_or_Object(obj.HeatSource, obj.SelfDef1, obj.SelfDef2)
                if not temp == EMPTY_FLOAT:
                    self._bodyForce(name, "Heat Source", temp)
                temp = getValue_or_Object(obj.TemperatureLowest, obj.SelfDef1, obj.SelfDef2)
                if not temp == EMPTY_FLOAT:
                    self._bodyForce(name, "Temperature Lower Limit", temp)
                temp = getValue_or_Object(obj.TemperatureHighest, obj.SelfDef1, obj.SelfDef2)
                if not temp == EMPTY_FLOAT:
                    self._bodyForce(name, "Temperature Upper Limit", temp)
                if obj.SmartHeat:
                    self._bodyForce(name, "Smart Heater Control", obj.SmartHeat)
           
                #CFD 
                temp = getValue_or_Object(obj.ForceX, obj.SelfDef1, obj.SelfDef2)
                if not temp == EMPTY_FLOAT:
                    self._bodyForce(name, "Flow BodyForce 1", temp)
                temp = getValue_or_Object(obj.ForceY, obj.SelfDef1, obj.SelfDef2)
                if not temp == EMPTY_FLOAT:
                    self._bodyForce(name, "Flow BodyForce 2", temp)
                temp = getValue_or_Object(obj.ForceZ, obj.SelfDef1, obj.SelfDef2)
                if not temp == EMPTY_FLOAT:
                    self._bodyForce(name, "Flow BodyForce 3", temp)
                if obj.Floating:
                    self._bodyForce(name, "Boussinesq", obj.Floating)
             
                #Elastic 
                temp = getValue_or_Object(obj.MassForce1, obj.SelfDef1, obj.SelfDef2)
                if not temp == EMPTY_FLOAT:
                    self._bodyForce(name, "Inertial BodyForce 1", temp)
                temp = getValue_or_Object(obj.MassForce2, obj.SelfDef1, obj.SelfDef2)
                if not temp == EMPTY_FLOAT:
                    self._bodyForce(name, "Inertial BodyForce 2", temp)
                temp = getValue_or_Object(obj.MassForce3, obj.SelfDef1, obj.SelfDef2)
                if not temp == EMPTY_FLOAT:
                    self._bodyForce(name, "Inertial BodyForce 3", temp)
                temp = getValue_or_Object(obj.VolumeForce1, obj.SelfDef1, obj.SelfDef2)
                if not temp == EMPTY_FLOAT:
                    self._bodyForce(name, "Stress BodyForce 1", temp)
                temp = getValue_or_Object(obj.VolumeForce2, obj.SelfDef1, obj.SelfDef2)
                if not temp == EMPTY_FLOAT:
                    self._bodyForce(name, "Stress BodyForce 2", temp)
                temp = getValue_or_Object(obj.VolumeForce3, obj.SelfDef1, obj.SelfDef2)
                if not temp == EMPTY_FLOAT:
                    self._bodyForce(name, "Stress BodyForce 3", temp)
              
                #Em Waves 
                temp = getValue_or_Object(obj.CurrentDensityRealX, obj.SelfDef1, obj.SelfDef2)
                if not temp == EMPTY_FLOAT:
                    self._bodyForce(name, "Current Density 1", temp)
                temp = getValue_or_Object(obj.CurrentDensityRealY, obj.SelfDef1, obj.SelfDef2)
                if not temp == EMPTY_FLOAT:
                    self._bodyForce(name, "Current Density 2", temp)
                temp = getValue_or_Object(obj.CurrentDensityRealZ, obj.SelfDef1, obj.SelfDef2)
                if not temp == EMPTY_FLOAT:
                    self._bodyForce(name, "Current Density 3", temp)
                temp = getValue_or_Object(obj.CurrentDensityImagX, obj.SelfDef1, obj.SelfDef2)
                if not temp == EMPTY_FLOAT:
                    self._bodyForce(name, "Current Density 1 im", temp)
                temp = getValue_or_Object(obj.CurrentDensityImagY, obj.SelfDef1, obj.SelfDef2)
                if not temp == EMPTY_FLOAT:
                    self._bodyForce(name, "Current Density 2 im", temp)
                temp = getValue_or_Object(obj.CurrentDensityImagZ, obj.SelfDef1, obj.SelfDef2)
                if not temp == EMPTY_FLOAT:
                    self._bodyForce(name, "Current Density 3 im", temp)
               
            self._handled(obj)

    def _handleElectrostatic(self):
        activeIn = []
        for equation in self.analysis.Group:
            if femutils.is_of_type(equation, "Fem::FemEquationElmerElectrostatic"):
                #if equation.References:
                #    activeIn = equation.References[0][1]
                #else:
                #    activeIn = self._getAllBodies()
                activeIn = self._getAllBodies()
                solverSection = self._getElectrostaticSolver(equation)
                for body in activeIn:
                    self._addSolver(body, solverSection)
        if activeIn:
            self._handleElectrostaticConstants()
            self._handleElectrostaticBndConditions()
            # self._handleElectrostaticInitial(activeIn)
            # self._handleElectrostaticBodyForces(activeIn)
            self._handleElectrostaticMaterial(activeIn)

    def _getElectrostaticSolver(self, equation):
        s = self._createLinearSolver(equation)
        s["Equation"] = "Stat Elec Solver"  # equation.Name
        s["Procedure"] = sifio.FileAttr("StatElecSolve/StatElecSolver")
        s["Variable"] = self._getUniqueVarName("Potential")
        s["Variable DOFs"] = 1
        s["Calculate Electric Field"] = equation.CalculateElectricField
        s["Calculate Electric Energy"] = equation.CalculateElectricEnergy
        s["Calculate Surface Charge"] = equation.CalculateSurfaceCharge
        s["Displace mesh"] = False
        s["Exec Solver"] = "Always"
        s["Stabilize"] = equation.Stabilize
        s["Bubbles"] = equation.Bubbles
        s["Optimize Bandwidth"] = True
        return s

    def _handleElectrostaticConstants(self):
        self._constant(
            "Permittivity Of Vacuum",
            getConstant("PermittivityOfVacuum", "T^4*I^2/(L*M)"))

    def _handleElectrostaticMaterial(self, bodies):
        for obj in self._getMember("App::MaterialObject"):
            m = obj.Material
            refs = (
                obj.References[0][1]
                if obj.References
                else self._getAllBodies())
            for name in (n for n in refs if n in bodies):
                if "RelativePermittivity" in m:
                    self._material(
                        name, "Relative Permittivity",
                        float(m["RelativePermittivity"]))

    def _handleElectrostaticBndConditions(self):
        for obj in self._getMember("Fem::ConstraintElectrostaticPotential"):
            if obj.References:
                for name in obj.References[0][1]:
                    if obj.Potential:
                        potential = getFromUi(obj.Potential, "V", "M*L^2/(T^3 * I)")
                        self._boundary(name, "Potential", potential)
                    if obj.PotentialConstant:
                        self._boundary(name, "Potential Constant", True)
                self._handled(obj)

    def _handleFluxsolver(self):
        activeIn = []
        for equation in self.analysis.Group:
            if femutils.is_of_type(equation, "Fem::FemEquationElmerFluxsolver"):
                #if equation.References:
                #    activeIn = equation.References[0][1]
                #else:
                #    activeIn = self._getAllBodies()
                activeIn = self._getAllBodies()
                solverSection = self._getFluxsolverSolver(equation)
                for body in activeIn:
                    self._addSolver(body, solverSection)

    def _getFluxsolverSolver(self, equation):
        s = self._createLinearSolver(equation)
        s["Equation"] = "Flux Solver"  # equation.Name
        s["Procedure"] = sifio.FileAttr("FluxSolver/FluxSolver")
        s["Flux Variable"] = equation.FluxVariable
        s["Calculate Flux"] = equation.CalculateFlux
        s["Calculate Grad"] = equation.CalculateGrad
        return s

    def _handleElasticity(self):
        activeIn = self._getAllBodies()
        for solver in self.analysis.Group:
            if solver.Label[0:6] == "Solver" and "Elastic" in solver.Equation:
                solverSection = self._getElasticitySolver(solver)
                for body in activeIn:
                    self._addSolver(body, solverSection)
                if activeIn:
                    self._handleElasticityConstants()
                    self._handleGeneralBndConditions()
                    self._handleElasticityInitial(activeIn)
                    self._handleElasticityBodyForces(activeIn)
                    self._handleGeneralMaterial(activeIn)

    def _getElasticitySolver(self, equation):
        s = self._createLinearSolver(equation)
        s["Equation"] = equation.Equation
        s["Procedure"] = sifio.FileAttr("StressSolve/StressSolver")
        s["Variable"] = self._getUniqueVarName("Displacement")
        s["Variable DOFs"] = 3
        #s["Eigen Analysis"] = equation.DoFrequencyAnalysis
        #s["Eigen System Values"] = equation.EigenmodesCount
        #s["Calculate Strains"] = equation.CalculateStrains
        #s["Calculate Stresses"] = equation.CalculateStresses
        #s["Calculate Principal"] = equation.CalculatePrincipal
        #s["Calculate Pangle"] = equation.CalculatePangle
        #s["Displace mesh"] = False
        s["Exec Solver"] = "Always"
        s["Stabilize"] = True
        #s["Bubbles"] = equation.Bubbles
        s["Optimize Bandwidth"] = True
        return s

    def _handleElasticityConstants(self):
        pass

    def _handleElasticityBndConditions(self):
        for obj in self._getMember("Fem::ConstraintPressure"):
            if obj.References:
                for name in obj.References[0][1]:
                    pressure = getFromUi(obj.Pressure, "MPa", "M/(L*T^2)")
                    if not obj.Reversed:
                        pressure *= -1
                    self._boundary(name, "Normal Force", pressure)
                self._handled(obj)
        for obj in self._getMember("Fem::ConstraintFixed"):
            if obj.References:
                for name in obj.References[0][1]:
                    self._boundary(name, "Displacement 1", 0.0)
                    self._boundary(name, "Displacement 2", 0.0)
                    self._boundary(name, "Displacement 3", 0.0)
                self._handled(obj)
        for obj in self._getMember("Fem::ConstraintForce"):
            if obj.References:
                for name in obj.References[0][1]:
                    force = getFromUi(obj.Force, "N", "M*L*T^-2")
                    self._boundary(name, "Force 1", obj.DirectionVector.x * force)
                    self._boundary(name, "Force 2", obj.DirectionVector.y * force)
                    self._boundary(name, "Force 3", obj.DirectionVector.z * force)
                    self._boundary(name, "Force 1 Normalize by Area", True)
                    self._boundary(name, "Force 2 Normalize by Area", True)
                    self._boundary(name, "Force 3 Normalize by Area", True)
                self._handled(obj)
        for obj in self._getMember("Fem::ConstraintDisplacement"):
            if obj.References:
                for name in obj.References[0][1]:
                    if not obj.xFree:
                        self._boundary(
                            name, "Displacement 1", obj.xDisplacement * 0.001)
                    elif obj.xFix:
                        self._boundary(name, "Displacement 1", 0.0)
                    if not obj.yFree:
                        self._boundary(
                            name, "Displacement 2", obj.yDisplacement * 0.001)
                    elif obj.yFix:
                        self._boundary(name, "Displacement 2", 0.0)
                    if not obj.zFree:
                        self._boundary(
                            name, "Displacement 3", obj.zDisplacement * 0.001)
                    elif obj.zFix:
                        self._boundary(name, "Displacement 3", 0.0)
                self._handled(obj)

    def _handleElasticityInitial(self, bodies):
        pass

    def _handleElasticityBodyForces(self, bodies):
        obj = self._getSingleMember("FemConstraintSelfWeight")
        if obj is not None:
            for name in bodies:
                gravity = getConstant("Gravity", "L/T^2")
                m = self._getBodyMaterial(name).Material

                densityQuantity = Units.Quantity(m["Density"])
                dimension = "M/L^3"
                if name.startswith("Edge"):
                    # not tested, but it seems needed
                    # because density does not exist (IMHO, bernd)
                    density = None
                    if density:
                        density.Unit = Units.Unit(-2, 1)
                    dimension = "M/L^2"
                density = convert(densityQuantity, dimension)

                force1 = gravity * obj.Gravity_x * density
                force2 = gravity * obj.Gravity_y * density
                force3 = gravity * obj.Gravity_z * density
                self._bodyForce(name, "Stress Bodyforce 1", force1)
                self._bodyForce(name, "Stress Bodyforce 2", force2)
                self._bodyForce(name, "Stress Bodyforce 3", force3)
            self._handled(obj)

    def _getBodyMaterial(self, name):
        for obj in self._getMember("App::MaterialObject"):
            if not obj.References or name in obj.References[0][1]:
                return obj
        return None

    def _handleElasticityMaterial(self, bodies):
        tempObj = self._getSingleMember("Fem::ConstraintInitialTemperature")
        if tempObj is not None:
            refTemp = getFromUi(tempObj.initialTemperature, "K", "O")
            for name in bodies:
                self._material(name, "Reference Temperature", refTemp)
        for obj in self._getMember("App::MaterialObject"):
            m = obj.Material
            refs = (
                obj.References[0][1]
                if obj.References
                else self._getAllBodies())
            for name in (n for n in refs if n in bodies):
                self._material(
                    name, "Density",
                    self._getDensity(m))
                self._material(
                    name, "Youngs Modulus",
                    self._getYoungsModulus(m))
                self._material(
                    name, "Poisson ratio",
                    float(m["PoissonRatio"]))
                self._material(
                    name, "Heat expansion Coefficient",
                    convert(m["ThermalExpansionCoefficient"], "O^-1"))

    def _getDensity(self, m):
        density = convert(m["Density"], "M/L^3")
        if self._getMeshDimension() == 2:
            density *= 1e3
        return density

    def _getYoungsModulus(self, m):
        youngsModulus = convert(m["YoungsModulus"], "M/(L*T^2)")
        if self._getMeshDimension() == 2:
            youngsModulus *= 1e3
        return youngsModulus

    def _handleFlow(self):
        activeIn = self._getAllBodies()
        for solver in self.analysis.Group:
            if solver.Label[0:6] == "Solver" and "CFD" in solver.Equation:
                solverSection = self._getFlowSolver(solver)
                for body in activeIn:
                    self._addSolver(body, solverSection)
                if activeIn:
                    self._handleFlowConstants()
                    self._handleGeneralBndConditions()
                    self._handleFlowInitialVelocity(activeIn)
                    # self._handleFlowInitial(activeIn)
                    # self._handleFlowBodyForces(activeIn)
                    self._handleFlowMaterial(activeIn)
                    self._handleFlowEquation(activeIn)

    def _getFlowSolver(self, equation):
        s = self._createNonlinearSolver(equation)
        s["Equation"] = "Navier-Stokes"
        s["Variable"] = "Flow Solution[Velocity:" + str(self._getMeshDimension()) + " Pressure:1]"
        s["Procedure"] = sifio.FileAttr("FlowSolve/FlowSolver")
        s["Exec Solver"] = "Always"
        s["Flow Model"] = "Full"
        s["Stabilize"] = True
        s["Bubbles"] = False
        s["Optimize Bandwidth"] = True
        s["Calculate Loads"] = True
        return s

    def _handleFlowConstants(self):
        gravity = getConstant("Gravity", "L/T^2")
        self._constant("Gravity", (0.0, -1.0, 0.0, gravity))

    def _handleFlowMaterial(self, bodies):
        for obj in self.analysis.Group:
            if not obj.Label[0:8]  == "Material":
                continue
            for subele in obj.References[0][1]:
                for name in bodies:
                    if self._rename(subele) == name:
                        if not obj.Density == 999.999:
                            self._material(name, "Density", obj.Density)
                        if not obj.HeatConduct == 999.999:
                            self._material(name, "Heat Conductivity", obj.HeatConduct)
                        if not obj.Viscosity == 999.999:
                            self._material(name, "Viscosity", obj.Viscosity)
                        if not obj.HeatExpansion == 999.999:
                            self._material(name, "Heat expansion Coefficient", obj.HeatExpansion)
                        if not obj.RefPressure == 999.999:
                            self._material(name, "Reference Pressure", obj.RefPressure)
                        if not obj.SpecHeatRatio == 999.999:
                            self._material(name, "Specific Heat Ratio", obj.SpecHeatRatio)
                        if not obj.CompressModel == "Empty":
                            self._material(name, "Compressibility Model", obj.CompressModel)
            self._handled(obj)

    def _handleFlowInitialVelocity(self, bodies):
        obj = self._getSingleMember("Fem::ConstraintInitialFlowVelocity")
        if obj is not None:
            for name in bodies:
                if obj.VelocityXEnabled:
                    velocity = getFromUi(obj.VelocityX, "m/s", "L/T")
                    self._initial(name, "Velocity 1", velocity)
                if obj.VelocityYEnabled:
                    velocity = getFromUi(obj.VelocityY, "m/s", "L/T")
                    self._initial(name, "Velocity 2", velocity)
                if obj.VelocityZEnabled:
                    velocity = getFromUi(obj.VelocityZ, "m/s", "L/T")
                    self._initial(name, "Velocity 3", velocity)
            self._handled(obj)

    def _handleFlowBndConditions(self):
        for obj in self.analysis.Group:
            if not obj.Label[0:8]  == "Boundary":
                continue
            name = obj.Label
            if obj.References:
                if obj.ConstraintType == "Flowing1":
                    if not obj.VelX == 999.999:
                        velocity = getFromUi(obj.VelX, "m/s", "L/T")
                        self._boundary(name, "Velocity 1", velocity)
                    if not obj.VelY == 999.999:
                        velocity = getFromUi(obj.VelY, "m/s", "L/T")
                        self._boundary(name, "Velocity 2", velocity)
                    if not obj.VelZ == 999.999:
                        velocity = getFromUi(obj.VelZ, "m/s", "L/T")
                        self._boundary(name, "Velocity 3", velocity)
                    if not obj.SlipCoeff == 999.999:
                        SlipCoeff = obj.SlipCoeff
                        self._boundary(name, "Slip Coefficient 1", SlipCoeff)
                        self._boundary(name, "Slip Coefficient 2", SlipCoeff)
                        self._boundary(name, "Slip Coefficient 3", SlipCoeff)
                    if not obj.Pressure == 999.999:
                        Pressure = obj.Pressure
                        self._boundary(name, "Pressure", Pressure)
                    if obj.NTCoord:
                        self._boundary(name, "Normal-Tangential Velocity", True)
                    self._handled(obj)

    def _handleFlowEquation(self, bodies):
        for b in bodies:
            self._equation(b, "Convection", "Computed")

    def _createLinearSolver(self, equation):
        s = sifio.createSection(sifio.SOLVER)
        #s.priority = equation.Priority
        s["Steady State Convergence Tolerance"]      = equation.SteadyTolerance
        s["Linear System Solver"]                    = equation.LinearSolverType
        if equation.LinearSolverType == "Direct":
            s["Linear System Direct Method"]         = equation.LinearDirectMethod
        elif equation.LinearSolverType == "Iterative":
            s["Linear System Iterative Method"]      = equation.LinearIterativeMethod
            if equation.LinearIterativeMethod == "BiCGStabl":
                s["BiCGStabl polynomial degree"]     = equation.BiCGStablDegree
            s["Linear System Max Iterations"]        = equation.LinearMaxIter
            s["Linear System Convergence Tolerance"] = equation.LinearTolerance
            s["Linear System Preconditioning"]       = equation.LinearPreconditioning 
        s["Linear System Abort Not Converged"]       = False
        s["Linear System Residual Output"]           = 1
        s["Linear System Precondition Recompute"]    = 1
        return s

    def _createNonlinearSolver(self, equation):
        s = self._createLinearSolver(equation)
        s["Nonlinear System Max Iterations"]          = equation.NonLinearMaxIter
        s["Nonlinear System Convergence Tolerance"]   = equation.NonLinearTolerance
        s["Nonlinear System Relaxation Factor"]       = equation.NonLinearRelax
        s["Nonlinear System Newton After Iterations"] = equation.NewtonStartIter
        s["Nonlinear System Newton After Tolerance"]  = equation.NewtonTolerance
        return s

    def _getUniqueVarName(self, varName):
        postfix = 1
        if varName in self._usedVarNames:
            varName += "%2d" % postfix
        while varName in self._usedVarNames:
            postfix += 1
            varName = varName[:-2] + "%2d" % postfix
        self._usedVarNames.add(varName)
        return varName

    def _getAllBodies(self):
        Rows = len(self.analysis.SolverList)
        Cols = len(self.analysis.MaterialList)
        bodies = []
        for j in range(Cols):
            body = ""
            for i in range(Rows):
                label = str(i+2) + "X" + str(j+2)
                if label in self.analysis.MappingList:
                    body = self.analysis.MaterialList[j].Label
                    break
            if "Material" in body:
                bodies.append(body)
        return bodies

    def _rename(self, name):
        obj = self.analysis.Mesh
        boundaryCount = len(obj.MeshGroupList)
        prefix = "body"
        if name[0:5] == "Solid":
            prefix = "body" + str(int(name[5:]) + boundaryCount)
        elif name[0:4] == "Face":
            prefix = "body" + str(int(name[4:]) + boundaryCount)
        else:
            prefix = name
        return prefix

    def _getMeshDimension(self):
        #obj = self._getSingleMember("Fem::FemMeshObject")
        obj = self.analysis.Mesh
        if "3D" in obj.ElementDimension:
            return 3
        elif "2D" in obj.ElementDimension:
            return 2
        elif "1D" in obj.ElementDimension:
            return 1
        elif obj.Part.Shape.Solids:
            return 3
        elif obj.Part.Shape.Faces:
            return 2
        elif obj.Part.Shape.Edges:
            return 1
        return None

    def _addOutputSolver(self):
        s = sifio.createSection(sifio.SOLVER)
        s["Equation"] = "ResultOutput"
        s["Exec Solver"] = "After simulation"
        s["Procedure"] = sifio.FileAttr("ResultOutputSolve/ResultOutputSolver")
        s["Output File Name"] = sifio.FileAttr("case")
        s["Vtu Format"] = True
        for name in self._getAllBodies():
            self._addSolver(name, s)

    def _writeSif(self):
        sifPath = os.path.join(self.directory, _SIF_NAME)
        with open(sifPath, "w") as fstream:
            sif = sifio.Sif(self._builder)
            sif.write(fstream)

    def _handled(self, obj):
        self._handledObjects.add(obj)

    def _simulation(self, key, attr):
        self._builder.simulation(key, attr)

    def _constant(self, key, attr):
        self._builder.constant(key, attr)

    def _initial(self, body, key, attr):
        self._builder.initial(body, key, attr)

    def _material(self, body, key, attr):
        self._builder.material(body, key, attr)

    def _equation(self, body, key, attr):
        self._builder.equation(body, key, attr)

    def _bodyForce(self, body, key, attr):
        self._builder.bodyForce(body, key, attr)

    def _addSolver(self, body, solverSection):
        self._builder.addSolver(body, solverSection)

    def _boundary(self, boundary, key, attr):
        self._builder.boundary(boundary, key, attr)

    def _addSection(self, section):
        self._builder.addSection(section)

    def _getMember(self, t):
        return femutils.get_member(self.analysis, t)

    def _getSingleMember(self, t):
        return femutils.get_single_member(self.analysis, t)


class WriteError(Exception):
    pass

##  @}
