# ***************************************************************************
# *                                                                         *
# *   Copyright (c) 2016 - Bernd Hahnebach <bernd@bimstatik.org>            *
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

__title__ = "Objects MySimII"
__author__ = "Bernd Hahnebach"
__url__ = "http://www.freecadweb.org"

## \addtogroup MySimII
#  @{

import FreeCAD


def makeMaterials(doc, name="Materials"):
    """makeMaterials(document, [name]):
    creates a material object attributing to a shape object"""
    obj = doc.addObject("MySimII::MaterialsPython", name)
    from femobjects import _MySimIIMaterials
    _MySimIIMaterials._MySimIIMaterials(obj)
    if FreeCAD.GuiUp:
        from femguiobjects import _ViewProviderMySimIIMaterials
        _ViewProviderMySimIIMaterials._ViewProviderMySimIIMaterials(obj.ViewObject)
    return obj


# ********* analysis objects *********************************************************************
def makeAnalysis(
    doc,
    name="Analysis"
):
    """makeAnalysis(document, [name]):
    makes a MySimII Analysis object"""
    obj = doc.addObject("MySimII::MySimIIAnalysis", name)
    return obj


# ********* constraint objects *******************************************************************
def makeConstraintBearing(
    doc,
    name="ConstraintBearing"
):
    """makeConstraintBearing(document, [name]):
    makes a MySimII ConstraintBearing object"""
    obj = doc.addObject("MySimII::ConstraintBearing", name)
    return obj


def makeConstraintBodyHeatSource(
    doc,
    name="ConstraintBodyHeatSource"
):
    """makeConstraintBodyHeatSource(document, [name]):
    makes a MySimII ConstraintBodyHeatSource object"""
    obj = doc.addObject("MySimII::ConstraintPython", name)
    from femobjects import _MySimIIConstraintBodyHeatSource
    _MySimIIConstraintBodyHeatSource.Proxy(obj)
    if FreeCAD.GuiUp:
        from femguiobjects import _ViewProviderMySimIIConstraintBodyHeatSource
        _ViewProviderMySimIIConstraintBodyHeatSource.ViewProxy(obj.ViewObject)
    return obj


def makeConstraintContact(
    doc,
    name="ConstraintContact"
):
    """makeConstraintContact(document, [name]):
    makes a MySimII ConstraintContact object"""
    obj = doc.addObject("MySimII::ConstraintContact", name)
    return obj


def makeConstraintDisplacement(
    doc,
    name="ConstraintDisplacement"
):
    """makeConstraintDisplacement(document, [name]):
    makes a MySimII ConstraintDisplacement object"""
    obj = doc.addObject("MySimII::ConstraintDisplacement", name)
    return obj


def makeConstraintElectrostaticPotential(
    doc,
    name="ConstraintElectrostaticPotential"
):
    """makeConstraintElectrostaticPotential(document, [name]):
    makes a MySimII ElectrostaticPotential object"""
    obj = doc.addObject("MySimII::ConstraintPython", name)
    from femobjects import _MySimIIConstraintElectrostaticPotential
    _MySimIIConstraintElectrostaticPotential.Proxy(obj)
    if FreeCAD.GuiUp:
        from femguiobjects import _ViewProviderMySimIIConstraintElectrostaticPotential
        _ViewProviderMySimIIConstraintElectrostaticPotential.ViewProxy(obj.ViewObject)
    return obj


def makeConstraintFixed(
    doc,
    name="ConstraintFixed"
):
    """makeConstraintFixed(document, [name]):
    makes a MySimII ConstraintFixed object"""
    obj = doc.addObject("MySimII::ConstraintFixed", name)
    return obj


def makeConstraintFlowVelocity(
    doc,
    name="ConstraintFlowVelocity"
):
    """makeConstraintFlowVelocity(document, [name]):
    makes a MySimII ConstraintFlowVelocity object"""
    obj = doc.addObject("MySimII::ConstraintPython", name)
    from femobjects import _MySimIIConstraintFlowVelocity
    _MySimIIConstraintFlowVelocity.Proxy(obj)
    if FreeCAD.GuiUp:
        from femguiobjects import _ViewProviderMySimIIConstraintFlowVelocity
        _ViewProviderMySimIIConstraintFlowVelocity.ViewProxy(obj.ViewObject)
    return obj


def makeConstraintFluidBoundary(
    doc,
    name="ConstraintFluidBoundary"
):
    """makeConstraintFluidBoundary(document, name):
    makes a MySimII ConstraintFluidBoundary object"""
    obj = doc.addObject("MySimII::ConstraintFluidBoundary", name)
    return obj


def makeConstraintForce(
    doc,
    name="ConstraintForce"
):
    """makeConstraintForce(document, [name]):
    makes a MySimII ConstraintForce object"""
    obj = doc.addObject("MySimII::ConstraintForce", name)
    return obj


def makeConstraintGear(
    doc,
    name="ConstraintGear"
):
    """makeConstraintGear(document, [name]):
    makes a MySimII ConstraintGear object"""
    obj = doc.addObject("MySimII::ConstraintGear", name)
    return obj


def makeConstraintHeatflux(
    doc,
    name="ConstraintHeatflux"
):
    """makeConstraintHeatflux(document, [name]):
    makes a MySimII ConstraintHeatflux object"""
    obj = doc.addObject("MySimII::ConstraintHeatflux", name)
    return obj


def makeConstraintInitialFlowVelocity(
    doc,
    name="ConstraintInitialFlowVelocity"
):
    """makeConstraintInitialFlowVelocity(document, [name]):
    makes a MySimII ConstraintInitialFlowVelocity object"""
    obj = doc.addObject("MySimII::ConstraintPython", name)
    from femobjects import _MySimIIConstraintInitialFlowVelocity
    _MySimIIConstraintInitialFlowVelocity.Proxy(obj)
    if FreeCAD.GuiUp:
        from femguiobjects import _ViewProviderMySimIIConstraintInitialFlowVelocity
        _ViewProviderMySimIIConstraintInitialFlowVelocity.ViewProxy(obj.ViewObject)
    return obj


def makeConstraintInitialTemperature(
    doc,
    name="ConstraintInitialTemperature"
):
    """makeConstraintInitialTemperature(document, name):
    makes a MySimII ConstraintInitialTemperature object"""
    obj = doc.addObject("MySimII::ConstraintInitialTemperature", name)
    return obj


def makeConstraintPlaneRotation(
    doc,
    name="ConstraintPlaneRotation"
):
    """makeConstraintPlaneRotation(document, [name]):
    makes a MySimII ConstraintPlaneRotation object"""
    obj = doc.addObject("MySimII::ConstraintPlaneRotation", name)
    return obj


def makeConstraintPressure(
    doc,
    name="ConstraintPressure"
):
    """makeConstraintPressure(document, [name]):
    makes a MySimII ConstraintPressure object"""
    obj = doc.addObject("MySimII::ConstraintPressure", name)
    return obj


def makeConstraintPulley(
    doc,
    name="ConstraintPulley"
):
    """makeConstraintPulley(document, [name]):
    makes a MySimII ConstraintPulley object"""
    obj = doc.addObject("MySimII::ConstraintPulley", name)
    return obj


def makeConstraintSelfWeight(
    doc,
    name="ConstraintSelfWeight"
):
    """makeConstraintSelfWeight(document, [name]):
    creates an self weight object to define a gravity load"""
    obj = doc.addObject("MySimII::ConstraintPython", name)
    from femobjects import _MySimIIConstraintSelfWeight
    _MySimIIConstraintSelfWeight._MySimIIConstraintSelfWeight(obj)
    if FreeCAD.GuiUp:
        from femguiobjects import _ViewProviderMySimIIConstraintSelfWeight
        _ViewProviderMySimIIConstraintSelfWeight._ViewProviderMySimIIConstraintSelfWeight(
            obj.ViewObject
        )
    return obj


def makeConstraintTemperature(
    doc,
    name="ConstraintTemperature"
):
    """makeConstraintTemperature(document, [name]):
    makes a MySimII ConstraintTemperature object"""
    obj = doc.addObject("MySimII::ConstraintTemperature", name)
    return obj


def makeConstraintTransform(
    doc,
    name="ConstraintTransform"
):
    """makeConstraintTransform(document, [name]):
    makes a MySimII ConstraintTransform object"""
    obj = doc.addObject("MySimII::ConstraintTransform", name)
    return obj


# ********* element definition objects ***********************************************************
def makeElementFluid1D(
    doc,
    name="ElementFluid1D"
):
    """makeElementFluid1D(document, [name]):
    creates an 1D fluid element object to define 1D flow"""
    obj = doc.addObject("MySimII::FeaturePython", name)
    from femobjects import _MySimIIElementFluid1D
    _MySimIIElementFluid1D._MySimIIElementFluid1D(obj)
    if FreeCAD.GuiUp:
        from femguiobjects import _ViewProviderMySimIIElementFluid1D
        _ViewProviderMySimIIElementFluid1D._ViewProviderMySimIIElementFluid1D(obj.ViewObject)
    return obj


def makeElementGeometry1D(
    doc,
    sectiontype="Rectangular",
    width=10.0,
    height=25.0,
    name="ElementGeometry1D"
):
    """makeElementGeometry1D(document, [width], [height], [name]):
    creates an 1D geometry element object to define a cross section"""
    obj = doc.addObject("MySimII::FeaturePython", name)
    from femobjects import _MySimIIElementGeometry1D
    _MySimIIElementGeometry1D._MySimIIElementGeometry1D(obj)
    sec_types = _MySimIIElementGeometry1D._MySimIIElementGeometry1D.known_beam_types
    if sectiontype not in sec_types:
        FreeCAD.Console.PrintError("Section type is not known. Set to " + sec_types[0] + " \n")
        obj.SectionType = sec_types[0]
    else:
        obj.SectionType = sectiontype
    obj.RectWidth = width
    obj.RectHeight = height
    obj.CircDiameter = height
    obj.PipeDiameter = height
    obj.PipeThickness = width
    if FreeCAD.GuiUp:
        from femguiobjects import _ViewProviderMySimIIElementGeometry1D
        _ViewProviderMySimIIElementGeometry1D._ViewProviderMySimIIElementGeometry1D(obj.ViewObject)
    return obj


def makeElementGeometry2D(
    doc,
    thickness=20.0,
    name="ElementGeometry2D"
):
    """makeElementGeometry2D(document, [thickness], [name]):
    creates an 2D geometry element object to define a plate thickness"""
    obj = doc.addObject("MySimII::FeaturePython", name)
    from femobjects import _MySimIIElementGeometry2D
    _MySimIIElementGeometry2D._MySimIIElementGeometry2D(obj)
    obj.Thickness = thickness
    if FreeCAD.GuiUp:
        from femguiobjects import _ViewProviderMySimIIElementGeometry2D
        _ViewProviderMySimIIElementGeometry2D._ViewProviderMySimIIElementGeometry2D(obj.ViewObject)
    return obj


def makeElementRotation1D(
    doc,
    name="ElementRotation1D"
):
    """makeElementRotation1D(document, [name]):
    creates an 1D geometry rotation element object to rotate a 1D cross section"""
    obj = doc.addObject("MySimII::FeaturePython", name)
    from femobjects import _MySimIIElementRotation1D
    _MySimIIElementRotation1D._MySimIIElementRotation1D(obj)
    if FreeCAD.GuiUp:
        from femguiobjects import _ViewProviderMySimIIElementRotation1D
        _ViewProviderMySimIIElementRotation1D._ViewProviderMySimIIElementRotation1D(obj.ViewObject)
    return obj


# ********* material objects *********************************************************************
def makeMaterialFluid(
    doc,
    name="FluidMaterial"
):
    """makeMaterialFluid(document, [name]):
    makes a MySimII Material for fluid"""
    obj = doc.addObject("App::MaterialObjectPython", name)
    from femobjects import _MySimIIMaterial
    _MySimIIMaterial._MySimIIMaterial(obj)
    obj.Category = "Fluid"
    if FreeCAD.GuiUp:
        from femguiobjects import _ViewProviderMySimIIMaterial
        _ViewProviderMySimIIMaterial._ViewProviderMySimIIMaterial(obj.ViewObject)
    return obj


def makeMaterialMechanicalNonlinear(
    doc,
    base_material,
    name="MechanicalMaterialNonlinear"
):
    """makeMaterialMechanicalNonlinear(document, base_material, [name]):
    creates a nonlinear material object"""
    obj = doc.addObject("MySimII::FeaturePython", name)
    from femobjects import _MySimIIMaterialMechanicalNonlinear
    _MySimIIMaterialMechanicalNonlinear._MySimIIMaterialMechanicalNonlinear(obj)
    obj.LinearBaseMaterial = base_material
    if FreeCAD.GuiUp:
        from femguiobjects import _ViewProviderMySimIIMaterialMechanicalNonlinear
        _ViewProviderMySimIIMaterialMechanicalNonlinear._ViewProviderMySimIIMaterialMechanicalNonlinear(
            obj.ViewObject
        )
    return obj


def makeMaterialReinforced(
    doc,
    name="MaterialReinforced"
):
    """makeMaterialReinforced(document, [matrix_material], [reinforcement_material], [name]):
    creates a reinforced material object"""
    obj = doc.addObject("App::MaterialObjectPython", name)
    from femobjects import _MySimIIMaterialReinforced
    _MySimIIMaterialReinforced._MySimIIMaterialReinforced(obj)
    if FreeCAD.GuiUp:
        from femguiobjects import _ViewProviderMySimIIMaterialReinforced
        _ViewProviderMySimIIMaterialReinforced._ViewProviderMySimIIMaterialReinforced(obj.ViewObject)
    return obj


def makeMaterialSolid(
    doc,
    name="MechanicalSolidMaterial"
):
    """makeMaterialSolid(document, [name]):
    makes a MySimII Material for solid"""
    obj = doc.addObject("App::MaterialObjectPython", name)
    from femobjects import _MySimIIMaterial
    _MySimIIMaterial._MySimIIMaterial(obj)
    obj.Category = "Solid"
    if FreeCAD.GuiUp:
        from femguiobjects import _ViewProviderMySimIIMaterial
        _ViewProviderMySimIIMaterial._ViewProviderMySimIIMaterial(obj.ViewObject)
    return obj


# ********* mesh objects *************************************************************************
def makeMeshBoundaryLayer(
    doc,
    base_mesh,
    name="MeshBoundaryLayer"
):
    """makeMeshBoundaryLayer(document, base_mesh, [name]):
    creates a MySimII mesh BoundaryLayer object to define boundary layer properties"""
    obj = doc.addObject("MySimII::FeaturePython", name)
    from femobjects import _MySimIIMeshBoundaryLayer
    _MySimIIMeshBoundaryLayer._MySimIIMeshBoundaryLayer(obj)
    # obj.BaseMesh = base_mesh
    # App::PropertyLinkList does not support append
    # we will use a temporary list to append the mesh BoundaryLayer obj. to the list
    tmplist = base_mesh.MeshBoundaryLayerList
    tmplist.append(obj)
    base_mesh.MeshBoundaryLayerList = tmplist
    if FreeCAD.GuiUp:
        from femguiobjects import _ViewProviderMySimIIMeshBoundaryLayer
        _ViewProviderMySimIIMeshBoundaryLayer._ViewProviderMySimIIMeshBoundaryLayer(obj.ViewObject)
    return obj


def makeMeshGmsh(
    doc,
    name="MeshGmsh"
):
    """makeMeshGmsh(document, [name]):
    makes a Gmsh MySimII mesh object"""
    obj = doc.addObject("MySimII::MySimIIMeshObjectPython", name)
    from femobjects import _MySimIIMeshGmsh
    _MySimIIMeshGmsh._MySimIIMeshGmsh(obj)
    if FreeCAD.GuiUp:
        from femguiobjects import _ViewProviderMySimIIMeshGmsh
        _ViewProviderMySimIIMeshGmsh._ViewProviderMySimIIMeshGmsh(obj.ViewObject)
    return obj


def makeMeshGroup(
    doc,
    base_mesh,
    use_label=False,
    name="MeshGroup"
):
    """makeMeshGroup(document, base_mesh, [use_label], [name]):
    creates a MySimII mesh region object to define properties for a region of a MySimII mesh"""
    obj = doc.addObject("MySimII::FeaturePython", name)
    from femobjects import _MySimIIMeshGroup
    _MySimIIMeshGroup._MySimIIMeshGroup(obj)
    obj.UseLabel = use_label
    # obj.BaseMesh = base_mesh
    # App::PropertyLinkList does not support append
    # we will use a temporary list to append the mesh group obj. to the list
    tmplist = base_mesh.MeshGroupList
    tmplist.append(obj)
    base_mesh.MeshGroupList = tmplist
    if FreeCAD.GuiUp:
        from femguiobjects import _ViewProviderMySimIIMeshGroup
        _ViewProviderMySimIIMeshGroup._ViewProviderMySimIIMeshGroup(obj.ViewObject)
    return obj


def makeMeshNetgen(
    doc,
    name="MeshNetgen"
):
    """makeMeshNetgen(document, [name]):
    makes a MySimII MeshShapeNetgenObject object"""
    obj = doc.addObject("MySimII::MySimIIMeshShapeNetgenObject", name)
    return obj


def makeMeshRegion(
    doc,
    base_mesh,
    element_length=0.0,
    name="MeshRegion"
):
    """makeMeshRegion(document, base_mesh, [element_length], [name]):
    creates a MySimII mesh region object to define properties for a region of a MySimII mesh"""
    obj = doc.addObject("MySimII::FeaturePython", name)
    from femobjects import _MySimIIMeshRegion
    _MySimIIMeshRegion._MySimIIMeshRegion(obj)
    obj.CharacteristicLength = element_length
    # obj.BaseMesh = base_mesh
    # App::PropertyLinkList does not support append
    # we will use a temporary list to append the mesh region obj. to the list
    tmplist = base_mesh.MeshRegionList
    tmplist.append(obj)
    base_mesh.MeshRegionList = tmplist
    if FreeCAD.GuiUp:
        from femguiobjects import _ViewProviderMySimIIMeshRegion
        _ViewProviderMySimIIMeshRegion._ViewProviderMySimIIMeshRegion(obj.ViewObject)
    return obj


def makeMeshResult(
    doc,
    name="MeshResult"
):
    """makeMeshResult(document, name): makes a MySimII MeshResult object"""
    obj = doc.addObject("MySimII::MySimIIMeshObjectPython", name)
    from femobjects import _MySimIIMeshResult
    _MySimIIMeshResult._MySimIIMeshResult(obj)
    if FreeCAD.GuiUp:
        from femguiobjects import _ViewProviderMySimIIMeshResult
        _ViewProviderMySimIIMeshResult._ViewProviderMySimIIMeshResult(obj.ViewObject)
    return obj


# ********* post processing objects **************************************************************
def makeResultMechanical(
    doc,
    name="MechanicalResult"
):
    """makeResultMechanical(document, [name]):
    creates an mechanical result object to hold MySimII results"""
    obj = doc.addObject("MySimII::MySimIIResultObjectPython", name)
    from femobjects import _MySimIIResultMechanical
    _MySimIIResultMechanical._MySimIIResultMechanical(obj)
    if FreeCAD.GuiUp:
        from femguiobjects import _ViewProviderMySimIIResultMechanical
        _ViewProviderMySimIIResultMechanical._ViewProviderMySimIIResultMechanical(obj.ViewObject)
    return obj


def makePostVtkFilterClipRegion(
    doc,
    base_vtk_result,
    name="VtkFilterClipRegion"
):
    """makePostVtkFilterClipRegion(document, base_vtk_result, [name]):
    creates an MySimII post processing region clip filter object (vtk based)"""
    obj = doc.addObject("MySimII::MySimIIPostClipFilter", name)
    tmp_filter_list = base_vtk_result.Filter
    tmp_filter_list.append(obj)
    base_vtk_result.Filter = tmp_filter_list
    del tmp_filter_list
    return obj


def makePostVtkFilterClipScalar(
    doc,
    base_vtk_result,
    name="VtkFilterClipScalar"
):
    """makePostVtkFilterClipScalar(document, base_vtk_result, [name]):
    creates an MySimII post processing scalar clip filter object (vtk based)"""
    obj = doc.addObject("MySimII::MySimIIPostScalarClipFilter", name)
    tmp_filter_list = base_vtk_result.Filter
    tmp_filter_list.append(obj)
    base_vtk_result.Filter = tmp_filter_list
    del tmp_filter_list
    return obj


def makePostVtkFilterCutFunction(
    doc,
    base_vtk_result,
    name="VtkFilterCutFunction"
):
    """makePostVtkFilterCutFunction(document, base_vtk_result, [name]):
    creates an MySimII post processing cut function filter object (vtk based)"""
    obj = doc.addObject("MySimII::MySimIIPostClipFilter", name)
    tmp_filter_list = base_vtk_result.Filter
    tmp_filter_list.append(obj)
    base_vtk_result.Filter = tmp_filter_list
    del tmp_filter_list
    return obj


def makePostVtkFilterWarp(
    doc,
    base_vtk_result,
    name="VtkFilterWarp"
):
    """makePostVtkFilterWarp(document, base_vtk_result, [name]):
    creates an MySimII post processing warp filter object (vtk based)"""
    obj = doc.addObject("MySimII::MySimIIPostWarpVectorFilter", name)
    tmp_filter_list = base_vtk_result.Filter
    tmp_filter_list.append(obj)
    base_vtk_result.Filter = tmp_filter_list
    del tmp_filter_list
    return obj


def makePostVtkResult(
    doc, base_result,
    name="VtkResult"
):
    """makePostVtkResult(document, base_result [name]):
    creates an MySimII post processing result object (vtk based) to hold MySimII results"""
    obj = doc.addObject("MySimII::MySimIIPostPipeline", name)
    obj.load(base_result)
    return obj


# ********* solver objects ***********************************************************************
def makeEquationElasticity(
    doc,
    base_solver
):
    """makeEquationElasticity(document, base_solver):
    creates a MySimII elasticity equation for a solver"""
    obj = doc.SolverElmer.addObject(
        doc.SolverElmer.Proxy.createEquation(doc.SolverElmer.Document, "Elasticity")
    )[0]
    return obj


def makeEquationElectrostatic(
    doc,
    base_solver
):
    """makeEquationElectrostatic(document, base_solver):
    creates a MySimII electrostatic equation for a solver"""
    obj = doc.SolverElmer.addObject(
        doc.SolverElmer.Proxy.createEquation(doc.SolverElmer.Document, "Electrostatic")
    )[0]
    return obj


def makeEquationFlow(
    doc,
    base_solver
):
    """makeEquationFlow(document, base_solver):
    creates a MySimII flow equation for a solver"""
    obj = doc.SolverElmer.addObject(
        doc.SolverElmer.Proxy.createEquation(doc.SolverElmer.Document, "Flow")
    )[0]
    return obj


def makeEquationFluxsolver(
    doc,
    base_solver
):
    """makeEquationFluxsolver(document, base_solver):
    creates a MySimII fluxsolver equation for a solver"""
    obj = doc.SolverElmer.addObject(
        doc.SolverElmer.Proxy.createEquation(doc.SolverElmer.Document, "Fluxsolver")
    )[0]
    return obj


def makeEquationHeat(
    doc,
    base_solver
):
    """makeEquationHeat(document, base_solver):
    creates a MySimII heat equation for a solver"""
    obj = doc.SolverElmer.addObject(
        doc.SolverElmer.Proxy.createEquation(doc.SolverElmer.Document, "Heat")
    )[0]
    return obj


def makeSolverCalculixCcxTools(
    doc,
    name="CalculiXccxTools"
):
    """makeSolverCalculixCcxTools(document, [name]):
    makes a Calculix solver object for the ccx tools module"""
    obj = doc.addObject("MySimII::MySimIISolverObjectPython", name)
    from femobjects import _MySimIISolverCalculix
    _MySimIISolverCalculix._MySimIISolverCalculix(obj)
    if FreeCAD.GuiUp:
        from femguiobjects import _ViewProviderMySimIISolverCalculix
        _ViewProviderMySimIISolverCalculix._ViewProviderMySimIISolverCalculix(obj.ViewObject)
    return obj


def makeSolverCalculix(
    doc,
    name="SolverCalculiX"
):
    """makeSolverCalculix(document, [name]):
    makes a Calculix solver object"""
    import femsolver.calculix.solver
    obj = femsolver.calculix.solver.create(doc, name)
    return obj


def makeSolverElmer(
    doc,
    name="SolverElmer"
):
    """makeSolverElmer(document, [name]):
    makes a Elmer solver object"""
    import femsolver.elmer.solver
    obj = femsolver.elmer.solver.create(doc, name)
    return obj


def makeSolverZ88(
    doc,
    name="SolverZ88"
):
    """makeSolverZ88(document, [name]):
    makes a Z88 solver object"""
    import femsolver.z88.solver
    obj = femsolver.z88.solver.create(doc, name)
    return obj


"""
# get the supportedTypes
App.newDocument()
module = "MySimII"
FreeCADGui.doCommand("import " + module)
for s in sorted(App.ActiveDocument.supportedTypes()):
    if s.startswith(module):
        print(s)

"""

##  @}
