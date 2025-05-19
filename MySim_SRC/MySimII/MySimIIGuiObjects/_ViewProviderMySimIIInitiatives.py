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

__title__ = "FreeCAD MySimII Initiatives ViewProvider for the document object"
__author__ = "Bernd Hahnebach Zhenwen Wan"
__url__ = "http://www.freecadweb.org"

## @package ViewProviderMySimIIInitiatives
#  \ingroup MySimII
#  \brief FreeCAD MySimII _ViewProviderMySimIIInitiatives

import sys
import FreeCAD
import FreeCADGui
import MySimIIGui

# for the panel
from MySimIIObjects import _MySimIIInitiatives
from . import MySimIISelectionWidgets
from Utils import PartsCheck
from PySide import QtCore
from PySide import QtGui
from PySide.QtCore import Qt
from PySide.QtGui import QApplication
import time


class _ViewProviderMySimIIInitiatives:
    "A View Provider for the MySimIIInitiatives object"

    def __init__(self, vobj):
        vobj.Proxy = self
        #Ensure the existence of parts, one of which is labeled as Model
        PartsCheck.PartsCheck()

    def getIcon(self):
        return ":/icons/MySimII-Initiatives.svg"

    def attach(self, vobj):
        self.ViewObject = vobj
        self.Object = vobj.Object

    def updateData(self, obj, prop):
        return

    def onChanged(self, vobj, prop):
        return

    def setEdit(self, vobj, mode):
        # hide all es
        for o in FreeCAD.ActiveDocument.Objects:
            if o.isDerivedFrom("MySimII::MySimIIInitiativesObject") and not (o == self.Object):
                o.ViewObject.hide()
        # show the  we like to edit
        self.ViewObject.show()
        # show task panel
        taskd = _TaskPanelMySimIIInitiatives(self.Object)
        taskd.obj = vobj.Object
        FreeCADGui.Control.showDialog(taskd)
        return True

    def unsetEdit(self, vobj, mode):
        FreeCADGui.Control.closeDialog()
        #self.ViewObject.hide()  # hide the  after edit is finished
        return True

    def doubleClicked(self, vobj):
        gui_doc = FreeCADGui.getDocument(vobj.Object.Document)
        gui_doc.setEdit(vobj.Object.Name)
        return True

    def __getstate__(self):
        return None

    def __setstate__(self, state):
        return None

    def onDelete(self, feature, subelements):
        return True

    def canDragObjects(self):
        return True

    def canDropObjects(self):
        return True

    def canDragObject(self, dragged_object):
            return False

    def canDropObject(self, incoming_object):
        return True



class _TaskPanelMySimIIInitiatives:
    """The TaskPanel for editing References property of
    MySimIIInitiatives objects and creation of new MySimII """

    def __init__(self, obj):
        self._obj = obj
        self.form0 = FreeCADGui.PySideUic.loadUi(
            FreeCAD.getHomePath() + "Mod/MySimII/Resources/ui/Initiatives.ui"
        )
        #setup UI and get information from documents 
        self.form0.InitiativeName.addItems(
            _MySimIIInitiatives._MySimIIInitiatives.InitiativeNames
        )
        self.form0.Unit.addItems(
            _MySimIIInitiatives._MySimIIInitiatives.Units
        )
        self.ExtraPartsList_Checked = []
        for part in obj.ExtraParts:
            self.ExtraPartsList_Checked.append(part.Label)
        self.selectionWidget = MySimIISelectionWidgets.GeometryElementsSelection(
            obj.References,
            ["Solid"]
        )
        self.get_from_doc()
        self.update_ui()

        QtCore.QObject.connect(
            self.form0.InitiativeName,
            QtCore.SIGNAL("activated(int)"),
            self.Name_chosen
        )
        QtCore.QObject.connect(
            self.form0.Unit,
            QtCore.SIGNAL("activated(int)"),
            self.Unit_chosen
        )
        QtCore.QObject.connect(
            self.form0.MainbodyValue,
            QtCore.SIGNAL("valueChanged(Base::Quantity)"),
            self.MainValue_changed
        )
        QtCore.QObject.connect(
            self.form0.ExtraValue,
            QtCore.SIGNAL("valueChanged(Base::Quantity)"),
            self.ExtraValue_changed
        )
        self.form = [self.form0, self.selectionWidget]

    def get_from_doc(self):
        self.InitiativeName = self._obj.InitiativeName
        self.Unit         = self._obj.Unit
        self.MainValue    = self._obj.MainValue
        self.ExtraValue   = self._obj.ExtraValue

    def set_to_doc(self):
        self._obj.InitiativeName = self.InitiativeName
        self._obj.Unit         = self.Unit
        self._obj.MainValue    = self.MainValue
        self._obj.ExtraValue   = self.ExtraValue
        self._obj.References = self.selectionWidget.references
        objs = []
        for ref in self.selectionWidget.references:
            if ref[1].startswith("Solid"):
                objs.append(ref[0])
            else:
                FreeCAD.Console.PrintError(
                    "Object {} with non-solid element: {} \n".format(ref[0].Name, ref[1])
                    )
        self._obj.ExtraParts = objs
        self.selectionWidget.setback_listobj_visibility()
        if self.selectionWidget.sel_server:
            FreeCADGui.Selection.removeObserver(self.selectionWidget.sel_server)
        for o in FreeCAD.ActiveDocument.Objects:
            if o.isDerivedFrom("Part::Feature") and \
                (o.Label == "Model") is True:
                self._obj.Mainbody = o

    def update_ui(self):
        "filling the widget"
        index = self.form0.InitiativeName.findText(self.InitiativeName)
        self.form0.InitiativeName.setCurrentIndex(index)
        index = self.form0.Unit.findText(self.Unit)
        self.form0.Unit.setCurrentIndex(index)
        self.form0.MainbodyValue.setText(self.MainValue.UserString)
        self.form0.ExtraValue.setText(self.ExtraValue.UserString)

    def update_timer_text(self):
        self.blank = ""

    def MainValue_changed(self, base):
        self.MainValue = base

    def ExtraValue_changed(self, base_quantity_value):
        self.ExtraValue = base_quantity_value

    def Name_chosen(self, index):
        if index < 0:
            return
        self.form0.InitiativeName.setCurrentIndex(index)
        self.InitiativeName = str(self.form0.InitiativeName.itemText(index))

    def Unit_chosen(self, index):
        if index < 0:
            return
        self.form0.Unit.setCurrentIndex(index)
        self.Unit = str(self.form0.Unit.itemText(index))

    def getStandardButtons(self):
        button_value = int(
            QtGui.QDialogButtonBox.Ok | QtGui.QDialogButtonBox.Apply | QtGui.QDialogButtonBox.Cancel
        )
        return button_value
        # show a OK, a apply and a Cancel button
        # def reject() is called on Cancel button
        # def clicked(self, button) is needed, to access the apply button

    def accept(self):
        self.set_to_doc()
        FreeCADGui.ActiveDocument.resetEdit()
        FreeCAD.ActiveDocument.recompute()
        return True

    def reject(self):
        FreeCADGui.ActiveDocument.resetEdit()
        FreeCAD.ActiveDocument.recompute()
        return True

    def clicked(self, button):
        if button == QtGui.QDialogButtonBox.Apply:
            self.set_to_doc()

