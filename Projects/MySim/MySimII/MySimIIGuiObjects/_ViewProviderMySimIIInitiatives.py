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

__title__ = "FreeCAD MySimII mesh initiative layer ViewProvider for the document object"
__author__ = "Bernd Hahnebach, Qingfeng Xia"
__url__ = "http://www.freecadweb.org"

## @package ViewProviderMySimIIInitiatives
#  \ingroup MySimII
#  \brief FreeCAD MySimII _ViewProviderMySimIIInitiatives

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



class _ViewProviderMySimIIInitiatives:
    "A View Provider for the MySimIIInitiatives object"

    def __init__(self, vobj):
        vobj.Proxy = self

    def getIcon(self):
        return ":/icons/MySimII-MySimIImesh-initiative-layer.svg"

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
        taskd = _TaskPanelMySimIIInitiatives(self.Object)
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


class _TaskPanelMySimIIInitiatives:
    """The TaskPanel for editing References property of MySimIIInitiatives objects"""

    def __init__(self, obj):

        self.obj = obj

        self.SubdomainsList = self.obj.Subdomains

        # parameter widget
        self.parameterWidget = FreeCADGui.PySideUic.loadUi(
            FreeCAD.getHomePath() + "Mod/MySimII/Resources/ui/Initiatives.ui"
        )
        self.form = self.parameterWidget
        self.parameterWidget_init()
        #CFD
        self.parameterWidget.lineEdit_VelX.textChanged.connect(self.VelX_textChanged)
        self.parameterWidget.lineEdit_VelY.textChanged.connect(self.VelY_textChanged)
        self.parameterWidget.lineEdit_VelZ.textChanged.connect(self.VelZ_textChanged)
        self.parameterWidget.lineEdit_Pressure.textChanged.connect(self.Pressure_textChanged)
        #Heat transfer
        self.parameterWidget.lineEdit_Temperature.textChanged.connect(self.Temperature_textChanged)
        #EM wave
        self.parameterWidget.lineEdit_E_X.textChanged.connect(self.E_X_textChanged)
        self.parameterWidget.lineEdit_E_Y.textChanged.connect(self.E_Y_textChanged)
        self.parameterWidget.lineEdit_E_Z.textChanged.connect(self.E_Z_textChanged)
        self.parameterWidget.lineEdit_M_X.textChanged.connect(self.M_X_textChanged)
        self.parameterWidget.lineEdit_M_Y.textChanged.connect(self.M_Y_textChanged)
        self.parameterWidget.lineEdit_M_Z.textChanged.connect(self.M_Z_textChanged)
        #Elastic
        self.parameterWidget.LE_Force1.textChanged.connect(self.Force1_textChanged)
        self.parameterWidget.LE_Force2.textChanged.connect(self.Force2_textChanged)
        self.parameterWidget.LE_Force3.textChanged.connect(self.Force3_textChanged)
        self.parameterWidget.LE_Displacement1.textChanged.connect(self.Displacement1_textChanged)
        self.parameterWidget.LE_Displacement2.textChanged.connect(self.Displacement2_textChanged)
        self.parameterWidget.LE_Displacement3.textChanged.connect(self.Displacement3_textChanged)
        self.parameterWidget.LE_MeshUpdate1.textChanged.connect(self.MeshUpdate1_textChanged)
        self.parameterWidget.LE_MeshUpdate2.textChanged.connect(self.MeshUpdate2_textChanged)
        self.parameterWidget.LE_MeshUpdate3.textChanged.connect(self.MeshUpdate3_textChanged)
        #self definition
        self.parameterWidget.textEdit_SelfDef1.textChanged.connect(self.SelfDef1_textChanged)
        self.parameterWidget.textEdit_SelfDef2.textChanged.connect(self.SelfDef2_textChanged)
        self.form.tw_Subdomains.itemChanged.connect(self.tw_Subdomains_changed)

    def parameterWidget_init(self):
        #CFD
        self.parameterWidget.lineEdit_VelX.setText(str(self.obj.VelX))
        self.parameterWidget.lineEdit_VelY.setText(str(self.obj.VelY))
        self.parameterWidget.lineEdit_VelZ.setText(str(self.obj.VelZ))
        self.parameterWidget.lineEdit_Pressure.setText(str(self.obj.Pressure))
        #Heat
        self.parameterWidget.lineEdit_Temperature.setText(str(self.obj.Temperature))
        #EM wave
        self.parameterWidget.lineEdit_E_X.setText(str(self.obj.E_X))
        self.parameterWidget.lineEdit_E_Y.setText(str(self.obj.E_Y))
        self.parameterWidget.lineEdit_E_Z.setText(str(self.obj.E_Z))
        self.parameterWidget.lineEdit_M_X.setText(str(self.obj.M_X))
        self.parameterWidget.lineEdit_M_Y.setText(str(self.obj.M_Y))
        self.parameterWidget.lineEdit_M_Z.setText(str(self.obj.M_Z))
        #self definition
        self.parameterWidget.textEdit_SelfDef1.setText(self.obj.SelfDef1)
        self.parameterWidget.textEdit_SelfDef2.setText(self.obj.SelfDef2)
        #Elastic
        self.parameterWidget.LE_Force1.setText(str(self.obj.Force1))
        self.parameterWidget.LE_Force2.setText(str(self.obj.Force2))
        self.parameterWidget.LE_Force3.setText(str(self.obj.Force3))
        self.parameterWidget.LE_Displacement1.setText(str(self.obj.Displacement1))
        self.parameterWidget.LE_Displacement2.setText(str(self.obj.Displacement2))
        self.parameterWidget.LE_Displacement3.setText(str(self.obj.Displacement3))
        self.parameterWidget.LE_MeshUpdate1.setText(str(self.obj.MeshUpdate1))
        self.parameterWidget.LE_MeshUpdate2.setText(str(self.obj.MeshUpdate2))
        self.parameterWidget.LE_MeshUpdate3.setText(str(self.obj.MeshUpdate3))
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

    def VelX_textChanged(self, text):
        self.obj.VelX =  (text)

    def VelY_textChanged(self, text):
        self.obj.VelY =  (text)

    def VelZ_textChanged(self, text):
        self.obj.VelZ =  (text)

    def E_X_textChanged(self, text):
        self.obj.E_X =  (text)

    def E_Y_textChanged(self, text):
        self.obj.E_Y =  (text)

    def E_Z_textChanged(self, text):
        self.obj.E_Z =  (text)

    def M_X_textChanged(self, text):
        self.obj.M_X =  (text)

    def M_Y_textChanged(self, text):
        self.obj.M_Y =  (text)

    def M_Z_textChanged(self, text):
        self.obj.M_Z =  (text)

    def Pressure_textChanged(self, text):
        self.obj.Pressure =  (text)

    def Force1_textChanged(self, text):
        self.obj.Force1 =  (text)

    def Force2_textChanged(self, text):
        self.obj.Force2 =  (text)

    def Force3_textChanged(self, text):
        self.obj.Force3 =  (text)

    def Displacement1_textChanged(self, text):
        self.obj.Displacement1 =  (text)

    def Displacement2_textChanged(self, text):
        self.obj.Displacement2 =  (text)

    def Displacement3_textChanged(self, text):
        self.obj.Displacement3 =  (text)

    def MeshUpdate1_textChanged(self, text):
        self.obj.MeshUpdate1 =  (text)

    def MeshUpdate2_textChanged(self, text):
        self.obj.MeshUpdate2 =  (text)

    def MeshUpdate3_textChanged(self, text):
        self.obj.MeshUpdate3 =  (text)

    def Temperature_textChanged(self, text):
        self.obj.Temperature =  (text)

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

