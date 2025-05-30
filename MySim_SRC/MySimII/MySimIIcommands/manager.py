# ***************************************************************************
# *                                                                         *
# *   Copyright (c) 2015 - FreeCAD Developers                               *
# *   Author (c) 2015 - Przemo Fiszt < przemo@firszt.eu>                    *
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

__title__ = "MySimII Commands"
__author__ = "Przemo Firszt"
__url__ = "http://www.freecadweb.org"

## \addtogroup MySimII
#  @{

import FreeCAD
import MySimIItools.MySimIIutils as MySimIIutils

if FreeCAD.GuiUp:
    import FreeCADGui
    import MySimIIGui
    from PySide import QtCore


class CommandManager(object):

    def __init__(self):
        self.resources = {
            "Pixmap": "MySimIIWorkbench",
            "MenuText": QtCore.QT_TRANSLATE_NOOP("MySimII_Command", "Default MySimII Command MenuText"),
            "Accel": "",
            "ToolTip": QtCore.QT_TRANSLATE_NOOP("MySimII_Command", "Default MySimII Command ToolTip")
        }
        # FIXME add option description
        self.is_active = None
        self.selobj = None
        self.selobj2 = None
        self.active_analysis = None

    def GetResources(self):
        return self.resources

    def IsActive(self):
        if not self.is_active:
            active = False
        elif self.is_active == "always":
            active = True
        elif self.is_active == "with_document":
            active = FreeCADGui.ActiveDocument is not None
        elif self.is_active == "with_analysis":
            active = (
                MySimIIGui.getActiveAnalysis() is not None
                and self.active_analysis_in_active_doc()
            )
        elif self.is_active == "with_results":
            active = (
                MySimIIGui.getActiveAnalysis() is not None
                and self.active_analysis_in_active_doc()
                and (self.results_present() or self.result_mesh_present())
            )
        elif self.is_active == "with_selresult":
            active = (
                MySimIIGui.getActiveAnalysis() is not None
                and self.active_analysis_in_active_doc()
                and self.result_selected()
            )
        elif self.is_active == "with_part_feature":
            active = (
                FreeCADGui.ActiveDocument is not None
                and self.part_feature_selected()
            )
        elif self.is_active == "with_MySimIImesh":
            active = (
                FreeCADGui.ActiveDocument is not None
                and self.MySimIImesh_selected()
            )
        elif self.is_active == "with_gmsh_MySimIImesh":
            active = (
                FreeCADGui.ActiveDocument is not None
                and self.gmsh_MySimIImesh_selected()
            )
        elif self.is_active == "with_MySimIImesh_andor_res":
            active = (
                FreeCADGui.ActiveDocument is not None
                and self.with_MySimIImesh_andor_res_selected()
            )
        elif self.is_active == "with_material":
            active = (
                MySimIIGui.getActiveAnalysis() is not None
                and self.active_analysis_in_active_doc()
                and self.material_selected()
            )
        elif self.is_active == "with_material_solid_which_has_no_nonlinear_material":
            active = (
                MySimIIGui.getActiveAnalysis() is not None
                and self.active_analysis_in_active_doc()
                and self.material_solid_selected()
                and self.has_no_nonlinear_material()
            )
        elif self.is_active == "with_solver":
            active = (
                MySimIIGui.getActiveAnalysis() is not None
                and self.active_analysis_in_active_doc()
                and self.solver_selected()
            )
        elif self.is_active == "with_solver_elmer":
            active = (
                MySimIIGui.getActiveAnalysis() is not None
                and self.active_analysis_in_active_doc()
                and self.solver_elmer_selected()
            )
        elif self.is_active == "with_analysis_without_solver":
            active = (
                MySimIIGui.getActiveAnalysis() is not None
                and self.active_analysis_in_active_doc()
                and not self.analysis_has_solver()
            )
        return active

    def results_present(self):
        results = False
        analysis_members = MySimIIGui.getActiveAnalysis().Group
        for o in analysis_members:
            if o.isDerivedFrom("MySimII::MySimIIResultObject"):
                results = True
        return results

    def result_mesh_present(self):
        result_mesh = False
        analysis_members = MySimIIGui.getActiveAnalysis().Group
        for o in analysis_members:
            if MySimIIutils.is_of_type(o, "MySimII::MySimIIMeshResult"):
                result_mesh = True
        return result_mesh

    def result_selected(self):
        sel = FreeCADGui.Selection.getSelection()
        if len(sel) == 1 and sel[0].isDerivedFrom("MySimII::MySimIIResultObject"):
            for o in MySimIIGui.getActiveAnalysis().Group:
                if o == sel[0]:
                    self.selobj = o
                    return True
        return False

    def part_feature_selected(self):
        sel = FreeCADGui.Selection.getSelection()
        if len(sel) == 1 and sel[0].isDerivedFrom("Part::Feature"):
            self.selobj = sel[0]
            return True
        else:
            return False

    def MySimIImesh_selected(self):
        sel = FreeCADGui.Selection.getSelection()
        if len(sel) == 1 and sel[0].isDerivedFrom("MySimII::MySimIIMeshObject"):
            self.selobj = sel[0]
            return True
        else:
            return False

    def gmsh_MySimIImesh_selected(self):
        sel = FreeCADGui.Selection.getSelection()
        if (
            len(sel) == 1
            and hasattr(sel[0], "Proxy")
            and sel[0].Proxy.Type == "MySimII::MySimIIMeshGmsh"
        ):
            self.selobj = sel[0]
            return True
        else:
            return False

    def material_selected(self):
        sel = FreeCADGui.Selection.getSelection()
        if len(sel) == 1 and sel[0].isDerivedFrom("App::MaterialObjectPython"):
            return True
        else:
            return False

    def material_solid_selected(self):
        sel = FreeCADGui.Selection.getSelection()
        if (
            len(sel) == 1
            and sel[0].isDerivedFrom("App::MaterialObjectPython")
            and hasattr(sel[0], "Category")
            and sel[0].Category == "Solid"
        ):
            self.selobj = sel[0]
            return True
        else:
            return False

    def has_no_nonlinear_material(self):
        "check if an nonlinear material exists which is already based on the selected material"
        for o in FreeCAD.ActiveDocument.Objects:
            if (
                hasattr(o, "Proxy")
                and o.Proxy is not None
                and o.Proxy.Type == "MySimII::MaterialMechanicalNonlinear"
                and o.LinearBaseMaterial == self.selobj
            ):
                FreeCAD.Console.PrintError(
                    "{} is based on the selected material: {}. "
                    "Only one nonlinear object for each material allowed.\n"
                    .format(o.Name, self.selobj)
                )
                return False
        return True

    def with_MySimIImesh_andor_res_selected(self):
        sel = FreeCADGui.Selection.getSelection()
        if len(sel) == 1 and sel[0].isDerivedFrom("MySimII::MySimIIMeshObject"):
            self.selobj = sel[0]
            return True
        elif len(sel) == 2:
            if sel[0].isDerivedFrom("MySimII::MySimIIMeshObject"):
                if sel[1].isDerivedFrom("MySimII::MySimIIResultObject"):
                    self.selobj = sel[0]  # mesh
                    self.selobj2 = sel[1]  # res
                    return True
                else:
                    return False
            elif sel[1].isDerivedFrom("MySimII::MySimIIMeshObject"):
                if sel[0].isDerivedFrom("MySimII::MySimIIResultObject"):
                    self.selobj = sel[1]  # mesh
                    self.selobj2 = sel[0]  # res
                    return True
                else:
                    return False
            else:
                return False
        else:
            return False

    def active_analysis_in_active_doc(self):
        analysis = MySimIIGui.getActiveAnalysis()
        if analysis.Document is FreeCAD.ActiveDocument:
            self.active_analysis = analysis
            return True
        else:
            return False

    def solver_selected(self):
        sel = FreeCADGui.Selection.getSelection()
        if len(sel) == 1 and sel[0].isDerivedFrom("MySimII::MySimIISolverObjectPython"):
            self.selobj = sel[0]
            return True
        else:
            return False

    def solver_elmer_selected(self):
        sel = FreeCADGui.Selection.getSelection()
        if (
            len(sel) == 1
            and hasattr(sel[0], "Proxy")
            and sel[0].Proxy.Type == "MySimII::MySimIISolverObjectElmer"
        ):
            self.selobj = sel[0]
            return True
        else:
            return False

    def analysis_has_solver(self):
        solver = False
        analysis_members = MySimIIGui.getActiveAnalysis().Group
        for o in analysis_members:
            if o.isDerivedFrom("MySimII::MySimIISolverObjectPython"):
                solver = True
        if solver is True:
            return True
        else:
            return False

    def hide_meshes_show_parts_constraints(self):
        if FreeCAD.GuiUp:
            for acnstrmesh in MySimIIGui.getActiveAnalysis().Group:
                if "Constraint" in acnstrmesh.TypeId:
                    acnstrmesh.ViewObject.Visibility = True
                if "Mesh" in acnstrmesh.TypeId:
                    # OvG: Hide meshes and show constraints and meshed part
                    # e.g. on purging results
                    acnstrmesh.ViewObject.Visibility = False

    # ****************************************************************************************
    # methods to add the objects to the document in FreeCADGui mode

    def add_obj_on_gui_set_edit(self, objtype):
        FreeCAD.ActiveDocument.openTransaction(
            "Create MySimII{}"
            .format(objtype)
        )
        FreeCADGui.addModule(
            "ObjectsMySimII"
        )
        FreeCADGui.doCommand(
            "MySimIIGui.getActiveAnalysis().addObject(ObjectsMySimII."
            "make{}(FreeCAD.ActiveDocument))"
            .format(objtype)
        )
        # no other obj should be selected if we go in task panel
        FreeCADGui.Selection.clearSelection()
        FreeCADGui.doCommand(
            "FreeCADGui.ActiveDocument.setEdit(FreeCAD.ActiveDocument.ActiveObject.Name)"
        )
        FreeCAD.ActiveDocument.recompute()

    def add_obj_on_gui_noset_edit(self, objtype):
        FreeCAD.ActiveDocument.openTransaction(
            "Create MySimII{}"
            .format(objtype)
        )
        FreeCADGui.addModule(
            "ObjectsMySimII"
        )
        FreeCADGui.doCommand(
            "MySimIIGui.getActiveAnalysis().addObject(ObjectsMySimII."
            "make{}(FreeCAD.ActiveDocument))"
            .format(objtype)
        )
        # FreeCAD.ActiveDocument.commitTransaction()  # solver command class had this line
        # no clear selection is done
        FreeCAD.ActiveDocument.recompute()

    def add_obj_on_gui_selobj_set_edit(self, objtype):
        FreeCAD.ActiveDocument.openTransaction(
            "Create MySimII{}"
            .format(objtype)
        )
        FreeCADGui.addModule(
            "ObjectsMySimII"
        )
        FreeCADGui.doCommand(
            "ObjectsMySimII.make{}("
            "FreeCAD.ActiveDocument, FreeCAD.ActiveDocument.{})"
            .format(objtype, self.selobj.Name)
        )
        FreeCADGui.Selection.clearSelection()
        FreeCADGui.doCommand(
            "FreeCADGui.ActiveDocument.setEdit(FreeCAD.ActiveDocument.ActiveObject.Name)"
        )
        FreeCAD.ActiveDocument.recompute()

    def add_obj_on_gui_selobj_noset_edit(self, objtype):
        FreeCAD.ActiveDocument.openTransaction(
            "Create MySimII{}"
            .format(objtype)
        )
        FreeCADGui.addModule(
            "ObjectsMySimII"
        )
        FreeCADGui.doCommand(
            "ObjectsMySimII.make{}("
            "FreeCAD.ActiveDocument, FreeCAD.ActiveDocument.{})"
            .format(objtype, self.selobj.Name)
        )
        FreeCADGui.Selection.clearSelection()
        FreeCAD.ActiveDocument.recompute()


##  @}
