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

## @package ViewProviderMySimIIMeshNodes
#  \ingroup MySimII
#  \brief FreeCAD MySimII _ViewProviderMySimIIMeshNodes

import FreeCAD
import FreeCADGui
import MySimIIGui  # needed to display the icons in TreeView

# for the panel
from PySide import QtCore
from . import MySimIISelectionWidgets

False if MySimIIGui.__name__ else True  # flake8, dummy MySimIIGui usage


class _ViewProviderMySimIIMeshNodes:
    "A View Provider for the MySimIIMeshNodes object"

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
            if o.isDerivedFrom("MySimII::MySimIIMeshNodesObject") and not (o == self.Object):
                o.ViewObject.hide()
        # show the  we like to edit
        self.ViewObject.show()
        # show task panel
        taskd = _TaskPanelMySimIIMeshNodes(self.Object)
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


class _TaskPanelMySimIIMeshNodes:
    """The TaskPanel for editing References property of MySimIIMeshNodes objects"""

    def __init__(self, obj):

        self.obj = obj

        # parameter widget
        self.parameterWidget = FreeCADGui.PySideUic.loadUi(
            FreeCAD.getHomePath() + "Mod/MySimII/Resources/ui/MeshNodes.ui"
        )
        self.parameterWidget.LabelText.setText(obj.Name)
        QtCore.QObject.connect(
            self.parameterWidget.LabelText,
            QtCore.SIGNAL("textchanged()"),
            self.LabelText_changed
        )

        # geometry selection widget
        # start with Solid in list!
        self.selectionWidget = MySimIISelectionWidgets.GeometryElementsSelection(
            obj.References,
            ["Solid", "Face", "Edge", "Vertex"]
        )

        # form made from param and selection widget
        self.form = [self.parameterWidget, self.selectionWidget]

    def accept(self):
        self.obj.LabelText = self.parameterWidget.LabelText.text()
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

    def LabelText_changed(self, text):
        self.parameterWidget.LabelText.setText(text)

