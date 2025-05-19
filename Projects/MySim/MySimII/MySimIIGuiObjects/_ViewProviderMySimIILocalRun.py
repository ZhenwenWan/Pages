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

__title__ = "FreeCAD MySimII mesh boundary layer ViewProvider for the document object"
__author__ = "Bernd Hahnebach, Qingfeng Xia"
__url__ = "http://www.freecadweb.org"

## @package ViewProviderMySimIILocalRun
#  \ingroup MySimII
#  \brief FreeCAD MySimII _ViewProviderMySimIILocalRun

import sys
import os
from os import path
import subprocess

import FreeCAD
import FreeCADGui
import MySimIIGui  # needed to display the icons in TreeView

# for the panel
from PySide import QtCore
from PySide import QtGui
from PySide.QtCore import Qt
from PySide.QtGui import QApplication
import time
from . import FemSelectionWidgets
from MySimIItools.PartsCheck import PartsCheck
zw_checked   = QtCore.Qt.CheckState.Checked
zw_unchecked = QtCore.Qt.CheckState.Unchecked

False if MySimIIGui.__name__ else True  # flake8, dummy MySimIIGui usage


class _ViewProviderMySimIILocalRun:
    "A View Provider for the MySimIILocalRun object"

    def __init__(self, vobj):
        vobj.Proxy = self

    def getIcon(self):
        return ":/icons/MySimIIWorkbench.svg"

    def attach(self, vobj):
        from pivy import coin
        self.ViewObject = vobj
        self.Object = vobj.Object
        self.standard = coin.SoGroup()
        vobj.addDisplayMode(self.standard, "Default")

    def getDisplayModes(self, obj):
        return ["Default"]

    def getDefaultDisplayMode(self):
        return "Default"

    def updateData(self, obj, prop):
        return

    def onChanged(self, vobj, prop):
        return

    def setEdit(self, vobj, mode=0):
        # hide all es
        for o in FreeCAD.ActiveDocument.Objects:
            if o.isDerivedFrom("MySimII::MySimIILocalRunObject") and not (o == self.Object):
                o.ViewObject.hide()
        # show the  we like to edit
        self.ViewObject.show()
        # show task panel
        taskd = _TaskPanelMySimIILocalRun(self.Object)
        taskd.obj = vobj.Object
        FreeCADGui.Control.showDialog(taskd)
        return True

    def unsetEdit(self, vobj, mode=0):
        FreeCADGui.Control.closeDialog()
        #self.ViewObject.hide()
        return True

    def doubleClicked(self, vobj):
        guidoc = FreeCADGui.getDocument(vobj.Object.Document)
        # check if another VP is in edit mode
        # https://forum.freecadweb.org/viewtopic.php?t=13077#p104702
        if not guidoc.getInEdit():
            guidoc.setEdit(vobj.Object.Name)
        else:
            from PySide.QtGui import QMessageBox
            message = "Active Task Dialog found! Please close this one before opening  a new one!"
            QMessageBox.critical(None, "Error in tree view", message)
            FreeCAD.Console.PrintError(message + "\n")
        return True

    def __getstate__(self):
        return None

    def __setstate__(self, state):
        return None

    def claimChildren(self):
        lst = self.Object.MaterialList + self.Object.BoundaryList + self.Object.ForcingList + self.Object.InitialList + self.Object.SolverList
        lst.append(self.Object.Simulation)
        lst.append(self.Object.Mesh)
        return lst

    def onDelete(self, feature, subelements):
        try:
            for obj in self.claimChildren():
                obj.ViewObject.show()
        except Exception as err:
            FreeCAD.Console.PrintError("Error in onDelete: " + err.message)
        return True

    def canDragObjects(self):
        return True

    def canDropObjects(self):
        return True

    def canDragObject(self, dragged_object):
        if hasattr(dragged_object, "Proxy") and dragged_object.Proxy.Type == "MySimII::FeaturePython":
            return True
        elif hasattr(dragged_object, "Proxy") and dragged_object.Proxy.Type == "Fem::FemMeshObjectPython":
            return True
        elif hasattr(dragged_object, "Proxy") and dragged_object.Proxy.Type == "Fem::FemSolverObjectPython":
            return True
        else:
            return False

    def canDropObject(self, incoming_object):
        return True

    def dragObject(self, selfvp, dragged_object):
        label = str(dragged_object.Label)
        if hasattr(dragged_object, "Proxy") and dragged_object.Proxy.Type == "MySimII::FeaturePython":
            if "Material" in label:
                objs = self.Object.MaterialList
                objs.remove(dragged_object)
                self.Object.MaterialList = objs
            elif "Boundary" in label:
                objs = self.Object.BoundaryList
                objs.remove(dragged_object)
                self.Object.BoundaryList = objs
            elif "Initiative" in label:
                objs = self.Object.InitialList
                objs.remove(dragged_object)
                self.Object.InitialList = objs
            elif "Forcing" in label:
                objs = self.Object.ForcingList
                objs.remove(dragged_object)
                self.Object.ForcingList = objs
            elif "Solver" in label:
                self.Object.Solver = dragged_object
        elif hasattr(dragged_object, "Proxy") and dragged_object.Proxy.Type == "Fem::FemMeshObjectPython":
            self.Object.Mesh = dragged_object
        elif hasattr(dragged_object, "Proxy") and dragged_object.Proxy.Type == "Fem::FemSolverObjectPython":
            self.Object.Solver = dragged_object

    def dropObject(self, selfvp, incoming_object):
        label = str(incoming_object.Label)
        if hasattr(incoming_object, "Proxy") and incoming_object.Proxy.Type == "MySimII::FeaturePython":
            if "Material" in label:
                objs = self.Object.MaterialList
                objs.append(incoming_object)
                self.Object.MaterialList = objs
            elif "Boundary" in label:
                objs = self.Object.BoundaryList
                objs.append(incoming_object)
                self.Object.BoundaryList = objs
            elif "Initiative" in label:
                objs = self.Object.InitialList
                objs.append(incoming_object)
                self.Object.InitialList = objs
            elif "Forcing" in label:
                objs = self.Object.ForcingList
                objs.append(incoming_object)
                self.Object.ForcingList = objs
            elif "Solver" in label:
                self.Object.Solver = incoming_object
        elif hasattr(incoming_object, "Proxy") and incoming_object.Proxy.Type == "Fem::FemMeshObjectPython":
            self.Object.Mesh = incoming_object
        elif hasattr(incoming_object, "Proxy") and incoming_object.Proxy.Type == "Fem::FemSolverObjectPython":
            self.Object.Solver = incoming_object




class _TaskPanelMySimIILocalRun:
    """The TaskPanel for editing References property of MySimIILocalRun objects"""

    def __init__(self, obj):

        self.obj = obj
        self.MaterialList = self.obj.MaterialList
        self.BoundaryList = self.obj.BoundaryList
        self.ForcingList = self.obj.ForcingList
        self.InitialList = self.obj.InitialList
        self.SolverList = self.obj.SolverList
        self.MappingList = self.obj.MappingList
        self.Mesh = self.obj.Mesh
        # parameter widget
        self.form1 = FreeCADGui.PySideUic.loadUi(
            FreeCAD.getHomePath() + "Mod/MySimII/Resources/ui/LocalRun1.ui"
        )
        self.form2 = FreeCADGui.PySideUic.loadUi(
            FreeCAD.getHomePath() + "Mod/MySimII/Resources/ui/LocalRun2.ui"
        )
        self.form3 = FreeCADGui.PySideUic.loadUi(
            FreeCAD.getHomePath() + "Mod/MySimII/Resources/ui/LocalRun3.ui"
        )
        self.form4 = FreeCADGui.PySideUic.loadUi(
            FreeCAD.getHomePath() + "Mod/MySimII/Resources/ui/LocalRun4.ui"
        )
        self.init_form()
        self.form2.tw_Boundary.itemChanged.connect(self.tw_Boundary_changed)
        self.form3.tw_Forcing.itemChanged.connect(self.tw_Forcing_changed)
        self.form4.tw_Initializing.itemChanged.connect(self.tw_Initializing_changed)
        self.form1.tw_Solver.itemChanged.connect(self.tw_Solver_changed)
        self.form1.comb_Mesh.currentIndexChanged.connect(self.comb_Mesh_changed)
        self.form1.comb_Simulation.currentIndexChanged.connect(self.comb_Simulation_changed)
        self.form = [self.form1, self.form2, self.form3, self.form4]

    def init_form(self):
        #Solver-Material mapping
        self.SolverList = []
        for obj in FreeCAD.ActiveDocument.Objects:
            if obj.isDerivedFrom("Fem::FemSolverObjectPython") and "Solver" in str(obj.Label):
                self.SolverList.append(obj)
        self.MaterialList = []
        for obj in FreeCAD.ActiveDocument.Objects:
            if obj.isDerivedFrom("MySimII::FeaturePython") and "Material" in str(obj.Label):
                self.MaterialList.append(obj)
        Cols = len(self.MaterialList)
        if Cols == 0:
            FreeCAD.Console.PrintError(" No material has been defined yet. \n")
        Rows = len(self.SolverList)
        if Rows == 0:
            FreeCAD.Console.PrintError(" No solver has been defined yet. \n")
        self.form1.tw_Solver.clear()
        self.form1.tw_Solver.setRowCount(Rows+1)
        self.form1.tw_Solver.setColumnCount(Cols+1)
        for i in range(Rows):
            for j in range(Cols):
                label = str(i+2)+"X"+str(j+2)
                item  = QtGui.QTableWidgetItem(label)
                if label in self.obj.MappingList:
                    item.setCheckState(zw_checked)
                else:
                    item.setCheckState(zw_unchecked)
                self.form1.tw_Solver.setItem(i+1,j+1,item)
        for i in range(Rows):
            obj = self.SolverList[i]
            item = QtGui.QTableWidgetItem(str(obj.Label))
            self.form1.tw_Solver.setItem(i+1,0,item)
        for j in range(Cols):
            obj = self.MaterialList[j]
            item = QtGui.QTableWidgetItem(str(obj.Label))
            self.form1.tw_Solver.setItem(0,j+1,item)
        
        #Boundary
        Rows = 0
        for obj in FreeCAD.ActiveDocument.Objects:
            if obj.isDerivedFrom("MySimII::FeaturePython") and "Boundary" in str(obj.Label):
                Rows += 1
        self.form2.tw_Boundary.clear()
        self.form2.tw_Boundary.setRowCount(Rows)
        self.form2.tw_Boundary.setColumnCount(1)
        Rows = 0
        for obj in FreeCAD.ActiveDocument.Objects:
            if obj.isDerivedFrom("MySimII::FeaturePython") and "Boundary" in str(obj.Label):
                item = QtGui.QTableWidgetItem(str(obj.Label))
                if obj in self.obj.BoundaryList:
                    item.setCheckState(zw_checked)
                else:
                    item.setCheckState(zw_unchecked)
                self.form2.tw_Boundary.setItem(Rows,0,item)
                Rows += 1
        
        #Forcing
        Rows = 0
        for obj in FreeCAD.ActiveDocument.Objects:
            if obj.isDerivedFrom("MySimII::FeaturePython") and "Forcing" in str(obj.Label):
                Rows += 1
        self.form3.tw_Forcing.clear()
        self.form3.tw_Forcing.setRowCount(Rows)
        self.form3.tw_Forcing.setColumnCount(1)
        Rows = 0
        for obj in FreeCAD.ActiveDocument.Objects:
            if obj.isDerivedFrom("MySimII::FeaturePython") and "Forcing" in str(obj.Label):
                item = QtGui.QTableWidgetItem(str(obj.Label))
                if obj in self.obj.ForcingList:
                    item.setCheckState(zw_checked)
                else:
                    item.setCheckState(zw_unchecked)
                self.form3.tw_Forcing.setItem(Rows,0,item)
                Rows += 1

        #Initializing
        Rows = 0
        for obj in FreeCAD.ActiveDocument.Objects:
            if obj.isDerivedFrom("MySimII::FeaturePython") and "Initiative" in str(obj.Label):
                Rows += 1
        self.form4.tw_Initializing.clear()
        self.form4.tw_Initializing.setRowCount(Rows)
        self.form4.tw_Initializing.setColumnCount(1)
        Rows = 0
        for obj in FreeCAD.ActiveDocument.Objects:
            if obj.isDerivedFrom("MySimII::FeaturePython") and "Initiative" in str(obj.Label):
                item = QtGui.QTableWidgetItem(str(obj.Label))
                if obj in self.obj.InitialList:
                    item.setCheckState(zw_checked)
                else:
                    item.setCheckState(zw_unchecked)
                self.form4.tw_Initializing.setItem(Rows,0,item)
                Rows += 1
        
        #Mesh
        self.form1.comb_Mesh.clear()
        for obj in FreeCAD.ActiveDocument.Objects:
            if obj.isDerivedFrom("Fem::FemMeshObjectPython"):
                self.form1.comb_Mesh.addItem(str(obj.Label))
        if(self.form1.comb_Mesh.count()==0):
            FreeCAD.Console.PrintMessage("No MESH has been configured yet!!! \n")
        if self.obj.Mesh:
            index = self.form1.comb_Mesh.findText(self.obj.Mesh.Label)
            self.form1.comb_Mesh.setCurrentIndex(index)

        #Simulation
        self.form1.comb_Simulation.clear()
        for obj in FreeCAD.ActiveDocument.Objects:
            if obj.isDerivedFrom("Fem::FemSolverObjectPython") and obj.Label[0:6] == "Simula":
                self.form1.comb_Simulation.addItem(str(obj.Label))
        if(self.form1.comb_Simulation.count()==0):
            FreeCAD.Console.PrintMessage("No SIMULATION has been configured yet!!! \n")
        if self.obj.Simulation:
            index = self.form1.comb_Simulation.findText(self.obj.Simulation.Label)
            self.form1.comb_Simulation.setCurrentIndex(index)

    def tw_Solver_changed(self,item):
        text = item.text()
        stat = item.checkState()
        if stat == QtCore.Qt.CheckState.Checked:
            if text not in self.MappingList:
                self.MappingList.append(text)
        if stat == QtCore.Qt.CheckState.Unchecked:
            if text in self.MappingList:
               self.MappingList.remove(text)

    def tw_Boundary_changed(self,item):
        text = item.text()
        obj = FreeCAD.ActiveDocument.getObject(text)
        stat = item.checkState()
        if stat == QtCore.Qt.CheckState.Checked:
            if obj not in self.BoundaryList:
                self.BoundaryList.append(obj)
        if stat == QtCore.Qt.CheckState.Unchecked:
            if obj in self.BoundaryList:
                self.BoundaryList.remove(obj)

    def tw_Forcing_changed(self,item):
        text = item.text()
        obj = FreeCAD.ActiveDocument.getObject(text)
        stat = item.checkState()
        if stat == QtCore.Qt.CheckState.Checked:
            if obj not in self.ForcingList:
                self.ForcingList.append(obj)
        if stat == QtCore.Qt.CheckState.Unchecked:
            if obj in self.ForcingList:
                self.ForcingList.remove(obj)

    def tw_Initializing_changed(self,item):
        text = item.text()
        obj = FreeCAD.ActiveDocument.getObject(text)
        stat = item.checkState()
        if stat == QtCore.Qt.CheckState.Checked:
            if obj not in self.InitialList:
                self.InitialList.append(obj)
        if stat == QtCore.Qt.CheckState.Unchecked:
            if obj in self.InitialList:
                self.InitialList.remove(obj)

    def comb_Mesh_changed(self,index):
        text = self.form1.comb_Mesh.currentText()
        obj = FreeCAD.ActiveDocument.getObject(text)
        self.obj.Mesh = obj

    def comb_Simulation_changed(self,index):
        text = self.form1.comb_Simulation.currentText()
        obj = FreeCAD.ActiveDocument.getObject(text)
        self.obj.Simulation = obj

    def accept(self):
        #collecting all configuration information
        self.obj.MaterialList = self.MaterialList
        self.obj.BoundaryList = self.BoundaryList
        self.obj.ForcingList  = self.ForcingList
        self.obj.InitialList  = self.InitialList
        self.obj.SolverList   = self.SolverList
        text                  = self.form1.comb_Mesh.currentText()
        self.obj.Mesh         = FreeCAD.ActiveDocument.getObject(text)
        text                  = self.form1.comb_Simulation.currentText()
        self.obj.Simulation   = FreeCAD.ActiveDocument.getObject(text)
        self.obj.MappingList  = self.MappingList

        #collecting all objects
        Group = [] 
        for obj in self.obj.MaterialList:
            Group.append(obj)
        for obj in self.obj.BoundaryList:
            Group.append(obj)
        for obj in self.obj.ForcingList:
            Group.append(obj)
        for obj in self.obj.InitialList:
            Group.append(obj)
        for obj in self.obj.SolverList:
            Group.append(obj)
        Group.append(self.obj.Simulation)
        Group.append(self.obj.Mesh)
        self.obj.Group = Group

        #check the validation of inputs
        if not self.isInputsValid():
            return

        WD = FreeCAD.getHomePath() + "data/Mesh_dir"
        # check the existance of work directory
        if not path.isdir(WD):
            FreeCAD.Console.PrintMessage(" Failed to locate the work directory.\n")
        
        #import class to write SIF and MeshFile
        from MySimIIsolver.elmer.writer import Writer as writemod
        writeobj = writemod(self.obj, WD)
        writeobj.write()

        # check the existance of mesh files
        MeshFile = WD + "/Mesh_dir/mesh.header"
        if not path.isfile(MeshFile):
            FreeCAD.Console.PrintMessage(" Failed to locate the mesh files.\n")

        # check the existance of SIF file
        SIF = WD + "/case.sif"
        if not path.isfile(SIF):
            FreeCAD.Console.PrintMessage(" Failed to locate the case.sif.\n")

        # check the existance of ElmerSolver.exe
        binary = os.getenv('ELMER_HOME') + "/bin/ElmerSolver.exe"
        if not path.isfile(binary):
            FreeCAD.Console.PrintMessage(" Failed to locate ElmerSolver.exe .\n")

        #starts to run the simulation
        self.run_Elmer([binary, SIF])

        self.recompute_and_set_back_all()
        return True

    def run_Elmer(self, commandlist):
        FreeCAD.Console.PrintError(" ".join(commandlist) + "\n")
        try:
            p = subprocess.Popen(commandlist, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            output, error = p.communicate()
            FreeCAD.Console.PrintMessage(output)
            FreeCAD.Console.PrintMessage(error)
            success = 'Successfully executed: {}\n'.format(" ".join(commandlist))
            FreeCAD.Console.PrintMessage(success)
        except:
            error = 'Error executing: {}\n'.format(" ".join(commandlist))
            FreeCAD.Console.PrintError(error)
            self.error = True

    def reject(self):
        self.recompute_and_set_back_all()
        return True

    def isInputsValid(self):
        # check the Material
        status = True
        if len(self.obj.MaterialList) == 0:
            FreeCAD.Console.PrintMessage("Error: MATERIAL has NOT been configured yet!!! \n")
            status = False

        # check the simulation
        status = True
        if not self.obj.Simulation:
            FreeCAD.Console.PrintMessage("Error: SIMULATION has NOT been configured yet!!! \n")
            status = False

        # check the equation
        status = True
        for solver in self.obj.SolverList:
            if not solver.Equation:
                FreeCAD.Console.PrintMessage("Error: EQUATION has NOT been configured yet!!! \n")
                status = False

        # check the existence of mesh file
        if not self.obj.Mesh:
            FreeCAD.Console.PrintMessage("Error: MESH has NOT been configured yet!!! \n")
            status = False
        return status

    def run_gmsh(self):
        QApplication.setOverrideCursor(Qt.WaitCursor)
        part = self.obj.Part
        if self.obj.MeshRegionList:
            #  other part obj might not have a Proxy, thus an exception would be raised
            if part.Shape.ShapeType == "Compound" and hasattr(part, "Proxy"):
                if part.Proxy.Type == "FeatureBooleanFragments" \
                        or part.Proxy.Type == "FeatureSlice" \
                        or part.Proxy.Type == "FeatureXOR":
                    error_message = (
                        "The shape to mesh is a boolean split tools Compound "
                        "and the mesh has mesh region list. "
                        "Gmsh could return unexpected meshes in such circumstances. "
                        "It is strongly recommended to extract the shape "
                        "to mesh from the Compound and use this one."
                    )
                    qtbox_title = (
                        "Shape to mesh is a BooleanFragmentsCompound "
                        "and mesh regions are defined"
                    )
                    QtGui.QMessageBox.critical(
                        None,
                        qtbox_title,
                        error_message
                    )
        self.gmsh_runs = True
        import femmesh.gmshtools as gmshtools
        gmsh_mesh = gmshtools.GmshTools(self.obj)
        error = ""
        try:
            error = gmsh_mesh.create_mesh()
        except:
            import sys
            print("Unexpected error when creating mesh: ", sys.exc_info()[0])
        if error:
            print(error)
        QApplication.restoreOverrideCursor()

    def recompute_and_set_back_all(self):
        doc = FreeCADGui.getDocument(self.obj.Document)
        doc.Document.recompute()
        doc.resetEdit()


