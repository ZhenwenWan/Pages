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

## @package ViewProviderMySimIIBoundaries
#  \ingroup MySimII
#  \brief FreeCAD MySimII _ViewProviderMySimIIBoundaries

import FreeCAD
import FreeCADGui
import MySimIIGui  # needed to display the icons in TreeView
False if False else MySimIIGui.__name__  # flake8, dummy MySimIIGui usage

# for the panel
from PySide import QtCore
from . import FemSelectionWidgets



class _ViewProviderMySimIIBoundaries:
    "A View Provider for the MySimIIBoundaries object"

    def __init__(self, vobj):
        vobj.Proxy = self

    def getIcon(self):
        return ":/icons/MySimII-MySimIImesh-boundary-layer.svg"

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
        taskd = _TaskPanelMySimIIBoundaries(self.Object)
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


class _TaskPanelMySimIIBoundaries:
    """The TaskPanel for editing References property of MySimIIBoundaries objects"""

    def __init__(self, obj):

        self.obj = obj

        self.ConstraintTypeList = ["Empty", "CFD", "Elastic", "Heat", "EM", "CFD+Elastic", "CFD+Heat"]

        # parameter widget
        self.parameterWidget = FreeCADGui.PySideUic.loadUi(
            FreeCAD.getHomePath() + "Mod/MySimII/Resources/ui/Boundaries.ui"
        )
        self.parameterWidget_init()
        #CFD
        self.parameterWidget.lineEdit_Vel.textChanged.connect(self.Vel_textChanged)
        self.parameterWidget.lineEdit_Pressure.textChanged.connect(self.Pressure_textChanged)
        self.parameterWidget.lineEdit_SlipCoeff.textChanged.connect(self.SlipCoeff_textChanged)
        self.parameterWidget.NTCoord.stateChanged.connect(self.NTCoord_stateChanged)
        self.parameterWidget.FSI.stateChanged.connect(self.FSI_stateChanged)
        self.parameterWidget.FreeSurf.stateChanged.connect(self.FreeSurf_stateChanged)
        self.parameterWidget.Wall.stateChanged.connect(self.Wall_stateChanged)
        self.parameterWidget.NoSlipWall.stateChanged.connect(self.NoSlipWall_stateChanged)
        #Heat transfer
        self.parameterWidget.lineEdit_HeatFlux.textChanged.connect(self.HeatFlux_textChanged)
        self.parameterWidget.lineEdit_Temperature.textChanged.connect(self.Temperature_textChanged)
        #EM wave
        self.parameterWidget.ComB_EM.currentIndexChanged.connect(self.ComB_EM_currentIndexChanged)
        self.parameterWidget.lineEdit_IncWave.textChanged.connect(self.IncWave_textChanged)
        self.parameterWidget.lineEdit_IncProp.textChanged.connect(self.IncProp_textChanged)
        #Elastic
        self.parameterWidget.LE_NormalForce.textChanged.connect(self.NormalForce_textChanged)
        self.parameterWidget.LE_Force.textChanged.connect(self.Force_textChanged)
        self.parameterWidget.LE_Displacement.textChanged.connect(self.Displacement_textChanged)
        self.parameterWidget.LE_MeshUpdate.textChanged.connect(self.MeshUpdate_textChanged)
        #General
        self.parameterWidget.LE_Dirichlet.textChanged.connect(self.Dirichlet_textChanged)
        self.parameterWidget.LE_Neumann.textChanged.connect(self.Neumann_textChanged)
        self.parameterWidget.LE_RobinCoef.textChanged.connect(self.RobinCoef_textChanged)
        self.parameterWidget.LE_RobinExt.textChanged.connect(self.RobinExt_textChanged)
        self.parameterWidget.CB_NTCoordG.stateChanged.connect(self.NTCoordG_stateChanged)
        #GeneralTypeF
        self.parameterWidget.LE_DirichletF.textChanged.connect(self.DirichletF_textChanged)
        self.parameterWidget.LE_NeumannF.textChanged.connect(self.NeumannF_textChanged)
        self.parameterWidget.LE_RobinCoefF.textChanged.connect(self.RobinCoefF_textChanged)
        self.parameterWidget.LE_RobinExtF.textChanged.connect(self.RobinExtF_textChanged)
        self.parameterWidget.CB_NTCoordF.stateChanged.connect(self.NTCoordF_stateChanged)
        #self definition
        self.parameterWidget.textEdit_SelfDef1.textChanged.connect(self.SelfDef1_textChanged)
        self.parameterWidget.textEdit_SelfDef2.textChanged.connect(self.SelfDef2_textChanged)
        #apply to geometrical elements
        self.selectionWidget = FemSelectionWidgets.GeometryElementsSelection(
            obj.References,["Face", "Edge"]
        )
        # form made from param and selection widget
        self.form = [self.parameterWidget, self.selectionWidget]

    def parameterWidget_init(self):
        #CFD
        self.parameterWidget.lineEdit_Vel.setText(str(self.obj.Vel))
        self.parameterWidget.lineEdit_Pressure.setText(str(self.obj.Pressure))
        self.parameterWidget.lineEdit_SlipCoeff.setText(str(self.obj.SlipCoeff))
        self.parameterWidget.lineEdit_Temperature.setText(str(self.obj.Temperature))
        self.parameterWidget.lineEdit_HeatFlux.setText(str(self.obj.HeatFlux))
        #EM wave
        self.parameterWidget.lineEdit_IncWave.setText(str(self.obj.IncWave))
        self.parameterWidget.lineEdit_IncProp.setText(str(self.obj.IncProp))
        index = self.parameterWidget.ComB_EM.findText(self.obj.ComB_EM)
        self.parameterWidget.ComB_EM.setCurrentIndex(index)
        #self definition
        self.parameterWidget.textEdit_SelfDef1.setText(self.obj.SelfDef1)
        self.parameterWidget.textEdit_SelfDef2.setText(self.obj.SelfDef2)
        #Elastic
        self.parameterWidget.LE_NormalForce.setText(str(self.obj.NormalForce))
        self.parameterWidget.LE_Force.setText(str(self.obj.Force))
        self.parameterWidget.LE_Displacement.setText(str(self.obj.Displacement))
        self.parameterWidget.LE_MeshUpdate.setText(str(self.obj.MeshUpdate))
        #General
        self.parameterWidget.LE_Dirichlet.setText(str(self.obj.Dirichlet))
        self.parameterWidget.LE_Neumann.setText(str(self.obj.Neumann))
        self.parameterWidget.LE_RobinCoef.setText(str(self.obj.RobinCoef))
        self.parameterWidget.LE_RobinExt.setText(str(self.obj.RobinExt))
        #GeneralTypeF
        self.parameterWidget.LE_DirichletF.setText(str(self.obj.DirichletF))
        self.parameterWidget.LE_NeumannF.setText(str(self.obj.NeumannF))
        self.parameterWidget.LE_RobinCoefF.setText(str(self.obj.RobinCoefF))
        self.parameterWidget.LE_RobinExtF.setText(str(self.obj.RobinExtF))

        if self.obj.NTCoord:
            self.parameterWidget.NTCoord.setCheckState(QtCore.Qt.CheckState.Checked)
        else:
            self.parameterWidget.NTCoord.setCheckState(QtCore.Qt.CheckState.Unchecked)
        
        if self.obj.NTCoordG:
            self.parameterWidget.CB_NTCoordG.setCheckState(QtCore.Qt.CheckState.Checked)
        else:
            self.parameterWidget.CB_NTCoordG.setCheckState(QtCore.Qt.CheckState.Unchecked)
        
        if self.obj.NTCoordF:
            self.parameterWidget.CB_NTCoordF.setCheckState(QtCore.Qt.CheckState.Checked)
        else:
            self.parameterWidget.CB_NTCoordF.setCheckState(QtCore.Qt.CheckState.Unchecked)
        
        if self.obj.Wall:
            self.parameterWidget.Wall.setCheckState(QtCore.Qt.CheckState.Checked)
        else:
            self.parameterWidget.Wall.setCheckState(QtCore.Qt.CheckState.Unchecked)
        
        if self.obj.NoSlipWall:
            self.parameterWidget.NoSlipWall.setCheckState(QtCore.Qt.CheckState.Checked)
        else:
            self.parameterWidget.NoSlipWall.setCheckState(QtCore.Qt.CheckState.Unchecked)
        
        if self.obj.FreeSurf:
            self.parameterWidget.FreeSurf.setCheckState(QtCore.Qt.CheckState.Checked)
        else:
            self.parameterWidget.FreeSurf.setCheckState(QtCore.Qt.CheckState.Unchecked)
        
        if self.obj.FSI:
            self.parameterWidget.FSI.setCheckState(QtCore.Qt.CheckState.Checked)
        else:
            self.parameterWidget.FSI.setCheckState(QtCore.Qt.CheckState.Unchecked)


    def Dirichlet_textChanged(self, text):
        self.obj.Dirichlet =  (text)

    def Neumann_textChanged(self, text):
        self.obj.Neumann =  (text)

    def RobinCoef_textChanged(self, text):
        self.obj.RobinCoef =  (text)

    def RobinExt_textChanged(self, text):
        self.obj.RobinExt =  (text)

    def DirichletF_textChanged(self, text):
        self.obj.DirichletF =  (text)

    def NeumannF_textChanged(self, text):
        self.obj.NeumannF =  (text)

    def RobinCoefF_textChanged(self, text):
        self.obj.RobinCoefF =  (text)

    def RobinExtF_textChanged(self, text):
        self.obj.RobinExtF =  (text)

    def Vel_textChanged(self, text):
        self.obj.Vel =  (text)

    def IncWave_textChanged(self, text):
        self.obj.IncWave =  (text)

    def IncProp_textChanged(self, text):
        self.obj.IncProp =  (text)

    def Pressure_textChanged(self, text):
        self.obj.Pressure =  (text)

    def SlipCoeff_textChanged(self, text):
        self.obj.SlipCoeff =  (text)

    def HeatFlux_textChanged(self, text):
        self.obj.HeatFlux =  (text)

    def NormalForce_textChanged(self, text):
        self.obj.NormalForce =  (text)

    def Force_textChanged(self, text):
        self.obj.Force =  (text)

    def Displacement_textChanged(self, text):
        self.obj.Displacement =  (text)

    def MeshUpdate_textChanged(self, text):
        self.obj.MeshUpdate =  (text)

    def Temperature_textChanged(self, text):
        self.obj.Temperature =  (text)

    def NTCoord_stateChanged(self, state):
        if state == QtCore.Qt.CheckState.Checked:
            self.obj.NTCoord =  True
        else:
            self.obj.NTCoord =  False

    def NTCoordG_stateChanged(self, state):
        if state == QtCore.Qt.CheckState.Checked:
            self.obj.NTCoordG =  True
        else:
            self.obj.NTCoordG =  False

    def NTCoordF_stateChanged(self, state):
        if state == QtCore.Qt.CheckState.Checked:
            self.obj.NTCoordF =  True
        else:
            self.obj.NTCoordF =  False

    def FSI_stateChanged(self, state):
        if state == QtCore.Qt.CheckState.Checked:
            self.obj.FSI =  True
        else:
            self.obj.FSI =  False

    def FreeSurf_stateChanged(self, state):
        if state == QtCore.Qt.CheckState.Checked:
            self.obj.FreeSurf =  True
        else:
            self.obj.FreeSurf =  False

    def Wall_stateChanged(self, state):
        if state == QtCore.Qt.CheckState.Checked:
            self.obj.Wall =  True
        else:
            self.obj.Wall =  False

    def NoSlipWall_stateChanged(self, state):
        if state == QtCore.Qt.CheckState.Checked:
            self.obj.NoSlipWall =  True
        else:
            self.obj.NoSlipWall =  False

    def ComB_EM_currentIndexChanged(self, index):
        self.obj.ComB_EM = self.obj.ComB_EMList[index]

    def CB_ConstraintType_currentIndexChanged(self, index):
        self.obj.ConstraintType = self.ConstraintTypeList[index]

    def SelfDef1_textChanged(self):
        self.obj.SelfDef1 =  self.parameterWidget.textEdit_SelfDef1.toPlainText()

    def SelfDef2_textChanged(self):
        self.obj.SelfDef2 =  self.parameterWidget.textEdit_SelfDef2.toPlainText()

    def accept(self):
        self.obj.References = self.selectionWidget.references
        self.recompute_and_set_back_all()
        return True

    def reject(self):
        self.recompute_and_set_back_all()
        return True

    def recompute_and_set_back_all(self):
        doc = FreeCADGui.getDocument(self.obj.Document)
        doc.Document.recompute()
        self.selectionWidget.setback_listobj_visibility()
        if self.selectionWidget.sel_server:
            FreeCADGui.Selection.removeObserver(self.selectionWidget.sel_server)
        doc.resetEdit()

