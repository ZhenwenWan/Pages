#ifndef MYSIMGUI_GEOMETRY_H
#define MYSIMGUI_GEOMETRY_H
#include <memory>
#include <vector>
#include <QtCore>
#include <QString>
#include <QStringList>
#include <QSettings>
#include <App/Document.h>
#include <App/Application.h>
#include <App/DocumentObject.h>
#include <Gui/ViewProvider.h>
#include <Gui/Application.h>
#include <Gui/Selection.h>
#include <Gui/MainWindow.h>
#include <Gui/ViewProviderDocumentObject.h>
#include <Mod/Part/App/PartFeature.h>

#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Compound.hxx>
#include <TopExp_Explorer.hxx>
#include <gp_Pnt.hxx>
#include <gp_Circ.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Elips.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <gp_Vec.hxx>
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepClass_FaceClassifier.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <Precision.hxx>
#include <TColgp_Array2OfPnt.hxx>


namespace MySimIIGui {

class Geometry
{
public:
  
     static void GeometrySum(); //scanning for the basic info
     //       ~Geometry(); //scanning for the basic info
     static void GeometryScanning();
     static void GeometryReader();
     static std::vector<int> SelectedModelFaces(void);
     static QString getFaceInfo(int FaceSN );
     static int getModelFace( std::vector<gp_Pnt> vertexlist );

protected:
	 
public:
  static int NDocObj, NShape, NFace, NEdge;
  static int         vNShape,vNFace,vNEdge;
  static int ModelFace;

};

}//namespace 	
#endif
