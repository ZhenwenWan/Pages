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

__title__ = "FreeCAD MySimII Simulation ViewProvider for the document object"
__author__ = "Bernd Hahnebach, Qingfeng Xia, Zhenwen Wan"
__url__ = "http://www.freecadweb.org"

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

False if MySimIIGui.__name__ else True  # flake8, dummy MySimIIGui usage


class _ViewProviderMySimIISimulation:
    "A View Provider for the MySimIISimulation object"

    def __init__(self, vobj):
        vobj.Proxy = self

    def getIcon(self):
        return ":/icons/MySimIISimulation.svg"

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
        taskd = _TaskPanelMySimIISimulation(self.Object)
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


class _TaskPanelMySimIISimulation:
    """The TaskPanel for editing References property of MySimIISimulation objects"""

    def __init__(self, obj):

        self.obj = obj
        self.SimulationTypeList = ["Empty", "Steady", "Transient", "Scanning"]
        self.TimeSteppingMethodList = ["Empty", "BDF", "Runge-Kutta", "Implicit Euler", "newmark"]
        # parameter widget
        self.form = FreeCADGui.PySideUic.loadUi(
            FreeCAD.getHomePath() + "Mod/MySimII/Resources/ui/Simulation.ui"
        )
        self.init_form()
        self.form.ComboBox_Type.activated.connect(self.ComboBox_Type_changed)
        self.form.ComboBox_TimeSteppingMethod.activated.connect(self.ComboBox_TimeSteppingMethod_changed)
        self.form.lineEdit_Starting.textChanged.connect(self.lineEdit_Starting_changed)
        self.form.lineEdit_Ending.textChanged.connect(self.lineEdit_Ending_changed)
        self.form.lineEdit_Step.textChanged.connect(self.lineEdit_Step_changed)
        self.form.lineEdit_MinIterSteps.textChanged.connect(self.lineEdit_MinIterSteps_changed)
        self.form.lineEdit_MaxIterSteps.textChanged.connect(self.lineEdit_MaxIterSteps_changed)
        self.form.lineEdit_BDF_Order.textChanged.connect(self.lineEdit_BDF_Order_changed)
        self.form.lineEdit_OutputSteps.textChanged.connect(self.lineEdit_OutputSteps_changed)
        self.form.lineEdit_ScalFactor.textChanged.connect(self.lineEdit_ScalFactor_changed)

    def init_form(self):
        #Simulation type list
        self.form.ComboBox_Type.clear()
        self.form.ComboBox_Type.addItems(self.SimulationTypeList)
        index = self.form.ComboBox_Type.findText(self.obj.SimulationType)
        self.form.ComboBox_Type.setCurrentIndex(index)
        #TimeSteppingMethod list
        self.form.ComboBox_TimeSteppingMethod.clear()
        self.form.ComboBox_TimeSteppingMethod.addItems(self.TimeSteppingMethodList)
        index = self.form.ComboBox_TimeSteppingMethod.findText(self.obj.TimeSteppingMethod)
        self.form.ComboBox_TimeSteppingMethod.setCurrentIndex(index)
        self.form.lineEdit_Starting.setText(str(self.obj.Start))
        self.form.lineEdit_Ending.setText(str(self.obj.End))
        self.form.lineEdit_Step.setText(str(self.obj.Step))
        self.form.lineEdit_MinIterSteps.setText(str(self.obj.SteadyStateMinIterations))
        self.form.lineEdit_MaxIterSteps.setText(str(self.obj.SteadyStateMaxIterations))
        self.form.lineEdit_BDF_Order.setText(str(self.obj.BDF_Order))
        self.form.lineEdit_OutputSteps.setText(str(self.obj.I_output))
        self.form.lineEdit_ScalFactor.setText(str(self.obj.Scal_fact))


    def ComboBox_Type_changed(self,index):
        self.obj.SimulationType = self.SimulationTypeList[index]

    def ComboBox_TimeSteppingMethod_changed(self,index):
        self.obj.TimeSteppingMethod = self.TimeSteppingMethodList[index]

    def lineEdit_Starting_changed(self, text):
        self.obj.Start = float(text)

    def lineEdit_Ending_changed(self, text):
        self.obj.End = float(text)

    def lineEdit_Step_changed(self, text):
        self.obj.Step = float(text)

    def lineEdit_OutputSteps_changed(self, text):
        self.obj.I_output = int(text)

    def lineEdit_MinIterSteps_changed(self, text):
        self.obj.SteadyStateMinIterations = int(text)

    def lineEdit_MaxIterSteps_changed(self, text):
        self.obj.SteadyStateMaxIterations = int(text)

    def lineEdit_BDF_Order_changed(self, text):
        self.obj.BDF_Order = int(text)

    def lineEdit_ScalFactor_changed(self, text):
        self.obj.Scal_fact = float(text)

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


