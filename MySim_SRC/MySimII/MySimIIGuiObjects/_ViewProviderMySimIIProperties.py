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

__title__ = "FreeCAD MySimIIII Properties ViewProvider for the document object"
__author__ = "Bernd Hahnebach Zhenwen Wan"
__url__ = "http://www.freecadweb.org"

## @package ViewProviderMySimIIIIProperties
#  \ingroup MySimIIII
#  \brief FreeCAD MySimIIII _ViewProviderMySimIIIIProperties

import sys
import FreeCAD
import FreeCADGui
import MySimIIIIGui

# for the panel
from MySimIIIIobjects import _MySimIIIIProperties
from PySide import QtCore
from PySide import QtGui
from PySide.QtCore import Qt
from PySide.QtGui import QApplication
import time


class _ViewProviderMySimIIIIProperties:
    "A View Provider for the MySimIIIIProperties object"

    def __init__(self, vobj):
        vobj.Proxy = self

    def getIcon(self):
        return ":/icons/MySimIIII-Properties.svg"

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
            if o.isDerivedFrom("MySimIIII::MySimIIIIPropertiesObject"):
                o.ViewObject.hide()
        # show the  we like to edit
        self.ViewObject.show()
        # show task panel
        taskd = _TaskPanelMySimIIIIProperties(self.Object)
        taskd.obj = vobj.Object
        FreeCADGui.Control.showDialog(taskd)
        return True

    def unsetEdit(self, vobj, mode):
        FreeCADGui.Control.closeDialog()
        self.ViewObject.hide()  # hide the  after edit is finished
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



class _TaskPanelMySimIIIIProperties:
    """The TaskPanel for editing References property of
    MySimIIIIProperties objects and creation of new MySimIIII """

    def __init__(self, obj):
        self._obj = obj
        self.form = FreeCADGui.PySideUic.loadUi(
            FreeCAD.getHomePath() + "Mod/MySimIIII/Resources/ui/Properties.ui"
        )

        self.Timer = QtCore.QTimer()
        self.Timer.start(100)  # 100 milli seconds
        self.Properties_runs = False
        self.console_message_Properties = ""

        QtCore.QObject.connect(
            self.form.Name,
            QtCore.SIGNAL("textchanged()"),
            self.Name_changed
        )
        QtCore.QObject.connect(
            self.form.MainValue,
            QtCore.SIGNAL("valueChanged(Base::Quantity)"),
            self.MainValue_changed
        )
        QtCore.QObject.connect(
            self.form.Unit,
            QtCore.SIGNAL("textchanged()"),
            self.Unit_changed
        )
        QtCore.QObject.connect(
            self.form.ExtraValue,
            QtCore.SIGNAL("valueChanged(Base::Quantity)"),
            self.ExtraValue_changed
        )
        QtCore.QObject.connect(
            self.Timer,
            QtCore.SIGNAL("timeout()"),
            self.update_timer_text
        )

        self.get_params()
        self.update()

    def get_params(self, Name_text):
        self.Name = Name_text

    def set_params(self, Name_text):
        self.Name = Name_text

    def update(self, Name_text):
        self.Name = Name_text

    def Name_changed(self, Name_text):
        self.Name = Name_text

    def MainValue_changed(self, base_quantity_value):
        self.MainValue = base_quantity_value

    def Unit_changed(self, Name_text):
        self.Unit = Name_text

    def ExtraValue_changed(self, base_quantity_value):
        self.ExtraValue = base_quantity_value

    def getStandardButtons(self):
        button_value = int(
            QtGui.QDialogButtonBox.Ok | QtGui.QDialogButtonBox.Apply | QtGui.QDialogButtonBox.Cancel
        )
        return button_value
        # show a OK, a apply and a Cancel button
        # def reject() is called on Cancel button
        # def clicked(self, button) is needed, to access the apply button

    def accept(self):
        self.set_params()
        FreeCADGui.ActiveDocument.resetEdit()
        FreeCAD.ActiveDocument.recompute()
        return True

    def reject(self):
        FreeCADGui.ActiveDocument.resetEdit()
        FreeCAD.ActiveDocument.recompute()
        return True

    def clicked(self, button):
        if button == QtGui.QDialogButtonBox.Apply:
            self.set_params()

