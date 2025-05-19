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

## @package ViewProviderMySimIIMeshCreate
#  \ingroup MySimII
#  \brief FreeCAD MySimII _ViewProviderMySimIIMeshCreate

import FreeCAD
import FreeCADGui
import MySimIIGui  # needed to display the icons in TreeView

# for the panel
from PySide import QtCore
from PySide import QtGui
from PySide.QtCore import Qt
from PySide.QtGui import QApplication
import time
from . import MySimIISelectionWidgets
from Utils.PartsCheck import PartsCheck
zw_checked   = QtCore.Qt.CheckState.Checked
zw_unchecked = QtCore.Qt.CheckState.Unchecked

False if MySimIIGui.__name__ else True  # flake8, dummy MySimIIGui usage


class _ViewProviderMySimIIMeshCreate:
    "A View Provider for the MySimIIMeshCreate object"

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
            if o.isDerivedFrom("MySimII::MySimIIMeshCreateObject") and not (o == self.Object):
                o.ViewObject.hide()
        # show the  we like to edit
        self.ViewObject.show()
        # show task panel
        taskd = _TaskPanelMySimIIMeshCreate(self.Object)
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


class _TaskPanelMySimIIMeshCreate:
    """The TaskPanel for editing References property of MySimIIMeshCreate objects"""

    def __init__(self, obj):

        self.obj = obj
        self.GroupList = []
        self.VaryingList = []
        self.FixedList = []

        # parameter widget
        self.form = FreeCADGui.PySideUic.loadUi(
            FreeCAD.getHomePath() + "Mod/MySimII/Resources/ui/MeshCreate.ui"
        )
        self.init_form()
        self.form.Coarsest.valueChanged.connect(self.Coarsest_changed)
        self.form.Finest.valueChanged.connect(self.Finest_changed)
        self.form.tw_VaryingMesh.itemChanged.connect(self.tw_VaryingMesh_changed)
        self.form.tw_FixedMesh.itemChanged.connect(self.tw_FixedMesh_changed)
        self.form.tw_Group.itemChanged.connect(self.tw_Group_changed)

    def init_form(self):
        Rows = 0
        for obj in FreeCAD.ActiveDocument.Objects:
            if obj.isDerivedFrom("MySimII::MySimIIMeshNodesObjectPython"):
                self.GroupList.append(obj)
                Rows += 1
        self.form.tw_Group.clear()
        self.form.tw_Group.setRowCount(Rows)
        self.form.tw_Group.setColumnCount(1)
        Rows = 0
        for obj in FreeCAD.ActiveDocument.Objects:
            if obj.isDerivedFrom("MySimII::MySimIIMeshNodesObjectPython"):
                item = QtGui.QTableWidgetItem(str(obj.Label))
                if obj.ViewObject.Visibility:
                    item.setCheckState(zw_checked)
                else:
                    item.setCheckState(zw_unchecked)
                self.form.tw_Group.setItem(Rows,0,item)
                Rows += 1
        Rows = 0
        for obj in FreeCAD.ActiveDocument.Objects:
            if obj.isDerivedFrom("MySimII::MySimIIMeshBoundariesObjectPython"):
                self.VaryingList.append(obj)
                Rows += 1
        self.form.tw_VaryingMesh.clear()
        self.form.tw_VaryingMesh.setRowCount(Rows)
        self.form.tw_VaryingMesh.setColumnCount(1)
        Rows = 0
        for obj in FreeCAD.ActiveDocument.Objects:
            if obj.isDerivedFrom("MySimII::MySimIIMeshBoundariesObjectPython"):
                item = QtGui.QTableWidgetItem(str(obj.Label))
                if obj.ViewObject.Visibility:
                    item.setCheckState(zw_checked)
                else:
                    item.setCheckState(zw_unchecked)
                self.form.tw_VaryingMesh.setItem(Rows,0,item)
                Rows += 1
        Rows = 0
        for obj in FreeCAD.ActiveDocument.Objects:
            if obj.isDerivedFrom("MySimII::MySimIIMeshRegionsObjectPython"):
                self.FixedList.append(obj)
                Rows += 1
        self.form.tw_FixedMesh.clear()
        self.form.tw_FixedMesh.setRowCount(Rows)
        self.form.tw_FixedMesh.setColumnCount(1)
        Rows = 0
        for obj in FreeCAD.ActiveDocument.Objects:
            if obj.isDerivedFrom("MySimII::MySimIIMeshRegionsObjectPython"):
                item = QtGui.QTableWidgetItem(str(obj.Label))
                if obj.ViewObject.Visibility:
                    item.setCheckState(zw_checked)
                else:
                    item.setCheckState(zw_unchecked)
                self.form.tw_FixedMesh.setItem(Rows,0,item)
                Rows += 1
        self.form.Coarsest.setText(self.obj.CharacteristicLengthMax.UserString)
        self.form.Finest.setText(self.obj.CharacteristicLengthMin.UserString)

    def Coarsest_changed(self, value):
         self.obj.CharacteristicLengthMax = value

    def Finest_changed(self, value):
         self.obj.CharacteristicLengthMin = value

    def tw_Group_changed(self,item):
        text = item.text()
        obj = FreeCAD.ActiveDocument.getObject(text)
        stat = item.checkState()
        if stat == QtCore.Qt.CheckState.Checked:
            if obj not in self.GroupList:
                self.GroupList.append(obj)
        if stat == QtCore.Qt.CheckState.Unchecked:
            if obj in self.GroupList:
                self.GroupList.remove(obj)

    def tw_VaryingMesh_changed(self,item):
        text = item.text()
        obj = FreeCAD.ActiveDocument.getObject(text)
        stat = item.checkState()
        if stat == QtCore.Qt.CheckState.Checked:
            if obj not in self.VaryingList:
                self.VaryingList.append(obj)
        if stat == QtCore.Qt.CheckState.Unchecked:
            if obj in self.VaryingList:
                self.VaryingList.remove(obj)

    def tw_FixedMesh_changed(self,item):
        text = item.text()
        obj = FreeCAD.ActiveDocument.getObject(text)
        stat = item.checkState()
        if stat == QtCore.Qt.CheckState.Checked:
            if obj not in self.FixedList:
                self.FixedList.append(obj)
        if stat == QtCore.Qt.CheckState.Unchecked:
            if obj in self.FixedList:
                self.FixedList.remove(obj)

    def accept(self):
        self.obj.MeshGroupList = self.GroupList 
        self.obj.MeshBoundaryLayerList = self.VaryingList
        self.obj.MeshRegionList = self.FixedList
        instant = PartsCheck()
        self.obj.Part = instant.CheckModelCount()
        self.run_gmsh()
        self.recompute_and_set_back_all()
        return True

    def reject(self):
        self.recompute_and_set_back_all()
        return True

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


