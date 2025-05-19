
__title__ = "Python utilities"
__author__ = "Zhenwen Wan"
__url__ = "http://www.mysim.com(Not yet)"

from PySide import QtCore
from PySide import QtGui
import FreeCAD

class PartsCheck:
    """Checking the parts of FreeCAD
    """

    def __init__(self):
        self.PartCount = 0
        self.ModelLabeledCount = 0
        self.CheckPartCount()
        self.CheckModelCount()

    def CheckPartCount(self):
        for o in FreeCAD.ActiveDocument.Objects:
            if o.isDerivedFrom("Part::Feature"):
                self.PartCount += 1
        if self.PartCount == 0:
            QtGui.QMessageBox.critical(None,"PartsCheck","No part is found!!!")

    def CheckModelCount(self):
        self.ModelLabeledCount = 0
        for o in FreeCAD.ActiveDocument.Objects:
            if o.isDerivedFrom("Part::Feature"):
                if str(o.Label).startswith("Model"):
                   model = o
                   self.ModelLabeledCount += 1
        if self.ModelLabeledCount != 1:
            QtGui.QMessageBox.critical(None,"PartsCheck","Model is missing or too many!!!")

        return model
