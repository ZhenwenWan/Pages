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

__title__ = "FreeCAD MySimII Materials ViewProvider for the document object"
__author__ = "Bernd Hahnebach Zhenwen Wan"
__url__ = "http://www.freecadweb.org"

## @package ViewProviderMySimIIMaterials
#  \ingroup MySimII
#  \brief FreeCAD MySimII _ViewProviderMySimIIMaterials

import sys
import FreeCAD
import FreeCADGui
import MySimIIGui

# for the panel
from MySimIIObjects import _MySimIIMaterials
from . import FemSelectionWidgets
from MySimIItools import PartsCheck
from PySide import QtCore
from PySide import QtGui
from PySide.QtCore import Qt
from PySide.QtGui import QApplication
import time


class _ViewProviderMySimIIMaterials:
    "A View Provider for the MySimIIMaterials object"

    def __init__(self, vobj):
        vobj.Proxy = self

    def getIcon(self):
        return ":/icons/MySimII-Materials.svg"

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
        # show task panel
        taskd = _TaskPanelMySimIIMaterials(self.Object)
        taskd.obj = vobj.Object
        FreeCADGui.Control.showDialog(taskd)
        return True

    def unsetEdit(self, vobj, mode):
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

class _TaskPanelMySimIIMaterials:
    """The TaskPanel for editing References property of
    MySimIIMaterials objects and creation of new MySimII """

    def __init__(self, obj):
        self.obj = obj
        self.MaterialTypes = ["Empty","General","Heat","CFD","Elastic","Electrostatic"]
        self.CompressModels = ["Empty", "Thermal","Artificial Compressible","perfect gas",
            "Incompressible","weakly compressible", "isothermal ideal gas"]
        self.form0 = FreeCADGui.PySideUic.loadUi(
            FreeCAD.getHomePath() + "Mod/MySimII/Resources/ui/Materials.ui" )
        self.init_form0()
        self.form0.CB_CompressModel.currentIndexChanged.connect(self.CB_CompressModel)
        self.form0.LE_Density.textChanged.connect(self.LE_Density)
        self.form0.LE_HeatConduct.textChanged.connect(self.LE_HeatConduct)
        self.form0.LE_HeatExpansion.textChanged.connect(self.LE_HeatExpansion)
        self.form0.LE_RefTemperature.textChanged.connect(self.LE_RefTemperature)
        self.form0.LE_HeatCapacity.textChanged.connect(self.LE_HeatCapacity)
        self.form0.LE_SpecHeatRatio.textChanged.connect(self.LE_SpecHeatRatio)
        self.form0.LE_Viscosity.textChanged.connect(self.LE_Viscosity)
        self.form0.LE_RefPressure.textChanged.connect(self.LE_RefPressure)
        self.form0.LE_YoungsModulus.textChanged.connect(self.LE_YoungsModulus)
        self.form0.LE_PoissonRatio.textChanged.connect(self.LE_PoissonRatio)
        self.form0.LE_MeshElasticModulus.textChanged.connect(self.LE_MeshElasticModulus)
        self.form0.LE_MeshPoissonRatio.textChanged.connect(self.LE_MeshPoissonRatio)
        self.form0.LE_CForce.textChanged.connect(self.LE_CForce)
        self.form0.LE_RForce.textChanged.connect(self.LE_RForce)
        self.form0.LE_TDepth.textChanged.connect(self.LE_TDepth)
        self.form0.LE_BFriction.textChanged.connect(self.LE_BFriction)
        
        self.selectionWidget = FemSelectionWidgets.GeometryElementsSelection(
            obj.References, ["Solid", "Face"], False )
        self.form = [self.form0, self.selectionWidget]

    def init_form0(self):
        self.form0.CB_CompressModel.clear()
        self.form0.CB_CompressModel.addItems(self.CompressModels)
        index = self.form0.CB_CompressModel.findText(self.obj.CompressModel)
        self.form0.CB_CompressModel.setCurrentIndex(index)

        self.form0.LE_Density.setText(str(self.obj.Density))
        self.form0.LE_HeatConduct.setText(str(self.obj.HeatConduct))
        self.form0.LE_HeatExpansion.setText(str(self.obj.HeatExpansion))
        self.form0.LE_RefTemperature.setText(str(self.obj.RefTemperature))
        self.form0.LE_HeatCapacity.setText(str(self.obj.HeatCapacity))
        self.form0.LE_SpecHeatRatio.setText(str(self.obj.SpecHeatRatio))
        self.form0.LE_Viscosity.setText(str(self.obj.Viscosity))
        self.form0.LE_RefPressure.setText(str(self.obj.RefPressure))
        self.form0.LE_YoungsModulus.setText(str(self.obj.YoungsModulus))
        self.form0.LE_PoissonRatio.setText(str(self.obj.PoissonRatio))
        self.form0.LE_MeshElasticModulus.setText(str(self.obj.MeshElasticModulus))
        self.form0.LE_MeshPoissonRatio.setText(str(self.obj.MeshPoissonRatio))
        self.form0.LE_RelativePermittivity.setText(str(self.obj.RelativePermittivity))
        self.form0.LE_CForce.setText(str(self.obj.CForce))
        self.form0.LE_RForce.setText(str(self.obj.RForce))
        self.form0.LE_TDepth.setText(str(self.obj.TDepth))
        self.form0.LE_BFriction.setText(str(self.obj.BFriction))


    def CB_MaterialType_init(self, index):
        self.form0.TabContent1.setEnabled(False)
        self.form0.TabContent2.setEnabled(False)
        self.form0.TabContent3.setEnabled(False)
        self.form0.TabContent4.setEnabled(False)
        self.form0.TabContent5.setEnabled(False)
        if index == 1:
            self.form0.TabContent1.setEnabled(True)
        if index == 2:
            self.form0.TabContent2.setEnabled(True)
        if index == 3:
            self.form0.TabContent3.setEnabled(True)
        if index == 4:
            self.form0.TabContent4.setEnabled(True)
        if index == 5:
            self.form0.TabContent5.setEnabled(True)
        self.form0.TabWid_1.setCurrentIndex(index-1)

    def CB_MaterialType(self, index):
        self.CB_MaterialType_init(index)
        self.obj.MaterialType = self.MaterialTypes[index]
    
    def CB_CompressModel(self, index):
        self.obj.CompressModel = self.CompressModels[index]


    def LE_Density(self, text):
        self.obj.Density = (text)
    
    def LE_HeatConduct(self, text):
        self.obj.HeatConduct = (text)
    
    def LE_HeatExpansion(self, text):
        self.obj.HeatExpansion = (text)
    
    def LE_RefTemperature(self, text):
        self.obj.RefTemperature = (text)
    
    def LE_HeatCapacity(self, text):
        self.obj.HeatCapacity = (text)
    
    def LE_SpecHeatRatio(self, text):
        self.obj.SpecHeatRatio = (text)
    
    def LE_RefPressure(self, text):
        self.obj.RefPressure = (text)
    
    def LE_Viscosity(self, text):
        self.obj.Viscosity = (text)
    
    def LE_YoungsModulus(self, text):
        self.obj.YoungsModulus = (text)
    
    def LE_PoissonRatio(self, text):
        self.obj.PoissonRatio = (text)
    
    def LE_MeshElasticModulus(self, text):
        self.obj.MeshElasticModulus = (text)
    
    def LE_MeshPoissonRatio(self, text):
        self.obj.MeshPoissonRatio = (text)
    
    def LE_RelativePermittivity(self, text):
        self.obj.RelativePermittivity = (text)
    
    def LE_CForce(self, text):
        self.obj.CForce = (text)
    
    def LE_RForce(self, text):
        self.obj.RForce = (text)
    
    def LE_TDepth(self, text):
        self.obj.TDepth = (text)
    
    def LE_BFriction(self, text):
        self.obj.BFriction = (text)
    
    def accept(self):
        self.obj.References = self.selectionWidget.references
        FreeCADGui.ActiveDocument.resetEdit()
        FreeCAD.ActiveDocument.recompute()
        return True

    def reject(self):
        self.init_form0()
        FreeCADGui.ActiveDocument.resetEdit()
        FreeCAD.ActiveDocument.recompute()
        return True

