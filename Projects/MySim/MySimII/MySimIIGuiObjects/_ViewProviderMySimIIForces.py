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

__title__ = "FreeCAD MySimII mesh force layer ViewProvider for the document object"
__author__ = "Bernd Hahnebach, Qingfeng Xia"
__url__ = "http://www.freecadweb.org"

## @package ViewProviderMySimIIForces
#  \ingroup MySimII
#  \brief FreeCAD MySimII _ViewProviderMySimIIForces

import FreeCAD
import FreeCADGui
import MySimIIGui  # needed to display the icons in TreeView
False if False else MySimIIGui.__name__  # flake8, dummy MySimIIGui usage

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



class _ViewProviderMySimIIForces:
    "A View Provider for the MySimIIForces object"

    def __init__(self, vobj):
        vobj.Proxy = self

    def getIcon(self):
        return ":/icons/MySimII-MySimIImesh-force-layer.svg"

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
            if o.isDerivedFrom("Fem::FemMeshObject"):
                o.ViewObject.hide()
        # show task panel
        taskd = _TaskPanelMySimIIForces(self.Object)
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


class _TaskPanelMySimIIForces:
    """The TaskPanel for editing References property of MySimIIForces objects"""

    def __init__(self, obj):

        self.obj = obj

        self.SubdomainsList = self.obj.Subdomains

        # parameter widget
        self.parameterWidget = FreeCADGui.PySideUic.loadUi(
            FreeCAD.getHomePath() + "Mod/MySimII/Resources/ui/Forces.ui"
        )
        self.form = self.parameterWidget
        self.parameterWidget_init()
        #CFD
        self.parameterWidget.lineEdit_ForceX.textChanged.connect(self.ForceX_textChanged)
        self.parameterWidget.lineEdit_ForceY.textChanged.connect(self.ForceY_textChanged)
        self.parameterWidget.lineEdit_ForceZ.textChanged.connect(self.ForceZ_textChanged)
        self.parameterWidget.checkBox_Floating.stateChanged.connect(self.Floating_stateChanged)
        #Heat transfer
        self.parameterWidget.lineEdit_HeatSource.textChanged.connect(self.HeatSource_textChanged)
        self.parameterWidget.lineEdit_TemperatureLowest.textChanged.connect(self.TemperatureLowest_textChanged)
        self.parameterWidget.lineEdit_TemperatureHighest.textChanged.connect(self.TemperatureHighest_textChanged)
        self.parameterWidget.checkBox_SmartHeat.stateChanged.connect(self.SmartHeat_stateChanged)
        #EM wave
        self.parameterWidget.lineEdit_CurrentDensityRealX.textChanged.connect(self.CurrentDensityRealX_textChanged)
        self.parameterWidget.lineEdit_CurrentDensityRealY.textChanged.connect(self.CurrentDensityRealY_textChanged)
        self.parameterWidget.lineEdit_CurrentDensityRealZ.textChanged.connect(self.CurrentDensityRealZ_textChanged)
        self.parameterWidget.lineEdit_CurrentDensityImagX.textChanged.connect(self.CurrentDensityImagX_textChanged)
        self.parameterWidget.lineEdit_CurrentDensityImagY.textChanged.connect(self.CurrentDensityImagY_textChanged)
        self.parameterWidget.lineEdit_CurrentDensityImagZ.textChanged.connect(self.CurrentDensityImagZ_textChanged)
        #Elastic
        self.parameterWidget.LE_MassForce1.textChanged.connect(self.MassForce1_textChanged)
        self.parameterWidget.LE_MassForce2.textChanged.connect(self.MassForce2_textChanged)
        self.parameterWidget.LE_MassForce3.textChanged.connect(self.MassForce3_textChanged)
        self.parameterWidget.LE_VolumeForce1.textChanged.connect(self.VolumeForce1_textChanged)
        self.parameterWidget.LE_VolumeForce2.textChanged.connect(self.VolumeForce2_textChanged)
        self.parameterWidget.LE_VolumeForce3.textChanged.connect(self.VolumeForce3_textChanged)
        #self definition
        self.parameterWidget.textEdit_SelfDef1.textChanged.connect(self.SelfDef1_textChanged)
        self.parameterWidget.textEdit_SelfDef2.textChanged.connect(self.SelfDef2_textChanged)
        self.form.tw_Subdomains.itemChanged.connect(self.tw_Subdomains_changed)

    def parameterWidget_init(self):
        #CFD
        self.parameterWidget.lineEdit_ForceX.setText(str(self.obj.ForceX))
        self.parameterWidget.lineEdit_ForceY.setText(str(self.obj.ForceY))
        self.parameterWidget.lineEdit_ForceZ.setText(str(self.obj.ForceZ))
        if self.obj.Floating:
            self.parameterWidget.checkBox_Floating.setCheckState(QtCore.Qt.CheckState.Checked)
        else:
            self.parameterWidget.checkBox_Floating.setCheckState(QtCore.Qt.CheckState.Unchecked)
        #Heat
        self.parameterWidget.lineEdit_HeatSource.setText(str(self.obj.HeatSource))
        self.parameterWidget.lineEdit_TemperatureLowest.setText(str(self.obj.TemperatureLowest))
        self.parameterWidget.lineEdit_TemperatureHighest.setText(str(self.obj.TemperatureHighest))
        if self.obj.SmartHeat:
            self.parameterWidget.checkBox_SmartHeat.setCheckState(QtCore.Qt.CheckState.Checked)
        else:
            self.parameterWidget.checkBox_SmartHeat.setCheckState(QtCore.Qt.CheckState.Unchecked)
        #EM wave
        self.parameterWidget.lineEdit_CurrentDensityRealX.setText(str(self.obj.CurrentDensityRealX))
        self.parameterWidget.lineEdit_CurrentDensityRealY.setText(str(self.obj.CurrentDensityRealY))
        self.parameterWidget.lineEdit_CurrentDensityRealZ.setText(str(self.obj.CurrentDensityRealZ))
        self.parameterWidget.lineEdit_CurrentDensityImagX.setText(str(self.obj.CurrentDensityImagX))
        self.parameterWidget.lineEdit_CurrentDensityImagY.setText(str(self.obj.CurrentDensityImagY))
        self.parameterWidget.lineEdit_CurrentDensityImagZ.setText(str(self.obj.CurrentDensityImagZ))
        #self definition
        self.parameterWidget.textEdit_SelfDef1.setText(self.obj.SelfDef1)
        self.parameterWidget.textEdit_SelfDef2.setText(self.obj.SelfDef2)
        #Elastic
        self.parameterWidget.LE_MassForce1.setText(str(self.obj.MassForce1))
        self.parameterWidget.LE_MassForce2.setText(str(self.obj.MassForce2))
        self.parameterWidget.LE_MassForce3.setText(str(self.obj.MassForce3))
        self.parameterWidget.LE_VolumeForce1.setText(str(self.obj.VolumeForce1))
        self.parameterWidget.LE_VolumeForce2.setText(str(self.obj.VolumeForce2))
        self.parameterWidget.LE_VolumeForce3.setText(str(self.obj.VolumeForce3))
        #Subdomain list
        Rows = 0
        for obj in FreeCAD.ActiveDocument.Objects:
            if obj.isDerivedFrom("MySimII::FeaturePython") and "Material" in str(obj.Label):
                Rows += 1
        self.form.tw_Subdomains.clear()
        self.form.tw_Subdomains.setRowCount(Rows)
        self.form.tw_Subdomains.setColumnCount(1)
        Rows = 0
        for obj in FreeCAD.ActiveDocument.Objects:
            if obj.isDerivedFrom("MySimII::FeaturePython") and "Material" in str(obj.Label):
                item = QtGui.QTableWidgetItem(str(obj.Label))
                if obj in self.obj.Subdomains:
                    item.setCheckState(zw_checked)
                else:
                    item.setCheckState(zw_unchecked)
                self.form.tw_Subdomains.setItem(Rows,0,item)
                Rows += 1

    def ForceX_textChanged(self, text):
        self.obj.ForceX =  (text)

    def ForceY_textChanged(self, text):
        self.obj.ForceY =  (text)

    def ForceZ_textChanged(self, text):
        self.obj.ForceZ =  (text)

    def Floating_stateChanged(self, state):
        if state == QtCore.Qt.CheckState.Checked:
            self.obj.Floating =  True
        else:
            self.obj.Floating =  False

    def CurrentDensityRealX_textChanged(self, text):
        self.obj.CurrentDensityRealX =  (text)

    def CurrentDensityRealY_textChanged(self, text):
        self.obj.CurrentDensityRealY =  (text)

    def CurrentDensityRealZ_textChanged(self, text):
        self.obj.CurrentDensityRealZ =  (text)

    def CurrentDensityImagX_textChanged(self, text):
        self.obj.CurrentDensityImagX =  (text)

    def CurrentDensityImagY_textChanged(self, text):
        self.obj.CurrentDensityImagY =  (text)

    def CurrentDensityImagZ_textChanged(self, text):
        self.obj.CurrentDensityImagZ =  (text)

    def Pressure_textChanged(self, text):
        self.obj.Pressure =  (text)

    def MassForce1_textChanged(self, text):
        self.obj.MassForce1 =  (text)

    def MassForce2_textChanged(self, text):
        self.obj.MassForce2 =  (text)

    def MassForce3_textChanged(self, text):
        self.obj.MassForce3 =  (text)

    def VolumeForce1_textChanged(self, text):
        self.obj.VolumeForce1 =  (text)

    def VolumeForce2_textChanged(self, text):
        self.obj.VolumeForce2 =  (text)

    def VolumeForce3_textChanged(self, text):
        self.obj.VolumeForce3 =  (text)

    def HeatSource_textChanged(self, text):
        self.obj.HeatSource =  (text)

    def TemperatureLowest_textChanged(self, text):
        self.obj.TemperatureLowest =  (text)

    def TemperatureHighest_textChanged(self, text):
        self.obj.TemperatureHighest =  (text)

    def SmartHeat_stateChanged(self, state):
        if state == QtCore.Qt.CheckState.Checked:
            self.obj.SmartHeat =  True
        else:
            self.obj.SmartHeat =  False

    def SelfDef1_textChanged(self):
        self.obj.SelfDef1 =  self.parameterWidget.textEdit_SelfDef1.toPlainText()

    def SelfDef2_textChanged(self):
        self.obj.SelfDef2 =  self.parameterWidget.textEdit_SelfDef2.toPlainText()

    def tw_Subdomains_changed(self,item):
        text = item.text()
        obj = FreeCAD.ActiveDocument.getObject(text)
        stat = item.checkState()
        if stat == QtCore.Qt.CheckState.Checked:
            if obj not in self.SubdomainsList:
                self.SubdomainsList.append(obj)
        if stat == QtCore.Qt.CheckState.Unchecked:
            if obj in self.SubdomainsList:
                self.SubdomainsList.remove(obj)

    def accept(self):
        self.obj.Subdomains = self.SubdomainsList
        self.recompute_and_set_back_all()
        return True

    def reject(self):
        self.recompute_and_set_back_all()
        return True

    def recompute_and_set_back_all(self):
        doc = FreeCADGui.getDocument(self.obj.Document)
        doc.Document.recompute()
        doc.resetEdit()

