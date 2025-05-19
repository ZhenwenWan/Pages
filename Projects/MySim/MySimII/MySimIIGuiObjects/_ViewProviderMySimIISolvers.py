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

## @package ViewProviderMySimIISolvers
#  \ingroup MySimII
#  \brief FreeCAD MySimII _ViewProviderMySimIISolvers

import FreeCAD
import FreeCADGui
import FreeCADGui as Gui
import MySimIIGui  # needed to display the icons in TreeView

# for the panel
from PySide import QtCore
from PySide import QtGui
from PySide.QtCore import Qt
from PySide.QtGui import QApplication
import time
from . import FemSelectionWidgets
zw_checked   = QtCore.Qt.CheckState.Checked
zw_unchecked = QtCore.Qt.CheckState.Unchecked

False if MySimIIGui.__name__ else True  # flake8, dummy MySimIIGui usage


class _ViewProviderMySimIISolvers:
    "A View Provider for the MySimIISolvers object"

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
            if o.isDerivedFrom("Fem::FemSolverPythonObject") and not (o == self.Object):
                o.ViewObject.hide()
        # show the  we like to edit
        self.ViewObject.show()
        # show task panel
        taskd = _TaskPanelMySimIISolvers(self.Object)
        taskd.obj = vobj.Object
        FreeCADGui.Control.showDialog(taskd)
        return True

    def unsetEdit(self, vobj, mode=0):
        FreeCADGui.Control.closeDialog()
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


class _TaskPanelMySimIISolvers:
    """The TaskPanel for editing References property of MySimIISolvers objects"""

    def __init__(self, obj):

        self.obj = obj
        self.EquationList = ["Empty", "CFD", "Elastic", "EM", "Heat", "Heat+CFD", "MeshUpdate", "Flow2D", "CFT2D"]
        self.LinearDirectMethods = ["umfpack", "mumps", "banded", "permon"]
        self.LinearIterativeMethods = ["BiCGStabl", "CG", "GMRes", "boomeramg", "umfpack", "FlexGMRes", "Idres", "GCR", "QMR", "TFQMR", "CGS", "Jacobi", "SGS"]
        self.PreconditioningMethods = ["none", "vanka", "BoomerAMG", "parasails", "AMS", "diagonal", "ILU", "ILU0", "ILU1", "ILU2", "BILU", "BILU1"]
        self.BiCGStablDegree = ["2", "3", "4", "5", "6", "7", "8", "9"]
        self.ExecutionWhenCoupling = ["always", "after simulation", "before simulation", "before all", "after all", "before timestep", "never"]
        # parameter widget
        self.form = FreeCADGui.PySideUic.loadUi(
            FreeCAD.getHomePath() + "Mod/MySimII/Resources/ui/Solvers.ui"
        )
        self.init_form()
        #Solver options
        self.form.ComboBox_Equations.currentIndexChanged.connect(self.ComboBox_Equations_changed)
        self.form.CB_LinearDirectMethod.currentIndexChanged.connect(self.CB_LinearDirectMethods_changed)
        self.form.CB_LinearIterativeMethod.currentIndexChanged.connect(self.CB_LinearIterativeMethods_changed)
        self.form.CB_PreconditioningMethod.currentIndexChanged.connect(self.CB_PreconditioningMethods_changed)
        self.form.CB_BiCGStablDegree.currentIndexChanged.connect(self.CB_BiCGStablDegree_changed)
        self.form.CB_ExecutionWhenCoupling.currentIndexChanged.connect(self.CB_ExecutionWhenCoupling_changed)
        self.form.DirectiveSolver.toggled.connect(self.RadioButton_changed)
        #Iteration parameters
        self.form.LE_SteadyTolerance.textChanged.connect(   self.LE_SteadyTolerance_changed)
        self.form.LE_LinearMaxIter.textChanged.connect(     self.LE_LinearMaxIter_changed)
        self.form.LE_LinearTolerance.textChanged.connect(   self.LE_LinearTolerance_changed)
        self.form.LE_NonLinearTolerance.textChanged.connect(self.LE_NonLinearTolerance_changed)
        self.form.LE_NonLinearMaxIter.textChanged.connect(  self.LE_NonLinearMaxIter_changed)
        self.form.LE_NonLinearRelax.textChanged.connect(    self.LE_NonLinearRelax_changed)
        self.form.LE_NewtonTolerance.textChanged.connect(   self.LE_NewtonTolerance_changed)
        self.form.LE_NewtonStartIter.textChanged.connect(   self.LE_NewtonStartIter_changed)

    def init_form(self):
        #Solver list
        self.form.ComboBox_Equations.clear()
        self.form.ComboBox_Equations.addItems(self.EquationList)
        index = self.form.ComboBox_Equations.findText(self.obj.Equation)
        self.form.ComboBox_Equations.setCurrentIndex(index)
        
        self.form.CB_LinearDirectMethod.addItems(self.LinearDirectMethods)
        index = self.form.CB_LinearDirectMethod.findText(self.obj.LinearDirectMethod)
        self.form.CB_LinearDirectMethod.setCurrentIndex(index)
        
        self.form.CB_LinearIterativeMethod.addItems(self.LinearIterativeMethods)
        index = self.form.CB_LinearIterativeMethod.findText(self.obj.LinearIterativeMethod)
        self.form.CB_LinearIterativeMethod.setCurrentIndex(index)
        
        self.form.CB_PreconditioningMethod.addItems(self.PreconditioningMethods)
        index = self.form.CB_PreconditioningMethod.findText(self.obj.LinearPreconditioning)
        self.form.CB_PreconditioningMethod.setCurrentIndex(index)
        
        self.form.CB_BiCGStablDegree.addItems(self.BiCGStablDegree)
        index = self.form.CB_BiCGStablDegree.findText(str(self.obj.BiCGStablDegree))
        self.form.CB_BiCGStablDegree.setCurrentIndex(index)
        
        self.form.CB_ExecutionWhenCoupling.addItems(self.ExecutionWhenCoupling)
        index = self.form.CB_ExecutionWhenCoupling.findText(self.obj.ExecutionWhenCoupling)
        self.form.CB_ExecutionWhenCoupling.setCurrentIndex(index)
       
        if self.obj.LinearSolverType == "Direct":
            self.form.IterativeSolver.setChecked(False)
            self.form.DirectiveSolver.setChecked(True)
        elif self.obj.LinearSolverType == "Iterative":
            self.form.IterativeSolver.setChecked(True)
            self.form.DirectiveSolver.setChecked(False)

        self.form.LE_SteadyTolerance.setText(   str(self.obj.SteadyTolerance))
        self.form.LE_LinearMaxIter.setText(     str(self.obj.LinearMaxIter))
        self.form.LE_LinearTolerance.setText(   str(self.obj.LinearTolerance))
        self.form.LE_NonLinearMaxIter.setText(  str(self.obj.NonLinearMaxIter))
        self.form.LE_NonLinearTolerance.setText(str(self.obj.NonLinearTolerance))
        self.form.LE_NonLinearRelax.setText(    str(self.obj.NonLinearRelax))
        self.form.LE_NewtonStartIter.setText(   str(self.obj.NewtonStartIter))
        self.form.LE_NewtonTolerance.setText(   str(self.obj.NewtonTolerance))


    def ComboBox_Equations_changed(self,index):
        self.obj.Equation = self.EquationList[index] 

    def CB_LinearDirectMethods_changed(self,index):
        self.obj.LinearDirectMethod = self.LinearDirectMethods[index] 

    def CB_LinearIterativeMethods_changed(self,index):
        self.obj.LinearIterativeMethod = self.LinearIterativeMethods[index] 

    def CB_PreconditioningMethods_changed(self,index):
        self.obj.LinearPreconditioning = self.PreconditioningMethods[index] 

    def CB_BiCGStablDegree_changed(self,index):
        self.obj.BiCGStablDegree = int(self.BiCGStablDegree[index]) 

    def CB_ExecutionWhenCoupling_changed(self,index):
        self.obj.ExecutionWhenCoupling = self.ExecutionWhenCoupling[index] 

    def RadioButton_changed(self,checkstate):
        if checkstate:
            self.obj.LinearSolverType = "Direct"
        else:
            self.obj.LinearSolverType = "Iterative"

    def LE_SteadyTolerance_changed(self, text):
        self.obj.SteadyTolerance = float(text)

    def LE_LinearMaxIter_changed(self, text):
        self.obj.LinearMaxIter = int(text)

    def LE_LinearTolerance_changed(self, text):
        self.obj.LinearTolerance = float(text)

    def LE_NonLinearTolerance_changed(self, text):
        self.obj.NonLinearTolerance = float(text)

    def LE_NonLinearMaxIter_changed(self, text):
        self.obj.NonLinearMaxIter = int(text)

    def LE_NonLinearRelax_changed(self, text):
        self.obj.NonLinearRelax = float(text)

    def LE_NewtonStartIter_changed(self, text):
        self.obj.NewtonStartIter = int(text)

    def LE_NewtonTolerance_changed(self, text):
        self.obj.NewtonTolerance = float(text)

    def accept(self):
        self.recompute_and_set_back_all()
        return True

    def reject(self):
        self.init_form()
        self.recompute_and_set_back_all()
        return True

    def recompute_and_set_back_all(self):
        doc = FreeCADGui.getDocument(self.obj.Document)
        doc.Document.recompute()
        doc.resetEdit()


