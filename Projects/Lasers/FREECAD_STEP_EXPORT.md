# FreeCAD and STEP export

This environment does not include FreeCAD/OpenCascade, so the generator cannot
write native `.FCStd` or `.step` files directly here.

Use `telecom_laser_geometry_freecad.FCMacro` in FreeCAD:

1. Open FreeCAD.
2. Run `Macro -> Macros... -> Execute` and select the macro.
3. The model will be recreated as colored `Part::Box` solids, one object per
   device part/layer.
4. Save as `.FCStd`, or select the solids and use `File -> Export...` to write
   `.step` / `.stp`.

For ParaView, open `telecom_laser_geometry.vtu`. Use the `part_id`,
`material_id`, or `layer_order` cell arrays to inspect the model layer by layer.
