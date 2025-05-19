/***************************************************************************
 *   Copyright (c) YEAR YOUR NAME         <Your e-mail address>            *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/

#include "PreCompiled.h"
#ifndef _PreComp_
#include <QMessageBox>
//#include <Geom_CylindricalSurface.hxx>
//#include <Geom_SphericalSurface.hxx>
#endif
#include <TopExp.hxx>
#include <gp_Pnt.hxx>
#include <gp_Circ.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Elips.hxx>
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Shape.hxx>
# include <gp_Pln.hxx>
# include <TopExp_Explorer.hxx>
# include <TopoDS.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include<BRepClass_FaceClassifier.hxx>

# include <BRepAdaptor_Curve.hxx>
# include <BRepAdaptor_Surface.hxx>
# include <gp_Dir.hxx>
# include <gp_Lin.hxx>
# include <gp_Pln.hxx>
# include <gp_Vec.hxx>
# include <ElCLib.hxx>
# include <ElSLib.hxx>
# include <Geom_BezierCurve.hxx>
# include <Geom_BezierSurface.hxx>
# include <Geom_BSplineCurve.hxx>
# include <Geom_BSplineSurface.hxx>
# include <Precision.hxx>
# include <TColgp_Array2OfPnt.hxx>
#include <TopoDS_Face.hxx>
#include <Base/Parameter.h>
#include <Base/Matrix.h>
#include <Base/Vector3D.h>
#include <Base/Interpreter.h>
#include <App/Application.h>
#include <App/Document.h>
#include <App/Annotation.h>
#include <Mod/Part/App/PartFeature.h>

#include <Base/Console.h>
#include <QStringList>
#include <QString>
#include <App/Document.h>
#include <Gui/Application.h>
#include <Gui/Command.h>
#include <Gui/Control.h>
#include <Gui/MainWindow.h>
#include <Gui/FileDialog.h>
#include <Gui/Action.h>
#include <Gui/Document.h>
#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/Tools.h>
#include <App/Document.h>
#include <App/DocumentObjectGroup.h>
#include <App/DocumentObserver.h>
#include <Gui/Selection.h>
#include <Gui/MainWindow.h>
#include <QObject>
#include <QMessageBox>
#include <Gui/Selection.h>
#include <io.h>
#include <direct.h>
#include <time.h>
#include <ctime>
#include <memory>
#include <windows.h>
#include <thread>
#include <iostream>
#include <Gui/ViewProvider.h>
#include <Gui/ViewProviderDocumentObject.h>

#include <App/PropertyLinks.h>
#include <Mod/Part/App/FeaturePartBoolean.h>
#include <Mod/Part/App/FeaturePartFuse.h>
#include <Mod/Part/App/FeaturePartCommon.h>
#include <App/PropertyUnits.h>
#include <Mod/Part/App/PrimitiveFeature.h>
#include <Mod/Part/App/FeaturePartBox.h>
#include <Mod/Part/App/FeaturePartCut.h>
#include <Mod/Part/App/FeatureExtrusion.h>
#include <Mod/Part/App/FeaturePartCircle.h>
#include <App/Property.h>
#include <BRepClass3d_SolidClassifier.hxx>
#include <QElapsedTimer>

#include "InitialMesh.h"
#include "SolutionSetup.h"
#include "FrequencySweep.h"
#include "MeshView.h"
#include "Boundaries.h"
#include "SelectFrequency.h"
#include "Contours.h"
#include "Glyphs.h"
#include "Animat.h"
#include "Geometry.h"
#include "SIF.h"
//#include<Mod/MySimII/Gui/InitialMesh.h>
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
using namespace std;
using namespace MySimIIGui;
QString Geopec=0;//生成geo时判断pec的编号使用
vector<string> geopec;

class CommonFunction{
public:
	
	typedef struct Edge
	{
		string first;
		string second;
		string center;
		string edgetypes;
	}edge;

	typedef struct PointNu
	{
		string Pointsu;
		int Pointnum;
	}pointnu;

	typedef struct Lines
	{
		int num;
		int first;
		int second;
	}lines;
	typedef struct Circle
	{//记录圆的point
		string Pointcircle;
		int circlenum;
	}circle;

	typedef struct Linecircle
	{//记录圆的线
		string linecircle;
		int linecirclenum;
	}linecircle;

	typedef struct PlaneSurface
	{
		int Planenum;//plan的编号
		double Planexyz;//共面不变的数据
		string planestr;//组成平面的数据
		int Axis;//共面的坐标轴
		int flag;//判断谁包含谁1plan2circle
		double length;//也是判断谁包含谁的，圆面与圆面，
		int Media;//记录是Media的
	}planesurface;

	typedef struct Geometry{//geo文件生成处理布尔运算
		string geom;//记录几何
		string boolean;//记录布尔模型的名称
		int num;//记录写入的模型
		string boole;//记录三种布尔类型
		int m;// 编号
		int plan;//记录是平面
	}geometry;

	typedef struct Boolean{
		string str;//组合布尔字符串
		int bnum;//布尔编号
		string Booleanname;
		bool operator <(Boolean & b) {
			return bnum<b.bnum;
		}
	}boolean;

	typedef struct Cut{//记录cut的base和tool
		string cutname;
		string basename;
		string toolname;
	}cut;

	typedef struct GeoDelete{//记录进行布尔运算的进行删除
		string str;
		int num1;
		int num2;
	}geodelete;

	/////////////////surface.txt处理的///////////////
	typedef struct Facemodel{
		string type;
		string number;
	}facemodel;

	typedef struct Facesurface{
		string type;
		string loop;
		string number;
	}facesurface;

	typedef struct Facelineoop{
		string type;
		string loop;
		string number;
	}facelineoop;

	typedef struct Faceline{
		string type;
		string loop;
		string number;
	}faceline;

	typedef struct Facepoint{
		string type;
		string loop;
		string number;
		string x;
		string y;
		string z;
	}facepoint;

	//输出surface时处理facedir
	typedef struct Facedir{
		int fnum;
		string edgeloop;
		string pointloop;
		string dir;
		double p2x;
		double p2y;
		double p2z;
	}facedir;

	typedef struct Fnumvector{
		int num;
		string edge;
	}fnumvector;

	typedef struct Orientation{
		int num;
		string dir;
	}orientation;

static void createsurface(){
	QString SaveFile = QString::fromLatin1("../ini/surfaces.txt");
	QFile SFile(SaveFile);
	if (!SFile.open(QIODevice::ReadWrite))
	{

		return;
	}
	QTextStream Savestreamfile(&SFile);
	////计数几何体
	int ShapeCountsum = 0;
	int geoflag = 0;//2018119
	App::Document* docs = App::GetApplication().getActiveDocument();
	//Base::Console().Message("Name %s\n", docs->getName());
	std::vector<App::DocumentObject*> sels = docs->getObjectsOfType(App::DocumentObject::getClassTypeId());
	for (std::vector<App::DocumentObject*>::iterator it = sels.begin(); it != sels.end(); ++it)
	{
		std::string Group = (*it)->getNameInDocument();
		QString Groups = QString::fromStdString(Group);
		if (Groups.left(5) == QObject::tr("Group")){
			//Base::Console().Message("Group==Group\n");
			continue;
		}
		if (Groups.left(5) == QObject::tr("Plane")){
			//Base::Console().Message("Group==Plane\n");
			geoflag = 1;
		}
		std::string line = (*it)->getNameInDocument();
		QString lines = QString::fromStdString(line);
		if (lines.left(4) == QObject::tr("Line")){
			//Base::Console().Message("line==line\n");
			continue;
		}
		Gui::ViewProvider* pcProv = Gui::Application::Instance->getViewProvider(*it);
		bool visibility = true;
		if (pcProv /*&& pcProv->getTypeId().isDerivedFrom(Gui::ViewProviderDocumentObject::getClassTypeId())*/) {
			visibility = ((Gui::ViewProviderDocumentObject*)pcProv)->Visibility.getValue();
		}
		// 读取几何体可见(Visibility)属性值  START  END
		if (visibility == false){// 不可见的跳过
			continue;
		}
		ShapeCountsum++;

	}

	QString ShapeCount = QObject::tr("ShapeCount %1\n").arg(ShapeCountsum);
	Savestreamfile << ShapeCount;
	///////
	int shapesum = --ShapeCountsum;
	int num = 0;//整体计数face
	int ShapeSumNo = 0;//计数几何体
	int ShapeNo = 0;//打印的几何体
	App::Document* doc = App::GetApplication().getActiveDocument();
	std::vector<App::DocumentObject*> sel = doc->getObjectsOfType(App::DocumentObject::getClassTypeId());
	for (std::vector<App::DocumentObject*>::iterator it = sel.begin(); it != sel.end(); ++it)
	{

		//Base::Console().Message("DocumentObject %s\n", (*it)->getTypeId().getName());
		
		Gui::ViewProvider* pcProv = Gui::Application::Instance->getViewProvider(*it);
		bool visibility = true;
		if (pcProv /*&& pcProv->getTypeId().isDerivedFrom(Gui::ViewProviderDocumentObject::getClassTypeId())*/) {
			visibility = ((Gui::ViewProviderDocumentObject*)pcProv)->Visibility.getValue();
		}
		// 读取几何体可见(Visibility)属性值  START  END
		if (visibility == false){// 不可见的跳过
			continue;
		}

		std::string Group = (*it)->getNameInDocument();
		QString Groups = QString::fromStdString(Group);
		if (Groups.left(5) == QObject::tr("Group")){
			//Base::Console().Message("Group==Group\n");
			continue;
		}

		std::string line = (*it)->getNameInDocument();
		QString lines = QString::fromStdString(line);
		if (lines.left(4) == QObject::tr("Line")){
			//Base::Console().Message("line==line\n");
			continue;
		}
		ShapeNo++;
		std::string docNames = (*it)->getNameInDocument();
		//Base::Console().Message("getNameInDocument %s \n", docNames);
		QString ss = QObject::tr("ShapeNo. %1  ShapeType(%2)").arg(ShapeNo).arg(QString::fromStdString(docNames));
		Savestreamfile << ss;
		Savestreamfile << "\n";


		App::DocumentObject *faceObj = (*it);

		const TopoDS_Shape& tShape = static_cast<Part::Feature *>(faceObj)->Shape.getValue();
		//记录一个几何体的面数
		TopExp_Explorer countFace;
		int facecount = 0;
		for (countFace.Init(tShape, TopAbs_FACE); countFace.More(); countFace.Next())
		{
			facecount++;
			const TopoDS_Face &actFaces = TopoDS::Face(countFace.Current());
			BRepAdaptor_Surface surfacec(actFaces);
			if (surfacec.GetType() == GeomAbs_Cylinder&&geoflag){
				facecount++;
			}
		}

		QString faceCount = QObject::tr("FaceCount %1\n").arg(facecount);
		Savestreamfile << faceCount;

		/////////////获取面
		TopExp_Explorer expFace;
		int FaceLocalNo = 0;
		for (expFace.Init(tShape, TopAbs_FACE); expFace.More(); expFace.Next())
		{
			num++;
			FaceLocalNo++;
			int faceNum = CommonFunction::GetFaceCodeWithGenCodeFromData(num);//直接获取网格面的编号
			QString ss = QObject::tr("FaceGlobalNo. %1  FaceLocalNo. %2\n").arg(faceNum).arg(FaceLocalNo);
			Savestreamfile << ss;
			//Savestreamfile << "\n";

			const TopoDS_Face &actFace = TopoDS::Face(expFace.Current());

			double cy = 0, cx = 0, cz = 0;
			///////////////////判断是什么面
			int flagPlan = 0;
			int flagwall = 0;///判断是柱面的标志2018
			QString planDir;
			gp_XYZ dir(0, 0, 0);
			//dir = Fem::Tools::getDirection(TopoDS::Face(actFace));
			BRepAdaptor_Surface surface(actFace);
			int kk = surface.Face().Orientation();
			if (surface.GetType() == GeomAbs_Plane) {

				Savestreamfile << "FaceType(Plane) ";
				dir = surface.Plane().Axis().Direction().XYZ();
				
				//////// ////////////////
				QString cX, cY, cZ;
				//QString ss;
				if (kk){
					cX.sprintf("%11.3e", 0 - dir.X());
					cY.sprintf("%11.3e", 0 - dir.Y());
					cZ.sprintf("%11.3e", 0 - dir.Z());
					planDir = QObject::tr("FaceDir(%1,%2,%3)\n").arg(cX).arg(cY).arg(cZ);
				}
				else{
					cX.sprintf("%11.3e", dir.X());
					cY.sprintf("%11.3e", dir.Y());
					cZ.sprintf("%11.3e", dir.Z());
					planDir = QObject::tr("FaceDir(%1,%2,%3)\n").arg(cX).arg(cY).arg(cZ);
				}
				//Savestreamfile << planDir;
				flagPlan = 1;
			}
			else if (surface.GetType() == GeomAbs_BSplineSurface) {

				Savestreamfile << "FaceType(BSplineSurface) ";
				QString cX, cY, cZ;
				cX.sprintf("%11.3e", 0);
				cY.sprintf("%11.3e", 0);
				cZ.sprintf("%11.3e", 0);
				planDir = QObject::tr("FaceDir(%1,%2,%3)\n").arg(cX).arg(cY).arg(cZ);

			}
			else if (surface.GetType() == GeomAbs_Cylinder) {

				//201818 Savestreamfile << "FaceType(Cylinder wall) ";
				if (!(flagwall&&geoflag)){//拆分时处理2018118
					Savestreamfile << "FaceType(Cylinder wall) ";
				}

				dir = surface.Cylinder().Axis().Direction().XYZ();
				double R = surface.Cylinder().Radius();
				flagwall = 1;//柱面的标志2018
				gp_Pnt l = surface.Cylinder().Location();//////////
				cx = l.X();
				cy = l.Y();
				cz = l.Z();
				////////////////////////

				QString cX, cY, cZ;
				QString ss;
				if (kk){
					cX.sprintf("%11.3e", 0 - dir.X());
					cY.sprintf("%11.3e", 0 - dir.Y());
					cZ.sprintf("%11.3e", 0 - dir.Z());
					//ss = QObject::tr("FaceDir(%1,%2,%3)\n").arg(cX).arg(cY).arg(cZ);
					planDir = QObject::tr("FaceDir(%1,%2,%3)\n").arg(cX).arg(cY).arg(cZ);
				}
				else{
					cX.sprintf("%11.3e", dir.X());
					cY.sprintf("%11.3e", dir.Y());
					cZ.sprintf("%11.3e", dir.Z());
					//ss = QObject::tr("FaceDir(%1,%2,%3)\n").arg(cX).arg(cY).arg(cZ);///////
					planDir = QObject::tr("FaceDir(%1,%2,%3)\n").arg(cX).arg(cY).arg(cZ);
				}
				
			}
			else if (surface.GetType() == GeomAbs_Cone) {

				Savestreamfile << "FaceType(Cone wall) ";
				dir = surface.Cone().Axis().Direction().XYZ();
				double R1 = surface.Cone().RefRadius();
				double R2 = surface.Cone().SemiAngle();
				
				gp_Pnt l = surface.Cone().Location();//////////
				cx = l.X();
				cy = l.Y();
				cz = l.Z();
				////////////////////////
				QString cX, cY, cZ;
				QString ss;
				if (kk){
					cX.sprintf("%11.3e", 0 - dir.X());
					cY.sprintf("%11.3e", 0 - dir.Y());
					cZ.sprintf("%11.3e", 0 - dir.Z());
					//ss = QObject::tr("FaceDir(%1,%2,%3)\n").arg(cX).arg(cY).arg(cZ);
					planDir = QObject::tr("FaceDir(%1,%2,%3)\n").arg(cX).arg(cY).arg(cZ);
				}
				else{
					cX.sprintf("%11.3e", dir.X());
					cY.sprintf("%11.3e", dir.Y());
					cZ.sprintf("%11.3e", dir.Z());
					//ss = QObject::tr("FaceDir(%1,%2,%3)\n").arg(cX).arg(cY).arg(cZ);////////
					planDir = QObject::tr("FaceDir(%1,%2,%3)\n").arg(cX).arg(cY).arg(cZ);
				}
				//Savestreamfile << ss;///////
				//flagPlan = 1;////////
			}
			else if (surface.GetType() == GeomAbs_Sphere) {

				Savestreamfile << "FaceType(Sphere shell) ";
				double  r = surface.Sphere().Radius();
				
				gp_Pnt l = surface.Sphere().Location();//////////
				cx = l.X();
				cy = l.Y();
				cz = l.Z();
				////////////////////////
				QString cX, cY, cZ;
				QString ss;
				if (kk){
					cX.sprintf("%11.3e", 0 - dir.X());
					cY.sprintf("%11.3e", 0 - dir.Y());
					cZ.sprintf("%11.3e", 0 - dir.Z());
					//ss = QObject::tr("FaceDir(%1,%2,%3)\n").arg(cX).arg(cY).arg(cZ);
					planDir = QObject::tr("FaceDir(%1,%2,%3)\n").arg(cX).arg(cY).arg(cZ);
				}
				else{
					cX.sprintf("%11.3e", dir.X());
					cY.sprintf("%11.3e", dir.Y());
					cZ.sprintf("%11.3e", dir.Z());
					///ss = QObject::tr("FaceDir(%1,%2,%3)\n").arg(cX).arg(cY).arg(cZ);
					planDir = QObject::tr("FaceDir(%1,%2,%3)\n").arg(cX).arg(cY).arg(cZ);
				}
				
			}
			else if (surface.GetType() == GeomAbs_Torus) {

				Savestreamfile << "FaceType(Torus wall) ";
				dir = surface.Torus().Axis().Direction().XYZ();
				double maR = surface.Torus().MajorRadius();
				double miR = surface.Torus().MinorRadius();
				
				gp_Pnt l = surface.Torus().Location();//////////
				cx = l.X();
				cy = l.Y();
				cz = l.Z();
				////////////////////////
				QString cX, cY, cZ;
				QString ss;
				if (kk){
					cX.sprintf("%11.3e", 0 - dir.X());
					cY.sprintf("%11.3e", 0 - dir.Y());
					cZ.sprintf("%11.3e", 0 - dir.Z());
					//ss = QObject::tr("FaceDir(%1,%2,%3)\n").arg(cX).arg(cY).arg(cZ);
					planDir = QObject::tr("FaceDir(%1,%2,%3)\n").arg(cX).arg(cY).arg(cZ);
				}
				else{
					cX.sprintf("%11.3e", dir.X());
					cY.sprintf("%11.3e", dir.Y());
					cZ.sprintf("%11.3e", dir.Z());
					///ss = QObject::tr("FaceDir(%1,%2,%3)\n").arg(cX).arg(cY).arg(cZ);
					planDir = QObject::tr("FaceDir(%1,%2,%3)\n").arg(cX).arg(cY).arg(cZ);//////////
				}
				//Savestreamfile << ss;//////

				//flagPlan = 1;////////
			}
			else if (surface.GetType() == GeomAbs_BezierSurface) {

				Savestreamfile << "FaceType(BezierSurface) ";
				QString cX, cY, cZ;
				cX.sprintf("%11.3e", 0);
				cY.sprintf("%11.3e", 0);
				cZ.sprintf("%11.3e", 0);
				planDir = QObject::tr("FaceDir(%1,%2,%3)\n").arg(cX).arg(cY).arg(cZ);
			}
			else if (surface.GetType() == GeomAbs_SurfaceOfRevolution) {

				Savestreamfile << "FaceType(SurfaceOfRevolution) ";
				dir = surface.AxeOfRevolution().Direction().XYZ();
				
				gp_Pnt l = surface.AxeOfRevolution().Location();//////////
				cx = l.X();
				cy = l.Y();
				cz = l.Z();
				QString cX, cY, cZ;
				QString ss;
				if (kk){
					cX.sprintf("%11.3e", 0 - dir.X());
					cY.sprintf("%11.3e", 0 - dir.Y());
					cZ.sprintf("%11.3e", 0 - dir.Z());
					
					planDir = QObject::tr("FaceDir(%1,%2,%3)\n").arg(cX).arg(cY).arg(cZ);
				}
				else{
					cX.sprintf("%11.3e", dir.X());
					cY.sprintf("%11.3e", dir.Y());
					cZ.sprintf("%11.3e", dir.Z());
					
					planDir = QObject::tr("FaceDir(%1,%2,%3)\n").arg(cX).arg(cY).arg(cZ);
				}
				
			}
			else if (surface.GetType() == GeomAbs_SurfaceOfExtrusion) {

				Savestreamfile << "FaceType(SurfaceOfExtrusion) ";
				QString cX, cY, cZ;
				cX.sprintf("%11.3e", 0);
				cY.sprintf("%11.3e", 0);
				cZ.sprintf("%11.3e", 0);
				planDir = QObject::tr("FaceDir(%1,%2,%3)\n").arg(cX).arg(cY).arg(cZ);

			}
			else if (surface.GetType() == GeomAbs_OffsetSurface) {

				Savestreamfile << "FaceType(OffsetSurface) ";
				QString cX, cY, cZ;
				cX.sprintf("%11.3e", 0);
				cY.sprintf("%11.3e", 0);
				cZ.sprintf("%11.3e", 0);
				planDir = QObject::tr("FaceDir(%1,%2,%3)\n").arg(cX).arg(cY).arg(cZ);


			}
			else if (surface.GetType() == GeomAbs_OtherSurface) {

				Savestreamfile << "FaceType(OtherSurface) ";
				QString cX, cY, cZ;
				cX.sprintf("%11.3e", 0);
				cY.sprintf("%11.3e", 0);
				cZ.sprintf("%11.3e", 0);
				planDir = QObject::tr("FaceDir(%1,%2,%3)\n").arg(cX).arg(cY).arg(cZ);

			}

			
			//////////从面里面获取边
			typedef list<CommonFunction::edge> edgelist;

			edgelist elist;
			////////
			////获取边的总数//
			TopExp_Explorer countEdge;
			int Edgesum = 0;
			for (countEdge.Init(expFace.Current(), TopAbs_EDGE); countEdge.More(); countEdge.Next())
			{
				Edgesum++;
			}
			if (!(flagwall&&geoflag)){//拆分时处理2018118
				QString EdgeCount = QObject::tr("EdgeCount %1\n").arg(Edgesum);
				Savestreamfile << EdgeCount;
			}

			TopExp_Explorer expEdge;
			int i = 0;
			vector<QString> V;
			map<string, string> M;
			int flagArch = 0;
			int Archsum = 0;
			for (expEdge.Init(expFace.Current(), TopAbs_EDGE); expEdge.More(); expEdge.Next())
			{
				i++;
				CommonFunction::edge structEdge;////////////


				const TopoDS_Edge &actEdge = TopoDS::Edge(expEdge.Current());
				///////////
				int flag = 0;
				BRepAdaptor_Curve curve(actEdge);
				if (curve.GetType() == GeomAbs_Line) {
					dir = curve.Line().Direction().XYZ();
					QString EdgeType = QObject::tr(" EdgeType(Line)\n");
					//Savestreamfile << EdgeType;
					structEdge.edgetypes = EdgeType.toStdString();
				}
				else if (curve.GetType() == GeomAbs_Circle){
					QString EdgeType = QObject::tr(" EdgeType(Arch)\n");
					gp_Pnt location = curve.Circle().Location();
					cx = location.X();
					cy = location.Y();
					cz = location.Z();
					flag = 1;
					Archsum++;
					structEdge.edgetypes = EdgeType.toStdString();
				}
				else if (curve.GetType() == GeomAbs_Ellipse){
					QString EdgeType = QObject::tr(" EdgeType(Ellipse)\n");

					structEdge.edgetypes = EdgeType.toStdString();
				}
				else if (curve.GetType() == GeomAbs_Hyperbola){
					QString EdgeType = QObject::tr(" EdgeType(Hyperbola)\n");

					structEdge.edgetypes = EdgeType.toStdString();
				}
				else if (curve.GetType() == GeomAbs_Parabola){
					QString EdgeType = QObject::tr(" EdgeType(Parabola)\n");

					structEdge.edgetypes = EdgeType.toStdString();
				}
				else if (curve.GetType() == GeomAbs_BezierCurve){
					QString EdgeType = QObject::tr(" EdgeType(BezierCurve)\n");

					structEdge.edgetypes = EdgeType.toStdString();
				}
				else if (curve.GetType() == GeomAbs_BSplineCurve){
					QString EdgeType = QObject::tr(" EdgeType(BSplineCurve)\n");

					structEdge.edgetypes = EdgeType.toStdString();
				}
				else if (curve.GetType() == GeomAbs_OtherCurve){
					QString EdgeType = QObject::tr(" EdgeType(OtherCurve)\n");
					structEdge.edgetypes = EdgeType.toStdString();
				}
				TopoDS_Vertex vertex = TopExp::FirstVertex(actEdge);
				gp_Pnt pnt = BRep_Tool::Pnt(vertex);
				TopoDS_Vertex vertexs = TopExp::LastVertex(actEdge);
				gp_Pnt pnts = BRep_Tool::Pnt(vertexs);
				QString strX, strY, strZ;
				strX.sprintf("%11.3e", pnt.X());
				strY.sprintf("%11.3e", pnt.Y());
				strZ.sprintf("%11.3e", pnt.Z());
				QString fipnt = QObject::tr("%1,%2,%3").arg(strX).arg(strY).arg(strZ);

				V.push_back(fipnt);
				structEdge.first = fipnt.toStdString();////////////////
				QString strsX, strsY, strsZ;
				strsX.sprintf("%11.3e", pnts.X());
				strsY.sprintf("%11.3e", pnts.Y());
				strsZ.sprintf("%11.3e", pnts.Z());
				QString lapnts = QObject::tr("%1,%2,%3").arg(strsX).arg(strsY).arg(strsZ);

				V.push_back(lapnts);
				structEdge.second = lapnts.toStdString();////////////////
				if (flag){
					QString cX, cY, cZ;
					cX.sprintf("%11.3e", cx);
					cY.sprintf("%11.3e", cy);
					cZ.sprintf("%11.3e", cz);
					QString center = QObject::tr("%1,%2,%3").arg(cX).arg(cY).arg(cZ);

					structEdge.center = center.toStdString();
				}
				else{
					structEdge.center = "1";
				}

				elist.push_back(structEdge);////////////

			}
			if (Edgesum == Archsum){
				flagArch = 1;
			}
			////////////////////center////////////
			if (flagPlan){
				if (flagArch){///如果是圆边
					QString cX, cY, cZ;
					cX.sprintf("%11.3e", cx);
					cY.sprintf("%11.3e", cy);
					cZ.sprintf("%11.3e", cz);
					QString Pcenter = QObject::tr("FaceCenter(%1,%2,%3) ").arg(cX).arg(cY).arg(cZ);
					Savestreamfile << Pcenter;
					Savestreamfile << planDir;
				}
				else{
					edgelist center;
					center = elist;
					string cstr;
					double sumX = 0;
					double sumY = 0;
					double sumZ = 0;
					int ci = 0;
					for (CommonFunction::edge C : center){
						cstr = C.second;
						string cfstr = C.first;
						ci++;
						QString cfirst = QString::fromStdString(C.first);
						double cfX = cfirst.section(QObject::tr(","), 0, 0).toDouble();
						double cfY = cfirst.section(QObject::tr(","), 1, 1).toDouble();
						double cfZ = cfirst.section(QObject::tr(","), 2, 2).toDouble();

						QString csecond = QString::fromStdString(C.second);
						double centX = csecond.section(QObject::tr(","), 0, 0).toDouble();
						double centY = csecond.section(QObject::tr(","), 1, 1).toDouble();
						double centZ = csecond.section(QObject::tr(","), 2, 2).toDouble();
						sumX += cfX + centX;
						sumY += cfY + centY;
						sumZ += cfZ + centZ;

					}
					QString clX, clY, clZ;
					clX.sprintf("%11.3e", sumX / (ci * 2));
					clY.sprintf("%11.3e", sumY / (ci * 2));
					clZ.sprintf("%11.3e", sumZ / (ci * 2));
					QString Pcenter = QObject::tr("FaceCenter(%1,%2,%3) ").arg(clX).arg(clY).arg(clZ);
					Savestreamfile << Pcenter;
					Savestreamfile << planDir;
				}
			}
			else{
				QString cX, cY, cZ;
				cX.sprintf("%11.3e", cx);
				cY.sprintf("%11.3e", cy);
				cZ.sprintf("%11.3e", cz);
				if (!(flagwall&&geoflag)){///拆分时再进行处理201818
					QString Pcenter = QObject::tr("FaceCenter(%1,%2,%3) ").arg(cX).arg(cY).arg(cZ);
					Savestreamfile << Pcenter;
					Savestreamfile << planDir;
				}
			}
			/////////////////////////////////////


			list<CommonFunction::edge>::iterator it;
			int p = 0;

			int Edsum = 0;
			string k;
			edgelist elist2;
			elist2 = elist;
			vector<string> Reverse;///逆序
			vector<string> EdgeTy;///边类型
			vector<string> Edgcenter;///
			for (CommonFunction::edge B : elist){
				p++;
				bool tof = true;
				if (p == 1){
					k = B.second;
				}
				for (it = elist2.begin(); it != elist2.end(); ++it){
					if (k == (*it).first){
						Edsum++;
						EdgeTy.push_back((*it).edgetypes);///
						Reverse.push_back((*it).first);///
						Reverse.push_back((*it).second);///
						if ((*it).center != "1"){
							Edgcenter.push_back((*it).center);////
						}
						else{///
							Edgcenter.push_back("1");////
						}///
						tof = false;
						k = (*it).second;
						elist2.erase(it);
						break;
					}
					if (k == (*it).second){
						Edsum++;
						EdgeTy.push_back((*it).edgetypes);///
						Reverse.push_back((*it).second);///
						Reverse.push_back((*it).first);///
						if ((*it).center != "1"){
							Edgcenter.push_back((*it).center);////
						}
						else{///
							Edgcenter.push_back("1");////
						}///
						tof = false;
						k = (*it).first;
						elist2.erase(it);
						break;
					}

				}//for	///////////////
			}


			//////////修改边的连接顺序为右手法则//////////////
			vector<string>::iterator itE;
			vector<string>::iterator itT;
			vector<string>::iterator itC;
			int No = 1;
			///vector<string>::reverse_iterator it;//reverse
			if (kk){//逆序输出
				for (itE = Reverse.end() - 1, itT = EdgeTy.end() - 1, itC = Edgcenter.end() - 1; itE != Reverse.begin() - 1, itT != EdgeTy.begin() - 1, itC != Edgcenter.begin() - 1; itE--, itT--, itC--){
					//for (itE = Reverse.end(), itT = EdgeTy.end(), itC = Edgcenter.end(); itE != Reverse.begin(), itT != EdgeTy.begin(), itC != Edgcenter.begin(); itE--, itT--, itC--){
					QString EdgeNo = QObject::tr("EdgeNo. %1 ").arg(No++);
					Savestreamfile << EdgeNo;
					Savestreamfile << QString::fromStdString(*itT);
					Savestreamfile << QString::fromStdString(*itE);
					Savestreamfile << ",";
					itE--;
					Savestreamfile << QString::fromStdString(*itE);
					if (*itC != "1"){
						Savestreamfile << ",";////
						Savestreamfile << QString::fromStdString(*itC);
					}
					Savestreamfile << "\n";
				}

			}
			else{//直接输出
				//QString pnt;
				////2018118
				double pntx = 0;
				double pnty = 0;
				double pntz = 0;
				double locationx = 0;
				double locationy = 0;
				double locationz = 0;
				double Rx = 0;
				double Ry = 0;
				double Rz = 0;
				QString line1;
				QString line2;
				int lineflag12 = 0;
				int archflag = 0;
				QString pnt;
				QString location;
				QString R;
				///
				for (itE = Reverse.begin(), itT = EdgeTy.begin(), itC = Edgcenter.begin(); itE != Reverse.end(), itT != EdgeTy.end(), itC != Edgcenter.end(); itE++, itT++, itC++){
					////
					/*Base::Console().Message("==x==y==z\n");
					if (*itC != "1"){
						locationx = QString::fromStdString(*itC).section(QObject::tr(","), 0, 0).toDouble();
						locationy = QString::fromStdString(*itC).section(QObject::tr(","), 1, 1).toDouble();
						locationz = QString::fromStdString(*itC).section(QObject::tr(","), 1, 1).toDouble();
						Base::Console().Message("2==x%f==y%f==z%f\n", locationx, locationy, locationz);
					}*/
					if (flagwall&&geoflag){//拆分柱面2018118
						Base::Console().Message("itT=arch=%s\n", *itC);
						if (*itT == " EdgeType(Arch)\n")
						{
							pntx = QString::fromStdString(*itE).section(QObject::tr(","), 0, 0).toDouble();
							pnty = QString::fromStdString(*itE).section(QObject::tr(","), 1, 1).toDouble();
							pntz = QString::fromStdString(*itE).section(QObject::tr(","), 2, 2).toDouble();
							//Base::Console().Message("1==x%f==y%f==z%f\n",pntx,pnty,pntz);
							itE++;
							locationx=QString::fromStdString(*itC).section(QObject::tr(","), 0, 0).toDouble();
							locationy = QString::fromStdString(*itC).section(QObject::tr(","), 1, 1).toDouble();
							locationz = QString::fromStdString(*itC).section(QObject::tr(","), 2, 2).toDouble();
							//Base::Console().Message("2==x%f==y%f==z%f\n",locationx,locationy,locationz);
							if (locationx!= pntx){
								if (locationx > pntx){
									Rx = (locationx - pntx) * 2 + pntx;
								}
								else{
									Rx = pntx - (pntx - locationx) * 2;
								}
							}
							else{
								Rx = pntx;
							}
							if (locationy != pnty){
								if (locationy> pnty){
									Ry = (locationy - pnty) * 2 + pnty;

								}
								else{
									Ry = pnty - (pnty - locationy) * 2;

								}
							}
							else{
								Ry = pnty;
							}
							if (locationz != pntz){
								if (locationz > pntz){
									Rz = (locationz - pntz) * 2 + pntz;
								}
								else{
									Rz = pntz - (pntz - locationz) * 2;
								}
							}
							else{
								Rz = pntz;
							}
							if (archflag == 0){//保存第一个arch的数据
								QString cX, cY, cZ;
								cX.sprintf("%11.3e", pntx);
								cY.sprintf("%11.3e", pnty);
								cZ.sprintf("%11.3e", pntz);
							  pnt = QObject::tr("%1,%2,%3 ").arg(cX).arg(cY).arg(cZ);
							  cX.sprintf("%11.3e", locationx);
							  cY.sprintf("%11.3e", locationy);
							  cZ.sprintf("%11.3e", locationz);
							  location = QObject::tr("%1,%2,%3 ").arg(cX).arg(cY).arg(cZ);
							  cX.sprintf("%11.3e", Rx);
							  cY.sprintf("%11.3e", Ry);
							  cZ.sprintf("%11.3e", Rz);
							  R = QObject::tr("%1,%2,%3 ").arg(cX).arg(cY).arg(cZ);
							  archflag = 1;
							}
						}//if arch
						//if (*itT == " EdgeType(Line)\n"){
						//	Base::Console().Message("itT=line=%s\n", *itC);
						//	if (lineflag12 == 0){
						//	line1=QString::fromStdString(*itE);
						//		itE++;
						//	line2=QString::fromStdString(*itE);
						//	lineflag12 = 1;
						//	}
						//}//if typeline
					}//if flagwall
					/////
					else{
						QString EdgeNo = QObject::tr("EdgeNo. %1 ").arg(No++);
						Savestreamfile << EdgeNo;
						Savestreamfile << QString::fromStdString(*itT);
						Savestreamfile << QString::fromStdString(*itE);
						Savestreamfile << ",";
						itE++;
						Savestreamfile << QString::fromStdString(*itE);
						if (*itC != "1"){
							Savestreamfile << ",";////
							Savestreamfile << QString::fromStdString(*itC);
						}
						Savestreamfile << "\n";
					}
				}//for
				if (flagwall&&geoflag){//拆分柱面写入surface2018118
					Savestreamfile << "FaceType(Cylinder wall) ";
					QString EdgeCount = QObject::tr("EdgeCount 4\n");
					Savestreamfile << EdgeCount;
					QString cXp, cYp, cZp;
					cXp.sprintf("%11.3e", cx);
					cYp.sprintf("%11.3e", cy);
					cZp.sprintf("%11.3e", cz);
				    QString Pcenter = QObject::tr("FaceCenter(%1,%2,%3) ").arg(cXp).arg(cYp).arg(cZp);
					Savestreamfile << Pcenter;
					Savestreamfile << planDir;
					QString cX, cY, cZ;
					cX.sprintf("%11.3e", pntx);
					cY.sprintf("%11.3e", pnty);
					cZ.sprintf("%11.3e", pntz);
					QString pnt2 = QObject::tr("%1,%2,%3 ").arg(cX).arg(cY).arg(cZ);
					cX.sprintf("%11.3e", locationx);
					cY.sprintf("%11.3e", locationy);
					cZ.sprintf("%11.3e", locationz);
					QString location2 = QObject::tr("%1,%2,%3 ").arg(cX).arg(cY).arg(cZ);
					cX.sprintf("%11.3e", Rx);
					cY.sprintf("%11.3e", Ry);
					cZ.sprintf("%11.3e", Rz);
					QString R2 = QObject::tr("%1,%2,%3 ").arg(cX).arg(cY).arg(cZ);
					Savestreamfile << QObject::tr("EdgeNo. 1 ");
					Savestreamfile << QObject::tr(" EdgeType(Arch)\n");
					Savestreamfile << QObject::tr("%1%2%3\n").arg(pnt).arg(R).arg(location);
					Savestreamfile << QObject::tr("EdgeNo. 2 ");
					Savestreamfile << QObject::tr(" EdgeType(Line)\n");
					Savestreamfile << QObject::tr("%1%2\n").arg(R).arg(R2);
					Savestreamfile << QObject::tr("EdgeNo. 3 ");
					Savestreamfile << QObject::tr(" EdgeType(Arch)\n");
					Savestreamfile << QObject::tr("%1%2%3\n").arg(R2).arg(pnt2).arg(location2);
					Savestreamfile << QObject::tr("EdgeNo. 4 ");
					Savestreamfile << QObject::tr(" EdgeType(Line)\n");
					Savestreamfile << QObject::tr("%1%2\n").arg(pnt2).arg(R);
					Savestreamfile << "\n";
					//第二个半面					
					FaceLocalNo++;
					int faceNums = CommonFunction::GetFaceCodeWithGenCodeFromData(num);//直接获取网格面的编号
					QString ss = QObject::tr("FaceGlobalNo. %1  FaceLocalNo. %2\n").arg(faceNums+1).arg(FaceLocalNo);
					Savestreamfile << ss;
					Savestreamfile << QObject::tr("FaceType(Cylinder wall) ");
					QString EdgeCounts = QObject::tr("EdgeCount 4\n");
					Savestreamfile << EdgeCounts;
					Savestreamfile << Pcenter;
					Savestreamfile << planDir;
					Savestreamfile << QObject::tr("EdgeNo. 1 ");
					Savestreamfile << QObject::tr(" EdgeType(Arch)\n");
					Savestreamfile << QObject::tr("%1%2%3\n").arg(R).arg(pnt).arg(location);
					Savestreamfile << QObject::tr("EdgeNo. 2 ");
					Savestreamfile << QObject::tr(" EdgeType(Line)\n");
					Savestreamfile << QObject::tr("%1%2\n").arg(pnt).arg(pnt2);
					Savestreamfile << QObject::tr("EdgeNo. 3 ");
					Savestreamfile << QObject::tr(" EdgeType(Arch)\n");
					Savestreamfile << QObject::tr("%1%2%3\n").arg(pnt2).arg(R2).arg(location2);
					Savestreamfile << QObject::tr("EdgeNo. 4 ");
					Savestreamfile << QObject::tr(" EdgeType(Line)\n");
					Savestreamfile << QObject::tr("%1%2").arg(R2).arg(R);
					Savestreamfile << "\n";
				}
			}//else
			//////////输出没有顺序连接起来的边///////
			if (elist2.size() != 0){
				list<CommonFunction::edge>::iterator it;
				for (it = elist2.begin(); it != elist2.end(); it++){
					QString EdgeNo = QObject::tr("EdgeNo. %1 ").arg(No++);
					Savestreamfile << EdgeNo;
					//EdgeTy.push_back((*it).edgetypes);///
					Savestreamfile << QString::fromStdString((*it).edgetypes);
					//Reverse.push_back((*it).second);///
					//Reverse.push_back((*it).first);///
					Savestreamfile << QString::fromStdString((*it).first);
					Savestreamfile << ",";
					Savestreamfile << QString::fromStdString((*it).second);
					if ((*it).center != "1"){
						Savestreamfile << ",";////
						//Edgcenter.push_back((*it).center);////
						Savestreamfile << QString::fromStdString((*it).center);
					}
					Savestreamfile << "\n";
				}

			}
			////////////////////////////////////////////////
			Savestreamfile << "\n";

		}
		//Base::Console().Message("22num==%d\n", num);
		Savestreamfile << "\n";
	}//for end
	SFile.close();

}
static void MapFaceNumberWithMesh(void)
{
	std::map<int, int> FaceMapNum;

	QSettings settings(QObject::tr("../ini/Default.ini"), QSettings::IniFormat);
	QString BoundaryMeshFile = settings.value(QObject::tr("MESHDATAFILE/BoundaryMesh")).toString();
	QString NodesMeshFile = settings.value(QObject::tr("MESHDATAFILE/NodesMesh")).toString();

	if (BoundaryMeshFile.isEmpty() || NodesMeshFile.isEmpty()){
		BoundaryMeshFile = QString::fromLatin1("../ini/mm/mesh.boundary");
		NodesMeshFile = QString::fromLatin1("../ini/mm/mesh.nodes");
	}

	QFile boundaryFile(BoundaryMeshFile);
	int totalLine = 0;

	QVector<int> boundary;
	if (!boundaryFile.open(QIODevice::ReadOnly))
	{
		return;
	}

	QTextStream textstream(&boundaryFile);

	totalLine = 0;
	while (!textstream.atEnd())
	{
		int buf;

		totalLine++;
		textstream >> buf;;
		if (textstream.atEnd()){
			break;
		}

		textstream >> buf;
		boundary.append(buf);

		textstream >> buf;
		textstream >> buf;
		textstream >> buf;

		textstream >> buf;
		boundary.append(buf);
		textstream >> buf;
		boundary.append(buf);
		textstream >> buf;
		boundary.append(buf);
	}
	boundaryFile.close();

	if (totalLine > 0){
		totalLine--;
	}

	if (totalLine < 1){
		return;
	}

	// get max face num
	int maxFaceCode = 0;
	for (int i = 0; i < totalLine; i++){
		if (boundary[4 * i] > maxFaceCode){
			maxFaceCode = boundary[4 * i];
		}
	}


	// sort   
	QVector<int> faceNode;
	for (int i = 0; i < maxFaceCode; i++)
	{
		for (int j = 0; j < totalLine; j++){
			if (boundary[4 * j] == i + 1){// samen face
				faceNode.append(boundary[4 * j]);
				faceNode.append(boundary[4 * j + 1]);
				faceNode.append(boundary[4 * j + 2]);
				faceNode.append(boundary[4 * j + 3]);
			}
		}
	}


	//  read node file

	QFile nodesFile(NodesMeshFile);

	QVector<double> nodes;
	if (!nodesFile.open(QIODevice::ReadOnly))
	{
		return;
	}

	QTextStream nodesstream(&nodesFile);

	while (!nodesstream.atEnd())
	{
		double buf;

		nodesstream >> buf;
		if (nodesstream.atEnd()){
			break;
		}
		nodes.append(buf);
		nodesstream >> buf;
		nodesstream >> buf;
		nodes.append(buf);
		nodesstream >> buf;
		nodes.append(buf);
		nodesstream >> buf;
		nodes.append(buf);
	}
	nodesFile.close();

	std::map<int, QList<int>> MultiFace;
	int maxMultifaceNum = 0;
	QMap<int, int> facemaps;
	App::Document* doc = App::GetApplication().getActiveDocument();
	std::vector<App::DocumentObject*> sel = doc->getObjectsOfType(App::DocumentObject::getClassTypeId());
	int num = 0;
	for (std::vector<App::DocumentObject*>::iterator it = sel.begin(); it != sel.end(); ++it)
	{
		Gui::ViewProvider* pcProv = Gui::Application::Instance->getViewProvider(*it);
		bool visibility = true;
		if (pcProv ) {
			visibility = ((Gui::ViewProviderDocumentObject*)pcProv)->Visibility.getValue();
		}
		if (visibility == false){// 不可见的跳过
			continue;
		}
		
		std::string Group = (*it)->getNameInDocument();
		QString Groups = QString::fromStdString(Group);
		if (Groups.left(5) == QObject::tr("Group")){
			//Base::Console().Message("Group==Group\n");
			continue;
		}
			std::string line = (*it)->getNameInDocument();
		QString lines = QString::fromStdString(line);
		if (lines.left(4) == QObject::tr("Line")){
			//Base::Console().Message("line==line\n");
			continue;
		}
		std::string docNames = (*it)->getNameInDocument();

		App::DocumentObject *faceObj = (*it);
		const TopoDS_Shape& tShape = static_cast<Part::Feature *>(faceObj)->Shape.getValue();
		TopExp_Explorer expFace;

		for (expFace.Init(tShape, TopAbs_FACE); expFace.More(); expFace.Next())
		{
			num++;
			const TopoDS_Face &actFace = TopoDS::Face(expFace.Current());
			TopExp_Explorer expVertex;
			int flagface = 0;

			QList<int> gpface;
			for (expVertex.Init(expFace.Current(), TopAbs_VERTEX); expVertex.More(); expVertex.Next())
			{
				TopoDS_Vertex  vtx = TopoDS::Vertex(expVertex.Current());
				gp_Pnt pt = BRep_Tool::Pnt(vtx);
				double ref, minref;
				minref = 9.99e99;
				int imin = 0;
				// first loop over boundary element to find the closest node
				for (int i = 0; i < totalLine; i++){
				   for (int k = 1; k < 4; k++){
					int j = faceNode[4 * i + k] - 1;
					ref = pow(pt.X() - nodes[j * 4 + 1], 2.0) + pow(pt.Y() - nodes[j * 4 + 2], 2.0) + pow(pt.Z() - nodes[j * 4 + 3], 2.0);
					if (ref < minref ){
						minref = ref;
						imin = faceNode[4 * i + k]; // sequential number of node
					}
				   }
				}
				// second loop over boundary element to find the faces which contain the closest node.
				for (int i = 0; i < totalLine; i++){
				   for (int k = 1; k < 4; k++){
					int j = faceNode[4 * i + k];
					if (j == imin){
				           gpface << faceNode[4 * i]; // face which contains the closest node.
					}
				   }
				}
			}

		        FaceMapNum[num] = num;
//		        FaceMapNum[num] = 0;
//			int FaceNodeCount, FaceNodeMax = 0;
//			for (int f = 1; f <= maxFaceCode; f++){
//				FaceNodeCount = 0;
//				for (int n = 0; n < gpface.size(); n++){
//				   if( f == gpface[n] ){
//				      FaceNodeCount++;
//				   }
//				}
//
//				if (FaceNodeCount > FaceNodeMax ){
//				    FaceNodeMax = FaceNodeCount;
//				    FaceMapNum[num] = f;
//				}
//			}
		}
	}


	QFile file(QObject::tr("../ini/facemap.dat"));
	if (!file.open(QIODevice::WriteOnly | QFile::Text))
	{
		Base::Console().Message("Open file fail: facemap.dat \n ");
		return;
	}
	QTextStream outt(&file);

	for (int i = 1; i <= num; i++){
		outt << QString::number(i) << "  " << QString::number(FaceMapNum[i]) << endl;
	}
	file.close();
	CommonFunction::UpdateSetupIniSurface();
}

// 从文件取到所要的面的编号
static int GetFaceCodeWithGenCodeFromData(int genFaceCode)
{

	QFile facefile(QObject::tr("../ini/facemap.dat"));
	int totalLine = 0;

	QVector<int> faceVec;
	if (!facefile.open(QIODevice::ReadOnly))
	{
		return 0;
	}

	QTextStream textstream(&facefile);
    int no=0;
	while (!textstream.atEnd())
	{
		//int buf1,buf2;
		int buf;

		textstream >> buf;;
		if (textstream.atEnd()){
			break;
		}
		totalLine++;

		faceVec.append(buf);
		textstream >> buf;
		faceVec.append(buf);
		/* if(buf1==genFaceCode){
			no=buf2;
			break;
		} */
	}
	facefile.close();

	 for (int i = 0; i < totalLine; i++){
		if (faceVec[i * 2] == genFaceCode){
			return faceVec[i * 2 + 1];
		}
	} 
	/* if(no!=0){
		return no;
	} */
	return 0;
}


// 更新边界和激励的INI文件里的 Surface
static void UpdateSetupIniSurface(void)
{

	//  更新边界
	{
		QSettings settings(QObject::tr("../ini/Boundaries.ini"), QSettings::IniFormat);

		//读取count数进行判断
		int sum = settings.value(QObject::tr("Boundaries/Count")).toInt();
		bool Vname = true;
		for (int i = 1; i <= sum; i++)
		{
			QString pty = QString::fromLatin1("Boundaries/%1").arg(i);
			QString value = settings.value(pty).toString();

			int surfaceNum = settings.value(value + QString::fromLatin1("/BounCount")).toInt();
			int num = surfaceNum;
			if (surfaceNum == 0){
				continue;
			}
			int ij=1;
			for (int j = 1; j<=surfaceNum; j++){
				QString surface = value + QString::fromLatin1("/Boundary%1").arg(j);
				int geoFaceCode = settings.value(surface).toInt();
				int faceNum = GetFaceCodeWithGenCodeFromData(geoFaceCode);
				if (faceNum == 0){
					num = num - 1;
					settings.remove(surface);
					settings.beginGroup(value);
					settings.setValue(QObject::tr("BounCount"), num);
					settings.endGroup();
					
				}
				else{
					settings.beginGroup(value);
					settings.setValue(QObject::tr("MeshFaceNum%1").arg(ij++), faceNum);
					settings.endGroup();
				}
			}
		}
	}


	// 更新激励
	{
	QSettings settings(QObject::tr("../ini/Excitation.ini"), QSettings::IniFormat);

	//读取count数进行判断
	int sum = settings.value(QObject::tr("ORDER/count")).toInt();
	bool Vname = true;
	for (int i = 1; i <= sum; i++)
	{
		QString pty = QString::fromLatin1("ORDER/%1").arg(i);
		QString value = settings.value(pty).toString();

		//QString surface = value + QString::fromLatin1("/Surface");

		int surfaceNum = settings.value(value + QString::fromLatin1("/SurfacesNum")).toInt();
		int num=surfaceNum;
		if (surfaceNum == 0){
			continue;
		}
		
		
			int ij=1;
		for (int j = 1; j <= surfaceNum; j++){
			QString surface = value + QString::fromLatin1("/Surface%1").arg(j);

			QString surfacevalue = settings.value(surface).toString();
			QStringList qsl = surfacevalue.split(QString::fromLatin1(","));
			if (qsl.count() > 3){

				int geoFaceCode = 0;
				///获取编号
				App::Document* doc = App::GetApplication().getActiveDocument();
				std::vector<App::DocumentObject*> sel = doc->getObjectsOfType(App::DocumentObject::getClassTypeId());
				int m = 0;
				for (std::vector<App::DocumentObject*>::iterator it = sel.begin(); it != sel.end(); ++it)
				{
					int flag = 0;
					std::string Group = (*it)->getNameInDocument();
					QString Groups = QString::fromStdString(Group);
					if (Groups == qsl[2]){
						flag = 1;
					}
					if (Groups.left(5) == QObject::tr("Group")){
						//Base::Console().Message("Group==Group\n");
						continue;
					}
					std::string line = (*it)->getNameInDocument();
					QString lines = QString::fromStdString(line);
					if (lines.left(4) == QObject::tr("Line")){
						//Base::Console().Message("line==line\n");
						continue;
					}
					std::string docNames = (*it)->getNameInDocument();
					/* Gui::ViewProvider* pcProv = Gui::Application::Instance->getViewProvider(*it);
					bool visibility = true;
					if (pcProv ) {
						visibility = ((Gui::ViewProviderDocumentObject*)pcProv)->Visibility.getValue();
					}
					if (visibility == false){
						continue;
					} */

					App::DocumentObject *faceObj = (*it);
					const TopoDS_Shape& tShape = static_cast<Part::Feature *>(faceObj)->Shape.getValue();
					TopExp_Explorer expFace;
					int num = 0;
					for (expFace.Init(tShape, TopAbs_FACE); expFace.More(); expFace.Next())
					{
						num++;
						m++;
						QString	name = QObject::tr("Face%1").arg(num);
						if (qsl[3] == name&&flag){
							geoFaceCode = m;
						}

					}
				}
				////
				int faceNum = GetFaceCodeWithGenCodeFromData(geoFaceCode);

				//Base::Console().Message("UpdateSetupIniSurface faceNum %d", faceNum);
				if (faceNum == 0){
					settings.remove(surface);
					num = num - 1;
					settings.beginGroup(value);
					settings.setValue(QObject::tr("SurfacesNum"), num);
					settings.endGroup();
					
				}
				else{
					settings.beginGroup(value);
					settings.setValue(QObject::tr("MeshFaceNum%1").arg(ij++), faceNum);
					settings.endGroup();
				}
			}
		}
	}
}

}


//static std::string GetFaceSelected(bool postprocess = false)
static std::vector<std::string> GetFaceSelected(bool postprocess = false, bool multiselect = true)
{
	std::string searchStrnew("");
	std::vector<std::string> searchStr;

	QStringList fonts;
	std::string bodyName("");
	std::string faceName("");
	App::DocumentObject *faceObj;

	
	//int faceNum = 0;
	TopoDS_Shape ref;
	QList<gp_Pnt> gpList;

	QString searchStra;
	std::vector<Gui::SelectionObject> selection = Gui::Selection().getSelectionEx();
	if (selection.size() == 0){
		QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Selection error"), QObject::tr("Nothing selected!"));
		return searchStr;
	}
	////判断使用geo
	/* int geoflag = 0;
	App::Document* docs = App::GetApplication().getActiveDocument();
	std::vector<App::DocumentObject*> sels = docs->getObjectsOfType(App::DocumentObject::getClassTypeId());
	for (std::vector<App::DocumentObject*>::iterator it = sels.begin(); it != sels.end(); ++it)
	{
		std::string Group = (*it)->getNameInDocument();
		QString Groups = QString::fromStdString(Group);
		if (Groups.left(5) == QObject::tr("Plane")){
			Base::Console().Message("plane\n");
			geoflag = 1;
			break;
		}
	} */
	///geo end
	
	QFile file(QObject::tr("../ini/face_fre.dat"));
		if (!file.open(QIODevice::WriteOnly | QFile::Text))
		{
			Base::Console().Message("Open file fail: face_fre.dat \n ");
			return searchStr;
		}
		QTextStream outt(&file);
		
	for (std::vector<Gui::SelectionObject>::iterator it = selection.begin(); it != selection.end(); ++it){
		if (static_cast<std::string>(it->getTypeName()).substr(0, 4).compare(std::string("Part")) != 0){
			QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Selection error"), QObject::tr("Selected object is not a part!"));
			return searchStr;
		}
		
		std::vector<std::string> subNames = it->getSubNames();
		
		if (subNames.size() == 0){// 可能选择的是几何体，所有面都设定
			App::DocumentObject *obj = it->getObject();
			const TopoDS_Shape& tShape = static_cast<Part::Feature *> (obj)->Shape.getValue();
			TopExp_Explorer expFace;
			// 添加所有面
			int i = 1;
			for (expFace.Init(tShape, TopAbs_FACE); expFace.More(); expFace.Next())
			{
				///柱面在geo时要进行分成两个面
				/* const TopoDS_Face &actFace = TopoDS::Face(expFace.Current());
				BRepAdaptor_Surface surface(actFace);
				QString name;
				if(surface.GetType() == GeomAbs_Cylinder && geoflag){
					name = QObject::tr("Face%1Cylinder").arg(i);
				} *////
				//else{
				QString	name = QObject::tr("Face%1").arg(i);
				//}
				subNames.push_back(name.toStdString());
				i++;
			}
		}

		//如果不允许面多选
		if (multiselect == false && subNames.size() > 1){
			QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Selection error"), QObject::tr("Face can't be mulit selected!"));
			return searchStr;
		}
		
		/* QFile file(QObject::tr("../ini/face_fre.dat"));
		if (!file.open(QIODevice::WriteOnly | QFile::Text))
		{
			Base::Console().Message("Open file fail: face_fre.dat \n ");
			return searchStr;
		}
		QTextStream outt(&file); */

		for (unsigned int subIt = 0; subIt < (subNames.size()); ++subIt){
			//std::string searchStr("");
			if (subNames[subIt].find("Vertex") != std::string::npos)
				//searchStr = "Vertex"
				continue;
			else if (subNames[subIt].find("Edge") != std::string::npos)
				//searchStr = "Edge"
				continue;
			else{
				gpList.clear();
				searchStrnew = "";
				fonts.clear();

				//searchStr = subNames[subIt];
				//searchStr.push_back(subNames[subIt]);

				//Base::Console().Message("getFeatName: %s \n ", it->getFeatName());
				faceObj = it->getObject();

				Part::Feature* feat = static_cast<Part::Feature*>(faceObj);
				//ref = feat->Shape.getShape().getSubShape(searchStr.c_str());
				ref = feat->Shape.getShape().getSubShape(subNames[subIt].c_str());
				//Base::Console().Message("subNames: %s \n ", subNames[subIt].c_str());
				TopExp_Explorer expVertex;
				int flag = 0;
				for (expVertex.Init(ref, TopAbs_VERTEX); expVertex.More(); expVertex.Next())
				{

					TopoDS_Vertex  vtx = TopoDS::Vertex(expVertex.Current());
					gp_Pnt pt = BRep_Tool::Pnt(vtx);

					gpList.append(pt);

					//Base::Console().Message("gp point:(%f,%f,%f)", pt.X(), pt.Y(), pt.Z());////////

					QString sss = QString::fromLatin1("%1,%2,%3").arg(QString::number(pt.X())).arg(QString::number(pt.Y())).arg(QString::number(pt.Z()));

					fonts << sss;
				}//dian

				QStringList str;
				//去除重复的
				int i, j, r = 0;
				for (i = 0; i < fonts.size(); i++) {
					for (j = 0; j < r; j++)
					if (fonts[j] == fonts[i]) break;
					if (j == r)
						str << fonts[i];
					r++;
				}

				QString linn = QString::fromAscii(",");

				QString strlist = str.join(linn);

				bodyName = faceObj->getNameInDocument();
				faceName = subNames[subIt];
				string com = ",";
				searchStrnew = it->getTypeName() + com + std::string(faceObj->getNameInDocument()) + com + subNames[subIt] + com + strlist.toStdString() + com;
				//searchStr.push_back(it->getTypeName() + com + std::string(faceObj->getNameInDocument()) + com + subNames[subIt] + com + strlist.toStdString() + com);
			}

			
			if (searchStrnew==""){
				return searchStr;
			}

			///  code face 
			App::Document* doc = App::GetApplication().getDocument(faceObj->getDocument()->getName());
			std::vector<App::DocumentObject*> sel = doc->getObjectsOfType(App::DocumentObject::getClassTypeId());
			int num = 0;
			bool findface = false;

			for (std::vector<App::DocumentObject*>::iterator it = sel.begin(); it != sel.end(); ++it)
			{
				std::string docNames = (*it)->getNameInDocument();
				//Base::Console().Message("getNameInDocument %s\n", docNames);

				// 20170905 ADD Start
				/* Gui::ViewProvider* pcProv = Gui::Application::Instance->getViewProvider(*it);
				bool visibility = true;
				if (pcProv ) {
					visibility = ((Gui::ViewProviderDocumentObject*)pcProv)->Visibility.getValue();
				}
				if (visibility == false){
					continue;
				} */
				// 20170905 ADD End


				/*		App::DocumentObject *faceObj = (*it);
				Part::Feature* feat = static_cast<Part::Feature*>(faceObj);*/

				//std::vector<std::string> subNames = (*it)->   >getSubNames();
				//for (unsigned int subIt = 0; subIt < (subNames.size()); ++subIt){
				//	if (subNames[subIt].find("Face") != std::string::npos){
				//		num++;
				//		if (bodyName.compare(subNames) == 0 && faceName.compare(subNames[subIt]) == 0){
				//			break;
				//		}
				//	}
				//}
				//App::DocumentObject* obj = direction.getValue();
				//std::vector<std::string> names = direction.getSubValues();
				//if (names.size() == 0)
				//	return Base::Vector3d(0, 0, 0);
				//std::string subName = names.front();
				//Part::Feature* feat = static_cast<Part::Feature*>(obj);
				//const Part::TopoShape& shape = feat->Shape.getShape();
				//if (shape.isNull())
				//	return Base::Vector3d(0, 0, 0);
				//TopoDS_Shape sh;
				//try {
				//	sh = shape.getSubShape(subName.c_str());
				//}

				////if (subNames.substr(0, 4).compare(std::string("Part")) == 0)
				////{
				App::DocumentObject *faceObj = (*it);
				//	Part::Feature* feat = static_cast<Part::Feature*>(faceObj);
				const TopoDS_Shape& tShape = static_cast<Part::Feature *>(faceObj)->Shape.getValue();
				TopExp_Explorer expFace;

				for (expFace.Init(tShape, TopAbs_FACE); expFace.More(); expFace.Next())
				{
					num++;

					const TopoDS_Face &actFace = TopoDS::Face(expFace.Current());
					//if (bodyName.compare(docNames) == 0 && actFace.IsSame(ref) == true){
					//	break;
					//}

					TopExp_Explorer expVertex;
					int flagface = 0;

					QList<gp_Pnt> vv;
					for (expVertex.Init(expFace.Current(), TopAbs_VERTEX); expVertex.More(); expVertex.Next())
					{
						TopoDS_Vertex  vtx = TopoDS::Vertex(expVertex.Current());
						gp_Pnt pt = BRep_Tool::Pnt(vtx);
						vv << pt;
						//Base::Console().Message("gp point:(%f,%f,%f)", pt.X(), pt.Y(), pt.Z());////////
					}
					if (vv.size() == gpList.size()){
						for (int i = 0; i < gpList.size(); i++){
							if (!(abs(vv.at(i).X() - gpList.at(i).X()) < 1e-4 && abs(vv.at(i).Y() - gpList.at(i).Y()) < 1e-4 && abs(vv.at(i).Z() - gpList.at(i).Z()) < 1e-4)){
								flagface = 1;
								break;
							}
						}
						if (flagface == 0 && bodyName.compare(docNames) == 0 && actFace.IsSame(ref) == true){
							findface = true;
							//Base::Console().Message("Find face:(%d)\n", num);
							break;
						}
					}

					//	const TopoDS_Face &actFace = TopoDS::Face(expFace.Current());
					////	Part::Feature *face = static_cast<Part::Feature*>(actFace);
					//	std::string  subfacenm = ->getNameInDocument();
					//	Base::Console().Message("face name %s", subfacenm);
					//	if (bodyName.compare(bodyName) == 0 && faceName.compare(subfacenm)){
					//		 ;// && faceName.compare(actFace.)//
					//		break;
					//	}
					//}
				}
				if (findface == true){
					break;
				}
			}
			if (findface != true){
				num = 0;
			}
			
			if (postprocess){
				int faceNum = GetFaceCodeWithGenCodeFromData(num);//   GetFaceNumberWithMesh(ref);
				//Base::Console().Message(" Post Mesh Face Num: %d\n ", faceNum);
				int face1 = faceNum;
				int face2 = num;
				if (faceNum > 0){
					num = faceNum;
					outt << "FaceNO=" << QString::number(num) /*<< "\tFreq=" << QString::number(1.0)*/ << endl;
				}
				//outt << "FaceNO=" << QString::number(num) /*<< "\tFreq=" << QString::number(1.0)*/ << endl;
				//outt << "Freq=" << QString::number(1.0) << endl;
				// << QString::number(1.0e9) << endl;
			//	outt << QString::number(face1) << endl << QString::number(face2) << endl;


			}
			
			//searchStr=QString::fromLatin1("%1,").arg(QString::number(num)).toStdString() + searchStr;
			searchStr.push_back(QString::fromLatin1("%1,").arg(QString::number(num)).toStdString() + searchStrnew);
		}
		//QSettings *SolutionFreqIniRead = new QSettings(QObject::tr("../ini/SolutionSetup.ini"), QSettings::IniFormat);
		//ui->SelectFreqComB->addItem(SolutionFreqIniRead->value(tr("/Setup1/SolutionFreq")).toString());
		//outt << "Freq = " << SolutionFreqIniRead->value(QObject::tr("/Setup1/SolutionFreq")).toString() << endl;
		//outt << "Freq = " << QString::number(1.0) << endl;
		//delete SolutionFreqIniRead;
		//file.close();
	}
		QSettings *SolutionFreqIniRead = new QSettings(QObject::tr("../ini/SolutionSetup.ini"), QSettings::IniFormat);
		outt << "Freq = " << SolutionFreqIniRead->value(QObject::tr("/Setup1/SolutionFreq")).toString() << endl;
		delete SolutionFreqIniRead;
	file.close();
	return searchStr;
}

//端口选择设置判断选择面的类型
static bool facetype()
{
	std::vector<Gui::SelectionObject> selection = Gui::Selection().getSelectionEx();
	for (std::vector<Gui::SelectionObject>::iterator it = selection.begin(); it != selection.end(); ++it){
		////////
		int fac = 0;
		std::vector<std::string> subNames = it->getSubNames();
		for (unsigned int subIt = 0; subIt < (subNames.size()); ++subIt){
			if (subNames[subIt].find("Vertex") != std::string::npos)

				continue;
			else if (subNames[subIt].find("Edge") != std::string::npos)
				continue;
			else{
				QString fa = QString::fromStdString(subNames[subIt]);
				fac = fa.mid(4).toInt();
			}
		}
		/////
		App::DocumentObject *obj = it->getObject();
		const TopoDS_Shape& tShape = static_cast<Part::Feature *>(obj)->Shape.getValue();
		TopExp_Explorer expFace;
		int k = 0;
		for (expFace.Init(tShape, TopAbs_FACE); expFace.More(); expFace.Next())
		{
			k++;
			const TopoDS_Face &actFace = TopoDS::Face(expFace.Current());
			BRepAdaptor_Surface surface(actFace);
			if (k == fac){
				if (surface.GetType() == GeomAbs_Plane)
				{
					TopExp_Explorer expEdge;
					int line = 0;
					int Arch = 0;
					for (expEdge.Init(expFace.Current(), TopAbs_EDGE); expEdge.More(); expEdge.Next())
					{

						const TopoDS_Edge &actEdge = TopoDS::Edge(expEdge.Current());
						BRepAdaptor_Curve curve(actEdge);
						if (curve.GetType() == GeomAbs_Line) {
							line++;
						
						}
						else if (curve.GetType() == GeomAbs_Circle){
							Arch++;
						}
						else
						{
							QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
								QObject::tr("Please select a rectangle face or Circle surface as a port.!"));
							return false;
						}
					}
					
					if ((line == 4&&Arch == 0)||(line==0&&Arch==2)){
						continue;
					}
					else{
						QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
							QObject::tr("Please select a rectangle face or Circle surface as a port!"));
						return false;
					}
				}
				
				else
				{
					QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
						QObject::tr("Please select a rectangle face or Circle surface as a port..!"));
					return false;
				}
			}//if
			}//for2
		
	}//for1
	return true;
}

//删除文件夹及文件
static bool removeFolderContent(const QString &folderDir)
{
	QDir dir(folderDir);
	QFileInfoList fileList;
	QFileInfo curFile;
	if (!dir.exists())  { return false; }//文件不存，则返回false
	fileList = dir.entryInfoList(QDir::Dirs | QDir::Files
		| QDir::Readable | QDir::Writable
		| QDir::Hidden | QDir::NoDotAndDotDot
		, QDir::Name);
	while (fileList.size()>0)
	{
		int infoNum = fileList.size();
		for (int i = infoNum - 1; i >= 0; i--)
		{
			curFile = fileList[i];
			if (curFile.isFile())//如果是文件，删除文件
			{
				QFile fileTemp(curFile.filePath());
				fileTemp.remove();
				fileList.removeAt(i);
			}
			if (curFile.isDir())//如果是文件夹
			{
				QDir dirTemp(curFile.filePath());
				QFileInfoList fileList1 = dirTemp.entryInfoList(QDir::Dirs | QDir::Files
					| QDir::Readable | QDir::Writable
					| QDir::Hidden | QDir::NoDotAndDotDot
					, QDir::Name);
				if (fileList1.size() == 0)//下层没有文件或文件夹
				{
					dirTemp.rmdir(QObject::tr("."));
					fileList.removeAt(i);
				}
				else//下层有文件夹或文件
				{
					for (int j = 0; j<fileList1.size(); j++)
					{
						if (!(fileList.contains(fileList1[j])))
							fileList.append(fileList1[j]);
					}
				}
			}
		}
	}
	return true;
}

static int geoFormat(int flag){

	/////
	DeleteFile("..\\ini\\box.geo");
	
	QString SaveFile = QString::fromLatin1("../ini/box.geo");
	QFile SFile(SaveFile);
	if (!SFile.open(QIODevice::ReadWrite))
	{

		return 0;
	}
	QTextStream Savestreamfile(&SFile);
	///

	///
	///获取网格控制参数
	QSettings settingf(QObject::tr("../ini/SolutionSetup.ini"), QSettings::IniFormat);
	double Freq = settingf.value(QObject::tr("Setup1/SolutionFreq")).toDouble();//获取频率
	QString Hz = settingf.value(QObject::tr("Setup1/SolutionFreq_Unit")).toString();
	//Base::Console().Message("Freq=====================%f\n",Freq);
	if (!Freq){
		Freq = 2;
		Hz = QObject::tr("GHz");
	}
	double Freqs = Freq;
	if (Hz == QObject::tr("GHz")){//
		Freqs = Freq*pow(10, 9);
	}
	if (Hz == QObject::tr("KHz")){//
		Freqs = Freq*pow(10, 3);
	}
	if (Hz == QObject::tr("MHz")){//
		Freqs = Freq*pow(10, 6);
	}
	double wavelength = (3 * pow(10, 8))/Freqs;
	
	QSettings settingss(QObject::tr("../ini/MeshSetting.ini"), QSettings::IniFormat);
	//QString MeshSize1 = settingss.value(QObject::tr("Mesh/totalgrids")).toString();//meshsize1
	QString MeshSize2 = settingss.value(QObject::tr("Mesh/Gridwavelength")).toString();//与波长计算得到meshsize2放入成meshsize4media
	
	//QString MeshSum = settingss.value(QObject::tr("Mesh/totalgrids")).toString();//获取的网格总数
	if (MeshSize2.isEmpty())
	{
		//MeshSize1 = QObject::tr("10");
		MeshSize2 = QObject::tr("10");
		//MeshSum = QObject::tr("1000");
	}
	double ms = MeshSize2.toDouble();
//      double meshsize = (wavelength/ms)*1000;
	double meshsize = ms;
	////
	//Savestreamfile << QObject::tr("Meshsize1=%1;\n").arg(MeshSize1);
	QSettings settingimport(QObject::tr("../ini/importfilename.ini"), QSettings::IniFormat);
	QString namefile=settingimport.value(QObject::tr("Name/filename")).toString();
	QString importflag=settingimport.value(QObject::tr("Name/flag")).toString();
	Savestreamfile << QObject::tr("MeshSizeGlobalCoarsest=%1;\n").arg(meshsize);
	Savestreamfile << QObject::tr("Mesh.CharacteristicLengthMax=MeshSizeGlobalCoarsest;\n");
	if (importflag==QObject::tr("true")){
		Savestreamfile << QObject::tr("Merge \"%1\";\n").arg(namefile);
		Savestreamfile << QObject::tr("SetFactory(\"OpenCASCADE\");\n");
		return 1;
	}
	Savestreamfile << "//+\n";
	Savestreamfile << "SetFactory(\"OpenCASCADE\");\n";
	int shapenum = 0;//记录模型数
	int Point = 0;
	//int sumpoint = 0;//记录点的总数
	//int Edgesum = 0;//总的计数line
	//计数点
	typedef list<CommonFunction::pointnu> pointlist;
	pointlist plist;
	int ppflag = 0;//记录是否是第一次判断点
	pointlist plist2;
	plist2 = plist;
	CommonFunction::pointnu structpoint;
	//记录line数
	typedef list<CommonFunction::lines> lineslist;
	lineslist llist;
	CommonFunction::lines structlines;
	lineslist llist2;
	llist2 = llist;////
	//记录布尔运算
	typedef list<CommonFunction::geometry> geolist;
	geolist geobool;
	CommonFunction::geometry structgeo;
	typedef list<CommonFunction::boolean> boollist;
	boollist bbool;
	CommonFunction::boolean structbool;
	//记录cut的base和Tool
	typedef list<CommonFunction::cut> cutlist;
	cutlist cutbool;
	CommonFunction::cut structcut;

	//记录进行布尔运算的
	typedef list<CommonFunction::geodelete> deletelist;
	deletelist geodelete;
	CommonFunction::geodelete structdelete;
    
	//记录Extrude的编号
	QMap<QString,int> Extrudenum;
	QMap<QString, int> Extrudfaceflag;//11-8记录拉伸出来是平面
    QMap<int,QString> Exmultiflag;//11-12记录是多个面在布尔运算时加入这些面。
	//输出circle和Ellipse
	//vector<QString> VExtrude;
	int ExvNum=0;
	int Exfacenum = 0;
	int facenum = 0;
	QMap<QString,QString> Recordplanenum;//记录单独平面的编号
	QMap<QString,QString> booleanplane;//记录平面的多次布尔运算判断时使用。
    QString ExtrudeDWireface=QObject::tr("");//记录Dwise形成的面数。
	///先遍历平面
	App::Document* pdocs = App::GetApplication().getActiveDocument();
	std::vector<App::DocumentObject*> psels = pdocs->getObjectsOfType(App::DocumentObject::getClassTypeId());
	for (std::vector<App::DocumentObject*>::iterator it = psels.begin(); it != psels.end(); ++it)
	{
		std::string Group = (*it)->getNameInDocument();
		QString Groups = QString::fromStdString(Group);
        Base::Console().Message("********1Group==%s\n", Group);
		if(Groups.left(9)==QObject::tr("Rectangle")||Groups.left(7)==QObject::tr("Polygon")){
			QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong"),
				QObject::tr("Unable to process rectangle and polygon"));
				return 0;
		}
		if (Groups.left(5) != QObject::tr("Plane") && Groups.left(6) != QObject::tr("Circle") && Groups.left(7) != QObject::tr("Ellipse")&&Groups.left(5)!= QObject::tr("DWire")){
			continue;
		}
		//{// 判断是否不可见的，不可见的跳过
		//	Gui::ViewProvider* pcProv = Gui::Application::Instance->getViewProvider(*it);
		//	bool visibility = true;
		//	if (pcProv) {
		//		visibility = ((Gui::ViewProviderDocumentObject*)pcProv)->Visibility.getValue();
		//	}
		//	if (visibility == false){// 不可见的跳过
		//		continue;
		//	}
		//}
		int boolean = 0;
		int flagp=0;
		App::GeoFeature* geometry;
		Part::Extrusion *Extrude;
		geometry = static_cast<App::GeoFeature*>(*it);
		const Base::Placement pos = geometry->Placement.getValue();
		double x = pos.getPosition().x;
		double y = pos.getPosition().y;
		double z = pos.getPosition().z;
		double V1 = pos.getRotation()[0];
		double V2 = pos.getRotation()[1];
		double V3 = pos.getRotation()[2];
		double V4 = pos.getRotation()[3];
		double angle;
		Base::Vector3d di;
		pos.getRotation().getValue(di, angle);
		angle = Base::toDegrees<double>(angle);
		std::vector<App::DocumentObject*> cut = (*it)->getInList();
		for (std::vector<App::DocumentObject*>::iterator its = cut.begin(); its != cut.end(); ++its)
		{//获取是否进行布尔运算


			std::string bools = (*its)->getNameInDocument();
			QString bl = QString::fromStdString(bools);
			 // 判断是否不可见的，没有进行布尔运算的Extrude不可写入geobool
			Gui::ViewProvider* pcProv = Gui::Application::Instance->getViewProvider(*its);
			bool visibility = true;
			if (pcProv) {
				visibility = ((Gui::ViewProviderDocumentObject*)pcProv)->Visibility.getValue();
			}
			if (visibility == false){//
				flagp=1;
			}
			if (bl.left(4) == QObject::tr("Line")){
				continue;
			}
			Base::Console().Message("********cuts==%s\n", bools);
			//boolean = 2;
			QString b = QString::fromStdString(bools);
			if (b.left(3) == QObject::tr("Cut")){
				//Base::Console().Message("bools==cut\n");
				structgeo.boolean = bools;
				structgeo.geom = Group;
				structgeo.boole = "cut";
				boolean = 2;
				booleanplane.insert(bl, Groups);
				continue;
			}
			if (b.left(6) == QObject::tr("Common")){
				//Base::Console().Message("bools==Common\n");
				structgeo.boolean = bools;
				structgeo.geom = Group;
				structgeo.boole = "common";
				boolean = 2;
				booleanplane.insert(bl, Groups);
				continue;
			}
			if (b.left(6) == QObject::tr("Fusion")){
				//Base::Console().Message("bools==Fusion\n");
				structgeo.boolean = bools;
				structgeo.geom = Group;
				structgeo.boole = "fusion";
				boolean = 2;
				booleanplane.insert(bl, Groups);
				continue;
			}
			App::DocumentObject *pfaceObj = (*its);
			const TopoDS_Shape& ptShape = static_cast<Part::Feature *>(pfaceObj)->Shape.getValue();
			TopExp_Explorer pexpFace;
			//获取面的个数
			for (pexpFace.Init(ptShape, TopAbs_FACE); pexpFace.More(); pexpFace.Next())
			{
				facenum++;
			}
			//Extrudename = (*its)->getNameInDocument();
			Extrude = static_cast<Part::Extrusion *>(*its);
            App::GeoFeature* Exgeometry;
            Exgeometry = static_cast<App::GeoFeature*>(*its);
            const Base::Placement Expos = Exgeometry->Placement.getValue();
            double Ex = Expos.getPosition().x;
            double Ey = Expos.getPosition().y;
            double Ez = Expos.getPosition().z;
            double EV1 = Expos.getRotation()[0];
            double EV2 = Expos.getRotation()[1];
            double EV3 = Expos.getRotation()[2];
            double EV4 = Expos.getRotation()[3];
            double Eangle;
            Base::Vector3d Edi;
            Expos.getRotation().getValue(Edi, Eangle);
            Eangle = Base::toDegrees<double>(Eangle);
			if (b.left(7) == QObject::tr("Extrude")){
				if (Groups.left(5) == QObject::tr("Plane")){
					Part::Plane *c = static_cast<Part::Plane *>(*it);
					double l = c->Length.getValue();
					double w = c->Width.getValue();
					int r = 0;
					QString box = QObject::tr("Rectangle(%1) = {%2, %3, %4, %5, %6, %7};\n").arg(++facenum).arg(x).arg(y).arg(z).arg(l).arg(w).arg(r);
					Savestreamfile << box;
					Base::Vector3d v = Extrude->Dir.getValue();
					QString Extrude = QObject::tr("Extrude {%2, %3, %4} { Surface{%5}; }\n").arg(v.x).arg(v.y).arg(v.z).arg(facenum);
					//VExtrude.push_back(Extrude);
					++shapenum;
					Savestreamfile << Extrude;
					//QString Rotation = QObject::tr("Rotate { {%1,%2,%3}, {%4,%5,%6}, Pi*%7 } { Volume{%8}; }\n").arg(di.x).arg(di.y).arg(di.z).arg(x).arg(y).arg(z).arg(angle / 180).arg(shapenum);
					//Savestreamfile << Rotation;
					boolean = 3;
					if (flagp == 0){
						structgeo.boolean = bools;
						structgeo.geom = bools;
						structgeo.boole = "1";
						structgeo.num = shapenum;
						structgeo.m = shapenum;
						structgeo.plan = 0;
						geobool.push_back(structgeo);
					}
					else{
						Extrudenum.insert(bl, shapenum);
						Extrudfaceflag.insert(bl, 0);
						flagp = 0;
					}
					continue;
				}

				if (Groups.left(6) == QObject::tr("Circle")){
					Part::Circle *c = static_cast<Part::Circle *>(*it);
					double Rc=c->Radius.getValue();
					double a1 = c->Angle0.getValue();
					double a2 = c->Angle1.getValue();
					QString box = QObject::tr("Circle(%1) = {%2, %3, %4, %5, %6*Pi, %7*Pi};\n").arg(++facenum).arg(x).arg(y).arg(z).arg(Rc).arg(a1 / 180).arg(a2 / 180);
					Savestreamfile << box;
					Base::Vector3d v = Extrude->Dir.getValue();
					QString Extrude = QObject::tr("Extrude {%2, %3, %4} { Line{%5}; }\n").arg(v.x).arg(v.y).arg(v.z).arg(facenum);
					Savestreamfile << Extrude;
					++shapenum;
					//VExtrude.push_back(Extrude);
					//QString Rotation = QObject::tr("Rotate { {%1,%2,%3}, {%4,%5,%6}, Pi*%7 } { Volume{%8}; }\n").arg(di.x).arg(di.y).arg(di.z).arg(x).arg(y).arg(z).arg(angle / 180).arg(shapenum);
					//Savestreamfile << Rotation;
					boolean = 3;
					if (flagp == 0){
						structgeo.boolean = bools;
						structgeo.geom = bools;
						structgeo.boole = "1";
						structgeo.num = shapenum;
						structgeo.m = shapenum;
						structgeo.plan = 1;
						geobool.push_back(structgeo);
					}
					else{
						Extrudenum.insert(bl, shapenum);
						Extrudfaceflag.insert(bl, 1);
						flagp = 0;
					}
					continue;
				}
				if (Groups.left(7) == QObject::tr("Ellipse")){
					Part::Ellipse *c = static_cast<Part::Ellipse *>(*it);
					double a1=c->Angle0.getValue();
					double a2=c->Angle1.getValue();
					double R1=c->MajorRadius.getValue();
					double R2 = c->MinorRadius.getValue();
					QString box = QObject::tr("Ellipse(%1) = {%2, %3, %4, %5, %6, %7*Pi, %8*Pi};\n").arg(++facenum).arg(x).arg(y).arg(z).arg(R1).arg(R2).arg(a1 / 180).arg(a2 / 180);
					Savestreamfile << box;
					Base::Vector3d v = Extrude->Dir.getValue();
					QString Extrude = QObject::tr("Extrude {%2, %3, %4} { Line{%5}; }\n").arg(v.x).arg(v.y).arg(v.z).arg(facenum);
					Savestreamfile << Extrude;
					++shapenum;
					//VExtrude.push_back(Extrude);
					//QString Rotation = QObject::tr("Rotate { {%1,%2,%3}, {%4,%5,%6}, Pi*%7 } { Volume{%8}; }\n").arg(di.x).arg(di.y).arg(di.z).arg(x).arg(y).arg(z).arg(angle / 180).arg(shapenum);
					//Savestreamfile << Rotation;
					boolean = 3;
					if (flagp == 0){
						structgeo.boolean = bools;
						structgeo.geom = bools;
						structgeo.boole = "1";
						structgeo.num = shapenum;
						structgeo.m = shapenum;
						structgeo.plan = 1;
						geobool.push_back(structgeo);
					}
					else{
						Extrudenum.insert(bl, shapenum);
						Extrudfaceflag.insert(bl, 1);
						flagp = 0;
					}
					continue;
				}
                if (Groups.left(5) == QObject::tr("DWire")){//布尔运算里面2018-11-6
					App::DocumentObject *faceObj = (*it); //->Shape.getValue();
					const TopoDS_Shape& tShape = static_cast<Part::Feature *>(faceObj)->Shape.getValue();
					TopExp_Explorer countvertex;
					int pointssum = 0;
					int num = facenum;
					int pointnm = 0;
					for (countvertex.Init(tShape, TopAbs_VERTEX); countvertex.More(); countvertex.Next())
					{
						pointssum++; 
						TopoDS_Vertex  vtx = TopoDS::Vertex(countvertex.Current());
						gp_Pnt pt = BRep_Tool::Pnt(vtx);
						if (pointssum % 2 == 1 && pointssum != 1){
							Base::Console().Message("********points=%d====x=%fy=%fz=%f\n", pointssum, pt.X(), pt.Y(), pt.Z());
						}
						else{
							QString point = QObject::tr("Point(%1) = {%2, %3, %4, %5};\n").arg(++facenum).arg(pt.X()).arg(pt.Y()).arg(pt.Z()).arg(pointssum);
							Savestreamfile << point;
							pointnm++;
						}
					}
					int linenum = facenum;
					QString surfacenum = QObject::tr("");
                    QString multinum = QObject::tr("");
					for (int i = ++num; i <= facenum;){
						if (i != (facenum - 1))
                        { 
                          ExtrudeDWireface=ExtrudeDWireface+QObject::tr("%1,").arg(++shapenum);
                          multinum=multinum+QObject::tr("%1,").arg(shapenum);
                        }
						QString line = QObject::tr("Line(%1) = {%2, %3};\n").arg(++linenum).arg(i++).arg(i);
						Savestreamfile << line;
                       // QString Rotation = QObject::tr("Rotate { {%1,%2,%3}, {%4,%5,%6}, Pi*%7 } { Line{%8}; }\n").arg(di.x).arg(di.y).arg(di.z).arg(x).arg(y).arg(z).arg(angle / 180).arg(surfacenum.left(surfacenum.length() - 1));
					    // Savestreamfile << Rotation;
						surfacenum = surfacenum + QObject::tr("%1,").arg(linenum);
						if (i == facenum){ break; }
					}
					facenum = facenum + pointnm+1;//编号得多加，不然后面会显示有的点或者line编号已存在。
                    Base::Console().Message("********ExtrudeDWireface=%s\n", ExtrudeDWireface.toStdString().c_str());
					QString Rotation = QObject::tr("Translate { %1,%2,%3} { Line{%4}; }\n").arg(Ex).arg(Ey).arg(Ez).arg(surfacenum.left(surfacenum.length() - 1));
					Savestreamfile << Rotation;
					Base::Vector3d v = Extrude->Dir.getValue();
					QString Extrude = QObject::tr("Extrude {%1, %2, %3} { Line{%4}; }\n").arg(v.x).arg(v.y).arg(v.z).arg(surfacenum.left(surfacenum.length() - 1));
					Savestreamfile << Extrude;
                    ++shapenum;
					//QString Rotation = QObject::tr("Rotate { {%1,%2,%3}, {%4,%5,%6}, Pi*%7 } { Surface{%8,%9}; }\n").arg(Edi.x).arg(Edi.y).arg(Edi.z).arg(Ex).arg(Ey).arg(Ez).arg(Eangle / 180).arg(ExtrudeDWireface.left(ExtrudeDWireface.length() - 1)).arg(shapenum);
                   // QString Rotation = QObject::tr("Translate { %1,%2,%3} { Surface{%8,%9}; }\n").arg(Ex).arg(Ey).arg(Ez).arg(ExtrudeDWireface.left(ExtrudeDWireface.length() - 1)).arg(shapenum);
					//Savestreamfile << Rotation;
                    Exmultiflag.insert(shapenum,multinum);
                    boolean = 3;
					if (flagp == 0){
						structgeo.boolean = bools;
						structgeo.geom = bools;
						structgeo.boole = "1";
						structgeo.num = shapenum;
						structgeo.m = shapenum;
						structgeo.plan = 1;
						geobool.push_back(structgeo);
					}
					else{
						Extrudenum.insert(bl, shapenum);
						Extrudfaceflag.insert(bl,1);
						flagp = 0;
					}
					continue;
                }
                
			}
			

		}//for 布尔运算
		
		{// 判断是否不可见的，不可见的跳过
			Gui::ViewProvider* pcProv = Gui::Application::Instance->getViewProvider(*it);
			bool visibility = true;
			if (pcProv) {
				visibility = ((Gui::ViewProviderDocumentObject*)pcProv)->Visibility.getValue();
			}
			if (visibility == false&&boolean!=2){// 不可见没有进行布尔运算的面的跳过
				continue;
			}
		}
		if (boolean == 3){
			continue;
		}
		if (boolean == 0){
			structgeo.boolean = "1";
			structgeo.geom = Group;
			structgeo.boole = "1";
		}
		///////////先保存平面
		if (Groups.left(5) == QObject::tr("Plane")){
					Part::Plane *c = static_cast<Part::Plane *>(*it);
					double l = c->Length.getValue();
					double w = c->Width.getValue();
					int r = 0;
					QString box = QObject::tr("Rectangle(%1) = {%2, %3, %4, %5, %6, %7};\n").arg(++facenum).arg(x).arg(y).arg(z).arg(l).arg(w).arg(r);
					Savestreamfile << box;
					QString Rotation = QObject::tr("Rotate { {%1,%2,%3}, {%4,%5,%6}, Pi*%7 } { Surface{%8}; }\n").arg(di.x).arg(di.y).arg(di.z).arg(x).arg(y).arg(z).arg(angle / 180).arg(facenum);
					Savestreamfile << Rotation;
					Recordplanenum.insert(QString::number(facenum), Groups);
		}
		if (Groups.left(6) == QObject::tr("Circle")){
					Part::Circle *c = static_cast<Part::Circle *>(*it);
					double Rc=c->Radius.getValue();
					double a1 = c->Angle0.getValue();
					double a2 = c->Angle1.getValue();
					QString box = QObject::tr("Circle(%1) = {%2, %3, %4, %5, %6*Pi, %7*Pi};\n").arg(++facenum).arg(x).arg(y).arg(z).arg(Rc).arg(a1 / 180).arg(a2 / 180);
					Savestreamfile << box;
					QString Rotation = QObject::tr("Rotate { {%1,%2,%3}, {%4,%5,%6}, Pi*%7 } { Surface{%8}; }\n").arg(di.x).arg(di.y).arg(di.z).arg(x).arg(y).arg(z).arg(angle / 180).arg(facenum);
					Savestreamfile << Rotation;
					Recordplanenum.insert(QString::number(facenum), Groups);
		}
		if (Groups.left(7) == QObject::tr("Ellipse")){
					Part::Ellipse *c = static_cast<Part::Ellipse *>(*it);
					double a1=c->Angle0.getValue();
					double a2=c->Angle1.getValue();
					double R1=c->MajorRadius.getValue();
					double R2 = c->MinorRadius.getValue();
					QString box = QObject::tr("Ellipse(%1) = {%2, %3, %4, %5, %6, %7*Pi, %8*Pi};\n").arg(++facenum).arg(x).arg(y).arg(z).arg(R1).arg(R2).arg(a1 / 180).arg(a2 / 180);
					Savestreamfile << box;
					QString Rotation = QObject::tr("Rotate { {%1,%2,%3}, {%4,%5,%6}, Pi*%7 } { Surface{%8}; }\n").arg(di.x).arg(di.y).arg(di.z).arg(x).arg(y).arg(z).arg(angle / 180).arg(facenum);
					Savestreamfile << Rotation;
					Recordplanenum.insert(QString::number(facenum), Groups);
		}
        if (Groups.left(5) == QObject::tr("DWire")){//2018-11-6
             App::DocumentObject *faceObj = (*it); //->Shape.getValue();
					const TopoDS_Shape& tShape = static_cast<Part::Feature *>(faceObj)->Shape.getValue();
					TopExp_Explorer countvertex;
					int pointssum = 0;
					int num = facenum;
					for (countvertex.Init(tShape, TopAbs_VERTEX); countvertex.More(); countvertex.Next())
					{
						pointssum++; 
						TopoDS_Vertex  vtx = TopoDS::Vertex(countvertex.Current());
						gp_Pnt pt = BRep_Tool::Pnt(vtx);
						if (pointssum % 2 == 1 && pointssum != 1){
							//Base::Console().Message("********points=%d====x=%fy=%fz=%f\n", pointssum, pt.X(), pt.Y(), pt.Z());
						}
						else{
							QString point = QObject::tr("Point(%1) = {%2, %3, %4, %5};\n").arg(++facenum).arg(pt.X()).arg(pt.Y()).arg(pt.Z()).arg(pointssum);
							Savestreamfile << point;
						}
					}
					int linenum = facenum;
					QString surfacenum = QObject::tr("");
					for (int i = ++num; i <= facenum;){
						QString line = QObject::tr("Line(%1) = {%2, %3};\n").arg(++linenum).arg(i++).arg(i);
						Savestreamfile << line;
						surfacenum = surfacenum + QObject::tr("%1,").arg(linenum);
						if (i == facenum){ break; }
					}
                    QString Rotation = QObject::tr("Rotate { {%1,%2,%3}, {%4,%5,%6}, Pi*%7 } { Line{%8}; }\n").arg(di.x).arg(di.y).arg(di.z).arg(x).arg(y).arg(z).arg(angle / 180).arg(surfacenum.left(surfacenum.length() - 1));
					Savestreamfile << Rotation;
					Recordplanenum.insert(QString::number(facenum), Groups);
					//Base::Vector3d v = Extrude->Dir.getValue();
					//QString Extrude = QObject::tr("Extrude {%2, %3, %4} { Line{%5}; }\n").arg(v.x).arg(v.y).arg(v.z).arg(surfacenum.left(surfacenum.length() - 1));
					//Savestreamfile << Extrude;       
        }
		structgeo.num = facenum;
		structgeo.m = facenum;
		structgeo.plan = 1;
		geobool.push_back(structgeo);
			
	}//for

	/*vector<QString>::iterator itV;
	for (itV = VExtrude.begin(); itV != VExtrude.end(); ++itV){
		Savestreamfile << *itV;
	}*/
	//遍历几何体
	
	App::Document* docs = App::GetApplication().getActiveDocument();
	std::vector<App::DocumentObject*> sels = docs->getObjectsOfType(App::DocumentObject::getClassTypeId());
	for (std::vector<App::DocumentObject*>::iterator it = sels.begin(); it != sels.end(); ++it)
	{//遍历工程树
		std::string Group = (*it)->getNameInDocument();
		int boolean = 0;
		int bol= 0;
		int error=0;//判断是否能够写成geo
		QString Groups = QString::fromStdString(Group);
		//Base::Console().Message("Groups======%s\n", Group.c_str());
		if ((Groups.left(3) == QObject::tr("Cut")) || (Groups.left(6) == QObject::tr("Common")) || (Groups.left(6) == QObject::tr("Fusion"))){
			//Base::Console().Message("line1======%s\n", Group);
			boolean = 1;
			bol++;
			error++;
			//continue;
		}
		if (Groups.left(4) == QObject::tr("Line")){
			continue;
		}
		if (Groups.left(5) == QObject::tr("Plane")){
			continue;
		}
		

		//{// 判断是否不可见的，不可见的跳过
		//	Gui::ViewProvider* pcProv = Gui::Application::Instance->getViewProvider(*it);
		//	bool visibility = true;
		//	if (pcProv) {
		//		visibility = ((Gui::ViewProviderDocumentObject*)pcProv)->Visibility.getValue();
		//	}
		//	if (visibility == false){// 不可见的跳过
		//		continue;
		//	}
		//}
		///////////////////////////		
		/////////////////////////////////////////////

		//Base::Console().Message("line======%s\n", Group);
		int visibilityflag = 0;
		std::vector<App::DocumentObject*> cut = (*it)->getInList();
		for (std::vector<App::DocumentObject*>::iterator its = cut.begin(); its != cut.end(); ++its)
		{//获取是否进行布尔运算


			std::string bools = (*its)->getNameInDocument();
			QString bl = QString::fromStdString(bools);
			if (bl.left(4) == QObject::tr("Line")){
				continue;
			}
			//Base::Console().Message("********boolean==%s\n", bools.c_str());
			boolean = 2;
			bol++;
			QString b = QString::fromStdString(bools);
			if (b.left(3) == QObject::tr("Cut")){
				visibilityflag = 1;
				//Base::Console().Message("bools==cut\n");
				structgeo.boolean = bools;
				structgeo.geom = Group;
				structgeo.boole = "cut";
				continue;
			}
			if (b.left(6) == QObject::tr("Common")){
				visibilityflag = 1;
				//Base::Console().Message("bools==Common\n");
				structgeo.boolean = bools;
				structgeo.geom = Group;
				structgeo.boole = "common";
				continue;
			}
			if (b.left(6) == QObject::tr("Fusion")){
				visibilityflag = 1;
				//Base::Console().Message("bools==Fusion\n");
				structgeo.boolean = bools;
				structgeo.geom = Group;
				structgeo.boole = "fusion";
				continue;
			}


		}//for 布尔运算

		if(visibilityflag==0){// 判断是否不可见的，没有进行布尔运算的不可见的跳过
			Gui::ViewProvider* pcProv = Gui::Application::Instance->getViewProvider(*it);
			bool visibility = true;
			if (pcProv) {
				visibility = ((Gui::ViewProviderDocumentObject*)pcProv)->Visibility.getValue();
			}
			if (visibility == false){// 不可见的跳过
				continue;
			}
			if (Groups.left(7) == QObject::tr("Extrude")){
				continue;
			}
		}
		
		//
		//Base::Console().Message("********V1*******************\n");
		if (Groups.left(3) == QObject::tr("Cut")){
			App::DocumentObject *Obj = (*it);
			Part::Cut *c = static_cast<Part::Cut *>(Obj);
			App::DocumentObject *a = c->Base.getValue();
			std::string base = a->getNameInDocument();
			App::DocumentObject *aa = c->Tool.getValue();
			std::string tool = aa->getNameInDocument();
			//Base::Console().Message("===========4=======Base====%s========\n", base);
			//Base::Console().Message("==========3========Tool=====%s======\n", tool);
			structcut.cutname = Group;
			structcut.basename = base;
			structcut.toolname;
			cutbool.push_back(structcut);
		}
		if (boolean == 1){
			continue;
		}
		if(bol==2){
			continue;
		}
		if (boolean == 0){
			structgeo.boolean = "1";
			structgeo.geom = Group;
			structgeo.boole = "1";
		}

		//Base::Console().Message("================================\n");
		App::GeoFeature* geometry = static_cast<App::GeoFeature*>(*it);
		const Base::Placement pos = geometry->Placement.getValue();
		double x = pos.getPosition().x;
		double y = pos.getPosition().y;
		double z = pos.getPosition().z;
		double V1 = pos.getRotation()[0];
		double V2 = pos.getRotation()[1];
		double V3 = pos.getRotation()[2];
		double V4 = pos.getRotation()[3];
		double angle;
		Base::Vector3d di;
		pos.getRotation().getValue(di, angle);
		angle = Base::toDegrees<double>(angle);
		//Base::Console().Message("********V2*******************\n");
		int bflag = 0;//记录布尔下面还有布尔运算
		if (Groups.left(3) == QObject::tr("Box")){
			bflag = 1;
			Part::Box *c = static_cast<Part::Box *>(*it);
			double h = c->Height.getValue();
			double w = c->Width.getValue();
			double l = c->Length.getValue();
			QString box = QObject::tr("Box(%1) = {%2, %3, %4, %5, %6, %7};\n").arg(++shapenum).arg(x).arg(y).arg(z).arg(l).arg(w).arg(h);
			Savestreamfile << box;
			QString Rotation = QObject::tr("Rotate { {%1,%2,%3}, {%4,%5,%6}, Pi*%7 } { Volume{%8}; }\n").arg(di.x).arg(di.y).arg(di.z).arg(x).arg(y).arg(z).arg(angle/180).arg(shapenum);
			Savestreamfile << Rotation;
			structgeo.num = shapenum;
			structgeo.m = shapenum;
			structgeo.plan = 0;
			geobool.push_back(structgeo);
			error++;
		}
		if (Groups.left(8) == QObject::tr("Cylinder")){
			bflag = 1;
			Part::Cylinder *c = static_cast<Part::Cylinder *>(*it);
			double h = c->Height.getValue();
			double R = c->Radius.getValue();
			double a = c->Angle.getValue();
			QString sphere = QObject::tr("Cylinder(%1) = {%2, %3, %4, 0, 0, %5, %6, %7*Pi};\n").arg(++shapenum).arg(x).arg(y).arg(z).arg(h).arg(R).arg(a / 180);
			Savestreamfile << sphere;
			QString Rotation = QObject::tr("Rotate { {%1,%2,%3}, {%4,%5,%6}, Pi*%7 } { Volume{%8}; }\n").arg(di.x).arg(di.y).arg(di.z).arg(x).arg(y).arg(z).arg(angle / 180).arg(shapenum);
			Savestreamfile << Rotation;
			structgeo.num = shapenum;
			structgeo.m = shapenum;
			structgeo.plan = 0;
			geobool.push_back(structgeo);
			error++;
		}
		if (Groups.left(4) == QObject::tr("Cone")){
			bflag = 1;
			Part::Cone *c = static_cast<Part::Cone *>(*it);
			double a = c->Angle.getValue();
			double R1 = c->Radius1.getValue();
			double R2 = c->Radius2.getValue();
			double h = c->Height.getValue();
			QString Cone = QObject::tr("Cone(%1) = { %2, %3, %4, 0, 0, %5, %6, %7, %8*Pi};\n").arg(++shapenum).arg(x).arg(y).arg(z).arg(h).arg(R1).arg(R2).arg(a / 180);
			Savestreamfile << Cone;
			QString Rotation = QObject::tr("Rotate { {%1,%2,%3}, {%4,%5,%6}, Pi*%7 } { Volume{%8}; }\n").arg(di.x).arg(di.y).arg(di.z).arg(x).arg(y).arg(z).arg(angle / 180).arg(shapenum);
			Savestreamfile << Rotation;
			structgeo.num = shapenum;
			structgeo.m = shapenum;
			structgeo.plan = 0;
			geobool.push_back(structgeo);
			error++;
		}
		if (Groups.left(6) == QObject::tr("Sphere")){
			bflag = 1;
			Part::Sphere *c = static_cast<Part::Sphere *>(*it);
			double A1 = c->Angle1.getValue();
			double A2 = c->Angle2.getValue();
			double A3 = c->Angle3.getValue();
			double R = c->Radius.getValue();
			QString sphere = QObject::tr("Sphere(%1) = {%2, %3, %4, %5, Pi*%6, Pi*%7, %8*Pi};\n").arg(++shapenum).arg(x).arg(y).arg(z).arg(R).arg(A1 / 180).arg(A2 / 180).arg(A3 / 180);
			Savestreamfile << sphere;
			QString Rotation = QObject::tr("Rotate { {%1,%2,%3}, {%4,%5,%6}, Pi*%7 } { Volume{%8}; }\n").arg(di.x).arg(di.y).arg(di.z).arg(x).arg(y).arg(z).arg(angle / 180).arg(shapenum);
			Savestreamfile << Rotation;
			structgeo.num = shapenum;
			structgeo.m = shapenum;
			structgeo.plan = 0;
			geobool.push_back(structgeo);
			error++;
		}
		if (Groups.left(5) == QObject::tr("Torus")){
			bflag = 1;
			Part::Torus *c = static_cast<Part::Torus *>(*it);
			double R1 = c->Radius1.getValue();
			double R2 = c->Radius2.getValue();
			double A1 = c->Angle1.getValue();
			double A2 = c->Angle2.getValue();
			double A3 = c->Angle3.getValue();
			QString sphere = QObject::tr("Torus(%1) = {%2, %3, %4, %5, %6, %7*Pi};\n").arg(++shapenum).arg(x).arg(y).arg(z).arg(R1).arg(R2).arg(A3 / 180);
			Savestreamfile << sphere;
			QString Rotation = QObject::tr("Rotate { {%1,%2,%3}, {%4,%5,%6}, Pi*%7 } { Volume{%8}; }\n").arg(di.x).arg(di.y).arg(di.z).arg(x).arg(y).arg(z).arg(angle / 180).arg(shapenum);
			Savestreamfile << Rotation;
			structgeo.num = shapenum;
			structgeo.m = shapenum;
			structgeo.plan = 0;
			geobool.push_back(structgeo);
			error++;
		}
		if (Groups.left(7) == QObject::tr("Extrude")){//...........
			QMap<QString, int>::Iterator Ex;
			QMap<QString, int>::Iterator Exfaceflag;
			Ex=Extrudenum.find(Groups);
			Exfaceflag=Extrudfaceflag.find(Groups);
			structgeo.num = Ex.value();
			structgeo.m = Ex.value();
			structgeo.plan = Exfaceflag.value();
			geobool.push_back(structgeo);
			error++;
			continue;
		}
		if(error==0){
			QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong"),
				QObject::tr("Geometric content that does not conform to GEO processing"));
				return 0;
		}
		if (bflag == 0){
			structgeo.num = ++shapenum;
			structgeo.m = shapenum;
			structgeo.plan = 0;
			geobool.push_back(structgeo);
		}


	}//for 遍历工程树
	//Base::Console().Message("********V3*******************\n");
	
	///////////////2018-6-11////////
	//int bo= 0;
	//int boo=0;
	App::Document* d = App::GetApplication().getActiveDocument();
	std::vector<App::DocumentObject*> se = d->getObjectsOfType(App::DocumentObject::getClassTypeId());
	for (std::vector<App::DocumentObject*>::iterator it = se.begin(); it != se.end(); ++it)
	{//遍历工程树
		std::string Group = (*it)->getNameInDocument();
		int bo= 0;
		int boo=0;
		QString Groups = QString::fromStdString(Group);
		if ((Groups.left(3) == QObject::tr("Cut")) || (Groups.left(6) == QObject::tr("Common")) || (Groups.left(6) == QObject::tr("Fusion"))){
			//Base::Console().Message("line1======%s\n", Group);
			bo= 1;
			//continue;
		}
		if (Groups.left(4) == QObject::tr("Line")){
			continue;
		}
		if (Groups.left(5) == QObject::tr("Plane")){
			continue;
		}

		int visibilityflag = 0;
		std::vector<App::DocumentObject*> cut = (*it)->getInList();
		QString b;
		for (std::vector<App::DocumentObject*>::iterator its = cut.begin(); its != cut.end(); ++its)
		{//获取是否进行布尔运算


			std::string bools = (*its)->getNameInDocument();
			b = QString::fromStdString(bools);
			if (b.left(4) == QObject::tr("Line")){
				continue;
			}
			//Base::Console().Message("********cuts==%s\n", bools);
			boo = 1;
			//boolean2=2;
			//QString b = QString::fromStdString(bools);
			if (b.left(3) == QObject::tr("Cut")){
				visibilityflag = 1;
				//Base::Console().Message("bools==cut\n");
				structgeo.boolean = bools;
				structgeo.geom = Group;
				structgeo.boole = "cut";
				continue;
			}
			if (b.left(6) == QObject::tr("Common")){
				visibilityflag = 1;
				//Base::Console().Message("bools==Common\n");
				structgeo.boolean = bools;
				structgeo.geom = Group;
				structgeo.boole = "common";
				continue;
			}
			if (b.left(6) == QObject::tr("Fusion")){
				visibilityflag = 1;
				//Base::Console().Message("bools==Fusion\n");
				structgeo.boolean = bools;
				structgeo.geom = Group;
				structgeo.boole = "fusion";
				continue;
			}


		}//for 布尔运算

		if(visibilityflag==0){// 判断是否不可见的，没有进行布尔运算的不可见的跳过
			Gui::ViewProvider* pcProv = Gui::Application::Instance->getViewProvider(*it);
			bool visibility = true;
			if (pcProv) {
				visibility = ((Gui::ViewProviderDocumentObject*)pcProv)->Visibility.getValue();
			}
			if (visibility == false){// 不可见的跳过
				continue;
			}
		}
		//
		//Base::Console().Message("********V1*******************\n");
		if (Groups.left(3) == QObject::tr("Cut")){
			App::DocumentObject *Obj = (*it);
			Part::Cut *c = static_cast<Part::Cut *>(Obj);
			App::DocumentObject *a = c->Base.getValue();
			std::string base = a->getNameInDocument();
			App::DocumentObject *aa = c->Tool.getValue();
			std::string tool = aa->getNameInDocument();
			//Base::Console().Message("===========4=======Base====%s========\n", base);
			//Base::Console().Message("==========3========Tool=====%s======\n", tool);
			structcut.cutname = Group;
			structcut.basename = base;
			structcut.toolname;
			cutbool.push_back(structcut);
		}
		/*if (boolean == 1){
			continue;
		}*/
		
		if(boo&&bo){
			structgeo.num = ++shapenum;
			structgeo.m = shapenum;
			if (true == booleanplane.contains(Groups)){
			   structgeo.plan = 1;
			   booleanplane.insert(b,Groups);
			}else{
			structgeo.plan = 0;
			}
			geobool.push_back(structgeo);
		}
		
	}
	
	/////////////////////////////////////
	list<CommonFunction::geometry>::iterator it;
	list<CommonFunction::geometry>::iterator its;
	list<CommonFunction::geometry>::iterator ita;
	list<CommonFunction::geometry>::iterator it2;
	list<CommonFunction::geometry>::iterator it3;
	list<CommonFunction::geometry>::iterator it4;
	list<CommonFunction::cut>::iterator itc;//cut
	geolist geobools,geoboolss;
	geobools = geobool;
	geoboolss = geobool;
	//geolist geoover;
	//CommonFunction::geometry structgeol;//最终输出时剪切使用
	int i = 0;//判断只能写入一次
	int shapenum1, shapenum2, shapenum3;
	int boonum = 0;
	//int flnum = 1;//判断是否第一次使用编号
	//int fcnum = 1;//判断是否第一次使用编号
	//int fonum = 1;//
	int planeflag = 0;//判断是平面的
	int firstplan=0;//记录平面是否是第一次循环
	QMap<string, int> plannummap;
	//int numover = 0;//记录最终的编号
	/*  for (it = geobools.begin(); it != geobools.end();++it){
		Base::Console().Message("==1==%s=====%s====%s====%d========%d==%d===\n", (*it).boolean, (*it).geom, (*it).boole,(*it).num, (*it).m,(*it).plan);
	} 
	return 0; */
	QMap<QString,QString> geounmap;
	for (it = geobools.begin(); it != geobools.end();){
		//Base::Console().Message("==1==%s=====%s====%s====%d========%d===plan=%d=\n", (*it).boolean, (*it).geom, (*it).boole,(*it).num, (*it).m, (*it).plan);
		int fcnum = 1;//判断是否第一次使用编号
		int flnum = 1;
		int fonum = 1;
		int unionflag=0;//union判断接触时判断是否是多个进行的union
		//Base::Console().Message("********V4*******************\n");
		QString Groups = QString::fromStdString((*it).geom);
		int brk=0;//退出本次循环
		if ((*it).plan){
			planeflag = 1;
			
		}
		else{
			planeflag = 0;
		}
		if (firstplan==0){
				plannummap.insert((*it).boolean, (*it).m);
				firstplan = 1;
			}
		else{
			if (true==plannummap.contains((*it).boolean)){//平面的面可能与体相同的号，判断不出大小，使用这个防止多次进行输出。
				it++;
				continue;
				
				}
			else{
					plannummap.insert((*it).boolean, (*it).m);
				}
			}
		
		int s1=0;
		QString b = QString::fromStdString((*it).geom);
		if(b.left(6)==QObject::tr("Fusion")||b.left(3)==QObject::tr("Cut")||b.left(6)==QObject::tr("Common"))//平面加不加s
		{
			s1=1;
		}
		string s = (*it).boolean;
		shapenum1 = (*it).num;
		shapenum2 = (*it).num;
		shapenum3 = (*it).num;
		string gm = (*it).geom;
		int flagits=0;//记录++
		for (its = geoboolss.begin(); its != geoboolss.end();){
			//Base::Console().Message("********V899*******************\n");
			//Base::Console().Message("    ==2==%s=====%s========%d=====%d==plan=%d\n", (*its).boolean, (*its).geom, (*its).num, (*its).m,(*its).plan);
			if ((*its).boole == "1"){
				its++;
				continue;
			}
			int s2=0;
				QString bb = QString::fromStdString((*its).geom);
				if(bb.left(6)==QObject::tr("Fusion")||bb.left(3)==QObject::tr("Cut")||bb.left(6)==QObject::tr("Common"))//平面加不加s
				{
					s2=1;
				}
			//Base::Console().Message("********V111*****s1=%d,s2=%d**************\n",s1,s2);
			bool T=((s == (*its).boolean) &&((*its).geom != gm) && ((*its).m>(*it).m));//正常体的
			bool TP= ((s == (*its).boolean) &&((*its).geom != gm) && planeflag);//平面的运算的，平面编号可能会与体相同
	        if(T||TP){
				
				//Base::Console().Message("********8741*******************\n");
			//}
			//if((s == (*its).boolean) &&((*its).geom != gm) && ((*its).m>(*it).m)){
				//Base::Console().Message("********V88*******************\n");
				if ((*it).boole == "fusion"){
					int flagit = 0;
					for (it2 = geobools.begin(); it2 != geobools.end(); ++it2){
						if ((*it2).geom == (*its).boolean){
							if (flnum){
								if (planeflag==1){//平面读取更新编号
									//Base::Console().Message("********V9915*******%d****%d********\n",(*it2).num,shapenum1);
									boonum = (*it2).num;
									flagit = 1;
									it4 = it;
									it = it2;
									(*it).num = boonum;
									(*it).plan = 1;
									it = it4;
									flnum = 0;
									//Base::Console().Message("********V9915*******%d************\n",(*it2).num);
									////2018-6-12///cut没有使用
									for (ita = geoboolss.begin(); ita != geoboolss.end(); ita++){
										if ((*ita).geom == (*it2).geom){
											(*ita).num = boonum;
										}
									}
									
								}
								else{//获取体的编号
									boonum = (*it2).num;
									flagit = 1;
									flnum = 0;
								}
							}
							else{
								if (planeflag==1){
									//Base::Console().Message("********Vnj*******************\n");
									flagit = 1;
									it4 = it;
									it = it2;
									(*it).num = shapenum1;
									(*it).plan = 1;
									it = it4;
									boonum = ++shapenum;
									////2018-6-12///
									for (ita = geoboolss.begin(); ita != geoboolss.end(); ita++){
										if ((*ita).geom == (*it2).geom){
											(*ita).num = boonum;
										}
									}
								}else{
									//Base::Console().Message("********V23145*******************\n");
									boonum = ++shapenum;
									flagit = 1;
									it4 = it;
									it = it2;
									(*it).num = boonum;
									it = it4;
									////2018-6-12///
									for (ita = geoboolss.begin(); ita != geoboolss.end();ita++){
										if ((*ita).geom == (*it2).geom){
											(*ita).num = boonum;
										}
									}
								}
							}

						}
					}
					if (flagit == 0){//没有这个的布尔运算说明它是最后一个。
						++shapenum;
						if (planeflag){
							if ((*its).plan){//两个都是平面
								//Base::Console().Message("********V2*******************\n");
								QString common;
								QString snums = QObject::tr("s%1s[]").arg(shapenum);
								if(s1==1&&s2!=1){
									common = QObject::tr("s%1s=BooleanUnion{ Surface{s%2s}; Delete; }{ Surface{%3}; Delete; };\n").arg(shapenum).arg(shapenum1).arg((*its).num);
									QString shap1 = QObject::tr("s%1s[]").arg(shapenum1);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(QString::number((*its).num));
									Recordplanenum.remove(shap1);
								}
								if(s1==1&&s2==1){
									common = QObject::tr("s%1s=BooleanUnion{ Surface{s%2s}; Delete; }{ Surface{s%3s}; Delete; };\n").arg(shapenum).arg(shapenum1).arg((*its).num);
									QString shap1 = QObject::tr("s%1s[]").arg(shapenum1);
									QString shap2 = QObject::tr("s%1s[]").arg((*its).num);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(shap2);
									Recordplanenum.remove(shap1);
								}
								if(s1!=1&&s2==1){
									common = QObject::tr("s%1s=BooleanUnion{ Surface{%2}; Delete; }{ Surface{s%3s}; Delete; };\n").arg(shapenum).arg(shapenum1).arg((*its).num);
									QString shap2 = QObject::tr("s%1s[]").arg((*its).num);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(shap2);
									Recordplanenum.remove(QString::number(shapenum1));
								}
								if(s1!=1&&s2!=1){
                                    QMap<int, QString>::Iterator Multiflag1,Multiflag2;//11-12
			                        Multiflag1=Exmultiflag.find(shapenum1);
                                    Multiflag2=Exmultiflag.find((*its).num);
                                    if(Multiflag1!=Exmultiflag.end()&&Multiflag2!=Exmultiflag.end()){//先考虑了两个都是Dwire多个平面的
                                        common = QObject::tr("s%1s=BooleanUnion{ Surface{%2%3}; Delete; }{ Surface{%4%5}; Delete; };\n").arg(shapenum).arg(Multiflag1.value()).arg(shapenum1).arg(Multiflag2.value()).arg((*its).num);
                                    }
                                    else if(Multiflag1!=Exmultiflag.end()&&Multiflag2==Exmultiflag.end()){//
                                        common = QObject::tr("s%1s=BooleanUnion{ Surface{%2%3}; Delete; }{ Surface{%4}; Delete; };\n").arg(shapenum).arg(Multiflag1.value()).arg(shapenum1).arg((*its).num);
                                    }
                                    else if(Multiflag1==Exmultiflag.end()&&Multiflag2!=Exmultiflag.end()){//
                                        common = QObject::tr("s%1s=BooleanUnion{ Surface{%2}; Delete; }{ Surface{%3%4}; Delete; };\n").arg(shapenum).arg(shapenum1).arg(Multiflag2.value()).arg((*its).num);
                                    }
                                    else{
                                        common = QObject::tr("s%1s=BooleanUnion{ Surface{%2}; Delete; }{ Surface{%3}; Delete; };\n").arg(shapenum).arg(shapenum1).arg((*its).num);
                                    }
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(QString::number((*its).num));
									Recordplanenum.remove(QString::number(shapenum1));
								}
								structbool.str = common.toStdString();
								structbool.bnum = shapenum;
								structbool.Booleanname = "Plane," + (*it).boolean;
								bbool.push_back(structbool);
								//numover = shapenum3;
								structdelete.str = "Plane," +(*it).boolean;
								structdelete.num1 = shapenum1;
								structdelete.num2 = (*its).num;
								geodelete.push_back(structdelete);
								
							}
							else{//一个是平面
								QString common;
								QString snums = QObject::tr("s%1s[]").arg(shapenum);
								if(s1==1){
								common = QObject::tr("s%1s=BooleanUnion{ Surface{s%2s}; Delete; }{ Volume{%3}; Delete; }\n").arg(shapenum).arg(shapenum1).arg((*its).num);
								QString shap1 = QObject::tr("s%1s[]").arg(shapenum1);
								Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
								Recordplanenum.remove(shap1);
								}
								else{
								common = QObject::tr("s%1s=BooleanUnion{ Surface{%2}; Delete; }{ Volume{%3}; Delete; }\n").arg(shapenum).arg(shapenum1).arg((*its).num);
								Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
								Recordplanenum.remove(QString::number(shapenum1));
								}
								structbool.str = common.toStdString();
								structbool.bnum = shapenum;
								structbool.Booleanname = "Plane," + (*it).boolean;
								bbool.push_back(structbool);
								//numover = shapenum3;
								structdelete.str = "Plane," +(*it).boolean;
								structdelete.num1 = shapenum1;
								structdelete.num2 = (*its).num;
								geodelete.push_back(structdelete);
								
								//Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
								//Recordplanenum.remove(QString::number(shapenum1));
							}
						}
						else{//两个都是体的
							if ((*its).plan){//一个是平面，布尔运算的面与体结合时会出现这种情况。eml体与面操作会出问题,只能按面union体
							    QString common;
								QString snums = QObject::tr("s%1s[]").arg(shapenum);
								if(s2==1){
								common = QObject::tr("s%1s=BooleanUnion{ Surface{s%2s}; Delete; }{ Volume{%3}; Delete; }\n").arg(shapenum).arg((*its).num).arg(shapenum1);
								QString shap1 = QObject::tr("s%1s[]").arg((*its).num);
								Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
								Recordplanenum.remove(shap1);
								}
								else{
								common = QObject::tr("s%1s=BooleanUnion{ Surface{%2}; Delete; }{ Volume{%3}; Delete; }\n").arg(shapenum).arg((*its).num).arg(shapenum1);
								Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
								Recordplanenum.remove(QString::number((*its).num));
								}
								structbool.str = common.toStdString();
								structbool.bnum = shapenum;
								structbool.Booleanname = "Plane," + (*it).boolean;
								bbool.push_back(structbool);
								//numover = shapenum3;
								structdelete.str = "Plane," +(*it).boolean;
								structdelete.num1 = shapenum1;
								structdelete.num2 = (*its).num;
								geodelete.push_back(structdelete);
							}
							else{//....
								QString fusion = QObject::tr("BooleanUnion(%1)={ Volume{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(shapenum).arg(shapenum1).arg((*its).num);
								//Savestreamfile << fusion;
								QString m;
								if(shapenum1!=(*it).num){//说明它是多个union
									m=QObject::tr("munion");
								}else{
									m = QString::fromStdString((*it).geom);
								}
								//Base::Console().Message("********V23584*******************\n");
								bool un=CommonFunction::Booleanunionoperation((*it).geom,(*its).geom);
								if(unionflag==1){//说明它多个union第一个就是接触的。
									un=true;
									//Base::Console().Message("********V2954*******************\n");
								}
								if(unionflag==2){
									un=false;
								}
								if(un){
									structbool.str = fusion.toStdString();
									structbool.bnum = shapenum;
									structbool.Booleanname=(*it).boolean;
									bbool.push_back(structbool);
									structdelete.str = (*it).boolean;
									structdelete.num1 = shapenum;
									structdelete.num2 = (*its).num;
									geodelete.push_back(structdelete);
									unionflag=1;
								}
								else{
									//Base::Console().Message("********V18845*******************\n");
									QString unstr = QObject::tr("%1,%2,%3,%4,%5").arg(shapenum).arg(m).arg(shapenum1).arg(QString::fromStdString((*its).geom)).arg((*its).num);
									QString namenum = QObject::tr("%1,%2").arg(QString::fromStdString((*it).boolean)).arg(shapenum);
									geounmap.insert(namenum,unstr);
									unionflag=2;
								}
							}
						}
							shapenum1 = shapenum;
							if (it == its){
								its++;
								geobools.erase(it++);
								flagits=1;
							}
							else{
								it3 = it;
								it = its++;
								geobools.erase(it);//删除已经输出的
								it = it3;
							}
						//}
					}
					else{
						if (planeflag){
							if ((*its).plan){//第一级平面的布尔运算
							//Base::Console().Message("********V1**********%d*********\n",boonum);
								QString common;
								QString snums = QObject::tr("s%1s[]").arg(shapenum);
								if(s1==1&&s2!=1){
									common = QObject::tr("s%1s=BooleanUnion{ Surface{s%2s}; Delete; }{ Surface{%3}; Delete; };\n").arg(boonum).arg(shapenum1).arg((*its).num);
									QString shap1 = QObject::tr("s%1s[]").arg(shapenum1);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(QString::number((*its).num));
									Recordplanenum.remove(shap1);
								}
								if(s1==1&&s2==1){
									common = QObject::tr("s%1s=BooleanUnion{ Surface{s%2s}; Delete; }{ Surface{s%3s}; Delete; };\n").arg(boonum).arg(shapenum1).arg((*its).num);
									QString shap1 = QObject::tr("s%1s[]").arg(shapenum1);
									QString shap2 = QObject::tr("s%1s[]").arg((*its).num);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(shap2);
									Recordplanenum.remove(shap1);
								}
								if(s1!=1&&s2==1){
									common = QObject::tr("s%1s=BooleanUnion{ Surface{%2}; Delete; }{ Surface{s%3s}; Delete; };\n").arg(boonum).arg(shapenum1).arg((*its).num);
									QString shap2 = QObject::tr("s%1s[]").arg((*its).num);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(shap2);
									Recordplanenum.remove(QString::number(shapenum1));
								}
								if(s1!=1&&s2!=1){
                                    QMap<int, QString>::Iterator Multiflag1,Multiflag2;//11-12
			                        Multiflag1=Exmultiflag.find(shapenum1);
                                    Multiflag2=Exmultiflag.find((*its).num);
                                    if(Multiflag1!=Exmultiflag.end()&&Multiflag2!=Exmultiflag.end()){//先考虑了两个都是Dwire多个平面的
                                        common = QObject::tr("s%1s=BooleanUnion{ Surface{%2%3}; Delete; }{ Surface{%4%5}; Delete; };\n").arg(boonum).arg(Multiflag1.value()).arg(shapenum1).arg(Multiflag2.value()).arg((*its).num);
                                    }
                                    else if(Multiflag1!=Exmultiflag.end()&&Multiflag2==Exmultiflag.end()){//
                                        common = QObject::tr("s%1s=BooleanUnion{ Surface{%2%3}; Delete; }{ Surface{%4}; Delete; };\n").arg(boonum).arg(Multiflag1.value()).arg(shapenum1).arg((*its).num);
                                    }
                                    else if(Multiflag1==Exmultiflag.end()&&Multiflag2!=Exmultiflag.end()){//
                                        common = QObject::tr("s%1s=BooleanUnion{ Surface{%2}; Delete; }{ Surface{%3%4}; Delete; };\n").arg(boonum).arg(shapenum1).arg(Multiflag2.value()).arg((*its).num);
                                    }
                                    else{
                                        common = QObject::tr("s%1s=BooleanUnion{ Surface{%2}; Delete; }{ Surface{%3}; Delete; };\n").arg(boonum).arg(shapenum1).arg((*its).num);
                                    }
									//common = QObject::tr("s%1s=BooleanUnion{ Surface{%2}; Delete; }{ Surface{%3}; Delete; };\n").arg(boonum).arg(shapenum1).arg((*its).num);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(QString::number((*its).num));
									Recordplanenum.remove(QString::number(shapenum1));
								}
							//QString common = QObject::tr("s%1s=BooleanUnion{ Surface{%2}; Delete; }{ Surface{%3}; Delete; };\n").arg(boonum).arg(shapenum1).arg((*its).num);
							structbool.str = common.toStdString();
							structbool.bnum = boonum;
							structbool.Booleanname = "Plane," + (*it).boolean;
							bbool.push_back(structbool);
							//numover = shapenum3;
							structdelete.str ="Plane," + (*it).boolean;
							structdelete.num1 = boonum;
							structdelete.num2 = (*its).num;
							geodelete.push_back(structdelete);
							//QString snums = QObject::tr("s%1s").arg(boonum);
							//Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
							//Recordplanenum.remove(QString::number((*its).num));
							//Recordplanenum.remove(QString::number(shapenum1));
							}
							else{
								QString snums = QObject::tr("s%1s[]").arg(boonum);
								QString common;
								if(s1==1){
								common= QObject::tr("s%1s=BooleanUnion{ Surface{s%2s}; Delete; }{ Volume{%3}; Delete; };\n").arg(boonum).arg(shapenum1).arg((*its).num);
								QString shap1 = QObject::tr("s%1s[]").arg(shapenum1);
								Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
								Recordplanenum.remove(shap1);
								}
								else{
								common= QObject::tr("s%1s=BooleanUnion{ Surface{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(boonum).arg(shapenum1).arg((*its).num);
								Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
								Recordplanenum.remove(QString::number(shapenum1));
								}
								structbool.str = common.toStdString();
								structbool.bnum = boonum;
								structbool.Booleanname = "Plane," + (*it).boolean;
								bbool.push_back(structbool);
								//numover = shapenum3;
								structdelete.str = "Plane," +(*it).boolean;
								structdelete.num1 = boonum;
								structdelete.num2 = (*its).num;
								geodelete.push_back(structdelete);
								//QString snums = QObject::tr("s%1s").arg(boonum);
								//Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
								//Recordplanenum.remove(QString::number(shapenum1));
							}
						}
						else{//第一级体的布尔运算
							if ((*its).plan){
								QString snums = QObject::tr("s%1s[]").arg(boonum);
								QString common;
								if(s2==1){
								common= QObject::tr("s%1s=BooleanUnion{ Surface{s%2s}; Delete; }{ Volume{%3}; Delete; };\n").arg(boonum).arg((*its).num).arg(shapenum1);
								QString shap1 = QObject::tr("s%1s[]").arg((*its).num);
								Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
								Recordplanenum.remove(shap1);
								}
								else{
								common= QObject::tr("s%1s=BooleanUnion{ Surface{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(boonum).arg((*its).num).arg(shapenum1);
								Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
								Recordplanenum.remove(QString::number((*its).num));
								}
								structbool.str = common.toStdString();
								structbool.bnum = boonum;
								structbool.Booleanname = "Plane," + (*it).boolean;
								bbool.push_back(structbool);
								structdelete.str = "Plane," +(*it).boolean;
								structdelete.num1 = boonum;
								structdelete.num2 = (*its).num;
								geodelete.push_back(structdelete);
							}
							else{
								QString fusion = QObject::tr("BooleanUnion(%1)={ Volume{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(boonum).arg(shapenum1).arg((*its).num);
								//Savestreamfile << fusion;
								QString m;
								if(shapenum1!=(*it).num){//说明它是多个union
									m=QObject::tr("munion");
								}else{
									m = QString::fromStdString((*it).geom);
								}
								//Base::Console().Message("********V217*******************\n");
								bool un=CommonFunction::Booleanunionoperation((*it).geom,(*its).geom);//判断是否接触的，不是重新处理。
								if(unionflag==1){//说明它多个union第一个就是接触的。
									un=true;
									//Base::Console().Message("********Vn*******************\n");
								}
								if(unionflag==2){
									un=false;
								}
							    if(un){
									structbool.str = fusion.toStdString();
									structbool.bnum = boonum;
									structbool.Booleanname=(*it).boolean;
									bbool.push_back(structbool);
									//numover = shapenum1;
									structdelete.str = (*it).boolean;
									structdelete.num1 = boonum;
									structdelete.num2 = (*its).num;
									geodelete.push_back(structdelete);
									unionflag=1;
								}
								else{
									//Base::Console().Message("********V23mi*******************\n");
									QString unstr = QObject::tr("%1,%2,%3,%4,%5").arg(boonum).arg(m).arg(shapenum1).arg(QString::fromStdString((*its).geom)).arg((*its).num);
									QString namenum = QObject::tr("%1,%2").arg(QString::fromStdString((*it).boolean)).arg(boonum);
									geounmap.insert(namenum,unstr);
									unionflag=2;
								}
							}
						}
							shapenum1 = boonum;
							if (it == its){
								its++;
								geobools.erase(it++);
								flagits=1;
							}
							else{
								it3 = it;
								it = its++;
								geobools.erase(it);//删除已经输出的
								it = it3;
							}
						//}
					}
				}
				else if ((*it).boole == "cut"){
					//Base::Console().Message("********V*******************\n");
					int flagit = 0;
					for (it2 = geobools.begin(); it2 != geobools.end(); ++it2){
						if ((*it2).geom == (*its).boolean){
							//Base::Console().Message("********Vxx*******************\n");
							if (fcnum){
								if (planeflag==1){
								//	Base::Console().Message("********V9915*******************\n");
									boonum = (*it2).num;
									flagit = 1;
									it4 = it;
									it = it2;
									(*it).num = boonum;
									(*it).plan = 1;
									it = it4;
									fcnum = 0;
									
									////2018-6-12///cut没有使用
									for (ita = geoboolss.begin(); ita != geoboolss.end(); ita++){
										if ((*ita).geom == (*it2).geom){
											(*ita).num = boonum;
										}
									}
								}
								else{
									boonum = (*it2).num;
									flagit = 1;
									fcnum = 0;
								}
							}
							else{
								if (planeflag==1){
									flagit = 1;
									it4 = it;
									it = it2;
									(*it).num = shapenum2;
									(*it).plan = 1;
									it = it4;
									////2018-6-12///
									boonum = ++shapenum;
									for (ita = geoboolss.begin(); ita != geoboolss.end(); ita++){
										if ((*ita).geom == (*it2).geom){
											(*ita).num = boonum;
										}
									}
								}
								else{
									boonum = ++shapenum;
									flagit = 1;
									it4 = it;
									it = it2;
									(*it).num = boonum;
									it = it4;
									////2018-6-12///
									for (ita = geoboolss.begin(); ita != geoboolss.end(); ita++){
										if ((*ita).geom == (*it2).geom){
											(*ita).num = boonum;
										}
									}
								}
							}
						}
					}
					if (flagit == 0){//多级布尔运算的
						//Base::Console().Message("********Vg*******************\n");
						++shapenum;
						if (planeflag==1){
							if ((*its).plan){
							//Base::Console().Message("********V5*******************\n");
							for (itc = cutbool.begin(); itc != cutbool.end(); ++itc){//判断base和tool
									if ((*itc).cutname == (*its).boolean){
										if ((*itc).basename == (*its).geom){
										     QString common;
											QString snums = QObject::tr("s%1s[]").arg(shapenum);
											if(s1==1&&s2!=1){
												common = QObject::tr("s%1s=BooleanDifference{ Surface{%2}; Delete; }{ Surface{s%3s}; Delete; };\n").arg(shapenum).arg((*its).num).arg(shapenum2);
												QString shap1 = QObject::tr("s%1s[]").arg(shapenum2);
												Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
												Recordplanenum.remove(QString::number((*its).num));
												Recordplanenum.remove(shap1);
											}
											if(s1==1&&s2==1){
												common = QObject::tr("s%1s=BooleanDifference{ Surface{s%2s}; Delete; }{ Surface{s%3s}; Delete; };\n").arg(shapenum).arg((*its).num).arg(shapenum2);
												QString shap1 = QObject::tr("s%1s[]").arg(shapenum2);
												QString shap2 = QObject::tr("s%1s[]").arg((*its).num);
												Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
												Recordplanenum.remove(shap2);
												Recordplanenum.remove(shap1);
											}
											if(s1!=1&&s2==1){
												common = QObject::tr("s%1s=BooleanDifference{ Surface{s%2s}; Delete; }{ Surface{%3}; Delete; };\n").arg(shapenum).arg((*its).num).arg(shapenum2);
												QString shap2 = QObject::tr("s%1s[]").arg((*its).num);
												Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
												Recordplanenum.remove(shap2);
												Recordplanenum.remove(QString::number(shapenum2));
											}
											if(s1!=1&&s2!=1){
                                                QMap<int, QString>::Iterator Multiflag1,Multiflag2;//11-12
                                                Multiflag1=Exmultiflag.find((*its).num);
                                                Multiflag2=Exmultiflag.find(shapenum2);
                                                if(Multiflag1!=Exmultiflag.end()&&Multiflag2!=Exmultiflag.end()){//先考虑了两个都是Dwire多个平面的
                                                    common = QObject::tr("s%1s=BooleanDifference{ Surface{%2%3}; Delete; }{ Surface{%4%5}; Delete; };\n").arg(shapenum).arg(Multiflag1.value()).arg((*its).num).arg(Multiflag2.value()).arg(shapenum2);
                                                }
                                                else if(Multiflag1!=Exmultiflag.end()&&Multiflag2==Exmultiflag.end()){//
                                                    common = QObject::tr("s%1s=BooleanDifference{ Surface{%2%3}; Delete; }{ Surface{%4}; Delete; };\n").arg(shapenum).arg(Multiflag1.value()).arg((*its).num).arg(shapenum2);
                                                }
                                                else if(Multiflag1==Exmultiflag.end()&&Multiflag2!=Exmultiflag.end()){//
                                                    common = QObject::tr("s%1s=BooleanDifference{ Surface{%2}; Delete; }{ Surface{%3%4}; Delete; };\n").arg(shapenum).arg((*its).num).arg(Multiflag2.value()).arg(shapenum2);
                                                }
                                                else{
                                                    common = QObject::tr("s%1s=BooleanDifference{ Surface{%2}; Delete; }{ Surface{%3}; Delete; };\n").arg(shapenum).arg((*its).num).arg(shapenum2);
                                                }
												//common = QObject::tr("s%1s=BooleanDifference{ Surface{%2}; Delete; }{ Surface{%3}; Delete; };\n").arg(shapenum).arg((*its).num).arg(shapenum2);
												Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
												Recordplanenum.remove(QString::number((*its).num));
												Recordplanenum.remove(QString::number(shapenum2));
											}
											//QString cut = QObject::tr("s%1s=BooleanDifference{ Surface{%2}; Delete;  }{ Surface{%3}; Delete; };\n").arg(shapenum).arg((*its).num).arg(shapenum2);
											structbool.str = common.toStdString();
											structbool.bnum = shapenum;
											structbool.Booleanname = "Plane," + (*it).boolean;
											bbool.push_back(structbool);
											//numover = shapenum2;
											structdelete.str = "Plane," +(*it).boolean;
											structdelete.num1 = shapenum;
											structdelete.num2 = (*its).num;
											geodelete.push_back(structdelete);
											/* QString snums = QObject::tr("s%1s").arg(shapenum);
											Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
											Recordplanenum.remove(QString::number((*its).num));
											Recordplanenum.remove(QString::number(shapenum2)); */
											break;
										}
										else{
											 QString common;
											QString snums = QObject::tr("s%1s[]").arg(shapenum);
											if(s1==1&&s2!=1){
												common = QObject::tr("s%1s=BooleanDifference{ Surface{s%2s}; Delete; }{ Surface{%3}; Delete; };\n").arg(shapenum).arg(shapenum2).arg((*its).num);
												QString shap1 = QObject::tr("s%1s[]").arg(shapenum2);
												Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
												Recordplanenum.remove(QString::number((*its).num));
												Recordplanenum.remove(shap1);
											}
											if(s1==1&&s2==1){
												common = QObject::tr("s%1s=BooleanDifference{ Surface{s%2s}; Delete; }{ Surface{s%3s}; Delete; };\n").arg(shapenum).arg(shapenum2).arg((*its).num);
												QString shap1 = QObject::tr("s%1s[]").arg(shapenum2);
												QString shap2 = QObject::tr("s%1s[]").arg((*its).num);
												Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
												Recordplanenum.remove(shap2);
												Recordplanenum.remove(shap1);
											}
											if(s1!=1&&s2==1){
												common = QObject::tr("s%1s=BooleanDifference{ Surface{%2}; Delete; }{ Surface{s%3s}; Delete; };\n").arg(shapenum).arg(shapenum2).arg((*its).num);
												QString shap2 = QObject::tr("s%1s[]").arg((*its).num);
												Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
												Recordplanenum.remove(shap2);
												Recordplanenum.remove(QString::number(shapenum2));
											}
											if(s1!=1&&s2!=1){
                                                QMap<int, QString>::Iterator Multiflag1,Multiflag2;//11-12
                                                Multiflag1=Exmultiflag.find(shapenum2);
                                                Multiflag2=Exmultiflag.find((*its).num);
                                                if(Multiflag1!=Exmultiflag.end()&&Multiflag2!=Exmultiflag.end()){//先考虑了两个都是Dwire多个平面的
                                                    common = QObject::tr("s%1s=BooleanDifference{ Surface{%2%3}; Delete; }{ Surface{%4%5}; Delete; };\n").arg(shapenum).arg(Multiflag1.value()).arg(shapenum2).arg(Multiflag2.value()).arg((*its).num);
                                                }
                                                else if(Multiflag1!=Exmultiflag.end()&&Multiflag2==Exmultiflag.end()){//
                                                    common = QObject::tr("s%1s=BooleanDifference{ Surface{%2%3}; Delete; }{ Surface{%4}; Delete; };\n").arg(shapenum).arg(Multiflag1.value()).arg(shapenum2).arg((*its).num);
                                                }
                                                else if(Multiflag1==Exmultiflag.end()&&Multiflag2!=Exmultiflag.end()){//
                                                    common = QObject::tr("s%1s=BooleanDifference{ Surface{%2}; Delete; }{ Surface{%3%4}; Delete; };\n").arg(shapenum).arg(shapenum2).arg(Multiflag2.value()).arg((*its).num);
                                                }
                                                else{
                                                    common = QObject::tr("s%1s=BooleanDifference{ Surface{%2}; Delete; }{ Surface{%3}; Delete; };\n").arg(shapenum).arg(shapenum2).arg((*its).num);
                                                }
												//common = QObject::tr("s%1s=BooleanDifference{ Surface{%2}; Delete; }{ Surface{%3}; Delete; };\n").arg(shapenum).arg(shapenum2).arg((*its).num);
												Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
												Recordplanenum.remove(QString::number((*its).num));
												Recordplanenum.remove(QString::number(shapenum2));
											}
											//QString cut = QObject::tr("s%1s=BooleanDifference{ Surface{%2}; Delete;  }{ Surface{%3}; Delete; };\n").arg(shapenum).arg(shapenum2).arg((*its).num);
											structbool.str = common.toStdString();
											structbool.bnum = shapenum;
											structbool.Booleanname = "Plane," + (*it).boolean;
											bbool.push_back(structbool);
											//numover = shapenum2;
											structdelete.str = "Plane," +(*it).boolean;
											structdelete.num1 = shapenum;
											structdelete.num2 = (*its).num;
											geodelete.push_back(structdelete);
											/* QString snums = QObject::tr("s%1s").arg(shapenum);
											Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
											Recordplanenum.remove(QString::number((*its).num));
											Recordplanenum.remove(QString::number(shapenum2)); */
											break;
										}
									}
							}//for base tool
							}
							else{
								QString snums = QObject::tr("s%1s[]").arg(shapenum);
								QString cut;
								if(s1==1){
									cut = QObject::tr("s%1s=BooleanDifference{ Surface{s%2s}; Delete;  }{ Volume{%3}; Delete; };\n").arg(shapenum).arg(shapenum2).arg((*its).num);
									QString shap1 = QObject::tr("s%1s[]").arg(shapenum2);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(shap1);
								}else{
									cut = QObject::tr("s%1s=BooleanDifference{ Surface{%2}; Delete;  }{ Volume{%3}; Delete; };\n").arg(shapenum).arg(shapenum2).arg((*its).num);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(QString::number(shapenum2));
								}
								structbool.str = cut.toStdString();
								structbool.bnum = shapenum;
								structbool.Booleanname = "Plane," + (*it).boolean;
								bbool.push_back(structbool);
								//numover = shapenum2;
								structdelete.str = "Plane," +(*it).boolean;
								structdelete.num1 = shapenum;
								structdelete.num2 = (*its).num;
								geodelete.push_back(structdelete);
								//QString snums = QObject::tr("s%1s").arg(shapenum);
								//Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
								
								//Recordplanenum.remove(QString::number(shapenum2));
							}
						}
						else{//体的布尔运算
							//++shapenum;
							//Base::Console().Message("********V6*******************\n");
							if ((*its).plan){//只能面剪切体
								QString snums = QObject::tr("s%1s[]").arg(shapenum);
								QString cut;
								if(s2==1){
									cut = QObject::tr("s%1s=BooleanDifference{ Surface{s%2s}; Delete;  }{ Volume{%3}; Delete; };\n").arg(shapenum).arg((*its).num).arg(shapenum2);
									QString shap1 = QObject::tr("s%1s[]").arg((*its).num);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(shap1);
								}else{
									cut = QObject::tr("s%1s=BooleanDifference{ Surface{%2}; Delete;  }{ Volume{%3}; Delete; };\n").arg(shapenum).arg((*its).num).arg(shapenum2);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(QString::number((*its).num));
								}
								structbool.str = cut.toStdString();
								structbool.bnum = shapenum;
								structbool.Booleanname = "Plane," + (*it).boolean;
								bbool.push_back(structbool);
								//numover = shapenum2;
								structdelete.str = "Plane," +(*it).boolean;
								structdelete.num1 = shapenum;
								structdelete.num2 = (*its).num;
								geodelete.push_back(structdelete);
							}
							else{
							for (itc = cutbool.begin(); itc != cutbool.end(); ++itc){//判断base和tool
								if ((*itc).cutname == (*its).boolean){
									if ((*itc).basename == (*its).geom){
										//Base::Console().Message("********V7*******************\n");
										QString cut = QObject::tr("BooleanDifference(%1)={ Volume{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(shapenum).arg((*its).num).arg(shapenum2);
										structbool.str = cut.toStdString();
										structbool.bnum = shapenum;
										structbool.Booleanname=(*it).boolean;
										bbool.push_back(structbool);
										//numover = shapenum;
										structdelete.str = (*it).boolean;
										structdelete.num2 = shapenum;
										structdelete.num1 = (*its).num;
										geodelete.push_back(structdelete);
										break;
									}
									else{
										//Base::Console().Message("********V8*******************\n");
										QString cut = QObject::tr("BooleanDifference(%1)={ Volume{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(shapenum).arg(shapenum2).arg((*its).num);
										structbool.str = cut.toStdString();
										structbool.bnum = shapenum;
										structbool.Booleanname=(*it).boolean;
										bbool.push_back(structbool);
										//numover = shapenum;
										//Base::Console().Message("********V81*******************\n");
										structdelete.str = (*it).boolean;
										structdelete.num1 = shapenum;
										structdelete.num2 = (*its).num;
										//Base::Console().Message("********V82*******************\n");
										geodelete.push_back(structdelete);
										break;
									}
								}
							}
							}
						}
							if (it == its){
								its++;
								geobools.erase(it++);
								flagits=1;
						    }
						    else{
								it3 = it;
								it = its++;
								geobools.erase(it);//删除已经输出的
								it = it3;
							}
						//}
						//Base::Console().Message("********V86*******************\n");
					}
					else{
						if (planeflag==1){
							//Base::Console().Message("********V9*******************\n");
							if ((*its).plan){
								for (itc = cutbool.begin(); itc != cutbool.end(); ++itc){//判断base和tool
								//Base::Console().Message("********V956*******************\n");
									if ((*itc).cutname == (*its).boolean){
										if ((*itc).basename == (*its).geom){
											QString common;
											QString snums = QObject::tr("s%1s[]").arg(shapenum);
											if(s1==1&&s2!=1){
												common = QObject::tr("s%1s=BooleanDifference{ Surface{%2}; Delete; }{ Surface{s%3s}; Delete; };\n").arg(boonum).arg((*its).num).arg(shapenum2);
												QString shap1 = QObject::tr("s%1s[]").arg(shapenum2);
												Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
												Recordplanenum.remove(QString::number((*its).num));
												Recordplanenum.remove(shap1);
											}
											if(s1==1&&s2==1){
												common = QObject::tr("s%1s=BooleanDifference{ Surface{s%2s}; Delete; }{ Surface{s%3s}; Delete; };\n").arg(boonum).arg((*its).num).arg(shapenum2);
												QString shap1 = QObject::tr("s%1s[]").arg(shapenum2);
												QString shap2 = QObject::tr("s%1s[]").arg((*its).num);
												Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
												Recordplanenum.remove(shap2);
												Recordplanenum.remove(shap1);
											}
											if(s1!=1&&s2==1){
												common = QObject::tr("s%1s=BooleanDifference{ Surface{s%2s}; Delete; }{ Surface{%3}; Delete; };\n").arg(boonum).arg((*its).num).arg(shapenum2);
												QString shap2 = QObject::tr("s%1s[]").arg((*its).num);
												Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
												Recordplanenum.remove(shap2);
												Recordplanenum.remove(QString::number(shapenum2));
											}
											if(s1!=1&&s2!=1){
                                                QMap<int, QString>::Iterator Multiflag1,Multiflag2;//11-12
                                                Multiflag1=Exmultiflag.find((*its).num);
                                                Multiflag2=Exmultiflag.find(shapenum2);
                                                if(Multiflag1!=Exmultiflag.end()&&Multiflag2!=Exmultiflag.end()){//先考虑了两个都是Dwire多个平面的
                                                    common = QObject::tr("s%1s=BooleanDifference{ Surface{%2%3}; Delete; }{ Surface{%4%5}; Delete; };\n").arg(boonum).arg(Multiflag1.value()).arg((*its).num).arg(Multiflag2.value()).arg(shapenum2);
                                                }
                                                else if(Multiflag1!=Exmultiflag.end()&&Multiflag2==Exmultiflag.end()){//
                                                    common = QObject::tr("s%1s=BooleanDifference{ Surface{%2%3}; Delete; }{ Surface{%4}; Delete; };\n").arg(boonum).arg(Multiflag1.value()).arg((*its).num).arg(shapenum2);
                                                }
                                                else if(Multiflag1==Exmultiflag.end()&&Multiflag2!=Exmultiflag.end()){//
                                                    common = QObject::tr("s%1s=BooleanDifference{ Surface{%2}; Delete; }{ Surface{%3%4}; Delete; };\n").arg(boonum).arg((*its).num).arg(Multiflag2.value()).arg(shapenum2);
                                                }
                                                else{
                                                    common = QObject::tr("s%1s=BooleanDifference{ Surface{%2}; Delete; }{ Surface{%3}; Delete; };\n").arg(boonum).arg((*its).num).arg(shapenum2);
                                                }
												//common = QObject::tr("s%1s=BooleanDifference{ Surface{%2}; Delete; }{ Surface{%3}; Delete; };\n").arg(boonum).arg((*its).num).arg(shapenum2);
												Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
												Recordplanenum.remove(QString::number((*its).num));
												Recordplanenum.remove(QString::number(shapenum2));
											}
										//QString cut = QObject::tr("s%1s=BooleanDifference{ Surface{%2}; Delete; }{ Surface{%3}; Delete; };\n").arg(boonum).arg((*its).num).arg(shapenum2);
										structbool.str = common.toStdString();
										structbool.bnum = boonum;
										structbool.Booleanname = "Plane," + (*it).boolean;
										bbool.push_back(structbool);
										//numover = shapenum2;
										structdelete.str = "Plane," +(*it).boolean;
										structdelete.num1 = boonum;
										structdelete.num2 = (*its).num;
										geodelete.push_back(structdelete);
										//QString snums = QObject::tr("s%1s").arg(boonum);
										//Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
										//Recordplanenum.remove(QString::number((*its).num));
										//Recordplanenum.remove(QString::number(shapenum2));
										break;
										}
										else{
											QString common;
											QString snums = QObject::tr("s%1s[]").arg(shapenum);
											if(s1==1&&s2!=1){
												common = QObject::tr("s%1s=BooleanDifference{ Surface{s%2s}; Delete; }{ Surface{%3}; Delete; };\n").arg(boonum).arg(shapenum2).arg((*its).num);
												QString shap1 = QObject::tr("s%1s[]").arg(shapenum2);
												Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
												Recordplanenum.remove(QString::number((*its).num));
												Recordplanenum.remove(shap1);
											}
											if(s1==1&&s2==1){
												common = QObject::tr("s%1s=BooleanDifference{ Surface{s%2s}; Delete; }{ Surface{s%3s}; Delete; };\n").arg(boonum).arg(shapenum2).arg((*its).num);
												QString shap1 = QObject::tr("s%1s[]").arg(shapenum2);
												QString shap2 = QObject::tr("s%1s[]").arg((*its).num);
												Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
												Recordplanenum.remove(shap2);
												Recordplanenum.remove(shap1);
											}
											if(s1!=1&&s2==1){
												common = QObject::tr("s%1s=BooleanDifference{ Surface{%2}; Delete; }{ Surface{s%3s}; Delete; };\n").arg(boonum).arg(shapenum2).arg((*its).num);
												QString shap2 = QObject::tr("s%1s[]").arg((*its).num);
												Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
												Recordplanenum.remove(shap2);
												Recordplanenum.remove(QString::number(shapenum2));
											}
											if(s1!=1&&s2!=1){
                                                QMap<int, QString>::Iterator Multiflag1,Multiflag2;//11-12
                                                Multiflag1=Exmultiflag.find(shapenum2);
                                                Multiflag2=Exmultiflag.find((*its).num);
                                                if(Multiflag1!=Exmultiflag.end()&&Multiflag2!=Exmultiflag.end()){//先考虑了两个都是Dwire多个平面的
                                                    common = QObject::tr("s%1s=BooleanDifference{ Surface{%2%3}; Delete; }{ Surface{%4%5}; Delete; };\n").arg(boonum).arg(Multiflag1.value()).arg(shapenum2).arg(Multiflag2.value()).arg((*its).num);
                                                }
                                                else if(Multiflag1!=Exmultiflag.end()&&Multiflag2==Exmultiflag.end()){//
                                                    common = QObject::tr("s%1s=BooleanDifference{ Surface{%2%3}; Delete; }{ Surface{%4}; Delete; };\n").arg(boonum).arg(Multiflag1.value()).arg(shapenum2).arg((*its).num);
                                                }
                                                else if(Multiflag1==Exmultiflag.end()&&Multiflag2!=Exmultiflag.end()){//
                                                    common = QObject::tr("s%1s=BooleanDifference{ Surface{%2}; Delete; }{ Surface{%3%4}; Delete; };\n").arg(boonum).arg(shapenum2).arg(Multiflag2.value()).arg((*its).num);
                                                }
                                                else{
                                                    common = QObject::tr("s%1s=BooleanDifference{ Surface{%2}; Delete; }{ Surface{%3}; Delete; };\n").arg(boonum).arg(shapenum2).arg((*its).num);
                                                }
												//common = QObject::tr("s%1s=BooleanDifference{ Surface{%2}; Delete; }{ Surface{%3}; Delete; };\n").arg(boonum).arg(shapenum2).arg((*its).num);
												Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
												Recordplanenum.remove(QString::number((*its).num));
												Recordplanenum.remove(QString::number(shapenum2));
											}
										//QString cut = QObject::tr("s%1s=BooleanDifference{ Surface{%2}; Delete; }{ Surface{%3}; Delete; };\n").arg(boonum).arg(shapenum2).arg((*its).num);
										structbool.str = common.toStdString();
										structbool.bnum = boonum;
										structbool.Booleanname = "Plane," + (*it).boolean;
										bbool.push_back(structbool);
										//numover = shapenum2;
										structdelete.str = "Plane," +(*it).boolean;
										structdelete.num1 = boonum;
										structdelete.num2 = (*its).num;
										geodelete.push_back(structdelete);
										//QString snums = QObject::tr("s%1s").arg(boonum);
										//Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
										//Recordplanenum.remove(QString::number((*its).num));
										//Recordplanenum.remove(QString::number(shapenum2));
										break;
										}
									}
								}
							}
							else{
								QString cut; 
								QString snums = QObject::tr("s%1s[]").arg(boonum);
								if(s1==1){
									cut= QObject::tr("s%1s=BooleanDifference{ Surface{s%2s}; Delete; }{ Volume{%3}; Delete; };\n").arg(boonum).arg(shapenum2).arg((*its).num);
									QString shap1 = QObject::tr("s%1s[]").arg(shapenum2);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
								    Recordplanenum.remove(shap1);
								}
								else{
									cut= QObject::tr("s%1s=BooleanDifference{ Surface{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(boonum).arg(shapenum2).arg((*its).num);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
								    Recordplanenum.remove(QString::number(shapenum2));
								}
								structbool.str = cut.toStdString();
								structbool.bnum = boonum;
								structbool.Booleanname = "Plane," + (*it).boolean;
								bbool.push_back(structbool);
								//numover = shapenum2;
								structdelete.str = "Plane," +(*it).boolean;
								structdelete.num1 = boonum;
								structdelete.num2 = (*its).num;
								geodelete.push_back(structdelete);
								
								//Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
								
								//Recordplanenum.remove(QString::number(shapenum2));
							}
						}
						else{
							if ((*its).plan){
								QString cut; 
								QString snums = QObject::tr("s%1s[]").arg(boonum);
								if(s2==1){
									cut= QObject::tr("s%1s=BooleanDifference{ Surface{s%2s}; Delete; }{ Volume{%3}; Delete; };\n").arg(boonum).arg((*its).num).arg(shapenum2);
									QString shap1 = QObject::tr("s%1s[]").arg((*its).num);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
								    Recordplanenum.remove(shap1);
								}
								else{
									cut= QObject::tr("s%1s=BooleanDifference{ Surface{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(boonum).arg((*its).num).arg(shapenum2);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
								    Recordplanenum.remove(QString::number((*its).num));
								}
								structbool.str = cut.toStdString();
								structbool.bnum = boonum;
								structbool.Booleanname = "Plane," + (*it).boolean;
								bbool.push_back(structbool);
								structdelete.str = "Plane," +(*it).boolean;
								structdelete.num1 = boonum;
								structdelete.num2 = (*its).num;
								geodelete.push_back(structdelete);
							}
							else{
								for (itc = cutbool.begin(); itc != cutbool.end(); ++itc){//判断base和tool
								if ((*itc).cutname == (*its).boolean){
									if ((*itc).basename == (*its).geom){
										//Base::Console().Message("********V10*******************\n");
										QString cut = QObject::tr("BooleanDifference(%1)={ Volume{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(boonum).arg((*its).num).arg(shapenum2);
										structbool.str = cut.toStdString();
										structbool.bnum = boonum;
										structbool.Booleanname=(*it).boolean;
										bbool.push_back(structbool);
										//numover = boonum;
										structdelete.str = (*it).boolean;
										structdelete.num2 = boonum;
										structdelete.num1 = (*its).num;
										geodelete.push_back(structdelete);
										break;
									}
									else{
										//Base::Console().Message("********V11*******************\n");
										QString cut = QObject::tr("BooleanDifference(%1)={ Volume{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(boonum).arg(shapenum2).arg((*its).num);
										structbool.str = cut.toStdString();
										structbool.bnum = boonum;
										structbool.Booleanname=(*it).boolean;
										bbool.push_back(structbool);
										//numover = boonum;
										structdelete.str = (*it).boolean;
										structdelete.num1 = boonum;
										structdelete.num2 = (*its).num;
										geodelete.push_back(structdelete);
										break;
									}
								}
								}
							}
						}
							if (it == its){
								its++;
								geobools.erase(it++);
								flagits=1;
						    }
						    else{
								it3 = it;
								it = its++;
								geobools.erase(it);//删除已经输出的
								it = it3;
						    }
						//}

					}
					//Base::Console().Message("********V87*******************\n");
				}
				else if ((*it).boole == "common"){
					//////
					//Base::Console().Message("********8742*******************\n");
					int flagit = 0;
					for (it2 = geobools.begin(); it2 != geobools.end(); ++it2){
						if ((*it2).geom == (*its).boolean){
							//Base::Console().Message("********8743*******************\n");
							if (fonum){
								if (planeflag){
									boonum = (*it2).num;
									flagit = 1;
									it4 = it;
									it = it2;
									(*it).num = boonum;
									(*it).plan = 1;
									it = it4;
									fonum = 0;
									////2018-6-12///
									for (ita = geoboolss.begin(); ita != geoboolss.end(); ita++){
										if ((*ita).geom == (*it2).geom){
											(*ita).num = boonum;
										}
									}
									
								}
								else{
									boonum = (*it2).num;
									flagit = 1;
									fonum = 0;
								}

							}
							else{
								if (planeflag){
									flagit = 1;
									it4 = it;
									it = it2;
									(*it).num = shapenum3;
									(*it).plan = 1;
									it = it4;
									////2018-6-12///
									boonum = ++shapenum;
									for (ita = geoboolss.begin(); ita != geoboolss.end(); ita++){
										if ((*ita).geom == (*it2).geom){
											(*ita).num = boonum;
										}
									}
								}
								else{
									boonum = ++shapenum;
									flagit = 1;
									it4 = it;
									it = it2;
									(*it).num = boonum;
									it = it4;
									////2018-6-12///
									for (ita = geoboolss.begin(); ita != geoboolss.end(); ita++){
										if ((*ita).geom == (*it2).geom){
											(*ita).num = boonum;
										}
									}
								}
							}
						}
					}
					if (flagit == 0){
						//Base::Console().Message("********8744*******************\n");
						++shapenum;
						if (planeflag){
							if ((*its).plan){
								QString common;
								QString snums = QObject::tr("s%1s[]").arg(shapenum);
								if(s1==1&&s2!=1){
									common = QObject::tr("s%1s=BooleanIntersection{ Surface{s%2s}; Delete; }{ Surface{%3}; Delete; };\n").arg(shapenum).arg(shapenum3).arg((*its).num);
									QString shap1 = QObject::tr("s%1s[]").arg(shapenum3);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(QString::number((*its).num));
									Recordplanenum.remove(shap1);
								}
								if(s1==1&&s2==1){
									common = QObject::tr("s%1s=BooleanIntersection{ Surface{s%2s}; Delete; }{ Surface{s%3s}; Delete; };\n").arg(shapenum).arg(shapenum3).arg((*its).num);
									QString shap1 = QObject::tr("s%1s[]").arg(shapenum3);
									QString shap2 = QObject::tr("s%1s[]").arg((*its).num);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(shap2);
									Recordplanenum.remove(shap1);
								}
								if(s1!=1&&s2==1){
									common = QObject::tr("s%1s=BooleanIntersection{ Surface{%2}; Delete; }{ Surface{s%3s}; Delete; };\n").arg(shapenum).arg(shapenum3).arg((*its).num);
									QString shap2 = QObject::tr("s%1s[]").arg((*its).num);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(shap2);
									Recordplanenum.remove(QString::number(shapenum3));
								}
								if(s1!=1&&s2!=1){
                                    QMap<int, QString>::Iterator Multiflag1,Multiflag2;//11-12
                                    Multiflag1=Exmultiflag.find(shapenum3);
                                    Multiflag2=Exmultiflag.find((*its).num);
                                    if(Multiflag1!=Exmultiflag.end()&&Multiflag2!=Exmultiflag.end()){//先考虑了两个都是Dwire多个平面的
                                       common = QObject::tr("s%1s=BooleanIntersection{ Surface{%2%3}; Delete; }{ Surface{%4%5}; Delete; };\n").arg(shapenum).arg(Multiflag1.value()).arg(shapenum3).arg(Multiflag2.value()).arg((*its).num);
                                    }
                                    else if(Multiflag1!=Exmultiflag.end()&&Multiflag2==Exmultiflag.end()){//
                                        common = QObject::tr("s%1s=BooleanIntersection{ Surface{%2%3}; Delete; }{ Surface{%4}; Delete; };\n").arg(shapenum).arg(Multiflag1.value()).arg(shapenum3).arg((*its).num);
                                    }
                                    else if(Multiflag1==Exmultiflag.end()&&Multiflag2!=Exmultiflag.end()){//
                                        common = QObject::tr("s%1s=BooleanIntersection{ Surface{%2}; Delete; }{ Surface{%3%4}; Delete; };\n").arg(shapenum).arg(shapenum3).arg(Multiflag2.value()).arg((*its).num);
                                    }
                                    else{
                                        common = QObject::tr("s%1s=BooleanIntersection{ Surface{%2}; Delete; }{ Surface{%3}; Delete; };\n").arg(shapenum).arg(shapenum3).arg((*its).num);
                                    }
									//common = QObject::tr("s%1s=BooleanIntersection{ Surface{%2}; Delete; }{ Surface{%3}; Delete; };\n").arg(shapenum).arg(shapenum3).arg((*its).num);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(QString::number((*its).num));
									Recordplanenum.remove(QString::number(shapenum3));
								}
								// QString common = QObject::tr("s%1s=BooleanIntersection{ Surface{%2}; Delete; }{ Surface{%3}; Delete; };\n").arg(shapenum).arg(shapenum3).arg((*its).num);
								structbool.str = common.toStdString();
								structbool.bnum = shapenum;
								structbool.Booleanname = "Plane," + (*it).boolean;
								bbool.push_back(structbool);
								//numover = shapenum3;
								structdelete.str = "Plane," +(*it).boolean;
								structdelete.num1 = shapenum;
								structdelete.num2 = (*its).num;
								geodelete.push_back(structdelete);
								//QString snums = QObject::tr("s%1s").arg(shapenum);
								//Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
								//Recordplanenum.remove(QString::number((*its).num));
								//Recordplanenum.remove(QString::number(shapenum3));
							}
							else{
								QString common;
								QString snums = QObject::tr("s%1s[]").arg(shapenum);
								if(s1==1){
									common= QObject::tr("s%1s=BooleanIntersection{ Surface{s%2s}; Delete; }{ Volume{%3}; Delete; };\n").arg(shapenum).arg(shapenum3).arg((*its).num);
									QString shap1 = QObject::tr("s%1s[]").arg(shapenum3);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(shap1);
								}else{
									common= QObject::tr("s%1s=BooleanIntersection{ Surface{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(shapenum).arg(shapenum3).arg((*its).num);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(QString::number(shapenum3));
								}
								structbool.str = common.toStdString();
								structbool.bnum = shapenum;
								structbool.Booleanname = "Plane," + (*it).boolean;
								bbool.push_back(structbool);
								//numover = shapenum3;
								structdelete.str = "Plane," +(*it).boolean;
								structdelete.num1 = shapenum;
								structdelete.num2 = (*its).num;
								geodelete.push_back(structdelete);
								//QString snums = QObject::tr("s%1s").arg(shapenum);
								//Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
							
								//Recordplanenum.remove(QString::number(shapenum3));
							}
						}
						else{
							//Base::Console().Message("********8745*******************\n");
							if ((*its).plan){
									QString common;
									QString snums = QObject::tr("s%1s[]").arg(shapenum);
								if(s2==1){
									common= QObject::tr("s%1s=BooleanIntersection{ Surface{s%2s}; Delete; }{ Volume{%3}; Delete; };\n").arg(shapenum).arg((*its).num).arg(shapenum3);
									QString shap1 = QObject::tr("s%1s[]").arg((*its).num);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(shap1);
								}else{
									common= QObject::tr("s%1s=BooleanIntersection{ Surface{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(shapenum).arg((*its).num).arg(shapenum3);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(QString::number((*its).num));
								}
									structbool.str = common.toStdString();
									structbool.bnum = shapenum;
									structbool.Booleanname = "Plane," + (*it).boolean;
									bbool.push_back(structbool);
									//numover = shapenum3;
									structdelete.str = "Plane," +(*it).boolean;
									structdelete.num1 = shapenum;
									structdelete.num2 = (*its).num;
									geodelete.push_back(structdelete);
							}
							//++shapenum;
							else{
							QString common = QObject::tr("BooleanIntersection(%1)={ Volume{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(shapenum).arg(shapenum3).arg((*its).num);
							structbool.str = common.toStdString();
							structbool.bnum = shapenum;
							structbool.Booleanname=(*it).boolean;
							bbool.push_back(structbool);
							//numover = shapenum;
							structdelete.str = (*it).boolean;
							structdelete.num1 = shapenum;
							structdelete.num2 = (*its).num;
							geodelete.push_back(structdelete);
							}
						}
						shapenum3 = shapenum;
						if (it == its){
							its++;
							geobools.erase(it++);
							flagits=1;
						}
						else{
							it3 = it;
							it = its++;
							geobools.erase(it);//删除已经输出的
							it = it3;
						}

					}
					else{
						//Base::Console().Message("********8746*******************\n");
						if (planeflag){
							if ((*its).plan){
								QString common;
								QString snums = QObject::tr("s%1s[]").arg(shapenum);
								if(s1==1&&s2!=1){
									common = QObject::tr("s%1s=BooleanIntersection{ Surface{s%2s}; Delete; }{ Surface{%3}; Delete; };\n").arg(boonum).arg(shapenum3).arg((*its).num);
									QString shap1 = QObject::tr("s%1s[]").arg(shapenum3);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(QString::number((*its).num));
									Recordplanenum.remove(shap1);
								}
								if(s1==1&&s2==1){
									common = QObject::tr("s%1s=BooleanIntersection{ Surface{s%2s}; Delete; }{ Surface{s%3s}; Delete; };\n").arg(boonum).arg(shapenum3).arg((*its).num);
									QString shap1 = QObject::tr("s%1s[]").arg(shapenum3);
									QString shap2 = QObject::tr("s%1s[]").arg((*its).num);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(shap2);
									Recordplanenum.remove(shap1);
								}
								if(s1!=1&&s2==1){
									common = QObject::tr("s%1s=BooleanIntersection{ Surface{%2}; Delete; }{ Surface{s%3s}; Delete; };\n").arg(boonum).arg(shapenum3).arg((*its).num);
									QString shap2 = QObject::tr("s%1s[]").arg((*its).num);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(shap2);
									Recordplanenum.remove(QString::number(shapenum3));
								}
								if(s1!=1&&s2!=1){
                                    QMap<int, QString>::Iterator Multiflag1,Multiflag2;//11-12
                                    Multiflag1=Exmultiflag.find(shapenum3);
                                    Multiflag2=Exmultiflag.find((*its).num);
                                    if(Multiflag1!=Exmultiflag.end()&&Multiflag2!=Exmultiflag.end()){//先考虑了两个都是Dwire多个平面的
                                       common = QObject::tr("s%1s=BooleanIntersection{ Surface{%2%3}; Delete; }{ Surface{%4%5}; Delete; };\n").arg(boonum).arg(Multiflag1.value()).arg(shapenum3).arg(Multiflag2.value()).arg((*its).num);
                                    }
                                    else if(Multiflag1!=Exmultiflag.end()&&Multiflag2==Exmultiflag.end()){//
                                        common = QObject::tr("s%1s=BooleanIntersection{ Surface{%2%3}; Delete; }{ Surface{%4}; Delete; };\n").arg(boonum).arg(Multiflag1.value()).arg(shapenum3).arg((*its).num);
                                    }
                                    else if(Multiflag1==Exmultiflag.end()&&Multiflag2!=Exmultiflag.end()){//
                                        common = QObject::tr("s%1s=BooleanIntersection{ Surface{%2}; Delete; }{ Surface{%3%4}; Delete; };\n").arg(boonum).arg(shapenum3).arg(Multiflag2.value()).arg((*its).num);
                                    }
                                    else{
                                        common = QObject::tr("s%1s=BooleanIntersection{ Surface{%2}; Delete; }{ Surface{%3}; Delete; };\n").arg(boonum).arg(shapenum3).arg((*its).num);
                                    }
									//common = QObject::tr("s%1s=BooleanIntersection{ Surface{%2}; Delete; }{ Surface{%3}; Delete; };\n").arg(boonum).arg(shapenum3).arg((*its).num);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(QString::number((*its).num));
									Recordplanenum.remove(QString::number(shapenum3));
								}
							//QString common = QObject::tr("s%1s=BooleanIntersection{ Surface{%2}; Delete; }{ Surface{%3}; Delete; };\n").arg(boonum).arg(shapenum3).arg((*its).num);
							structbool.str = common.toStdString();
							structbool.bnum = boonum;
							structbool.Booleanname = "Plane," + (*it).boolean;
							bbool.push_back(structbool);
							//numover = shapenum3;
							structdelete.str = "Plane," +(*it).boolean;
							structdelete.num1 = boonum;
							structdelete.num2 = (*its).num;
							geodelete.push_back(structdelete);
							//QString snums = QObject::tr("s%1s").arg(boonum);
							//Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
							//Recordplanenum.remove(QString::number((*its).num));
							//Recordplanenum.remove(QString::number(shapenum3));
							}
							else{
								QString snums = QObject::tr("s%1s[]").arg(boonum);
								QString common;
								if(s1==1){
									 common = QObject::tr("s%1s=BooleanIntersection{ Surface{s%2s}; Delete; }{ Volume{%3}; Delete; };\n").arg(boonum).arg(shapenum3).arg((*its).num);
									QString shap1 = QObject::tr("s%1s[]").arg(shapenum3);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(shap1);
								}
								else{
									 common = QObject::tr("s%1s=BooleanIntersection{ Surface{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(boonum).arg(shapenum3).arg((*its).num);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(QString::number(shapenum3));
								}
								structbool.str = common.toStdString();
								structbool.bnum = boonum;
								structbool.Booleanname = "Plane," + (*it).boolean;
								bbool.push_back(structbool);
								//numover = shapenum3;
								structdelete.str = (*it).boolean;
								structdelete.num1 = boonum;
								structdelete.num2 = (*its).num;
								geodelete.push_back(structdelete);
								
								//Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
								
								//Recordplanenum.remove(QString::number(shapenum3));
							}
						}
						else{
							if ((*its).plan){
								QString snums = QObject::tr("s%1s[]").arg(boonum);
								QString common;
								if(s2==1){
									 common = QObject::tr("s%1s=BooleanIntersection{ Surface{s%2s}; Delete; }{ Volume{%3}; Delete; };\n").arg(boonum).arg((*its).num).arg(shapenum3);
									QString shap1 = QObject::tr("s%1s[]").arg((*its).num);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(shap1);
								}
								else{
									 common = QObject::tr("s%1s=BooleanIntersection{ Surface{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(boonum).arg((*its).num).arg(shapenum3);
									Recordplanenum.insert(snums, QString::fromStdString((*it).boolean));
									Recordplanenum.remove(QString::number((*its).num));
								}
								structbool.str = common.toStdString();
								structbool.bnum = boonum;
								structbool.Booleanname = "Plane," + (*it).boolean;
								bbool.push_back(structbool);
								//numover = shapenum3;
								structdelete.str = (*it).boolean;
								structdelete.num1 = boonum;
								structdelete.num2 = (*its).num;
								geodelete.push_back(structdelete);
							}
							else{
								QString common = QObject::tr("BooleanIntersection(%1)={ Volume{%2};Delete; }{ Volume{%3}; Delete; };\n").arg(boonum).arg(shapenum3).arg((*its).num);
								structbool.str = common.toStdString();
								structbool.bnum = boonum;
								structbool.Booleanname=(*it).boolean;
								bbool.push_back(structbool);
								structdelete.str = (*it).boolean;
								structdelete.num1 = boonum;
								structdelete.num2 = (*its).num;
								geodelete.push_back(structdelete);
							}
						}
						shapenum3 = boonum;
						if (it == its){
							its++;
							geobools.erase(it++);
							flagits=1;
						}
						else{
							it3 = it;
							it = its++;
							geobools.erase(it);//删除已经输出的
							it = it3;
						}


					}
				}
				//Base::Console().Message("********V88*******************\n");
			}//if
			else{
				its++;
			}
			//Base::Console().Message("********V89*******************\n");
			//Base::Console().Message("********V3333*******************\n");
			//else{
			//++its;
			//}
		}//its for
		if(flagits!=1){it++;}
	}//it for
	//Base::Console().Message("********V12*******************\n");
	
	 if(!geounmap.isEmpty()){//输出多个union的内容
		QString slist;
		QString boollean1, boollean2;
		int m,n=0;
		QStringList mlist;
		
		//Base::Console().Message("******2********%d*\n",geounmap.size());
		for (QMap<QString, QString>::iterator iter = geounmap.begin(); iter != geounmap.end();)
		{
			//Base::Console().Message("******1*********\n");
				int i = 0;
				boollean1 = iter.key().section(QObject::tr(","), 0, 0);
				m = iter.key().section(QObject::tr(","), 1, 1).toInt();
				QString geoname1 = iter.value().section(QObject::tr(","), 1, 1);
				int geonum1=iter.value().section(QObject::tr(","), 2, 2).toInt();
				QString geoname2 = iter.value().section(QObject::tr(","), 3, 3);
				int geonum2=iter.value().section(QObject::tr(","), 4, 4).toInt();
				//Base::Console().Message("******1****d=%d**s=%d**m=%d********==%s\n",geonum1,geonum2,m,boollean1.toStdString().c_str());
				if (geounmap.size() == 1){//925只有一个需要直接输出。
					QString fusion1 = QObject::tr("BooleanUnion(%1)={ Volume{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(m).arg(geonum1).arg(geonum2);
					structbool.str = fusion1.toStdString();
					structbool.bnum = m;
					structbool.Booleanname = boollean1.toStdString();
					bbool.push_back(structbool);
					structdelete.str = boollean1.toStdString();
					structdelete.num1 = m;
					structdelete.num2 = geonum2;
					geodelete.push_back(structdelete);
					break;
				}//end925
				for (QMap<QString, QString>::iterator ites = geounmap.begin(); ites != geounmap.end();)
				{
					boollean2 = ites.key().section(QObject::tr(","), 0, 0);
					n = ites.key().section(QObject::tr(","), 1, 1).toInt();
					
					if (boollean2 == boollean1){ 
						if(m==n){ites++;continue; }
						QString geoname3 = ites.value().section(QObject::tr(","), 1, 1);
						int geonum3 = ites.value().section(QObject::tr(","), 2, 2).toInt();
						QString geoname4 = ites.value().section(QObject::tr(","), 3, 3);
						int geonum4 = ites.value().section(QObject::tr(","), 4, 4).toInt();
						//Base::Console().Message("******5641******%s***%s***%s*******%s\n",geoname1.toStdString().c_str(),geoname2.toStdString().c_str(),geoname3.toStdString().c_str(),geoname4.toStdString().c_str());
						QString fusion1;
						QString fusion2;
						bool tf1 = CommonFunction::Booleanunionoperation(geoname1.toStdString(), geoname2.toStdString());
						if (tf1){
							i=1;
						 fusion1 = QObject::tr("BooleanUnion(%1)={ Volume{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(m).arg(geonum1).arg(geonum2);
						 fusion2 = QObject::tr("BooleanUnion(%1)={ Volume{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(n).arg(geonum3).arg(geonum4);
						// Base::Console().Message("******5641****1\n");
						}
						bool tf2 = CommonFunction::Booleanunionoperation(geoname1.toStdString(), geoname3.toStdString());
						if (tf2){
							i=1;
						 fusion1 = QObject::tr("BooleanUnion(%1)={ Volume{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(m).arg(geonum1).arg(geonum3);
						 fusion2 = QObject::tr("BooleanUnion(%1)={ Volume{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(n).arg(geonum2).arg(geonum4);
						 //Base::Console().Message("******5641****2\n");
						}
						bool tf3 = CommonFunction::Booleanunionoperation(geoname1.toStdString(), geoname4.toStdString());
						if (tf3){
							i=1;
						 fusion1 = QObject::tr("BooleanUnion(%1)={ Volume{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(m).arg(geonum1).arg(geonum4);
						 fusion2 = QObject::tr("BooleanUnion(%1)={ Volume{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(n).arg(geonum2).arg(geonum3);
						 //Base::Console().Message("******5641****3\n");
						}
						bool tf4 = CommonFunction::Booleanunionoperation(geoname2.toStdString(), geoname3.toStdString());
						if (tf4){
							i=1;
						 fusion1 = QObject::tr("BooleanUnion(%1)={ Volume{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(m).arg(geonum2).arg(geonum3);
						 fusion2 = QObject::tr("BooleanUnion(%1)={ Volume{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(n).arg(geonum1).arg(geonum4);
						 //Base::Console().Message("******5641****4\n");
						}
						bool tf5 = CommonFunction::Booleanunionoperation(geoname2.toStdString(), geoname4.toStdString());
						if (tf5){
							i=1;
						 fusion1 = QObject::tr("BooleanUnion(%1)={ Volume{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(m).arg(geonum2).arg(geonum4);
						 fusion2 = QObject::tr("BooleanUnion(%1)={ Volume{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(n).arg(geonum1).arg(geonum3);
						 //Base::Console().Message("******5641****5\n");
						}
						if(i!=1){
							fusion1 = QObject::tr("BooleanUnion(%1)={ Volume{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(m).arg(geonum1).arg(geonum2);
						 fusion2 = QObject::tr("BooleanUnion(%1)={ Volume{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(n).arg(geonum3).arg(geonum4);
						}
						structbool.str = fusion1.toStdString();
						structbool.bnum = m;
						structbool.Booleanname = boollean1.toStdString();
						bbool.push_back(structbool);			
						structdelete.str = boollean1.toStdString();
						structdelete.num1 = m;
						structdelete.num2 = geonum2;
						geodelete.push_back(structdelete);
						
						structbool.str = fusion2.toStdString();
						structbool.bnum = n;
						structbool.Booleanname = boollean1.toStdString();
						bbool.push_back(structbool);			
						structdelete.str = boollean1.toStdString();
						structdelete.num1 = n;
						structdelete.num2 = geonum4;
						geodelete.push_back(structdelete);
						ites=geounmap.erase(ites);
						iter=geounmap.erase(iter);
						i=2;
						break;//两两能组成结合的可以。
					}
					else{
						ites++;
					}
				}//ites
			if(i!=2){
				iter++;
			}
				
		}//for iter
		
		
	}
	//输出布尔过程
	list<CommonFunction::boolean>::iterator bit;
	list<CommonFunction::boolean>::iterator bits;
	bbool.sort();

	//boollist bbool2;
	int bbnum=bbool.size();
	//int bitnum = 0;
	for (bit = bbool.begin(); bit != bbool.end(); ++bit){
		//Base::Console().Message("******1******%d***%s\n",(*bit).bnum,(*bit).str.c_str());
		Savestreamfile << QString::fromStdString((*bit).str);
		QString planestr;
		 int pflag = 0;
		if (QString::fromStdString((*bit).Booleanname).left(5) == QObject::tr("Plane")){//面的布尔运算位移不同
			planestr=QString::fromStdString((*bit).Booleanname).section(QObject::tr(","), 1, 1);
			pflag = 1;
		} 
		string translate;
		if (pflag){
			translate = Booleanoperationinformation(planestr.toStdString());
		} 
		else{
			translate = Booleanoperationinformation((*bit).Booleanname);
		}
		int num;
		string bname = (*bit).Booleanname;
		num = (*bit).bnum;
		for (bits = bbool.begin(); bits != bbool.end();){//判断是否union的最后一步，它可能是多个体进行的
		//Base::Console().Message("******124******%d***%s\n",(*bits).bnum,(*bits).Booleanname.c_str());
			if(bname==(*bits).Booleanname&&num!=(*bits).bnum){
				//Base::Console().Message("******100******%d***%s\n",(*bits).bnum,(*bits).Booleanname.c_str());
				Savestreamfile << QString::fromStdString((*bits).str);//625
				num=(*bits).bnum;
				bits=bbool.erase(bits);
			}else{
				++bits;
			}
		}
		//bitnum++;
		if (translate != "NULL" /*&& num == (*bit).bnum*/){
			//QString tran = QString::fromStdString(translate);
			QString Axsix=QString::fromStdString(translate).section(QObject::tr(","), 0, 0);
			QString Axsiy=QString::fromStdString(translate).section(QObject::tr(","), 1, 1);
			QString Axsiz=QString::fromStdString(translate).section(QObject::tr(","), 2, 2);
			QString angle=QString::fromStdString(translate).section(QObject::tr(","), 3, 3);
			QString x=QString::fromStdString(translate).section(QObject::tr(","), 4, 4);
			QString y=QString::fromStdString(translate).section(QObject::tr(","), 5, 5);
			QString z=QString::fromStdString(translate).section(QObject::tr(","), 6, 6);
			QString Rotation;
			QString Translate;
			if (pflag){
				 Rotation = QObject::tr("Rotate { {%1,%2,%3}, {0,0,0},%4 } { Surface{%5}; }\n").arg(Axsix).arg(Axsiy).arg(Axsiz).arg(angle).arg(num);
				Translate = QObject::tr("Translate {%1,%2,%3}{Surface{%4};}\n").arg(x).arg(y).arg(z).arg(num);
			}
			else{
				Savestreamfile <<QObject::tr("Rotate { {%1,%2,%3}, {0,0,0},%4 } { Volume{%5}; }\n").arg(Axsix).arg(Axsiy).arg(Axsiz).arg(angle).arg(num);
				Savestreamfile << QObject::tr("Translate {%1,%2,%3}{Volume{%4};}\n").arg(x).arg(y).arg(z).arg(num);
			}
			//Savestreamfile << Rotation;
			//Savestreamfile << Translate;
		}
	}


	///////最外层和介质PEC的处理
	int flagRad = 0;
	int CoveringBox = 0;
	string outname = CommonFunction::outObjStrName();//获取最大几何体
	Base::Console().Message("******outname***********************%s\n",outname.c_str());
	QSettings settingrad(QObject::tr("../ini/Boundaries.ini"), QSettings::IniFormat);
	for (std::vector<App::DocumentObject*>::iterator it = sels.begin(); it != sels.end(); ++it)
	{
		QString Groups = QString::fromStdString((*it)->getNameInDocument());
		if (Groups.left(4) == QObject::tr("Line")){
			continue;
		}
		{// 判断是否不可见的，不可见的跳过
			Gui::ViewProvider* pcProv = Gui::Application::Instance->getViewProvider(*it);
			bool visibility = true;
			if (pcProv) {
				visibility = ((Gui::ViewProviderDocumentObject*)pcProv)->Visibility.getValue();
			}
			if (visibility == false){// 不可见的跳过
				continue;
			}
		}
		Base::Console().Message("   name***********************%s\n",(*it)->getNameInDocument());
		int flagRad = CommonFunction::DecideRad(*it);//获取设置的Rad
        Base::Console().Message("   flahRad1***********************%d\n",flagRad);
		if(flag==1){//把设置Rad的标志写入文件
			settingrad.setValue(QObject::tr("%1/RADFLAG").arg(Groups), flagRad);
		}
		if(flag==2){//获取Rad设置的情况，有些面会没有在了，所以要记录最开始设置的情况
			flagRad = settingrad.value(QObject::tr("%1/RADFLAG").arg(Groups)).toInt();
		}
		Base::Console().Message("   flahRad***********************%d\n",flagRad);
		if (flagRad){
			if (QString::fromStdString(outname) == QString(QLatin1String((*it)->getNameInDocument()))){//有最外层和全部面设置为Rad才能为空气罩
				CoveringBox = 1;//获取最外层几何体全部设置了Rad
				break;
			}
		}
	}
	Base::Console().Message("******CoveringBox***********************%d\n",CoveringBox);
	if (outname != "NULL"&&CoveringBox == 0){
		QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong"),
							QObject::tr("Please set up the external surface as the radiation boundary!"));
		return 0;
	}
	////20184/11
	QSettings settings(QObject::tr("../ini/Model.ini"), QSettings::IniFormat);
	settings.beginGroup(QObject::tr("model"));
	settings.setValue(QObject::tr("MaxGeom"), QString::fromStdString(outname));
	settings.endGroup();
	list<CommonFunction::geometry>::iterator ito;
	list<CommonFunction::geodelete>::iterator del;
	int flagpec = 0;
	int base = 0;
	int tool = 0;
	int bflag=0;
	int Modelsum = 0;
	int OtherBodyflag=0;//判断是第一次
	int OtherBody=0;
	int firstBody=0;
	QString Fragmentstool=QObject::tr("");//629
    QString FragmenExttool=QObject::tr("");//11-2增加拉伸处理的判断。
	int Fragmentsbase=0;//629
	std::map<int,string> unionbox;
	QStringList boolmap;
	QStringList boollistmap;
    QStringList boolEtmap;//11-2增加拉伸处理的判断。
	QSettings settingsp(QObject::tr("../ini/MeshSetting.ini"), QSettings::IniFormat);
	int msum = settingsp.value(QObject::tr("Model/Modelsum")).toInt();
	double meshsizepart = settingsp.value(QObject::tr("Model/MeshSizeParts")).toDouble();
	QSettings settingpec(QObject::tr("../ini/Boundaries.ini"), QSettings::IniFormat);
	for (std::vector<App::DocumentObject*>::iterator it = sels.begin(); it != sels.end(); ++it)
	{
		std::string  cutname = (*it)->getNameInDocument();
		//Base::Console().Message("******2018****627**pec=%d*****CoveringBox=%d****name=%s*************\n",flags,CoveringBox,cutname.c_str());
		QString Groups = QString::fromStdString(cutname);
		int flags = CommonFunction::DecidePec(*it);//获取设置的pec
		if(flag==1){//把设置pec的标志写入文件
			settingpec.setValue(QObject::tr("%1/PECFLAG").arg(Groups), flags);
		}
		if(flag==2){//获取pec设置的情况，有些面会没有在了，所以要记录最开始设置的情况
			flags = settingpec.value(QObject::tr("%1/PECFLAG").arg(Groups)).toInt();
		}
		
		int flagExc = CommonFunction::DecideExci(*it);//获取设置的Exc
		
		if (Groups.left(4) == QObject::tr("Line")){
			continue;
		}
		if (Groups.left(5) == QObject::tr("Plane")){
			continue;
		}
		{// 判断是否不可见的，不可见的跳过
			Gui::ViewProvider* pcProv = Gui::Application::Instance->getViewProvider(*it);
			bool visibility = true;
			if (pcProv) {
				visibility = ((Gui::ViewProviderDocumentObject*)pcProv)->Visibility.getValue();
			}
			if (visibility == false){// 不可见的跳过
				continue;
			}
		}
		if (cutname == outname){ 
		     int partnum;//626输出只有一个空气罩里面都是平面的网格控制
			for (ito = geobools.begin(); ito != geobools.end(); ++ito){
				if ((*ito).geom == cutname){
					partnum=(*ito).m;
				}
			}
			for (del = geodelete.begin(); del != geodelete.end(); ++del){
				if ((*del).str == cutname){
					partnum=(*del).num1 >(*del).num2 ? (*del).num1 : (*del).num2;
				}
			}
			for(int i=1;i<=msum;i++){
					QString surface = QString::fromLatin1("Model/Model%1").arg(i);
					QString  bstr = settingsp.value(surface).toString();
					if(bstr.toStdString()==cutname){
					Savestreamfile << QObject::tr("Characteristic Length{ PointsOf{ Volume{%1};} } = %2;\n").arg(partnum).arg(meshsizepart);
					settingsp.setValue(surface, QObject::tr("NULL"));
					}
				}
		   continue;
		}//247
		
		int outflag = 0, cutflag = 0;
        int tensileflag=1;//拉伸出来的平面不能使用剪切函数，所以在这个改了使其使用平面那个函数。
	  if (CoveringBox){
			flagpec = 1;
			int planflags=0;
			for (ito = geobools.begin(); ito != geobools.end(); ++ito){
				 //if (QString::fromStdString((*ito).geom).left(5) == QObject::tr("Plane")){
                 if (QString::fromStdString((*ito).geom).left(5) == QObject::tr("Plane")){
					 planflags=1;
					continue;
					} 
				if ((*ito).geom == cutname){
					cutflag = 1;
                    tensileflag=1;
					tool = (*ito).m;
                    if(QString::fromStdString((*ito).geom).left(7) == QObject::tr("Extrude")){//11-1
                   // Base::Console().Message("******t***********************\n");
                     tensileflag=0;
                    }
				}
				if (((*ito).geom == outname)&&(bflag==0)){
					outflag = 1;
                    //tensileflag=1;
                    //Base::Console().Message("******ts*******%s****************\n",outname);
					base = (*ito).m;
					Fragmentsbase=(*ito).m;
					for(int i=1;i<=msum;i++){
						QString surface = QString::fromLatin1("Model/Model%1").arg(i);
						QString  bstr = settingsp.value(surface).toString();
						if(bstr.toStdString()==outname){
							Savestreamfile << QObject::tr("Characteristic Length{ PointsOf{ Volume{%1};} } = %2;\n").arg(base).arg(meshsizepart);
							settingsp.setValue(surface, QObject::tr("NULL"));
						}
					}
				}
			}
          //  Base::Console().Message("******to************%d***********\n",tensileflag);
			for (del = geodelete.begin(); del != geodelete.end(); ++del){//布尔运算重新编号的//布尔运算应该拉伸不会出拉伸，但添加上好像也没用
				//if (QString::fromStdString((*del).str).left(5) == QObject::tr("Plane")){//11-1
                    if (QString::fromStdString((*del).str).left(5) == QObject::tr("Plane")){//11-1
					planflags=1;
					continue;
				}
				if (((*del).str == outname) && (bflag == 0)){
					outflag = 1;
                    tensileflag=1;
					base=(*del).num1 >(*del).num2 ? (*del).num1 : (*del).num2;
					Fragmentsbase=(*del).num1 >(*del).num2 ? (*del).num1 : (*del).num2;//629
					for(int i=1;i<=msum;i++){
						QString surface = QString::fromLatin1("Model/Model%1").arg(i);
						QString  bstr = settingsp.value(surface).toString();
						if(bstr.toStdString()==outname){
							Savestreamfile << QObject::tr("Characteristic Length{ PointsOf{ Volume{%1};} } = %2;\n").arg(base).arg(meshsizepart);
							settingsp.setValue(surface, QObject::tr("NULL"));
						}
					}
				}
				
				if ((*del).str == cutname){
					//tool = (*del).num1;
					cutflag = 1;
                    tensileflag=1;
					tool = (*del).num1 >(*del).num2 ? (*del).num1 : (*del).num2;
                    if(QString::fromStdString((*del).str).left(7) == QObject::tr("Extrude")){//11-1
                   // Base::Console().Message("******ol1***********************\n");
                     tensileflag=0;//拉伸出来的平面不能使用剪切函数，所以在这个改了使其使用平面那个函数。
                     continue;
                    }
				}
			}
		//}
			
			if(flag==2){
				for(int i=1;i<=msum;i++){
					QString surface = QString::fromLatin1("Model/Model%1").arg(i);
					QString  bstr = settingsp.value(surface).toString();
					if(bstr.toStdString()==cutname){
					Savestreamfile << QObject::tr("Characteristic Length{ PointsOf{ Volume{%1};} } = %2;\n").arg(tool).arg(meshsizepart);
					settingsp.setValue(surface, QObject::tr("NULL"));
					}
				}
			}
			//bflag=1;
			QString cut;
            if(tensileflag==0){//拉伸添加11-1
                flags=0;
                //Base::Console().Message("******tool1***********************%d\n",tool);
            }
			if(flags==1){//629修改成使用BooleanFragments的方式,平面的不能加入剪切
				if(tool==0){continue;};
				 cut = QObject::tr("BooleanDifference(%1)={ Volume{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(++shapenum).arg(base).arg(tool);
				Savestreamfile << cut;
				Fragmentsbase= shapenum;//7-3 
				//Fragmentstool=Fragmentstool+QObject::tr("%1,").arg(tool);
				boolmap<<QObject::tr("%1").arg(tool);
				boolmap<<QObject::tr("%1").arg(base);
				bflag=1;
				base=shapenum;
				//Base::Console().Message("******tool1***********************%d\n",tool);
			}
			else{
				//cut = QObject::tr("BooleanDifference(%1)={ Volume{%2}; Delete; }{ Volume{%3};};\n").arg(++shapenum).arg(base).arg(tool);
				if(tool==0){continue;};
				if (true == boolmap.contains(QObject::tr("%1").arg(tool))){
					//Base::Console().Message("******tool2***********************%d\n",tool);
					continue;}
				//Fragmentstool=Fragmentstool+QObject::tr("%1,").arg(tool);
				//Base::Console().Message("******tool3***********************%d\n",tool);
                //11-2修改加了一个拉伸的处理
				//boollistmap<<QObject::tr("%1").arg(tool);
                if(tensileflag==0){
                    boolEtmap<<QObject::tr("%1").arg(tool);
                }
                else{
                    boollistmap<<QObject::tr("%1").arg(tool);
                }
			}
			QSettings settingmodel(QObject::tr("../ini/ModelNo.ini"), QSettings::IniFormat);
			settingmodel.setValue(QObject::tr("Model/Modelname"), QString::fromStdString(outname));
			settingmodel.setValue(QObject::tr("Model/geoNo"), shapenum);
			settingmodel.setValue(QObject::tr("Model/base"), base);
			settingmodel.setValue(QObject::tr("Model/tool"), tool);

			//base=shapenum;
			

			

		}//if 最外层的处理部分
		else{//不是有空气盒子的合成一体
			int pflag=0;//平面的不进行处理
			for (ito = geobools.begin(); ito != geobools.end(); ++ito){
				if ((*ito).geom == cutname){
					OtherBody = (*ito).m;
				}
			}
			for (del = geodelete.begin(); del != geodelete.end(); ++del){//布尔运算重新编号的
				if (QString::fromStdString((*del).str).left(5) == QObject::tr("Plane")){//面的布尔运算不进行操作
					QString planestr=QString::fromStdString((*del).str).section(QObject::tr(","), 1, 1);
					pflag = 1;
					if (planestr== QString::fromStdString(cutname)){
					OtherBody = (*del).num1 >(*del).num2 ? (*del).num1 : (*del).num2;
					}
				} 
				if ((*del).str == cutname){
					OtherBody = (*del).num1 >(*del).num2 ? (*del).num1 : (*del).num2;
				}
			}
			/////
			if (flag == 2){
				int msum = settingsp.value(QObject::tr("Model/Modelsum")).toInt();
				double meshsizepart = settingsp.value(QObject::tr("Model/MeshSizeParts")).toDouble();
				for(int i=1;i<=msum;i++){
					QString surface = QString::fromLatin1("Model/Model%1").arg(i);
					QString  bstr = settingsp.value(surface).toString();
					if(bstr.toStdString()==cutname){
					Savestreamfile << QObject::tr("Characteristic Length{ PointsOf{ Volume{%1};} } = %2;\n").arg(OtherBody).arg(meshsizepart);
					settingsp.setValue(surface, QObject::tr("NULL"));
					}
				}
			}
			/////
			if(pflag==1){
				continue;
			}
			unionbox.insert(pair<int, string>(OtherBody, cutname));
		}//else
	}//for
	/////////////2018/912//进行过剪切的就不加入进Fragmentstool里面
	for (int i = 0; i < boollistmap.size(); i++){
		//Base::Console().Message("geolists==%d=%s\n", i, boollistmap[i].toStdString().c_str());
		//int a=boollistmap[i].
		if (true == boolmap.contains(boollistmap[i])){
			//Base::Console().Message("******tool2***********************%d\n",boollistmap[i]);
					continue;
		}
		else{
			//Base::Console().Message("******tool1***********************%d\n",boollistmap[i]);
			Fragmentstool=Fragmentstool+QObject::tr("%1,").arg(boollistmap[i]);
		}
		//Base::Console().Message("******tool***********************%d\n",boollistmap[i]);
	}
    ////11-2增加一个拉伸的判断
    for (int i = 0; i < boolEtmap.size(); i++){
		if (true == boolmap.contains(boolEtmap[i])){
					continue;
		}
		else{
			FragmenExttool=FragmenExttool+QObject::tr("%1,").arg(boolEtmap[i]);
		}
	}
	/////////

	////2018/6-11/////////处理非接触面union的问题
	if (unionbox.size() > 1){
		//Base::Console().Message("******unionbox=%d**********************\n",unionbox.size());
		QString slist;
		string box1, box2;
		int m,n=0;
		for (map<int, string>::iterator iter = unionbox.begin(); iter != unionbox.end(); iter++)
		{
				int i = 0;
				box1 = iter->second;
				m = iter->first;
				for (map<int, string>::iterator ites = unionbox.begin(); ites != unionbox.end(); ites++)
				{
					if (iter->second == ites->second){ continue; }
						box2 = ites->second;
						bool tf = CommonFunction::Booleanunionoperation(box1, box2);
						if (tf){
							i++;
						}
				
				}//ites
				if (i == 1){//只与一个接触可能是开头或者结尾的体
					slist = QString::fromStdString(box1);
					break;
				}
		}//for iter
	
		QStringList mlist;
		QStringList geolists;
		int k = 0;
		App::Document* docc = App::GetApplication().getActiveDocument();
		std::vector<App::DocumentObject*> selsc = docc->getObjectsOfType(App::DocumentObject::getClassTypeId());
		for (std::vector<App::DocumentObject*>::iterator itc = selsc.begin(); itc != selsc.end(); ++itc)
		{
			std::string Group = (*itc)->getNameInDocument();
			QString Groups = QString::fromStdString(Group);
			if (Groups.left(5) == QObject::tr("Group")){
				continue;
			}
			if (Groups.left(5) == QObject::tr("Plane")){
				continue;
			}
			if (Groups.left(4) == QObject::tr("Line")){
				continue;
			}
			Gui::ViewProvider* pcProv = Gui::Application::Instance->getViewProvider(*itc);
			bool visibility = true;
			if (pcProv /*&& pcProv->getTypeId().isDerivedFrom(Gui::ViewProviderDocumentObject::getClassTypeId())*/) {
				visibility = ((Gui::ViewProviderDocumentObject*)pcProv)->Visibility.getValue();
			}
			// 读取几何体可见(Visibility)属性值  START  END
			if (visibility == false){// 不可见的跳过
				continue;
			}
			/* if (k == 0)
			{
				slist = Groups;
			} */
			mlist << Groups;
			k++;
		}
		geolists=Sortgeo(slist, k, geolists);
		int geo1;
		int f = 0;
		for (int i = 0; i < geolists.size(); i++){
			for (map<int, string>::iterator nter = unionbox.begin(); nter != unionbox.end();)
			{
				if (geolists[i].toStdString() == nter->second){
					QSettings settingNo(QObject::tr("../ini/ModelNo.ini"), QSettings::IniFormat);
					if (f == 0){ 
						geo1 = nter->first;
						f = 1;
						nter = unionbox.erase(nter);
						break;
					}
					if (f == 1){
						QString cut = QObject::tr("BooleanUnion(%1)={ Volume{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(++shapenum).arg(geo1).arg(nter->first);
						Savestreamfile << cut;
						f = 2;
						int i = settingNo.value(QObject::tr("Count/count")).toInt();
						//if（i == NULL{ i = 0; }
						settingNo.setValue(QObject::tr("Count/count"), ++i);
						//Base::Console().Message("200  =%d,flag=%d,shapenum=%d\n",i,flag,shapenum);
						settingNo.setValue(QObject::tr("%1/geoNo").arg(i), shapenum);
						settingNo.setValue(QObject::tr("%1/ModelName").arg(i), QString::fromStdString(nter->second));
					}
					else{
						QString cut = QObject::tr("BooleanUnion(%1)={ Volume{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(++shapenum).arg(shapenum - 1).arg(nter->first);
						Savestreamfile << cut;
						int i = settingNo.value(QObject::tr("Count/count")).toInt();
						//if（i==NULL{ i = 0; }
						settingNo.setValue(QObject::tr("Count/count"), ++i);
						//Base::Console().Message("202  =%d,flag=%d,shapenum=%d\n",i,flag,shapenum);
						settingNo.setValue(QObject::tr("%1/geoNo").arg(i), shapenum);
						settingNo.setValue(QObject::tr("%1/ModelName").arg(i), QString::fromStdString(nter->second));
					}
					nter=unionbox.erase(nter);

				}
				else{
					nter++;
				}
			}//for map
		}//for i
		
		if (unionbox.size()){//可能有些模型没有输出出来。但可能会出现有些模型与其他模型都不接触。
			for (map<int, string>::iterator nter = unionbox.begin(); nter != unionbox.end(); nter++)
			{
				QString cut = QObject::tr("BooleanUnion(%1)={ Volume{%2}; Delete; }{ Volume{%3}; Delete; };\n").arg(++shapenum).arg(shapenum - 1).arg(nter->first);
				Savestreamfile << cut;
				QSettings settingNo(QObject::tr("../ini/ModelNo.ini"), QSettings::IniFormat);
				int i = settingNo.value(QObject::tr("Count/count")).toInt();
				settingNo.setValue(QObject::tr("Count/count"), ++i);
				//Base::Console().Message("201   =%d,flag=%d\n",i,flag);
			    settingNo.setValue(QObject::tr("%1/geoNo").arg(i), shapenum);
			    settingNo.setValue(QObject::tr("%1/ModelName").arg(i), QString::fromStdString(nter->second));
			}
		}

	 
		
	}
	////2018/6-11//end//////
	
	if(Fragmentstool!=QObject::tr("")){
		QString Fragments = QObject::tr("b()=BooleanFragments{ Volume{%1}; Delete; }{Volume{%2}; Delete; };\n").arg(Fragmentsbase).arg(Fragmentstool.left(Fragmentstool.length() - 1));
		Savestreamfile << Fragments;
       
	}
    ////11-2增加一个拉伸的判断
    if(FragmenExttool!=QObject::tr("")){
        if(Fragmentstool!=QObject::tr("")){
            if(ExtrudeDWireface!=QObject::tr("")){//增加DWire形成的面
                 QString Fragments = QObject::tr("d()=BooleanFragments{ Volume{b()}; Delete; }{Surface{%1,%2}; Delete; };\n").arg(ExtrudeDWireface.left(ExtrudeDWireface.length() - 1)).arg(FragmenExttool.left(FragmenExttool.length() - 1));
                 Savestreamfile << Fragments;
            }
            else{
                QString Fragments = QObject::tr("d()=BooleanFragments{ Volume{b()}; Delete; }{Surface{%1}; Delete; };\n").arg(FragmenExttool.left(FragmenExttool.length() - 1));
                Savestreamfile << Fragments;
            }
        }
        else{
             if(ExtrudeDWireface!=QObject::tr("")){//增加DWire形成的面
                 QString Fragments = QObject::tr("d()=BooleanFragments{ Volume{%1}; Delete; }{Surface{%2,%3}; Delete; };\n").arg(Fragmentsbase).arg(ExtrudeDWireface.left(ExtrudeDWireface.length() - 1)).arg(FragmenExttool.left(FragmenExttool.length() - 1));
                 Savestreamfile << Fragments;
            }
            else{
                QString Fragments = QObject::tr("d()=BooleanFragments{ Volume{%1}; Delete; }{Surface{%2}; Delete; };\n").arg(Fragmentsbase).arg(FragmenExttool.left(FragmenExttool.length() - 1));
                Savestreamfile << Fragments;
            }
        }
	}

	if(Recordplanenum.size()>=1){//二维模块对计算域修改
	   QString planenum=QObject::tr("");
	   QMap<QString,QString>::iterator iter;
		for (iter=Recordplanenum.begin();iter!=Recordplanenum.end();iter++){  
			planenum=planenum+QObject::tr("%1,").arg(iter.key());
			for(int i=1;i<=msum;i++){
					QString surface = QString::fromLatin1("Model/Model%1").arg(i);
					QString  bstr = settingsp.value(surface).toString();
					if(bstr==iter.value()){
					if(flag==2){
					Savestreamfile << QObject::tr("Characteristic Length{ PointsOf{ Surface{%1};} } = %2;\n").arg(iter.key()).arg(meshsizepart);
						}
					//settingsp.setValue(surface, QObject::tr("NULL"));
					}
				}
			//Base::Console().Message("201   %s\n", planenum.toStdString().c_str());
		}
		//Base::Console().Message("201 22  %s\n", planenum.toStdString().c_str());
		if(Fragmentstool!=QObject::tr("")){
            if(FragmenExttool!=QObject::tr("")){
                QString Fragments = QObject::tr("BooleanFragments{ Volume{d()}; Delete; }{Surface{%1}; Delete; }\n").arg(planenum.left(planenum.length() - 1));
                Savestreamfile << Fragments;
            }else{
                QString Fragments = QObject::tr("BooleanFragments{ Volume{b()}; Delete; }{Surface{%1}; Delete; }\n").arg(planenum.left(planenum.length() - 1));
                Savestreamfile << Fragments;
            }
		}else{
			if(Fragmentsbase!=0){
				QString Fragments = QObject::tr("BooleanFragments{ Volume{%1}; Delete; }{Surface{%2}; Delete; }\n").arg(Fragmentsbase).arg(planenum.left(planenum.length() - 1));
				Savestreamfile << Fragments;
			}else{
			QString Fragments = QObject::tr("BooleanFragments{ Volume{%1}; Delete; }{Surface{%2}; Delete; }\n").arg(shapenum).arg(planenum.left(planenum.length() - 1));
			Savestreamfile << Fragments;
			}
		}
	}
	
	////////initialMesh.ini里面获取2018314//////
	if (flag == 2){
		///////获取geo里面面的编号
		QStringList llist;
		QString SifFilegeo = QString::fromLatin1("../ini/box.geo_unrolled");
		QFile SFilegeo(SifFilegeo);
		if (!SFilegeo.open(QIODevice::ReadOnly))
		{
			QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong"),
				QObject::tr(" read the box.geo_unrolled file failed!"));
			return 1;
		}
		QTextStream SIFstreamfilegeo(&SFilegeo);
		QString bufgeo;
		while (!SIFstreamfilegeo.atEnd())//while start
		{
			bufgeo = SIFstreamfilegeo.readLine();
			/* if (bufgeo.left(12) == QObject::tr("Surface Loop"))//获取构成体的面
			{
				QRegExp seps(QObject::tr("[()]"));
				QString beta = bufgeo.section(seps, 1, 1);
				QRegExp sep(QObject::tr("[{}]"));
				QString b = bufgeo.section(sep, 1, 1);
				llist =llist<<b.split(QObject::tr(","));
			} */
			if (bufgeo.left(9) == QObject::tr("Line Loop"))//获取面的编号信息
		    {
			QRegExp seps(QObject::tr("[()]"));
			QString beta = bufgeo.section(seps, 1, 1);
			llist =llist<<beta;
			
		   }
		}//while end
		//////////geob编号end////////////
		QString fstr = QObject::tr("");
		QString Efstr = QObject::tr("");
		int meshnum = 0;
		QString snums = QObject::tr("");
		//boundary
		QSettings settingsmesh(QObject::tr("../ini/MeshSetting.ini"), QSettings::IniFormat);
		int bsum = settingsmesh.value(QObject::tr("Boundary/Boundarysum")).toInt();
		if (bsum){
			
			Savestreamfile << QObject::tr("Field[%1] = Attractor;\n").arg(++meshnum);
			for (int i = 1; i <= bsum; i++){
				
				QString surface = QString::fromLatin1("Boundary/Boundary%1").arg(i);
				QString  bstr = settingsmesh.value(surface).toString();
				QSettings Bsettings(QObject::tr("../ini/Boundaries.ini"), QSettings::IniFormat);
				int Num = Bsettings.value(QObject::tr("%1/BounCount").arg(bstr)).toInt();
				for (int j = 1; j <= Num; j++){
					
					QString surface = bstr + QString::fromLatin1("/MeshFaceNum%1").arg(j);
					int face = Bsettings.value(surface).toInt();
					
					//fstr = fstr + QObject::tr("%1,").arg(face);
					fstr = fstr + QObject::tr("%1,").arg(llist[face-1]);
				}
			}
			double blcmin = settingsmesh.value(QObject::tr("Boundary/FinestGridTimes")).toDouble();
			double blcmax = settingsmesh.value(QObject::tr("Boundary/CoarestGridTimes")).toDouble();
			double distmin = settingsmesh.value(QObject::tr("Boundary/FinesetDistance")).toDouble();
			double distmax = settingsmesh.value(QObject::tr("Boundary/CoarsetDistance")).toDouble();
			Savestreamfile << QObject::tr("Field[%1].FacesList = { ").arg(meshnum);
			Savestreamfile << fstr.left(fstr.length() - 1);
			Savestreamfile << QObject::tr(" };\n");
			Savestreamfile << QObject::tr("Field[%1] = Threshold;\n").arg(++meshnum);
			Savestreamfile << QObject::tr("Field[%1].IField = %2;\n").arg(meshnum).arg(meshnum-1);
			Savestreamfile << QObject::tr("Field[%1].LcMin = MeshSizeGlobalCoarsest/%2;\n").arg(meshnum).arg(blcmin);
			Savestreamfile << QObject::tr("Field[%1].LcMax = MeshSizeGlobalCoarsest/%2;\n").arg(meshnum).arg(blcmax);
			Savestreamfile << QObject::tr("Field[%1].DistMin = %2;\n").arg(meshnum).arg(distmin);
			Savestreamfile << QObject::tr("Field[%1].DistMax = %2;\n").arg(meshnum).arg(distmax);
			snums = snums + QObject::tr("%1,").arg(meshnum);
		}
		int esum = settingsmesh.value(QObject::tr("Exciation/Exciationsum")).toInt();
		if (esum){
			Savestreamfile << QObject::tr("Field[%1] = Attractor;\n").arg(++meshnum);
			for (int i = 1; i <= esum; i++){
				QString surface = QString::fromLatin1("Exciation/Exciation%1").arg(i);
				QString  bstr = settingsmesh.value(surface).toString();
				QSettings Esettings(QObject::tr("../ini/Excitation.ini"), QSettings::IniFormat);
				int Num = Esettings.value(QObject::tr("%1/SurfacesNum").arg(bstr)).toInt();
				
				for (int j = 1; j <= Num; j++){
					
					QString surface = bstr + QString::fromLatin1("/MeshFaceNum%1").arg(j);
					int face = Esettings.value(surface).toInt();
					
					Efstr = Efstr + QObject::tr("%1,").arg(llist[face-1]);
					
				}
			}
			double elcmin = settingsmesh.value(QObject::tr("Exciation/FinestGridTimes")).toDouble();
			double elcmax = settingsmesh.value(QObject::tr("Exciation/CoarestGridTimes")).toDouble();
			double eistmin = settingsmesh.value(QObject::tr("Exciation/FinesetDistance")).toDouble();
			double eistmax = settingsmesh.value(QObject::tr("Exciation/CoarsetDistance")).toDouble();
			Savestreamfile << QObject::tr("Field[%1].FacesList = { ").arg(meshnum);
			Savestreamfile << Efstr.left(Efstr.length() - 1);
			Savestreamfile << QObject::tr(" };\n");
			Savestreamfile << QObject::tr("Field[%1] = Threshold;\n").arg(++meshnum);
			Savestreamfile << QObject::tr("Field[%1].IField = %2;\n").arg(meshnum).arg(meshnum-1);
			Savestreamfile << QObject::tr("Field[%1].LcMin = MeshSizeGlobalCoarsest/%2;\n").arg(meshnum).arg(elcmin);
			Savestreamfile << QObject::tr("Field[%1].LcMax = MeshSizeGlobalCoarsest/%2;\n").arg(meshnum).arg(elcmax);
			Savestreamfile << QObject::tr("Field[%1].DistMin = %2;\n").arg(meshnum).arg(eistmin);
			Savestreamfile << QObject::tr("Field[%1].DistMax = %2;\n").arg(meshnum).arg(eistmax);
			snums = snums + QObject::tr("%1,").arg(meshnum);
		}

		//model

		
		//QString meshfacestr = settingsmesh.value(QObject::tr("Model/facesum")).toString();
		//Base::Console().Message("str=%s||%s\n", fstr.toStdString().c_str(),snums.toStdString().c_str());

		if (snums != QObject::tr("")){
			Savestreamfile << QObject::tr("Field[%1] = Min;\n").arg(++meshnum);
			Savestreamfile << QObject::tr("Field[%1].FieldsList = {%2};\n").arg(meshnum).arg(snums.left(snums.length() - 1));
			Savestreamfile << QObject::tr("Background Field = %1;\n").arg(meshnum);
		}

	}//if
	else{
		int modelMax = 0;
		QSettings settingNo(QObject::tr("../ini/ModelNo.ini"), QSettings::IniFormat);
		list<CommonFunction::geodelete>::iterator ds;
		for (ds = geodelete.begin(); ds != geodelete.end(); ++ds){
			//Base::Console().Message("geodelete==%s,num1ok=%d,num2=%d\n", (*ds).str, (*ds).num1, (*ds).num2);
			if ((*ds).num1 > (*ds).num2){
				modelMax = (*ds).num1;
			}
			else{
				modelMax = (*ds).num2;
			}
			if (QString::fromStdString((*ds).str).left(5) == QObject::tr("Plane")){ continue; }//628平面不写入
			int i = settingNo.value(QObject::tr("Count/count")).toInt();
			settingNo.setValue(QObject::tr("Count/count"), ++i);
			settingNo.setValue(QObject::tr("%1/geoNo").arg(i), modelMax);
			settingNo.setValue(QObject::tr("%1/ModelName").arg(i), QString::fromStdString((*ds).str));

		}
		list<CommonFunction::geometry>::iterator ggs;
		for (ggs = geobools.begin(); ggs != geobools.end(); ++ggs){
			//Base::Console().Message("geodbools==%s,num=%d,num2=%d\n", (*ggs).geom, (*ggs).m, (*ggs).num);
			if ((*ggs).m > (*ggs).num){
				modelMax = (*ggs).m;
			}
			else{
				modelMax = (*ggs).num;
			}
			if (QString::fromStdString((*ggs).geom).left(5) == QObject::tr("Plane")){ continue; }//
			int i = settingNo.value(QObject::tr("Count/count")).toInt();
			settingNo.setValue(QObject::tr("Count/count"), ++i);
			settingNo.setValue(QObject::tr("%1/geoNo").arg(i), modelMax);
			settingNo.setValue(QObject::tr("%1/ModelName").arg(i), QString::fromStdString((*ggs).geom));
		}
		geopec.clear();
	}
	/////////end2018314/////

	
	SFile.close();
	return 1;
}


//获取最外层几何体名称
static string outObjStrName(){

	App::Document* outObj = App::GetApplication().getActiveDocument();
	std::vector<App::DocumentObject*> sel= outObj->getObjectsOfType(App::DocumentObject::getClassTypeId());
	// 需要包含的头文件 #include <BRepClass3d_SolidClassifier.hxx>

	// 判断最外层几何体，一个一个几何体比较，比较所有顶点是否在该几何体上或内部.  
	// ???  如果不存在一个最外层的几何体时，如有圆柱存在有时判断会出错，有待改进.  
	// 但是如果存在最外层的几何体,可以找出最外层几何体
	// 利用的点是否在几何体上判断， 没找到几何体之间关系的判断方法。  
	std::string outObjStrName = "NULL";
	//遍历所有几何体
	int partnum=0,partflag=0;;//记录是否是只有一个体。
	for (std::vector<App::DocumentObject*>::iterator it = sel.begin(); it != sel.end(); ++it)
	{
		
		std::string Grou = (*it)->getNameInDocument();
		QString Grous = QString::fromStdString(Grou);
		if (Grous.left(5) == QObject::tr("Group")){
			//Base::Console().Message("Group==Group\n");
			continue;
		}
		if (Grous.left(4) == QObject::tr("Line")){
					continue;
		}
		/* if (Grous.left(5) == QObject::tr("Plane")){
					continue;
		} */
		//取得几何体名
		std::string objName = (*it)->getNameInDocument();
		//Base::Console().Message("getNameInDocument Obj Name = %s \n", objName.c_str());

		{// 判断是否不可见的，不可见的跳过
			Gui::ViewProvider* pcProv = Gui::Application::Instance->getViewProvider(*it);
			bool visibility = true;
			if (pcProv) {
				visibility = ((Gui::ViewProviderDocumentObject*)pcProv)->Visibility.getValue();
			}
			if (visibility == false){// 不可见的跳过
				continue;
			}
		}
       partflag++;
		App::DocumentObject *Obj = (*it);
		const TopoDS_Shape& tShape = static_cast<Part::Feature *>(Obj)->Shape.getValue();
		BRepClass3d_SolidClassifier solidClassifier(tShape);
		// 遍历素有几何体的顶点与上面的几何体比较，如果所有几何体的顶点都在该结合体内(上),就作为最外层结合体   
		bool outObj = true;
		for (std::vector<App::DocumentObject*>::iterator it1 = sel.begin(); it1 != sel.end(); ++it1)
		{
			//Base::Console().Message("ObjName= %s \n", (*it1)->getNameInDocument());
			//outObj = true;////如果只有一个体没有最外层2018.2.7
			if (it == it1){// 比较的几何体本身，跳过  
				//outObj = false;//如果只有一个体没有最外层2018.2.7
				if(partflag==1){
					partnum++;
			    }
				continue;
			}
			std::string Group = (*it1)->getNameInDocument();
			QString Groups = QString::fromStdString(Group);
			if (Groups.left(5) == QObject::tr("Group")){
				//Base::Console().Message("Group==Group\n");
				continue;
			}
			if (Groups.left(4) == QObject::tr("Line")){
					continue;
			}
			/* if (Groups.left(5) == QObject::tr("Plane")){
					continue;
			} */


			{// 判断是否不可见的，不可见的跳过
				Gui::ViewProvider* pcProv = Gui::Application::Instance->getViewProvider(*it1);
				bool visibility = true;
				if (pcProv) {
					visibility = ((Gui::ViewProviderDocumentObject*)pcProv)->Visibility.getValue();
				}
				if (visibility == false){// 不可见的跳过
					continue;
				}
			}
			if(partflag==1){
               partnum++;
			}
			App::DocumentObject *Obj1 = (*it1);
			const TopoDS_Shape& tShape1 = static_cast<Part::Feature *>(Obj1)->Shape.getValue();

			TopExp_Explorer expVertex;
			// 遍历几何体的顶点
			for (expVertex.Init(tShape1, TopAbs_VERTEX); expVertex.More(); expVertex.Next())
			{
				TopoDS_Vertex  vtx = TopoDS::Vertex(expVertex.Current());
				gp_Pnt pt = BRep_Tool::Pnt(vtx);
				solidClassifier.Perform(pt, 1.0E-3);
				Standard_Boolean test = (solidClassifier.State() == TopAbs_OUT);
				if (test == true){//顶点在几何体外
					outObj = false;
					break;
				}
			}

			// 如果有顶点在几何体外，跳出该几何体，检查下一个几何体
			if (outObj == false){
				break;
			}
		}
		// 所有顶点都在改几何体内，作为最外层结合体
		if (outObj == true){
			outObjStrName = objName;
			break;
		}
	}


	//Base::Console().Message(" **  Out Obj Name = %s %d\n", outObjStrName.c_str(),partnum);
	QString outerPart = QString::fromStdString(outObjStrName);//
	if (outObjStrName == "NULL"||partnum==1){
		return "NULL";
	}
	else{
		return outObjStrName;
	}
}


//判断pec
static int DecidePec(App::DocumentObject *faceObj)
{
	QSettings settings(QObject::tr("../ini/Boundaries.ini"), QSettings::IniFormat);
	const TopoDS_Shape& tShape = static_cast<Part::Feature *>(faceObj)->Shape.getValue();
	TopExp_Explorer expFace;
	//Base::Console().Message("******2018****52511**********************\n");
	int num = 0, i;
	std::vector<std::string> PerfE;
	std::vector<std::string> PerfH;
	for (expFace.Init(tShape, TopAbs_FACE); expFace.More(); expFace.Next())
	{
		num++;

	}
	//Base::Console().Message("getNameInDocument %s\n", faceObj->getNameInDocument());
	//Base::Console().Message("num %d\n", num);
	//Base::Console().Message("******2018****5251**********************\n");
	int sum = settings.value(QObject::tr("PRIORITY/count")).toInt();
	for (i = 1; i <= sum; i++)
	{
		QString str = settings.value(QObject::tr("PRIORITY/%1").arg(i)).toString();
		QString strs = str.left(5);
		//Base::Console().Message("strs== %s\n", strs.toStdString());
		if (strs == QObject::tr("PerfE")){
			//Base::Console().Message("perfenum==pko\n");
			PerfE.push_back(str.toStdString());
			//Base::Console().Message("perfenum==ll\n");
		}
		if (strs == QObject::tr("PerfH"))
		{
			PerfH.push_back(str.toStdString());

		}
	}
	//
	//Base::Console().Message("******2018****5252**********************\n");
	std::vector<std::string>::iterator ite;
	std::vector<std::string>::iterator ith;
	int snum = 0;
	QStringList facemap;
	for (ite = PerfE.begin(); ite != PerfE.end(); ++ite){
		int surfacesnum = settings.value(QObject::tr("%1/SurfacesNum").arg(QString::fromStdString(*ite))).toInt();
		int f = 0;
		for (f = 1; f <= surfacesnum; f++){
			QString face = settings.value(QObject::tr("%1/Surface%2").arg(QString::fromStdString(*ite)).arg(f)).toString();
			QString Box = face.section(QObject::tr(","), 2, 2);
			QString Bfaces = face.section(QObject::tr(","), 3, 3);
			//Base::Console().Message("Boxh== %s\n", Box.toStdString());
			if (Box == QString(QLatin1String((faceObj)->getNameInDocument())))
			{
				if (false == facemap.contains(Bfaces)){
					facemap<<Bfaces;
				}
				//snum++;
			}
			
		}
		if (num == snum&&num != 0){
			//Geopec= QString::fromStdString(*ite);
			geopec.push_back(*ite);
		}
	}
	//Base::Console().Message("******2018****5253**********************\n");
	for (ith = PerfH.begin(); ith != PerfH.end(); ++ith){
		int surfaceshnum = settings.value(QObject::tr("%1/SurfacesNum").arg(QString::fromStdString(*ith))).toInt();
		int f = 0;
		for (f = 1; f <= surfaceshnum; f++){
			QString face = settings.value(QObject::tr("%1/Surface%2").arg(QString::fromStdString(*ith)).arg(f)).toString();
			QString Box = face.section(QObject::tr(","), 2, 2);
			//Base::Console().Message("Boxh== %s\n", Box.toStdString());
				QString Bfaces = face.section(QObject::tr(","), 3, 3);
			if (Box == QString(QLatin1String((faceObj)->getNameInDocument())))
			{
				//snum++;
				if (false == facemap.contains(Bfaces)){
					facemap<<Bfaces;
				}
			}
			
		}
		if (num == snum&&num != 0){
			//Geopec = QString::fromStdString(*ith);
			geopec.push_back(*ith);
		}
	}
	//Base::Console().Message("******2018****5254**********************\n");
	//Base::Console().Message("getNameInDocument %s\n", faceObj->getNameInDocument());
	snum=facemap.size();
	//Base::Console().Message("num %d\n", num);
	//Base::Console().Message("snum %d\n", snum);
	if (num == snum&&num != 0){
		return 1;
	}
	else{
		return 0;
	}


}

//判断Rad
static int DecideRad(App::DocumentObject *faceObj)
{
	QSettings settings(QObject::tr("../ini/Boundaries.ini"), QSettings::IniFormat);
	const TopoDS_Shape& tShape = static_cast<Part::Feature *>(faceObj)->Shape.getValue();
	TopExp_Explorer expFace;
	int num = 0, i;
	std::vector<std::string> Rad;
	//std::vector<std::string> PerfH;
	for (expFace.Init(tShape, TopAbs_FACE); expFace.More(); expFace.Next())
	{
		num++;

	}

	int sum = settings.value(QObject::tr("PRIORITY/count")).toInt();
	for (i = 1; i <= sum; i++)
	{
		QString str = settings.value(QObject::tr("PRIORITY/%1").arg(i)).toString();
		QString strs = str.left(3);
		//Base::Console().Message("strs== %s\n", strs.toStdString());
		if (strs == QObject::tr("Rad")){
		
			Rad.push_back(str.toStdString());
			
		}
		
	}
	//
	std::vector<std::string>::iterator ite;
	QStringList facemap;
	int snum = 0;
	for (ite = Rad.begin(); ite != Rad.end(); ++ite){
		int surfacesnum = settings.value(QObject::tr("%1/SurfacesNum").arg(QString::fromStdString(*ite))).toInt();
		int f = 0;
		for (f = 1; f <= surfacesnum; f++){
			QString face = settings.value(QObject::tr("%1/Surface%2").arg(QString::fromStdString(*ite)).arg(f)).toString();
			QString Box = face.section(QObject::tr(","), 2, 2);
			QString Bfaces = face.section(QObject::tr(","), 3, 3);
			if (Box == QString(QLatin1String((faceObj)->getNameInDocument())))
			{
				//snum++;
				if (false == facemap.contains(Bfaces)){
					facemap<<Bfaces;
				}
			}

		}
		/*if (num == snum&&num != 0){
			geopec.push_back(*ite);
		}*/
	}
	snum=facemap.size();
	//Base::Console().Message("num %d\n", num);
	//Base::Console().Message("snum %d\n", snum);
	if (num == snum&&num != 0){
		return 1;
	}
	else{
		return 0;
	}


}

//判断设置了激励
static int DecideExci(App::DocumentObject *faceObj)
{
	QSettings settings(QObject::tr("../ini/Excitation.ini"), QSettings::IniFormat);
	const TopoDS_Shape& tShape = static_cast<Part::Feature *>(faceObj)->Shape.getValue();
	TopExp_Explorer expFace;
	int num = 0, i;
	std::vector<std::string> Wave;
	std::vector<std::string> Lump;
	for (expFace.Init(tShape, TopAbs_FACE); expFace.More(); expFace.Next())
	{
		num++;

	}

	int sum = settings.value(QObject::tr("ORDER/Count")).toInt();
	for (i = 1; i <= sum; i++)
	{
		QString str = settings.value(QObject::tr("ORDER/%1").arg(i)).toString();
		QString strs = str.left(8);
		
		if (strs == QObject::tr("WavePort")){
			
			Wave.push_back(str.toStdString());
			
		}
		if (strs == QObject::tr("LumpedPo"))
		{
			Lump.push_back(str.toStdString());

		}
	}
	//
	std::vector<std::string>::iterator ite;
	std::vector<std::string>::iterator ith;
	int snum = 0;
	for (ite = Wave.begin(); ite != Wave.end(); ++ite){
		int surfacesnum = settings.value(QObject::tr("%1/SurfacesNum").arg(QString::fromStdString(*ite))).toInt();
		int f = 0;
		for (f = 1; f <= surfacesnum; f++){
			QString face = settings.value(QObject::tr("%1/Surface%2").arg(QString::fromStdString(*ite)).arg(f)).toString();
			QString Box = face.section(QObject::tr(","), 2, 2);
			if (Box == QString(QLatin1String((faceObj)->getNameInDocument())))
			{
				snum++;
			}

		}
		if (num == snum&&num != 0){
			//Geopec= QString::fromStdString(*ite);
			geopec.push_back(*ite);
		}
	}
	for (ith = Lump.begin(); ith != Lump.end(); ++ith){
		int surfaceshnum = settings.value(QObject::tr("%1/SurfacesNum").arg(QString::fromStdString(*ith))).toInt();
		int f = 0;
		for (f = 1; f <= surfaceshnum; f++){
			QString face = settings.value(QObject::tr("%1/Surface%2").arg(QString::fromStdString(*ith)).arg(f)).toString();
			QString Box = face.section(QObject::tr(","), 2, 2);
			//Base::Console().Message("Boxh== %s\n", Box.toStdString());
			if (Box == QString(QLatin1String((faceObj)->getNameInDocument())))
			{
				snum++;
			}

		}
	}
	if (snum&&num != 0){
		return 1;
	}
	else{
		return 0;
	}


}


//使用geo更新surface
static int geosurface(int N)
{
	//Base::Console().Message("---------------...------------");
	QString SifFile = QString::fromLatin1("../ini/box.geo_unrolled");
	QFile SFile(SifFile);
	if (!SFile.open(QIODevice::ReadOnly))
	{
		QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong"),
			QObject::tr(" read the box.geo_unrolled file failed!"));
		return N;
	}
	QTextStream SIFstreamfile(&SFile);
	QString SaveFile = QString::fromLatin1("../ini/surface1.txt");
	QFile CFile(SaveFile);
	if (!CFile.open(QIODevice::ReadWrite))
	{
		QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong"),
			QObject::tr(" read the box.txt file failed!"));
		return N;
	}
	QTextStream Savestreamfile(&CFile);
	QString buf;
	typedef list<CommonFunction::facemodel> facemodel;
	facemodel facemodels;
	CommonFunction::facemodel structmodel;

	typedef list<CommonFunction::facesurface> facesurface;
	facesurface facesurfaces;
	CommonFunction::facesurface structsurface;

	typedef list<CommonFunction::facelineoop> facelineoop;
	facelineoop facelineoops;
	CommonFunction::facelineoop structlineoop;

	typedef list<CommonFunction::faceline> faceline;
	faceline facelines;
	CommonFunction::faceline structline;

	typedef list<CommonFunction::facepoint> facepoint;
	facepoint facepoints;
	CommonFunction::facepoint structpoint;

	QMap<QString, QString> pointmap;//2018-6-19
	QMap<int, CommonFunction::faceline> structlinemap;
	QMap<int, CommonFunction::facelineoop> facelineoopmap;
	QMap<QString, int> surfacetxtnummap;//2018-7-4
	int surfacetxtnum=1;;
	int fnum=0;
	//QSettings settingm(QObject::tr("../ini/MeshMap.ini"), QSettings::IniFormat);
	while (!SIFstreamfile.atEnd())//while start
	{
		buf = SIFstreamfile.readLine();

		if (buf.right(5) == QObject::tr("newp;"))
		{
			//Savestreamfile << "newp=" << buf.left(5)<<"\n";
		}

		if (buf.left(6) == QObject::tr("Volume"))//获取体
		{
			
			QRegExp sep(QObject::tr("[{}]"));
			QString beta = buf.section(sep, 1, 1);
			
			structmodel.type = "Volum";
			structmodel.number = beta.toStdString();
			facemodels.push_back(structmodel);
		}

		if (buf.left(12) == QObject::tr("Surface Loop"))//获取构成体的面
		{
			QRegExp seps(QObject::tr("[()]"));
			QString beta = buf.section(seps, 1, 1);
			QRegExp sep(QObject::tr("[{}]"));
			QString b = buf.section(sep, 1, 1);
			structsurface.type = "Surface";
			structsurface.loop = b.toStdString();
			structsurface.number = beta.toStdString();
			facesurfaces.push_back(structsurface);
		}

		if (buf.left(9) == QObject::tr("Line Loop"))//获取构成面的边
		{
			QRegExp seps(QObject::tr("[()]"));
			QString beta = buf.section(seps, 1, 1);
			QRegExp sep(QObject::tr("[{}]"));
			QString b = buf.section(sep, 1, 1);
			structlineoop.type = "Lineloop";
			structlineoop.loop = b.toStdString();
			structlineoop.number = beta.toStdString();
			facelineoops.push_back(structlineoop);
			facelineoopmap.insert(beta.toInt(), structlineoop);
			surfacetxtnummap.insert(beta,surfacetxtnum++);//获取面的编号信息
			//settingm.setValue(QObject::tr("Meshnum/%1").arg(beta), surfacetxtnum-1);
		}

		if (buf.left(5) == QObject::tr("Line("))//获取构成边的点
		{
			QRegExp seps(QObject::tr("[()]"));
			QString beta = buf.section(seps, 1, 1);
			QRegExp sep(QObject::tr("[{}]"));
			QString b = buf.section(sep, 1, 1);
			structline.type = "Line";
			structline.loop = b.toStdString();
			structline.number = beta.toStdString();
			facelines.push_back(structline);
			structlinemap.insert(beta.toInt(),structline);
		}

		if (buf.left(6) == QObject::tr("Spline"))//获取构成边的点
		{
			QRegExp seps(QObject::tr("[()]"));
			QString beta = buf.section(seps, 1, 1);
			QRegExp sep(QObject::tr("[{}]"));
			QString b = buf.section(sep, 1, 1);
			structline.type = "Spline";
			structline.loop = b.toStdString();
			structline.number = beta.toStdString();
			facelines.push_back(structline);
			structlinemap.insert(beta.toInt(), structline);
		}

		if (buf.left(6) == QObject::tr("Circle"))//获取构成边的点的信息
		{
			QRegExp seps(QObject::tr("[()]"));
			QString beta = buf.section(seps, 1, 1);
			QRegExp sep(QObject::tr("[{}]"));
			QString b = buf.section(sep, 1, 1);
			structline.type = "Circle";
			structline.loop = b.toStdString();
			structline.number = beta.toStdString();
			facelines.push_back(structline);
			structlinemap.insert(beta.toInt(), structline);
		}
		/* if (buf.left(14) == QObject::tr("Plane Surface(")||buf.left(8) == QObject::tr("Surface("))//获取面的编号信息
		{//在line loop里面获取
			QRegExp seps(QObject::tr("[()]"));
			QString beta = buf.section(seps, 1, 1);
			surfacetxtnummap.insert(beta,surfacetxtnum++);
			
		} */
		if (buf.left(5) == QObject::tr("Point"))//获取点的信息
		{
			QRegExp seps(QObject::tr("[()]"));
			QString beta = buf.section(seps, 1, 1);
			QRegExp sep(QObject::tr("[{}]"));
			QString b = buf.section(sep, 1, 1);
			QString x = b.section(QObject::tr(","), 0, 0);
			QString y = b.section(QObject::tr(","), 1, 1);
			QString z = b.section(QObject::tr(","), 2, 2);
			QString strX, strY, strZ;
			strX.sprintf("%11.3e", x.toDouble());
			strY.sprintf("%11.3e", y.toDouble());
			strZ.sprintf("%11.3e", z.toDouble());
			QString fipnt = QObject::tr("%1,%2,%3").arg(strX).arg(strY).arg(strZ);
		/*	Savestreamfile << "Point=" << beta << "*****Points=" << b << "\n";
			Savestreamfile << fipnt << "\n";*/

			structpoint.type = "Point";
			structpoint.loop = fipnt.toStdString();
			structpoint.number = beta.toStdString();
			structpoint.x = x.toStdString();
			structpoint.y = y.toStdString();
			structpoint.z = z.toStdString();
			facepoints.push_back(structpoint);
			QString pointxyz= QObject::tr("%1,%2,%3").arg(x).arg(y).arg(z);
			pointmap.insert(beta,pointxyz);
		}
		if(buf.left(9) == QObject::tr("Surface {")){
			fnum++;
		}

		QCoreApplication::processEvents();
	}//while end

	typedef list<CommonFunction::facedir> facedir;
	facedir facedirs;
	CommonFunction::facedir structdir;
	typedef list<CommonFunction::fnumvector> fnumvector;
	fnumvector fnumvectors;
	CommonFunction::fnumvector structfnumvector;
	typedef list<CommonFunction::orientation> orientation;
	orientation orientations;
	CommonFunction::orientation structorientation;
	//vector<int> fnumvector;//记录凸面
	list<CommonFunction::facemodel>::iterator m;
	list<CommonFunction::facesurface>::iterator s;
	list<CommonFunction::facelineoop>::iterator lp;
	int mo=facemodels.size();
	int shapeNo = 1;
	//int surfaceNo = 1;
	QSettings settingmodel(QObject::tr("../ini/ModelNo.ini"), QSettings::IniFormat);
	//Savestreamfile << "Volum=sum="<<mo << "\n";
	if (mo||fnum){
		Savestreamfile << QObject::tr("ShapeCount %1\n").arg(mo+fnum);
	}
	for (m = facemodels.begin(); m != facemodels.end(); ++m){
		string mnum = (*m).number;
		//Savestreamfile << QObject::tr("ShapeNo. %1  ShapeType(%2)\n").arg(QString::fromStdString(mnum)).arg(QString::fromStdString((*m).type));
		Savestreamfile << QObject::tr("ShapeNo. %1  ShapeType(%2)\n").arg(shapeNo++).arg(QString::fromStdString((*m).type));
		/////////
		QString N=settingmodel.value(QObject::tr("Model/geoNo")).toString();
		int Count= settingmodel.value(QObject::tr("Count/count")).toInt();
		if (N == QString::fromStdString(mnum)){
			settingmodel.setValue(QObject::tr("Model/MeshNo"), shapeNo-1);
		}
		for (int num = 0; num < Count; num++){
			QString cou = settingmodel.value(QObject::tr("%1/geoNo").arg(num+1)).toString();
			if (cou== QString::fromStdString(mnum)){
				settingmodel.setValue(QObject::tr("%1/MeshNo").arg(num+1), shapeNo - 1);
			}
		}
		//////////////
		vector<string> dirpointve;//$
		for (s = facesurfaces.begin(); s != facesurfaces.end(); ++s){
			int i = 0;
			QString  surface = QObject::tr("");
			if (mnum == (*s).number){
				QString bufm = QString::fromStdString((*s).loop);
				QStringList flist = bufm.split(QObject::tr(","));
				int fcount = flist.size();
				Savestreamfile << QObject::tr("FaceCount %1\n").arg(fcount);
				while (1){
					surface = bufm.section(QObject::tr(","), i, i).simplified();
					i++;
					if (surface!=QObject::tr("")){
						QMap<QString, int>::Iterator ittxtnum;
						 ittxtnum = surfacetxtnummap.find(surface);
						//Savestreamfile << QObject::tr("FaceGlobalNo. %1  FaceLocalNo. %2\n").arg(surfaceNo++).arg(i);
						 Savestreamfile << QObject::tr("FaceGlobalNo. %1  FaceLocalNo. %2\n").arg(ittxtnum.value()).arg(i);
						//structdir.fnum=surfaceNo - 1;//$
						structdir.fnum = abs(surface.toInt());
						QMap<int, CommonFunction::facelineoop>::Iterator pit;
						pit = facelineoopmap.find(abs(surface.toInt()));
							int j = 0;
							QString lineoop = QObject::tr("");
							QString buflp = QString::fromStdString(pit->loop);
								QStringList llist = buflp.split(QObject::tr(","));
								structdir.edgeloop = pit->loop;//$
								int lcount = llist.size();
								Savestreamfile << QObject::tr("FaceType(%1) EdgeCount %2\n").arg(QObject::tr("Plane")).arg(lcount);
								vector<string> Xvector,Yvector,Zvector,Pvector;
								while (1){
									lineoop = buflp.section(QObject::tr(","), j, j).simplified();
									j++;
									if (lineoop!=QObject::tr("")){
											int k = 0;
											QString line =QObject::tr("");
											int abss = 0;
											int flagCir = 0;
											int flagSpline = 0;
											if (lineoop.toDouble() < 0){ abss = 1; k = 1; }//记录正负修改顺序
											QMap<int, CommonFunction::faceline>::Iterator lit;
											lit=structlinemap.find(abs(lineoop.toInt()));
												if (lit->type == "Circle"){ flagCir = 1; }//判断是circle
												if (lit->type == "Spline"){ flagSpline = 1; }//判断是Spline
												QString bufl = QString::fromStdString(lit->loop);
												QStringList plist = bufl.split(QObject::tr(","));
												int Pcount = plist.size();
												QString Edgetype = QObject::tr("EdgeNo.%1 EdgeType(%2) PointCount(%3)").arg(j).arg(QString::fromStdString(lit->type)).arg(Pcount);
												Pvector.push_back(Edgetype.toStdString());
												if (flagCir&&abss){ k = 2; }//circles输出三个点
												if (flagSpline&&abss){ k = 20; }//Spline输出三个点
												QString first, second, third;
												int flag = 0, thr = 0;
												while (1){
													if (abss){
														if (flagCir){
															line = bufl.section(QObject::tr(","), k, k).simplified();
															//Savestreamfile << "liness1=" << line << "\n";//
															if (k == 0){ k = 4; }//避免多次输出
															k--;
														}
														else if (flagSpline){
															line = bufl.section(QObject::tr(","), k, k).simplified();
															if (k == 0){ k = 22; }//避免多次输出
															k--;
														}
														else{
															line = bufl.section(QObject::tr(","), k, k).simplified();
													
															if (k == 0){ k = 3; }//避免多次输出
															k--;
														}
													}
													else{
														line = bufl.section(QObject::tr(","), k, k).simplified();
													
														k++;
													}
													if (line!=QObject::tr("")){
														
													QMap<QString,QString>::Iterator it;
														it=pointmap.find(line);
														QString b = it.value();
														QString x = b.section(QObject::tr(","), 0, 0);
														QString y = b.section(QObject::tr(","), 1, 1);
														QString z = b.section(QObject::tr(","), 2, 2);
														QString strX, strY, strZ;
														strX.sprintf("%11.3e", x.toDouble());
														strY.sprintf("%11.3e", y.toDouble());
														strZ.sprintf("%11.3e", z.toDouble());
														QString fipnt = QObject::tr("%1,%2,%3").arg(strX).arg(strY).arg(strZ);
																dirpointve.push_back(line.toStdString());//$
																Xvector.push_back(x.toStdString());
																Yvector.push_back(y.toStdString());
																Zvector.push_back(z.toStdString());
																
																if (flag == 0){
																	first = fipnt;
																	flag = 1;
																}
																else if (flag == 1){
																	second = fipnt;
																	if (!flagCir){
																		QString plist = QObject::tr("%1, %2").arg(first).arg(second);
																		Pvector.push_back(plist.toStdString());
																		flag = 0;
																	}
																	else{
																		thr = 3;
																		flag = 3;
																	}

																}
																else if(thr){
																	third = fipnt;
																	QString plist = QObject::tr("%1, %2, %3").arg(first).arg(second).arg(third);
																	Pvector.push_back(plist.toStdString());
																	flag = 0;
																	thr = 0;
																	
																}
																if (Pcount == 21&&k==21){
																	QString plist = QObject::tr("%1").arg(fipnt);
																	Pvector.push_back(plist.toStdString());
																}
														
													}
													else{
														break;
													}
												}//while
										
									}
									else{
										//面的信息处理
										vector<string>::iterator x,y,z,p;
										double strX = 0,strY=0,strZ=0;
										int count=0;
										double x1 = 0, x2 = 0, y1 = 0, y2 = 0, z1 = 0, z2 = 0, x3 = 0, x4 = 0, y3 = 0, y4 = 0, z3 = 0, z4 = 0;
										for (x = Xvector.begin(), y = Yvector.begin(), z = Zvector.begin(); x!= Xvector.end(), y!=Yvector.end(), z!=Zvector.end(); ++x,++y,++z){
											strX = strX + QString::fromStdString(*x).toDouble();
											strY = strY + QString::fromStdString(*y).toDouble();
											strZ = strZ + QString::fromStdString(*z).toDouble();
											count++;
											if (count == 1){ 
												x1 = QString::fromStdString(*x).toDouble();
												y1 = QString::fromStdString(*y).toDouble();
												z1 = QString::fromStdString(*z).toDouble();
											}
											if (count == 2){
												x2 = QString::fromStdString(*x).toDouble();
												y2 = QString::fromStdString(*y).toDouble();
												z2 = QString::fromStdString(*z).toDouble();
												structdir.p2x = x2;
												structdir.p2y = y2;
												structdir.p2z = z2;
											}
											if (count == 3){
												x3 = QString::fromStdString(*x).toDouble();
												y3 = QString::fromStdString(*y).toDouble();
												z3 = QString::fromStdString(*z).toDouble();
											}
											if (count == 4){
												x4 = QString::fromStdString(*x).toDouble();
												y4 = QString::fromStdString(*y).toDouble();
												z4 = QString::fromStdString(*z).toDouble();
											}

										}
										QString clX, clY, clZ;
										clX.sprintf("%11.3e", strX / count);
										clY.sprintf("%11.3e", strY / count);
										clZ.sprintf("%11.3e", strZ / count);
										Savestreamfile << QObject::tr("FaceCenter(%1,%2,%3) ").arg(clX).arg(clY).arg(clZ);
										//Base::Console().Message("--------**%f,,,,%d----\n", strX, count);
										double Vx1 = (x2 - x1);
										double Vy1 = (y2 - y1);
										double Vz1 = (z2 - z1);
										double Vx2 = (x4 - x1);
										double Vy2 = (y4 - y1);
										double Vz2 = (z4 - z1);
										double Vx = Vy1*Vz2 - Vz1*Vy2;
										double Vy = Vz1*Vx2 - Vx1*Vz2;
										double Vz = Vx1*Vy2 - Vy1*Vx2;
										double sum = Vx + Vy + Vz;
										QString DX, DY, DZ;
										DX.sprintf("%11.3e", Vx / fabs(sum));
										DY.sprintf("%11.3e", Vy / fabs(sum));
										DZ.sprintf("%11.3e", Vz / fabs(sum));
										if (_isnan(DX.toDouble())){
											DX.sprintf("%11.3e", 0);
											DY.sprintf("%11.3e", 0);
											DZ.sprintf("%11.3e", 0);
											Savestreamfile << QObject::tr("FaceDir(%1,%2,%3)\n").arg(DX).arg(DY).arg(DZ);
											structdir.dir = QObject::tr("%1,%2,%3").arg(DX).arg(DY).arg(DZ).toStdString();//$
										}
										else{
												Savestreamfile << QObject::tr("FaceDir(%1,%2,%3)\n").arg(DX).arg(DY).arg(DZ);
												structdir.dir = QObject::tr("%1,%2,%3").arg(DX).arg(DY).arg(DZ).toStdString();//$
										}
										for (p = Pvector.begin(); p != Pvector.end(); ++p){
											Savestreamfile << QString::fromStdString(*p)<<"\n";
										}
										Xvector.clear();
										Yvector.clear();
										Zvector.clear();
										Pvector.clear();
										Savestreamfile << "\n";
										
										break;
									}
									QCoreApplication::processEvents();
								}//while							
							
						//一个面的完成
						facedirs.push_back(structdir);
					}
					else{
						
						break;
					}
					QCoreApplication::processEvents();
				}//while
			}//if

		}//for s
		//2018-4-17一个体完成
		//////////////////更新facedir/////////////
		//去重
		//Base::Console().Message("--------*********----\n");
		sort(dirpointve.begin(), dirpointve.end());
		dirpointve.erase(unique(dirpointve.begin(), dirpointve.end()), dirpointve.end());
		vector<int> numvect;//记录凸面编号
		list<CommonFunction::facedir>::iterator di1;
		list<CommonFunction::facedir>::iterator di2;
		for (di1 = facedirs.begin(); di1 != facedirs.end(); ++di1){
			QString dir = QString::fromStdString((*di1).dir);
			double dirx = dir.section(QObject::tr(","), 0, 0).simplified().toDouble();
			double diry = dir.section(QObject::tr(","), 1, 1).simplified().toDouble();
			double dirz = dir.section(QObject::tr(","), 2, 2).simplified().toDouble();
			//获取面上的一个点与平面外一点形成矢量vi
			double x = (*di1).p2x;
			 double y = (*di1).p2y;
			double z = (*di1).p2z;
			int i = 0, j = 0, k = 0;//记录点乘结果
			QString edgeloop1 = QString::fromStdString((*di1).edgeloop);
			QStringList plist1 = edgeloop1.split(QObject::tr(","));//构成面的边
			//Base::Console().Message("edgeloop=%s------------\n", (*di1).edgeloop);
			//获取这个面的点
			QStringList pointplist;
			QMap<int, CommonFunction::faceline>::Iterator lit;
			for (int ss = 0; ss < plist1.size(); ss++){
				        lit = structlinemap.find(abs(plist1[ss].toInt()));
						QString bufl = QString::fromStdString(lit->loop);
						pointplist = pointplist+ bufl.split(QObject::tr(","));
                          
			}
			pointplist.removeDuplicates();//去除重复的点
			for (di2 = facedirs.begin(); di2 != facedirs.end(); ++di2){
				if ((*di1).fnum == (*di2).fnum){ continue; }
				vector<string>::iterator v;
				for (v = dirpointve.begin(); v != dirpointve.end(); ++v){//一个体的所有点，
					QString poi=QString::fromStdString(*v);
					int flag = 1;
					for (int ii = 0; ii < pointplist.size(); ii++){
						if (poi == pointplist[ii].simplified()){
						//	Base::Console().Message("plist=%d=%s-\n", ii, pointplist[ii].simplified().toStdString());
							flag = 0;
							break;//poi是这个面的点了
						}
					}
					if (flag){//这个点与面的点不相同进行点乘
						double nx=0,ny=0,nz=0;
						QMap<QString, QString>::Iterator it;
						it = pointmap.find(poi);
						QString b = it.value();
						QString qx = b.section(QObject::tr(","), 0, 0);
						QString qy = b.section(QObject::tr(","), 1, 1);
						QString qz = b.section(QObject::tr(","), 2, 2);
						nx = qx.toDouble();
						ny = qy.toDouble();
						nz = qz.toDouble();
						double vx = nx - x;
						double vy = ny - y;
						double vz = nz - z;
						k++;
						double sum = dirx*vx + diry*vy + dirz*vz;
						if (sum < 0){ i++; }
						if (sum>0){ j++; }
					}//if flag
				}//for 
				

			}//for di2
			//Base::Console().Message("-----------==k=%d==i=%d------%d------\n",k,i,j);
			if (k == i){//找到凸面
				structfnumvector.num = (*di1).fnum;
				structfnumvector.edge = (*di1).edgeloop;
				fnumvectors.push_back(structfnumvector);
				numvect.push_back((*di1).fnum);
				//Base::Console().Message("-----------fum===%d------------\n", (*di1).fnum);
			}
		  QCoreApplication::processEvents();
		}//for di1
		/////////////////更新facesdir完成///////////
		//////////查找与凸面相同的边////////
		list<CommonFunction::facedir>::iterator f1;
		list<CommonFunction::facedir>::iterator f2;
		vector<int>::iterator vflag;
		list<CommonFunction::fnumvector>::iterator vf;
		for (f1 = facedirs.begin(); f1 != facedirs.end(); ++f1){
			QString dir = QString::fromStdString((*f1).dir);
			double dirx = dir.section(QObject::tr(","), 0, 0).simplified().toDouble();
			double diry = dir.section(QObject::tr(","), 1, 1).simplified().toDouble();
			double dirz = dir.section(QObject::tr(","), 2, 2).simplified().toDouble();
			QString edgeloop1 = QString::fromStdString((*f1).edgeloop);
			QStringList plist1 = edgeloop1.split(QObject::tr(","));

			vflag = find(numvect.begin(), numvect.end(), (*f1).fnum);
			if (vflag != numvect.end()){//找到它是凸面
				
				structorientation.num = (*f1).fnum;
				structorientation.dir = QObject::tr("%1,%2,%3").arg(dirx).arg(diry).arg(dirz).toStdString();
				orientations.push_back(structorientation);
			}//if
			else{
				int flagori = 0;
				for (vf = fnumvectors.begin(); vf != fnumvectors.end(); ++vf){
					QString edgevf = QString::fromStdString((*vf).edge);
					QStringList plistvf = edgevf.split(QObject::tr(","));
					int flagedge = 0;
					for (int i = 0; i < plist1.size(); i++){
						for (int j = 0; j < plistvf.size(); j++){
							if (fabs(plist1[i].toDouble()) == fabs(plistvf[j].toDouble())){
								flagedge = 1;
								if (plist1[i].toDouble() == plistvf[j].toDouble()){//边连接顺序一致
									flagori = 2;
								}//if
								else{
									flagori = 1;
								}//else
							}//if
						}//for j
					}//for i
					if (flagedge){ break; }
				}//vf

				if (flagori == 1){//不需要改变
					structorientation.num = (*f1).fnum;
					structorientation.dir = QObject::tr("%1,%2,%3").arg(dirx).arg(diry).arg(dirz).toStdString();
					orientations.push_back(structorientation);
				}
				if (flagori == 2){//需要改变方向
					structorientation.num = (*f1).fnum;
					structorientation.dir = QObject::tr("%1,%2,%3").arg(0-dirx).arg(0-diry).arg(0-dirz).toStdString();
					orientations.push_back(structorientation);
				}
			    
			}//else
		
		QCoreApplication::processEvents();

		}//for f1
		dirpointve.clear();
		facedirs.clear();
		fnumvectors.clear();
		fnumvectors.clear();
		///////查找完成///////
	}//for m

	SFile.close();
	CFile.close();
	list<CommonFunction::orientation>::iterator ori;
	for (ori = orientations.begin(); ori != orientations.end(); ++ori){
		//Base::Console().Message("num=%d------dir==%s------\n", (*ori).num,(*ori).dir);
	}
	/////更新surfaces/只更新体的///
	QString ReadFile = QString::fromLatin1("../ini/surface1.txt");
	QFile RFile(ReadFile);
	if (!RFile.open(QIODevice::ReadOnly))
	{
		QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong"),
			QObject::tr(" read the surface.txt file failed!"));
		return N;
	}
	QTextStream Readsurfacefile(&RFile);
	QString SavFile = QString::fromLatin1("../ini/surface.txt");
	QFile SvFile(SavFile);
	if (!SvFile.open(QIODevice::ReadWrite))
	{
		QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong"),
			QObject::tr(" Save the surfce.txt file failed!"));
		return N;
	}
	QTextStream Savesurfacefile(&SvFile);
	QString bufs;
	list<CommonFunction::orientation>::iterator dr;
	while (!Readsurfacefile.atEnd())//while start
	{
		bufs = Readsurfacefile.readLine();
		Savesurfacefile << bufs;
		Savesurfacefile << "\n";
		QRegExp seps(QObject::tr("[()]"));
		QString edgetype = bufs.section(seps, 1, 1);
		if (bufs.left(12) == QObject::tr("FaceGlobalNo"))
		{
			//Base::Console().Message("No=1==%s\n", bufs.toStdString().c_str());
			QString num = bufs.mid(14, 3).simplified();//获取编号
			//Base::Console().Message("No=%s==%d\n", num.toStdString().c_str(), num.toInt());
			bufs = Readsurfacefile.readLine();
			Savesurfacefile << bufs;
			Savesurfacefile << "\n";
			bufs = Readsurfacefile.readLine();
			QString dir;
			int flag = 0;
			for (dr = orientations.begin(); dr != orientations.end(); ++dr){
				if ((*dr).num == num.toInt()){
					flag = 1;
					QString sdir = QString::fromStdString((*dr).dir);
					double dirx = sdir.section(QObject::tr(","), 0, 0).simplified().toDouble();
					double diry = sdir.section(QObject::tr(","), 1, 1).simplified().toDouble();
					double dirz = sdir.section(QObject::tr(","), 2, 2).simplified().toDouble();
					QString DX, DY, DZ;
					DX.sprintf("%11.3e", dirx);
					DY.sprintf("%11.3e", diry);
					DZ.sprintf("%11.3e", dirz);
					dir = QObject::tr("FaceDir(%1,%2,%3)\n").arg(DX).arg(DY).arg(DZ);//获取编号
					break;
				}//if
			}//for
			if(flag){
				bufs = bufs.left(48) + dir;
				Savesurfacefile << bufs;
				//Savesurfacefile << "\n";
			}
			else{
				Savesurfacefile << bufs;
				Savesurfacefile << "\n";
			}
		}//if

		if (edgetype == QObject::tr("Spline")){
		  
			Savesurfacefile << Readsurfacefile.readLine() <<",";
			Savesurfacefile << Readsurfacefile.readLine() <<",";
			Savesurfacefile << Readsurfacefile.readLine() <<",";
			Savesurfacefile << Readsurfacefile.readLine() <<",";
			Savesurfacefile << Readsurfacefile.readLine() <<",";
			Savesurfacefile << Readsurfacefile.readLine() <<",";
			Savesurfacefile << Readsurfacefile.readLine() <<",";
			Savesurfacefile << Readsurfacefile.readLine() <<",";
			Savesurfacefile << Readsurfacefile.readLine() <<",";
			Savesurfacefile << Readsurfacefile.readLine() <<",";
			Savesurfacefile << Readsurfacefile.readLine();
			//Savesurfacefile << Readsurfacefile.readLine();
			Savesurfacefile << "\n";
		}
		
		QCoreApplication::processEvents();
	}//while
	//////////////surfcae///////////
	//////////判断没有在体里面的面
	for (lp = facelineoops.begin(); lp != facelineoops.end(); ++lp){//总的面数
		QString sumsurface = QString::fromStdString((*lp).number);
			int sufflag = 1;
		for (s = facesurfaces.begin(); s != facesurfaces.end(); ++s){//体的面数
			QString bufm = QString::fromStdString((*s).loop);
			QStringList flist = bufm.split(QObject::tr(","));
			int fcount = flist.size();
			for (int i = 0; i<fcount; i++){
				QString	surface = bufm.section(QObject::tr(","), i, i).simplified();
				if (sumsurface == surface)
				{
					sufflag = 0;
					break;
				}
			}//for i
		}//for s
		if (sufflag){//没有相同的面
			//Savestreamfile << QObject::tr("ShapeCount %1\n").arg(++mo);
			Savesurfacefile << QObject::tr("ShapeNo. %1  ShapeType(Plane)\n").arg(shapeNo++);
			Savesurfacefile << QObject::tr("FaceCount 1\n");
			//Savesurfacefile << QObject::tr("FaceGlobalNo. %1  FaceLocalNo. 1\n").arg(surfaceNo++);
			Savesurfacefile << QObject::tr("FaceGlobalNo. %1  FaceLocalNo. 1\n").arg(sumsurface);
			int j = 0;
			QString lineoop = QObject::tr("");
			QString buflp = QString::fromStdString((*lp).loop);
			QStringList llist = buflp.split(QObject::tr(","));
			int lcount = llist.size();
			Savesurfacefile << QObject::tr("FaceType(%1) EdgeCount %2\n").arg(QObject::tr("Plane")).arg(lcount);
			vector<string> Xvector, Yvector, Zvector, Pvector;
			//geopec.push_back("sdd");
			while (1){
				lineoop = buflp.section(QObject::tr(","), j, j).simplified();
				j++;
				if (lineoop != QObject::tr("")){
						int k = 0;
						QString line = QObject::tr("");
						int abss = 0;
						int flagCir = 0;
						int flagSpline = 0;
						if (lineoop.toDouble() < 0){ abss = 1; k = 1; }//记录正负修改顺序
						
						QMap<int, CommonFunction::faceline>::Iterator lit;
						lit = structlinemap.find(abs(lineoop.toInt()));
							if (lit->type == "Circle"){ flagCir = 1; }//判断是circle
							if (lit->type == "Spline"){ flagSpline = 1; }//判断是Spline
							QString bufl = QString::fromStdString(lit->loop);
							QStringList plist = bufl.split(QObject::tr(","));
							int Pcount = plist.size();
							QString Edgetype = QObject::tr("EdgeNo.%1 EdgeType(%2) PointCount(%3)").arg(j).arg(QString::fromStdString(lit->type)).arg(Pcount);
							Pvector.push_back(Edgetype.toStdString());
							if (flagCir&&abss){ k = 2; }//circles输出三个点
							if (flagSpline&&abss){ k = 20; }//Spline输出三个点
							QString first, second, third;
							int flag = 0, thr = 0;
							while (1){
								if (abss){
									if (flagCir){
										line = bufl.section(QObject::tr(","), k, k).simplified();
										//Savestreamfile << "liness1=" << line << "\n";//
										if (k == 0){ k = 4; }//避免多次输出
										k--;
									}
									else if (flagSpline){
										line = bufl.section(QObject::tr(","), k, k).simplified();
										if (k == 0){ k = 22; }//避免多次输出
										k--;
									}
									else{
										line = bufl.section(QObject::tr(","), k, k).simplified();

										if (k == 0){ k = 3; }//避免多次输出
										k--;
									}
								}
								else{
									line = bufl.section(QObject::tr(","), k, k).simplified();

									k++;
								}
								if (line != QObject::tr("")){

									QMap<QString, QString>::Iterator it;
									it = pointmap.find(line);
									QString b = it.value();
									QString x = b.section(QObject::tr(","), 0, 0);
									QString y = b.section(QObject::tr(","), 1, 1);
									QString z = b.section(QObject::tr(","), 2, 2);
									QString strX, strY, strZ;
									strX.sprintf("%11.3e", x.toDouble());
									strY.sprintf("%11.3e", y.toDouble());
									strZ.sprintf("%11.3e", z.toDouble());
									QString fipnt = QObject::tr("%1,%2,%3").arg(strX).arg(strY).arg(strZ);
									Xvector.push_back(x.toStdString());
									Yvector.push_back(y.toStdString());
									Zvector.push_back(z.toStdString());
											
											if (flag == 0){
												first = fipnt;
												flag = 1;
											}
											else if (flag == 1){
												second = fipnt;
												if (!flagCir){
													QString plist = QObject::tr("%1, %2").arg(first).arg(second);
													Pvector.push_back(plist.toStdString());
													flag = 0;
												}
												else{
													thr = 3;
													flag = 3;
												}

											}
											else if (thr){
												third = fipnt;
												QString plist = QObject::tr("%1, %2, %3").arg(first).arg(second).arg(third);
												Pvector.push_back(plist.toStdString());
												flag = 0;
												thr = 0;

											}
											if (Pcount == 21 && k == 21){
												QString plist = QObject::tr("%1").arg(fipnt);
												Pvector.push_back(plist.toStdString());
											}

								}
								else{
									break;
								}
							}//while
						//}//if
					//}//for
				}
				else{
					//面的信息处理
					vector<string>::iterator x, y, z, p;
					double strX = 0, strY = 0, strZ = 0;
					int count = 0;
					double x1 = 0, x2 = 0, y1 = 0, y2 = 0, z1 = 0, z2 = 0, x3 = 0, x4 = 0, y3 = 0, y4 = 0, z3 = 0, z4 = 0;
					for (x = Xvector.begin(), y = Yvector.begin(), z = Zvector.begin(); x != Xvector.end(), y != Yvector.end(), z != Zvector.end(); ++x, ++y, ++z){
						strX = strX + QString::fromStdString(*x).toDouble();
						strY = strY + QString::fromStdString(*y).toDouble();
						strZ = strZ + QString::fromStdString(*z).toDouble();
						count++;
						if (count == 1){
							x1 = QString::fromStdString(*x).toDouble();
							y1 = QString::fromStdString(*y).toDouble();
							z1 = QString::fromStdString(*z).toDouble();
						}
						if (count == 2){
							x2 = QString::fromStdString(*x).toDouble();
							y2 = QString::fromStdString(*y).toDouble();
							z2 = QString::fromStdString(*z).toDouble();
						}
						if (count == 3){
							x3 = QString::fromStdString(*x).toDouble();
							y3 = QString::fromStdString(*y).toDouble();
							z3 = QString::fromStdString(*z).toDouble();
						}
						if (count == 4){
							x4 = QString::fromStdString(*x).toDouble();
							y4 = QString::fromStdString(*y).toDouble();
							z4 = QString::fromStdString(*z).toDouble();
						}

					}
					QString clX, clY, clZ;
					clX.sprintf("%11.3e", strX / count);
					clY.sprintf("%11.3e", strY / count);
					clZ.sprintf("%11.3e", strZ / count);
					//Base::Console().Message("--------**%f,,,,%d----\n",strX,count);
					Savesurfacefile << QObject::tr("FaceCenter(%1,%2,%3) ").arg(clX).arg(clY).arg(clZ);
					double Vx1 = (x2 - x1);
					double Vy1 = (y2 - y1);
					double Vz1 = (z2 - z1);
					double Vx2 = (x4 - x1);
					double Vy2 = (y4 - y1);
					double Vz2 = (z4 - z1);
					double Vx = Vy1*Vz2 - Vz1*Vy2;
					double Vy = Vz1*Vx2 - Vx1*Vz2;
					double Vz = Vx1*Vy2 - Vy1*Vx2;
					double sum = Vx + Vy + Vz;
					QString DX, DY, DZ;
					DX.sprintf("%11.3e", Vx / fabs(sum));
					DY.sprintf("%11.3e", Vy / fabs(sum));
					DZ.sprintf("%11.3e", Vz / fabs(sum));
					if (_isnan(DX.toDouble())){
						DX.sprintf("%11.3e", 0);
						DY.sprintf("%11.3e", 0);
						DZ.sprintf("%11.3e", 0);
						Savesurfacefile << QObject::tr("FaceDir(%1,%2,%3)\n").arg(DX).arg(DY).arg(DZ);
					}
					else{
						Savesurfacefile << QObject::tr("FaceDir(%1,%2,%3)\n").arg(DX).arg(DY).arg(DZ);
					}
					int ssflag = 0;
					for (p = Pvector.begin(); p != Pvector.end(); ++p){
						QRegExp seps(QObject::tr("[()]"));
						QString edgetype = QString::fromStdString(*p).section(seps, 1, 1);
						if (edgetype == QObject::tr("Spline")){
							Savesurfacefile << QString::fromStdString(*p)<<"\n"; 
							if (ssflag == 1){ Savesurfacefile << "\n"; }
							ssflag = 1;
							continue;
						}
						if (edgetype == QObject::tr("Circle") || edgetype == QObject::tr("Line")){ 
						   Savesurfacefile << QString::fromStdString(*p)<<"\n"; 
							if (ssflag == 1){ Savesurfacefile << "\n"; }
							ssflag = 2;
							continue;
						}
						if (ssflag == 1){
							Savesurfacefile << QString::fromStdString(*p);
							continue; 
						}
						if (ssflag == 2){ 
							Savesurfacefile << QString::fromStdString(*p)<<"\n"; 
							ssflag = 0;
						    continue; 
						}
					}
					if (ssflag == 1){
						Savesurfacefile << "\n";
					}
					Xvector.clear();
					Yvector.clear();
					Zvector.clear();
					Pvector.clear();
					Savesurfacefile << "\n";

					break;
				}
			}//while							


		}//if sufflag
	}//for lp

    /////////////////
	
	SvFile.close();
	RFile.close();
	return N;
}

//材料文件里面添加体编号
static void ModelNO()
{
	QSettings settingmodel(QObject::tr("../ini/Model.ini"), QSettings::IniFormat);
	QSettings settingmodelno(QObject::tr("../ini/ModelNo.ini"), QSettings::IniFormat);
	int count = settingmodelno.value(QObject::tr("Count/count")).toInt();
	int namecount= settingmodel.value(QObject::tr("Name/count")).toInt();
	int CutName = settingmodelno.value(QObject::tr("Model/MeshNo")).toInt();
	QString Cname = settingmodelno.value(QObject::tr("Model/Modelname")).toString();
	int flags = 0;
	for (int i = 1; i <= count; i++){
		QString ModelName = settingmodelno.value(QObject::tr("%1/ModelName").arg(i)).toString();
		QString MeshNo = settingmodelno.value(QObject::tr("%1/MeshNo").arg(i)).toString();
		int flag = 0;
		
		for (int j = 1; j <= namecount; j++){
			QString name = settingmodel.value(QObject::tr("Name/%1").arg(j)).toString();
			if (name == ModelName){
				settingmodel.setValue(QObject::tr("%1/MeshNO").arg(name), MeshNo);
				flag = 1;
			}
			if (name == Cname){
				settingmodel.setValue(QObject::tr("%1/MeshNO").arg(name), CutName);
				flags = 1;
			}
		}

		if (flag == 0 && MeshNo!=QObject::tr("")){
			int countno = settingmodel.value(QObject::tr("Name/count")).toInt();
			settingmodel.setValue(QObject::tr("Name/count"), countno + 1);
			settingmodel.setValue(QObject::tr("Name/%1").arg(countno + 1), ModelName);
			settingmodel.setValue(QObject::tr("%1/material").arg(ModelName),QObject::tr("vacuum"));
			settingmodel.setValue(QObject::tr("%1/RelativePermittivity").arg(ModelName), 1.0);
			settingmodel.setValue(QObject::tr("%1/RelativePermeability").arg(ModelName), 1.0);
			settingmodel.setValue(QObject::tr("%1/BulkConductivity").arg(ModelName), 0.0);
			settingmodel.setValue(QObject::tr("%1/MeshNO").arg(ModelName), MeshNo);
		}


		

	}
	if (flags == 0 && CutName){
		int countno = settingmodel.value(QObject::tr("Name/count")).toInt();
		settingmodel.setValue(QObject::tr("Name/count"), countno + 1);
		settingmodel.setValue(QObject::tr("Name/%1").arg(countno + 1), Cname);
		settingmodel.setValue(QObject::tr("%1/material").arg(Cname), QObject::tr("vacuum"));
		settingmodel.setValue(QObject::tr("%1/RelativePermittivity").arg(Cname), 1.0);
		settingmodel.setValue(QObject::tr("%1/RelativePermeability").arg(Cname), 1.0);
		settingmodel.setValue(QObject::tr("%1/BulkConductivity").arg(Cname), 0.0);
		settingmodel.setValue(QObject::tr("%1/MeshNO").arg(Cname), CutName);
	}
	//剪切的
	/*int CutName = settingmodelno.value(QObject::tr("Model/MeshNo")).toInt();
	QString cutname = QObject::tr("cutmodel");
	if (CutName){
		int countno = settingmodel.value(QObject::tr("Name/count")).toInt();
		settingmodel.setValue(QObject::tr("Name/count"), countno + 1);
		settingmodel.setValue(QObject::tr("Name/%1").arg(countno + 1), cutname);
		settingmodel.setValue(QObject::tr("%1/RelativePermittivity").arg(cutname), 1.0);
		settingmodel.setValue(QObject::tr("%1/RelativePermeability").arg(cutname), 1.0);
		settingmodel.setValue(QObject::tr("%1/BulkConductivity").arg(cutname), 0.0);
		settingmodel.setValue(QObject::tr("%1/MeshNO").arg(cutname), CutName);
	}*/

	//删除进行了网格划分没有在了的体的材料内容
	//2018为了读取网格映射时材料
	/*QSettings settingNom(QObject::tr("../ini/Model.ini"), QSettings::IniFormat);
	int namecounts = settingNom.value(QObject::tr("Name/count")).toInt();
	for (int i = 1; i <= namecounts; i++){
		QString name = settingNom.value(QObject::tr("Name/%1").arg(i)).toString();
		QString MeshNo = settingNom.value(QObject::tr("%1/MeshNo").arg(name)).toString();
		if (MeshNo == QObject::tr("")){
			settingNom.remove(name);
		}
	}*/
}

static int GriddedMaterials(int N,int Meshsum)
{
	//pa.show();
	//pa->show();
	int NN=45-N;
	int MM=Meshsum/NN;
	QString GridFile = QString::fromLatin1("../ini/GriddedMaterials.ini");
	QFile gridFile(GridFile);
	if (!gridFile.open(QIODevice::WriteOnly))
	{
		return 0;
	}
	QTextStream Gridstream(&gridFile);
	QSettings settings(QObject::tr("../ini/Model.ini"), QSettings::IniFormat);
	int count = settings.value(QObject::tr("Name/count")).toInt();
	///没有设置材料或者材料相同的
	int vum=0;
	for (int i = 1; i <= count; i++){
		//Base::Console().Message("GriddedNOMaterials  1\n");
		QString name= settings.value(QObject::tr("Name/%1").arg(i)).toString();
		QString countname= settings.value(QObject::tr("%1/material").arg(name)).toString();
		if(countname==QObject::tr("vacuum")){
			vum++;
		}
	}
	if(count==0||vum==count){
		//Base::Console().Message("GriddedNOMaterials\n");
		int p=GriddedNOMaterials(MM,N);
		return p;
	}
		
	///////
	QString cX, cY, cZ;
	QString X, Y, Z;
	double x = 1.0, y = 1.0, z = 0;
	X.sprintf("%16.9e", x);
	Y.sprintf("%16.9e", y);
	Z.sprintf("%16.9e", z);
	QString m1,m2,m3,m4;
	QString nu = QObject::tr(" ");
	QString n1 = QObject::tr("1");
	QString n0 = QObject::tr("0");
	QString RelativePermittivityG1,RelativePermeabilityG1,BulkConductivityG1;
	QString RelativePermittivityG2,RelativePermeabilityG2,BulkConductivityG2;
	QString RelativePermittivityG3,RelativePermeabilityG3,BulkConductivityG3;
	QString RelativePermittivityG4,RelativePermeabilityG4,BulkConductivityG4;
	QString BoundaryMeshFile = QString::fromLatin1("../ini/mm/mesh.elements");
	QFile boundaryFile(BoundaryMeshFile);
				if (!boundaryFile.open(QIODevice::ReadOnly))
				{
					return 0;
				}
	QTextStream textstream(&boundaryFile);
	string outname=outObjStrName();
	//Base::Console().Message("name=== %s\n", outname);
	QStringList listxyz=nodesmaterials();
	//Base::Console().Message("name===1\n");
	//Base::Console().Message("num1==%s\n");
	//static MySimIIGui::Analysis pa;
	//QDateTime ttime = QDateTime::currentDateTime();//获取系统现在的时间
	//QString brttime = ttime.toString(QObject::tr("yyyy-MM-dd hh:mm:ss ddd")); //设置显示格式
	//pa.ui->textEdit->clear();
	while (!textstream.atEnd())
	{
					int buf,buf1,buf2,buf3,buf4,buf5;
					textstream >> buf;//读取编号Element_i
					if (textstream.atEnd()){
						break;
					}

					textstream >> buf5;
					textstream >> buf5;
					textstream >> buf1;//获取第一个节点
                    //string materials1=nodesmaterials(buf1);
					string materials1 = listxyz[buf1-1].toStdString();
					//Base::Console().Message("num2=%d===%s\n",buf1,materials1.c_str());
					//Base::Console().Message("num====%s\n",materials1.c_str());
					double m1x = QString::fromStdString(materials1).section(QObject::tr(","), 0, 0).toDouble();
					double m1y = QString::fromStdString(materials1).section(QObject::tr(","), 1, 1).toDouble();
					double m1z = QString::fromStdString(materials1).section(QObject::tr(","), 2, 2).toDouble();
				
					
					textstream >> buf2;//获取第二个节点
					//string materials2=nodesmaterials(buf2);
					string materials2 = listxyz[buf2-1].toStdString();
					//Base::Console().Message("num2=%d===%s\n",buf2,materials2.c_str());
					double m2x = QString::fromStdString(materials2).section(QObject::tr(","), 0, 0).toDouble();
					double m2y = QString::fromStdString(materials2).section(QObject::tr(","), 1, 1).toDouble();
					double m2z = QString::fromStdString(materials2).section(QObject::tr(","), 2, 2).toDouble();
				
					
					textstream >> buf3;//获取第三个节点
					//string materials3=nodesmaterials(buf3);
					string materials3 = listxyz[buf3-1].toStdString();
					//Base::Console().Message("num2=%d===%s\n",buf3,materials3.c_str());
					double m3x = QString::fromStdString(materials3).section(QObject::tr(","), 0, 0).toDouble();
					double m3y = QString::fromStdString(materials3).section(QObject::tr(","), 1, 1).toDouble();
					double m3z = QString::fromStdString(materials3).section(QObject::tr(","), 2, 2).toDouble();
			
					
					textstream >> buf4;//获取第四个节点
					//string materials4=nodesmaterials(buf4);
					string materials4 = listxyz[buf4-1].toStdString();
					//Base::Console().Message("num2=%d===%s\n",buf4,materials4.c_str());
					double m4x=QString::fromStdString(materials4).section(QObject::tr(","), 0, 0).toDouble();
					double m4y = QString::fromStdString(materials4).section(QObject::tr(","), 1, 1).toDouble();
					double m4z = QString::fromStdString(materials4).section(QObject::tr(","), 2, 2).toDouble();
					
					//Base::Console().Message("num2");
					/////////
					//return 0;
					//QDateTime ttime = QDateTime::currentDateTime();//获取系统现在的时间
	               //QString brttime = ttime.toString(QObject::tr("yyyy-MM-dd hh:mm:ss ddd"));
				   //QString pstarts = QObject::tr("nodes: %1 %2 %3 %4\n").arg(buf1).arg(buf2).arg(buf3).arg(buf4);
					//pa.ui->textEdit->insertPlainText(pstarts);
					
					if (buf%MM==0){
						if (N < 50){
							//Base::Console().Message("numnnnnnnnnnnnnnnnnnnn");
						}
					}
			QString nodes1=QObject::tr(""),nodes2=QObject::tr(""),nodes3=QObject::tr(""),nodes4=QObject::tr(""),ns=QObject::tr(",");
			vector<string> delnodes;//记录重复的几何数
			App::Document* outObj = App::GetApplication().getActiveDocument();
			std::vector<App::DocumentObject*> sel= outObj->getObjectsOfType(App::DocumentObject::getClassTypeId());
			int max=sel.size();//判断几何体数
				gp_Pnt pt1,pt2,pt3,pt4;
				pt1.SetX(m1x);
				pt1.SetY(m1y);
				pt1.SetZ(m1z);
				pt2.SetX(m2x);
				pt2.SetY(m2y);
				pt2.SetZ(m2z);
				pt3.SetX(m3x);
				pt3.SetY(m3y);
				pt3.SetZ(m3z);
				pt4.SetX(m4x);
				pt4.SetY(m4y);
				pt4.SetZ(m4z);
			//遍历所有几何体
			for (std::vector<App::DocumentObject*>::iterator it = sel.begin(); it != sel.end(); ++it)
			{
				std::string Grou = (*it)->getNameInDocument();
				QString Grous = QString::fromStdString(Grou);
				if (Grous.left(5) == QObject::tr("Group")){
					//Base::Console().Message("Group==Group\n");
					continue;
				}
				if (Grous.left(4) == QObject::tr("Line")){
					continue;
				}
				if (Grous.left(5) == QObject::tr("Plane")){
					continue;
				}
				//取得几何体名
				std::string objName = (*it)->getNameInDocument();
                if(objName==outname){continue;}
				{// 判断是否不可见的，不可见的跳过
					Gui::ViewProvider* pcProv = Gui::Application::Instance->getViewProvider(*it);
					bool visibility = true;
					if (pcProv) {
						visibility = ((Gui::ViewProviderDocumentObject*)pcProv)->Visibility.getValue();
					}
					if (visibility == false){// 不可见的跳过
						continue;
					}
				}
				App::DocumentObject *Obj = (*it);
				const TopoDS_Shape& tShape = static_cast<Part::Feature *>(Obj)->Shape.getValue();
				BRepClass3d_SolidClassifier solidClassifier1(tShape);
				BRepClass3d_SolidClassifier solidClassifier2(tShape);
				BRepClass3d_SolidClassifier solidClassifier3(tShape);
				BRepClass3d_SolidClassifier solidClassifier4(tShape);
				
				solidClassifier1.Perform(pt1, 0.0001);
				Standard_Boolean test1 = (solidClassifier1.State()!= TopAbs_OUT);
				solidClassifier2.Perform(pt2,0.0001);
				Standard_Boolean test2 = (solidClassifier2.State()!= TopAbs_OUT);
				solidClassifier3.Perform(pt3,0.0001);
				Standard_Boolean test3 = (solidClassifier3.State()!= TopAbs_OUT);
				solidClassifier4.Perform(pt4,0.0001);
				Standard_Boolean test4 = (solidClassifier4.State()!= TopAbs_OUT);

				/* TopoDS_Face aFace = TopoDS::Face(tShape);
				BRepClass_FaceClassifier PointOnSurf1;
				PointOnSurf1.Perform(aFace, pt1, 0.0001);
				Standard_Boolean isSatisfy1 = (PointOnSurf1.State() == TopAbs_IN || PointOnSurf1.State() == TopAbs_ON);
				BRepClass_FaceClassifier PointOnSurf2;
				PointOnSurf2.Perform(aFace, pt2, 0.0001);
				Standard_Boolean isSatisfy2 = (PointOnSurf2.State() == TopAbs_IN || PointOnSurf2.State() == TopAbs_ON);
				BRepClass_FaceClassifier PointOnSurf3;
				PointOnSurf3.Perform(aFace, pt3, 0.0001);
				Standard_Boolean isSatisfy3 = (PointOnSurf3.State() == TopAbs_IN || PointOnSurf3.State() == TopAbs_ON);
				BRepClass_FaceClassifier PointOnSurf4;
				PointOnSurf4.Perform(aFace, pt4, 0.0001);
				Standard_Boolean isSatisfy4 = (PointOnSurf4.State() == TopAbs_IN || PointOnSurf4.State() == TopAbs_ON); */
				if (test1 == true){//顶点在几何体里面
					//return objName;
					Standard_Boolean t1= solidClassifier1.IsOnAFace();
					if(t1){
				   nodes1 =nodes1+n1+ns + QString::fromStdString(objName)+ns;
				   delnodes.push_back(objName);
					}else{
					nodes1 =nodes1+n0+ns + QString::fromStdString(objName)+ns;
					delnodes.push_back(objName);					
					}
			   }
				if (test2 == true){//顶点在几何体里面
					Standard_Boolean t2 = solidClassifier2.IsOnAFace();
					if(t2){
						nodes2 =nodes2+n1 +ns+ QString::fromStdString(objName)+ns;
						delnodes.push_back(objName);
					}else{
						nodes2 =nodes2+n0 +ns+ QString::fromStdString(objName)+ns;
						delnodes.push_back(objName);
					}//return objName;
			   }
				if (test3 == true){//顶点在几何体里面
					Standard_Boolean t3 = solidClassifier3.IsOnAFace();
					if(t3){
						nodes3 = nodes3+n1+ns+ QString::fromStdString(objName)+ns;
						delnodes.push_back(objName);
					}else{
						nodes3 = nodes3+n0 +ns+ QString::fromStdString(objName)+ns;
						delnodes.push_back(objName);
					}//return objName;
			   }
				if (test4 == true){//顶点在几何体里面
					  //nodes4=nodes4+QString::fromStdString(objName);
					  Standard_Boolean t4 = solidClassifier4.IsOnAFace();
					  if(t4){
						  nodes4=nodes4+n1+ns+QString::fromStdString(objName)+ns;
						  delnodes.push_back(objName);
					  }
					  else{
						  nodes4=nodes4+n0+ns+QString::fromStdString(objName)+ns;
						  delnodes.push_back(objName);
					  }
					//return objName;
			   }
			   if(test1&&test2&&test3&&test4){
				   break;
			   }

				
			}//for it
			//Base::Console().Message("num3");
			if(outname!="NULL"){//有空气盒子的，获取时可能没有获取到的
				if(nodes1==QObject::tr("")){
					string tol=cycletolerance(pt1,outname);
					if(tol=="NULL"){
						nodes1=n0+ns+QString::fromStdString(outname)+ns;
						delnodes.push_back(outname);
					}
					else{
						nodes1 = QString::fromStdString(tol);
					}
				}
				if(nodes2==QObject::tr("")){
					string tol=cycletolerance(pt2,outname);
					if(tol=="NULL"){
						nodes2=n0+ns+QString::fromStdString(outname)+ns;
						delnodes.push_back(outname);
					}
					else{
						nodes2 = QString::fromStdString(tol);
					}
				}
				if(nodes3==QObject::tr("")){
					string tol=cycletolerance(pt3,outname);
					if(tol=="NULL"){
						nodes3=n0+ns+QString::fromStdString(outname)+ns;
						delnodes.push_back(outname);
					}
					else{
						nodes3 = QString::fromStdString(tol);
					}
				}
				if(nodes4==QObject::tr("")){
					string tol=cycletolerance(pt4,outname);
					if(tol=="NULL"){
						nodes4=n0+ns+QString::fromStdString(outname)+ns;
						delnodes.push_back(outname);
					}
					else{
						nodes4 = QString::fromStdString(tol);
					}
				}
			}
			if(nodes1==NULL){
				string tol=cycletolerance(pt1,outname);
				if(tol=="NULL"){
					QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong"),
					QObject::tr("Node %1 was not found.").arg(buf1));
					return 0;
				}
				else{
					nodes1 = QString::fromStdString(tol);
				}
			}
			if(nodes2==NULL){
				string tol=cycletolerance(pt2,outname);
				if(tol=="NULL"){
					QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong"),
					QObject::tr("Node %1 was not found.").arg(buf2));
					return 0;
				}
				else{
					nodes2 = QString::fromStdString(tol);
				}
			}
			if(nodes3==NULL){
				string tol=cycletolerance(pt3,outname);
				if(tol=="NULL"){
					QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong"),
					QObject::tr("Node %1 was not found.").arg(buf3));
					return 0;
				}
				else{
					nodes3 = QString::fromStdString(tol);
				}
			}
			if(nodes4==NULL){
				string tol=cycletolerance(pt4,outname);
				if(tol=="NULL"){
					QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong"),
					QObject::tr("Node %1 was not found.").arg(buf4));
					return 0;
				}
				else{
					nodes4 = QString::fromStdString(tol);
				}
			}
			nodes1=nodes1.left(nodes1.length() - 1);
			nodes2=nodes2.left(nodes2.length() - 1);
			nodes3=nodes3.left(nodes3.length() - 1);
			nodes4=nodes4.left(nodes4.length() - 1);
			int waringflag=0;
			//Base::Console().Message("num4");
			//Base::Console().Message("size()=%s&&%s&&%s&&%s\n",nodes1.toStdString().c_str(),nodes2.toStdString().c_str(),nodes3.toStdString().c_str(),nodes4.toStdString().c_str());
			QStringList list1 = nodes1.split(QObject::tr(",")); if (list1.size()>4){ Base::Console().Message("%s\n", nodes1.toStdString().c_str()); Materialswarning(); return 0; }if (list1.size() == 4){ waringflag++; }
			QStringList list2 = nodes2.split(QObject::tr(",")); if (list2.size()>4){ Base::Console().Message("%s\n", nodes2.toStdString().c_str()); Materialswarning(); return 0; }if (list2.size() == 4){ waringflag++; }
			QStringList list3 = nodes3.split(QObject::tr(",")); if (list3.size()>4){ Base::Console().Message("%s\n", nodes3.toStdString().c_str()); Materialswarning(); return 0; }if (list3.size() == 4){ waringflag++; }
			QStringList list4 = nodes4.split(QObject::tr(",")); if (list4.size()>4){ Base::Console().Message("%s\n", nodes4.toStdString().c_str()); Materialswarning(); return 0; }if (list4.size() == 4){ waringflag++; }
			/* if(waringflag>=4){//如果一个单元的四个节点都同时隶属于模块A和B,错误
				QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong"),
			   QObject::tr("Model was invalid, as four nodes locate one interface."));
		        return 0;
			} *///微带天线会出现四个节点都在界面上
			//Base::Console().Message("num5");
			sort(delnodes.begin(), delnodes.end());
		    delnodes.erase(unique(delnodes.begin(), delnodes.end()), delnodes.end());
			if(delnodes.size()>2&&outname=="NULL"){//四个节点多余2个几何模块错误
				QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong"),
			   QObject::tr("Model was invalid, as its parts may improperly overlay.2"));
		        return 0;
			}
			if(delnodes.size()>4&&outname!="NULL"){//四个节点多余3个几何模块错误
			//Base::Console().Message("size()==%d=\n",delnodes.size());
			for (vector<string>::iterator it = delnodes.begin(); it != delnodes.end(); ++it)
              // Base::Console().Message("size(it)==%s=\n",(*it).c_str());
				QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong"),
			   QObject::tr("Model was invalid, as its parts may improperly overlay.3"));
		        return 0;
			}
			//Base::Console().Message("num6");
			delnodes.clear();
			QString Matermodel1,Matermodel2,Matermodel3,Matermodel4;//记录每个节点的几何名称
			///
			//Base::Console().Message("num7");
			//Base::Console().Message("nodes1=%snodes2=%snodes3=%snodes4=%s",nodes1.toStdString().c_str(),nodes2.toStdString().c_str(),nodes3.toStdString().c_str(),nodes4.toStdString().c_str());
            string list=materialsdecide(list1,list2,list3,list4);
		    Matermodel1=QString::fromStdString(list).section(QObject::tr(","), 0, 0);
			Matermodel2=QString::fromStdString(list).section(QObject::tr(","), 1, 1);
			Matermodel3=QString::fromStdString(list).section(QObject::tr(","), 2, 2);
			Matermodel4=QString::fromStdString(list).section(QObject::tr(","), 3, 3);
			////
			
		int flag1=0,flag2=0,flag3=0,flag4=0;	
		//Base::Console().Message("num1\n");
		/*if (buf1 == 173 || buf2 == 173 || buf3 == 173 || buf4 == 173){
			Base::Console().Message("%s,%s,%s,%s\n", Matermodel1.toStdString().c_str(), Matermodel2.toStdString().c_str(), Matermodel3.toStdString().c_str(), Matermodel4.toStdString().c_str());
		}*/
	   //m1
	   for (int j = 1; j <= count; j++){
			QString Bname=settings.value(QObject::tr("Name/%1").arg(j)).toString();
		    if (Bname == Matermodel1){
				RelativePermittivityG1 = settings.value(QObject::tr("%1/RelativePermittivity").arg(Bname)).toString();
				RelativePermeabilityG1 = settings.value(QObject::tr("%1/RelativePermeability").arg(Bname)).toString();
				BulkConductivityG1 = settings.value(QObject::tr("%1/BulkConductivity").arg(Bname)).toString();
				flag1=1;
			}
			if (Bname ==Matermodel2){
				RelativePermittivityG2 = settings.value(QObject::tr("%1/RelativePermittivity").arg(Bname)).toString();
				RelativePermeabilityG2 = settings.value(QObject::tr("%1/RelativePermeability").arg(Bname)).toString();
				BulkConductivityG2 = settings.value(QObject::tr("%1/BulkConductivity").arg(Bname)).toString();
				flag2=1;
			}
			if (Bname == Matermodel3){
				RelativePermittivityG3 = settings.value(QObject::tr("%1/RelativePermittivity").arg(Bname)).toString();
				RelativePermeabilityG3 = settings.value(QObject::tr("%1/RelativePermeability").arg(Bname)).toString();
				BulkConductivityG3= settings.value(QObject::tr("%1/BulkConductivity").arg(Bname)).toString();
				flag3=1;
			}
			if (Bname ==Matermodel4){
				RelativePermittivityG4 = settings.value(QObject::tr("%1/RelativePermittivity").arg(Bname)).toString();
				RelativePermeabilityG4 = settings.value(QObject::tr("%1/RelativePermeability").arg(Bname)).toString();
				BulkConductivityG4 = settings.value(QObject::tr("%1/BulkConductivity").arg(Bname)).toString();
				flag4=1;
			}
		}
		if(flag1==0){
			m1=X+nu+Y+nu+Z;
	    }else{
			cX.sprintf("%16.9e", RelativePermittivityG1.toDouble());
			cY.sprintf("%16.9e", RelativePermeabilityG1.toDouble());
		    cZ.sprintf("%16.9e", BulkConductivityG1.toDouble());
			m1=cX+nu+cY+nu+cZ;
			//flag1=0;
		}
		 //m2
	   /* for (int j = 1; j <= count; j++){
			QString Bname=settings.value(QObject::tr("Name/%1").arg(j)).toString();
		    if (Bname ==Matermodel2){
				RelativePermittivityG = settings.value(QObject::tr("%1/RelativePermittivity").arg(Bname)).toString();
				RelativePermeabilityG = settings.value(QObject::tr("%1/RelativePermeability").arg(Bname)).toString();
				BulkConductivityG = settings.value(QObject::tr("%1/BulkConductivity").arg(Bname)).toString();
				flag=1;
			}
		} */
		if(flag2==0){
			m2=X+nu+Y+nu+Z;
	    }else{
			cX.sprintf("%16.9e", RelativePermittivityG2.toDouble());
			cY.sprintf("%16.9e", RelativePermeabilityG2.toDouble());
		    cZ.sprintf("%16.9e", BulkConductivityG2.toDouble());
			m2=cX+nu+cY+nu+cZ;
			//flag2=0;
		}
		 //m3
	   /* for (int j = 1; j <= count; j++){
			QString Bname=settings.value(QObject::tr("Name/%1").arg(j)).toString();
		    if (Bname == Matermodel3){
				RelativePermittivityG = settings.value(QObject::tr("%1/RelativePermittivity").arg(Bname)).toString();
				RelativePermeabilityG = settings.value(QObject::tr("%1/RelativePermeability").arg(Bname)).toString();
				BulkConductivityG = settings.value(QObject::tr("%1/BulkConductivity").arg(Bname)).toString();
				flag=1;
			}
		} */
		if(flag3==0){
			m3=X+nu+Y+nu+Z;
	    }else{
			cX.sprintf("%16.9e", RelativePermittivityG3.toDouble());
			cY.sprintf("%16.9e", RelativePermeabilityG3.toDouble());
		    cZ.sprintf("%16.9e", BulkConductivityG3.toDouble());
			m3=cX+nu+cY+nu+cZ;
			//flag3=0;
		}
		 //m4
	   /* for (int j = 1; j <= count; j++){
			QString Bname=settings.value(QObject::tr("Name/%1").arg(j)).toString();
		    if (Bname ==Matermodel4){
				RelativePermittivityG = settings.value(QObject::tr("%1/RelativePermittivity").arg(Bname)).toString();
				RelativePermeabilityG = settings.value(QObject::tr("%1/RelativePermeability").arg(Bname)).toString();
				BulkConductivityG = settings.value(QObject::tr("%1/BulkConductivity").arg(Bname)).toString();
				flag=1;
			}
		} */
		if(flag4==0){
			m4=X+nu+Y+nu+Z;
	    }else{
			cX.sprintf("%16.9e", RelativePermittivityG4.toDouble());
			cY.sprintf("%16.9e", RelativePermeabilityG4.toDouble());
		    cZ.sprintf("%16.9e", BulkConductivityG4.toDouble());
			m4=cX+nu+cY+nu+cZ;
			//flag4=0;
		}
			
			//Efstr.left(Efstr.length() - 1)
			/////////////
					//Gridstream << buf << " " << buf1 << " " << buf2 << " " << buf3 << " " << buf4 << " " << QString::fromStdString(materials1) << " " << QString::fromStdString(materials2) << " " << QString::fromStdString(materials3) << " " << QString::fromStdString(materials4) << "\n";//写入
					Gridstream << buf << nu << buf1 << nu << buf2 << nu << buf3 << nu<< buf4 << nu << m1 << nu << m2<< nu <<m3 << nu << m4 << "\n";//写入
			      //Gridstream << buf << " " << buf1 << " " << buf2 << " " << buf3 << " " << buf4 << " " << nodes1<< " " << nodes2<< " " << nodes3 << " " <<nodes4<< "\n";//写入
					//Gridstream << buf << " " << buf1 << " " << buf2 << " " << buf3 << " " << buf4 << " " << Matermodel1<< " " << Matermodel2<< " " << Matermodel3 << " " <<Matermodel4<< "\n";//写入
					//QElapsedTimer et;
					//et.start();
					//while (et.elapsed()<10)
						QCoreApplication::processEvents();
	}
	//QCoreApplication::processEvents();
	QApplication::restoreOverrideCursor();
	boundaryFile.close();
	gridFile.close();
	return N;
}//GriddedMaterials
static string ElementonShape(gp_Pnt pt1,string outname)
{
			QString nodes1 = QObject::tr(""), ns = QObject::tr(",");
			QString n1 = QObject::tr("1");
			QString n0 = QObject::tr("0");
			double totl = 0.0001;
			App::Document* outObj = App::GetApplication().getActiveDocument();
			std::vector<App::DocumentObject*> sel= outObj->getObjectsOfType(App::DocumentObject::getClassTypeId());
			//遍历所有几何体
			for (std::vector<App::DocumentObject*>::iterator it = sel.begin(); it != sel.end(); ++it)
			{
				std::string Grou = (*it)->getNameInDocument();
				QString Grous = QString::fromStdString(Grou);
				if (Grous.left(5) == QObject::tr("Group")){
					//Base::Console().Message("Group==Group\n");
					continue;
				}
				if (Grous.left(4) == QObject::tr("Line")){
					continue;
				}
				if (Grous.left(5) == QObject::tr("Plane")){
					continue;
				}
				//取得几何体名
				std::string objName = (*it)->getNameInDocument();
                if(objName==outname){continue;}
				{// 判断是否不可见的，不可见的跳过
					Gui::ViewProvider* pcProv = Gui::Application::Instance->getViewProvider(*it);
					bool visibility = true;
					if (pcProv) {
						visibility = ((Gui::ViewProviderDocumentObject*)pcProv)->Visibility.getValue();
					}
					if (visibility == false){// 不可见的跳过
						continue;
					}
				}
				App::DocumentObject *Obj = (*it);
				const TopoDS_Shape& tShape = static_cast<Part::Feature *>(Obj)->Shape.getValue();
				BRepClass3d_SolidClassifier solidClassifier1(tShape);
				solidClassifier1.Perform(pt1, totl);//体
				Standard_Boolean test1 = (solidClassifier1.State() == TopAbs_IN || solidClassifier1.State() == TopAbs_ON);
				TopoDS_Face aFace = TopoDS::Face(tShape);
				//Handle_Geom_Surface Surface =BRep_Tool::Surface(TopoDS::Face(tShape));
				BRepClass_FaceClassifier PointOnSurf;
				PointOnSurf.Perform(aFace, pt1, totl);
				Standard_Boolean isSatisfy1 = (PointOnSurf.State() == TopAbs_IN || PointOnSurf.State() == TopAbs_ON);
				//TopoDS_Edge anEdge = TopoDS::Edge(tShape);
				//GeomAPI_ProjectPointOnCurve PointOnCurve(anEdge);
				//PointOnCurve.Perform(pt1);
				//Standard_Boolean isSatisfy2 = (PointOnCurve.NbPoints() > 0 && PointOnCurve.LowerDistance() <= totl);
				
				if (test1 == true||isSatisfy1==true){//顶点在几何体里面
					//return objName;
					Standard_Boolean t1= solidClassifier1.IsOnAFace();
					if(t1){
				         nodes1 =nodes1+n1+ns+QString::fromStdString(objName)+ns;
					}
					else{
					    nodes1 =nodes1+n0+ns+QString::fromStdString(objName)+ns;				
					}
			   }
			}
			
			if(nodes1!=NULL){
				return nodes1.toStdString();
			}
			else{
				return "NULL";
			}
  
  
}
static string cycletolerance(gp_Pnt pt1,string outname)
{
	//Base::Console().Message("num1\n");
	QString nodes1=QObject::tr(""),ns=QObject::tr(",");
	QString n1 = QObject::tr("1");
	QString n0 = QObject::tr("0");
	double totl=0.0001;
	while(1){
	         totl=totl*10;
             App::Document* outObj = App::GetApplication().getActiveDocument();
			std::vector<App::DocumentObject*> sel= outObj->getObjectsOfType(App::DocumentObject::getClassTypeId());
			//遍历所有几何体
			for (std::vector<App::DocumentObject*>::iterator it = sel.begin(); it != sel.end(); ++it)
			{
				std::string Grou = (*it)->getNameInDocument();
				QString Grous = QString::fromStdString(Grou);
				if (Grous.left(5) == QObject::tr("Group")){
					//Base::Console().Message("Group==Group\n");
					continue;
				}
				if (Grous.left(4) == QObject::tr("Line")){
					continue;
				}
				if (Grous.left(5) == QObject::tr("Plane")){
					continue;
				}
				//取得几何体名
				std::string objName = (*it)->getNameInDocument();
                if(objName==outname){continue;}
				{// 判断是否不可见的，不可见的跳过
					Gui::ViewProvider* pcProv = Gui::Application::Instance->getViewProvider(*it);
					bool visibility = true;
					if (pcProv) {
						visibility = ((Gui::ViewProviderDocumentObject*)pcProv)->Visibility.getValue();
					}
					if (visibility == false){// 不可见的跳过
						continue;
					}
				}
				App::DocumentObject *Obj = (*it);
				const TopoDS_Shape& tShape = static_cast<Part::Feature *>(Obj)->Shape.getValue();
				BRepClass3d_SolidClassifier solidClassifier1(tShape);
				solidClassifier1.Perform(pt1, totl);
				Standard_Boolean test1 = (solidClassifier1.State()!= TopAbs_OUT);
				TopExp_Explorer countFace;
				Standard_Boolean isSatisfy1=false;
				if(test1!=true){
					for (countFace.Init(tShape, TopAbs_FACE); countFace.More(); countFace.Next())
					{
						const TopoDS_Face &aFace = TopoDS::Face(countFace.Current());
						BRepClass_FaceClassifier PointOnSurf;
						PointOnSurf.Perform(aFace, pt1, totl);
						//isSatisfy1 = (PointOnSurf.State() == TopAbs_IN || PointOnSurf.State() == TopAbs_ON);
						 /* if(PointOnSurf.State() == TopAbs_IN){
							Base::Console().Message("tolerance=1====%s\n",objName.c_str());
							
						}  */
						if(PointOnSurf.State() == TopAbs_ON){
							//Base::Console().Message("tolerance=2===%s\n",objName.c_str());
							isSatisfy1 =true;
							break;
						}
						 /* if(PointOnSurf.State() == TopAbs_OUT){
							Base::Console().Message("tolerance=3===%s\n",objName.c_str());
							
						}  */
						/* if(isSatisfy1==true){
							Base::Console().Message("tolerance=t====%s\n",objName.c_str());
							break;
						} */
						
						
					}
				}
				/* const TopoDS_Face &aFace = TopoDS::Face(tShape);
				BRepClass_FaceClassifier PointOnSurf;
				PointOnSurf.Perform(aFace, pt1, totl);
				Standard_Boolean isSatisfy1 = (PointOnSurf.State() == TopAbs_IN || PointOnSurf.State() == TopAbs_ON); */
				if (test1 == true || isSatisfy1==true){//顶点在几何体里面
					//return objName;
					//Base::Console().Message("tolerance=err==%f==%f==%f==%s\n",pt1.X(),pt1.Y(),pt1.Z(),objName.c_str());
					Standard_Boolean t1= solidClassifier1.IsOnAFace();
					if(t1){
				         nodes1 =nodes1+n1+ns+QString::fromStdString(objName)+ns;
					}
					else{
					    nodes1 =nodes1+n0+ns+QString::fromStdString(objName)+ns;				
					}
					break;//找不到的都是在曲面的上，第一次找到就可以退出
			   }
			}//for
			if(nodes1!=NULL){
				//Base::Console().Message("%s\n", nodes1.toStdString().c_str());
				break;
			}
			if(totl>10){
				break;
			}
	}
	//Base::Console().Message("tolerance==%f\n",totl);
	if(nodes1!=NULL){
		return nodes1.toStdString();
	}
	else{
		return "NULL";
	}
}
static void Materialswarning()
{
	QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong"),
	QObject::tr("Model was invalid."));
}
static QStringList nodesmaterials()
{
	//return "ok";
	//Base::Console().Message("num2==\n");
	QStringList listnodes;
	QString NodesMeshFile = QString::fromLatin1("../ini/mm/mesh.nodes");
	QFile nodesFile(NodesMeshFile);
	if (!nodesFile.open(QIODevice::ReadOnly))
	{
		return listnodes;
	}
	QTextStream nodesstream(&nodesFile);
	//double nodesX,nodesY,nodesZ;
	//int i=0;
	//Base::Console().Message("num4==\n");
	while (!nodesstream.atEnd())
	{
		int buf;
		double bufx,bufy,bufz,bufs;
        nodesstream	>>buf;
		if (nodesstream.atEnd()){
						break;
		}
		nodesstream>>bufs;
		nodesstream>>bufx;
		nodesstream>>bufy;
		nodesstream>>bufz;
       /*  if(buf==elementbuf){
		   nodesX=bufx;
		   nodesY=bufy;
		   nodesZ=bufz;
		   break;
	    } */
		//Base::Console().Message("num5==\n");
	   QString xyz=QObject::tr("%1,%2,%3").arg(bufx).arg(bufy).arg(bufz);
	   //listnodes[i]=xyz;
	   listnodes.append(xyz);
	   //Base::Console().Message("num6==\n");
	   //Base::Console().Message("num2====%s\n",xyz.toStdString().c_str());
	  // i++;
	}
	//QString xyz=QObject::tr("%1,%2,%3").arg(nodesX).arg(nodesY).arg(nodesZ);
	nodesFile.close();
	//Base::Console().Message("num3==\n");
	return listnodes;
 
	
}//nodesmaterials
static string materialsdecide(QStringList list1,QStringList list2,QStringList list3,QStringList list4)
{
	//Base::Console().Message("num8");
	QString Matermodel1,Matermodel2,Matermodel3,Matermodel4;
			if(list1.size()==2){Matermodel1=list1[1];}
			if(list2.size()==2){Matermodel2=list2[1];}
			if(list3.size()==2){Matermodel3=list3[1];}
			if(list4.size()==2){Matermodel4=list4[1];}
			//Base::Console().Message("num9");
			if(Matermodel1==NULL){//说明在界面上1-1，
				if(Matermodel2==NULL){//说明在两个界面,1-1,2-1
					if(Matermodel3==NULL)//说明在三个界面上,1-1,2-1,3-1
					{
						if(Matermodel4==NULL){//1-1,2-1,3-1,4-1
							//error
							//Base::Console().Message("num10");
							Matermodel1=list1[1];
							Matermodel2=list1[1];
							Matermodel3=list1[1];
							Matermodel3=list1[1]; 
						}
						else{//取界面外一点,这种情况应该不会出现
							Matermodel1=Matermodel4;
							Matermodel2=Matermodel4;
							Matermodel3=Matermodel4;
						}
					}
					else{//3不为null
						if(Matermodel4==NULL){//1,2,4为null//在界面上这种情况应该不会出现
							Matermodel1=Matermodel3;
							Matermodel2=Matermodel3;
							Matermodel4=Matermodel3;
						}
						else{//1,2在界面上，取外任意相同的一个
							if(list1[1]==Matermodel3||list1[3]==Matermodel3){
								Matermodel1=Matermodel3;
								//Matermodel2=Matermodel3;
							}
							if(list2[1]==Matermodel3||list2[3]==Matermodel3){
								Matermodel2=Matermodel3;
							}
							if(list1[1]==Matermodel4||list1[3]==Matermodel4){
								Matermodel1=Matermodel4;
								//Matermodel2=Matermodel4;
							}
							if(list2[1]==Matermodel4||list2[3]==Matermodel4){
								Matermodel2=Matermodel4;
							}
						}
					}//3null
					
				}//2null
				else{//2不为null
					if(Matermodel3==NULL){//1.3为null
						if(Matermodel4==NULL){//1.3.4为null这种情况应该不会出现
							Matermodel1=Matermodel2;
							Matermodel3=Matermodel2;
							Matermodel4=Matermodel2;
						}
						else{//1,3为null,取外两个
							if(list1[1]==Matermodel2||list1[3]==Matermodel2){
								Matermodel1=Matermodel2;
								//
							}
							if(list3[1]==Matermodel2||list3[3]==Matermodel2){
								Matermodel3=Matermodel2;
							}
							if(list1[1]==Matermodel4||list1[3]==Matermodel4){
								Matermodel1=Matermodel4;
								//
							}
							if(list3[1]==Matermodel4||list3[3]==Matermodel4){
								Matermodel3=Matermodel4;
							}
						}
						
					}//3 为null
					else{//1为null
						if(Matermodel4==NULL){//1,4为null,1-1,2-0,3-0,4-1
						    if(list1[1]==Matermodel2||list1[3]==Matermodel2){
								Matermodel1=Matermodel2;
								
							}
							if(list4[1]==Matermodel2||list4[3]==Matermodel2){
								Matermodel4=Matermodel2;
							}
							if(list1[1]==Matermodel3||list1[3]==Matermodel3){
								Matermodel1=Matermodel3;
								
							}
							if(list4[1]==Matermodel3||list4[3]==Matermodel3){
								Matermodel4=Matermodel3;
							}
							
						}//1-1,2-0,3-0,4-1
						else{//2.3.4不为NUll1-1,2-0,3-0,4-0 000
							if((list1[1]==Matermodel2&&list1[1]==Matermodel3&&list1[1]==Matermodel4)||(list1[3]==Matermodel2&&list1[3]==Matermodel3&&list1[3]==Matermodel4)){
								Matermodel1=Matermodel2;
							}
							else if((list1[1]==Matermodel2&&list1[1]==Matermodel3)||(list1[1]==Matermodel4&&list1[1]==Matermodel2)||(list1[1]==Matermodel4&&list1[1]==Matermodel3)){
								Matermodel1=list1[1];
							}
							else if((list1[3]==Matermodel2&&list1[3]==Matermodel3)||(list1[3]==Matermodel4&&list1[3]==Matermodel2)||(list1[3]==Matermodel4&&list1[3]==Matermodel3)){
								Matermodel1=list1[3];
							}
							else{
								//error
								Matermodel1=list1[1];
							}

						}//1-1,2-0,3-0,4-0
					}
					
				}//2else
			}
		    else{//1不为null,1-0
				if (Matermodel2 == NULL){//1不为null,1-0,2-1
					if (Matermodel3 == NULL){//1不为null,1-0,2-1,3-1
						if (Matermodel4 == NULL){//1不为null,1-0,2-1,3-1,4-1这种情况应该不会出现
							Matermodel2 = Matermodel1;
							Matermodel3 = Matermodel1;
							Matermodel4 = Matermodel1;
						}
						else{//1,4不为null1-0,2-1,3-1,4-0,任取两个
							
							if(list2[1]==Matermodel1||list2[3]==Matermodel1){
								Matermodel2 = Matermodel1;
							    
							}
							if(list3[1]==Matermodel1||list3[3]==Matermodel1){
								Matermodel3 = Matermodel1;
							}
							if(list2[1]==Matermodel4||list2[3]==Matermodel4){
								Matermodel2 = Matermodel4;
							   
							}
						    if(list3[1]==Matermodel4||list3[3]==Matermodel4){
							 Matermodel3 = Matermodel4;
						    }
						}

					}
					else{//1.3不为null，1-0,2-1,3-0
						if (Matermodel4 == NULL){//1.3不为null1-0,2-1,3-0,4-1
							if(list2[1]==Matermodel1||list2[3]==Matermodel1){
								Matermodel2 = Matermodel1;
								
							}
							if(list4[1]==Matermodel1||list4[3]==Matermodel1){
								Matermodel4 = Matermodel1;
							}
							if(list2[1]==Matermodel3||list2[3]==Matermodel3){
								Matermodel2 = Matermodel3;
							   
							}
							if(list4[1]==Matermodel3||list4[3]==Matermodel3){
								 Matermodel4 = Matermodel3;
							}
							
						}
						else{//1.3.4不为null1-0,2-1,3-0,4-0 000
							if((list2[1]==Matermodel1&&list2[1]==Matermodel3&&list2[1]==Matermodel4)||(list2[3]==Matermodel1&&list2[3]==Matermodel3&&list2[3]==Matermodel4)){
								Matermodel2=Matermodel3;
							}
							else if((list2[1]==Matermodel1&&list2[1]==Matermodel3)||(list2[1]==Matermodel4&&list2[1]==Matermodel1)||(list2[1]==Matermodel4&&list2[1]==Matermodel3)){
								Matermodel2=list2[1];
							}
							else if((list2[3]==Matermodel1&&list2[3]==Matermodel3)||(list2[3]==Matermodel4&&list2[3]==Matermodel1)||(list2[3]==Matermodel4&&list2[3]==Matermodel3)){
								Matermodel2=list2[3];
							}
							else{
								//error
								Matermodel2=list2[1];
							}

						}

					}
				}
				else{//1.2不为null1-0,2-0
					if(Matermodel3==NULL){//1.2不为null,1-0,2-0,3-1
						if(Matermodel4==NULL){//1.2不为null1-0,2-0,3-1,4-1								
							if(list3[1]==Matermodel1||list3[3]==Matermodel1){
								Matermodel3=Matermodel1;
								
							}
							if(list4[1]==Matermodel1||list4[3]==Matermodel1){
								Matermodel4=Matermodel1;
							}
							if(list3[1]==Matermodel2||list3[3]==Matermodel2){
								Matermodel3=Matermodel2;
								
							}
							if(list4[1]==Matermodel2||list4[3]==Matermodel2){
								Matermodel4=Matermodel2;
							}
						}
						else{//1,2,4不为null1-0,2-0,3-1,4-0 000
							if((list3[1]==Matermodel1&&list3[1]==Matermodel2&&list3[1]==Matermodel4)||(list3[3]==Matermodel2&&list3[3]==Matermodel1&&list3[3]==Matermodel4)){
								Matermodel3=Matermodel2;
							}
							else if((list3[1]==Matermodel1&&list3[1]==Matermodel2)||(list3[1]==Matermodel4&&list3[1]==Matermodel1)||(list3[1]==Matermodel4&&list3[1]==Matermodel2)){
								Matermodel3=list3[1];
							}
							else if((list3[3]==Matermodel1&&list3[3]==Matermodel2)||(list3[3]==Matermodel4&&list3[3]==Matermodel1)||(list3[3]==Matermodel4&&list3[3]==Matermodel2)){
								Matermodel3=list3[3];
							}
					     else{
						   //error
						   Matermodel3=list3[1];
					     }
							
						}
					}
					else{//1.2.3不为null1-0,2-0,3-0
						if(Matermodel4==NULL){//1.2.3不为null1-0,2-0,3-0,4-1 000
							if((list4[1]==Matermodel1&&list4[1]==Matermodel2&&list4[1]==Matermodel3)||(list4[3]==Matermodel2&&list4[3]==Matermodel1&&list4[3]==Matermodel3)){
								Matermodel4=Matermodel2;
							}
							else if((list4[1]==Matermodel1&&list4[1]==Matermodel2)||(list4[1]==Matermodel3&&list4[1]==Matermodel1)||(list4[1]==Matermodel3&&list4[1]==Matermodel2)){
								Matermodel4=list4[1];
							}
							else if((list4[3]==Matermodel1&&list4[3]==Matermodel2)||(list4[3]==Matermodel3&&list4[3]==Matermodel1)||(list4[3]==Matermodel3&&list4[3]==Matermodel2)){
								Matermodel4=list4[3];
							}
							else{
								//error
								Matermodel4=list4[1];
							}
						}
					}
					
				}
				
			}
			
			
QString xyz=QObject::tr("%1,%2,%3,%4").arg(Matermodel1).arg(Matermodel2).arg(Matermodel3).arg(Matermodel4);
//Base::Console().Message("num=%s",xyz.toStdString().c_str());
return xyz.toStdString();
	
}//materialsdecide

static int GriddedNOMaterials(int mm,int N)
{
	QString GridFile = QString::fromLatin1("../ini/GriddedMaterials.ini");
	QFile gridFile(GridFile);
	if (!gridFile.open(QIODevice::WriteOnly))
	{
		return 0;
	}
	QTextStream Gridstream(&gridFile);
	QString X, Y, Z;
	double x = 1.0, y = 1.0, z = 0;
	X.sprintf("%16.9e", x);
	Y.sprintf("%16.9e", y);
	Z.sprintf("%16.9e", z);
	QString m1,m2,m3,m4;
	QString nu = QObject::tr(" ");
	QString BoundaryMeshFile = QString::fromLatin1("../ini/mm/mesh.elements");
	QFile boundaryFile(BoundaryMeshFile);
	if (!boundaryFile.open(QIODevice::ReadOnly))
		{
		 return 0;
		}
	QTextStream textstream(&boundaryFile);
	while (!textstream.atEnd())
	{
					int buf,buf1,buf2,buf3,buf4,buf5;
					textstream >> buf;//读取编号Element_i
					if (textstream.atEnd()){
						break;
					}

					textstream >> buf5;
					textstream >> buf5;
					textstream >> buf1;//获取第一个节点
					textstream >> buf2;//获取第二个节点
					textstream >> buf3;//获取第三个节点
					textstream >> buf4;//获取第四个节点
					if (buf%mm==0){
						if (N < 50){
						}
					}
					m1=X+nu+Y+nu+Z;
					Gridstream << buf << nu << buf1 << nu << buf2 << nu << buf3 << nu<< buf4 << nu << m1 << nu << m1<< nu <<m1 << nu << m1 << "\n";//写入
					QCoreApplication::processEvents();
	}
	QApplication::restoreOverrideCursor();
	boundaryFile.close();
	gridFile.close();
	return N;
	
}//GriddedNOMaterials
static string Booleanoperationinformation(string booleanname)
{
	string boolname="NULL";
	//遍历几何体
	App::Document* docs = App::GetApplication().getActiveDocument();
	std::vector<App::DocumentObject*> sels = docs->getObjectsOfType(App::DocumentObject::getClassTypeId());
	for (std::vector<App::DocumentObject*>::iterator it = sels.begin(); it != sels.end(); ++it)
	{//遍历工程树
		std::string Group = (*it)->getNameInDocument();
		QString Groups = QString::fromStdString(Group);
		if (Groups.left(4) == QObject::tr("Line")){
			continue;
		}
		if (Groups.left(5) == QObject::tr("Plane")){
			continue;
		}
		{// 判断是否不可见的，不可见的跳过
			Gui::ViewProvider* pcProv = Gui::Application::Instance->getViewProvider(*it);
			bool visibility = true;
			if (pcProv) {
				visibility = ((Gui::ViewProviderDocumentObject*)pcProv)->Visibility.getValue();
			}
			if (visibility == false){// 不可见的跳过
				if (!((Groups.left(3) == QObject::tr("Cut")) || (Groups.left(6) == QObject::tr("Common")) || (Groups.left(6) == QObject::tr("Fusion")))){
				    continue;
				}
				
				
			}
		}
		if(booleanname==Group){
			App::GeoFeature* geometry = static_cast<App::GeoFeature*>(*it);
			const Base::Placement pos = geometry->Placement.getValue();
			double x = pos.getPosition().x;
			double y = pos.getPosition().y;
			double z = pos.getPosition().z;
			double V1 = pos.getRotation()[0];
			double V2 = pos.getRotation()[1];
			double V3 = pos.getRotation()[2];
			double V4 = pos.getRotation()[3];
			double angle;
			Base::Vector3d di;
			pos.getRotation().getValue(di, angle);
			angle = Base::toDegrees<double>(angle);
			//QString box = QObject::tr("Box(%1) = {%2, %3, %4, %5, %6, %7};\n").arg(x).arg(y).arg(z).arg(l).arg(w).arg(h);
			//Savestreamfile << box;
			QString Rotation=QObject::tr("%1,%2,%3,Pi*%4,%5,%6,%7").arg(di.x).arg(di.y).arg(di.z).arg(angle/180).arg(x).arg(y).arg(z);
			boolname = Rotation.toStdString();
		}
	}
	if(boolname!="NULL"){
		return boolname;
	}
	else{
		return "NULL";
	}
}//Boolean operation information
//geo生成时判断几何体是否接触
static bool Booleanunionoperation(string box1,string box2)
{
	
	App::Document* docs = App::GetApplication().getActiveDocument();
	std::vector<App::DocumentObject*> sels = docs->getObjectsOfType(App::DocumentObject::getClassTypeId());
	for (std::vector<App::DocumentObject*>::iterator it = sels.begin(); it != sels.end(); ++it)
	{//遍历工程树
		std::string Group = (*it)->getNameInDocument();
		QString Groups = QString::fromStdString(Group);
		if(box1!=Group&&box2!=Group){
			continue;
		}
		if (Groups.left(4) == QObject::tr("Line")){
			continue;
		}
		if (Groups.left(5) == QObject::tr("Plane")){
			continue;
		}
		/* {// 判断是否不可见的，不可见的跳过
			Gui::ViewProvider* pcProv = Gui::Application::Instance->getViewProvider(*it);
			bool visibility = true;
			if (pcProv) {
				visibility = ((Gui::ViewProviderDocumentObject*)pcProv)->Visibility.getValue();
			}
			if (visibility == false){// 不可见的跳过
				continue;
			}
		} */
		App::DocumentObject *Obj = (*it);
		const TopoDS_Shape& tShape = static_cast<Part::Feature *>(Obj)->Shape.getValue();
		BRepClass3d_SolidClassifier solidClassifier(tShape);
		//
		
		App::Document* doc= App::GetApplication().getActiveDocument();
		std::vector<App::DocumentObject*> sel = doc->getObjectsOfType(App::DocumentObject::getClassTypeId());
		for (std::vector<App::DocumentObject*>::iterator its = sel.begin(); its != sel.end(); ++its)
		{//遍历工程树
			std::string Grou= (*its)->getNameInDocument();
			QString Gro= QString::fromStdString(Grou);
			if(box1!=Grou&&box2!=Grou){
				continue;
			}
			if(Gro==Groups){
				continue;
			}
			if (Gro.left(4) == QObject::tr("Line")){
				continue;
			}
			if (Gro.left(5) == QObject::tr("Plane")){
				continue;
			}
			/* {// 判断是否不可见的，不可见的跳过
				Gui::ViewProvider* pcProv = Gui::Application::Instance->getViewProvider(*its);
				bool visibility = true;
				if (pcProv) {
					visibility = ((Gui::ViewProviderDocumentObject*)pcProv)->Visibility.getValue();
				}
				if (visibility == false){// 不可见的跳过
					continue;
				}
			} */
			
			App::DocumentObject *Objs = (*its);
			const TopoDS_Shape& tShapes = static_cast<Part::Feature *>(Objs)->Shape.getValue();
			TopExp_Explorer countFace;
			//int facecount = 0;
			for (countFace.Init(tShapes, TopAbs_FACE); countFace.More(); countFace.Next())
			{
			
				const TopoDS_Face &actFaces = TopoDS::Face(countFace.Current());
				BRepAdaptor_Surface surfacec(actFaces);
				
				TopExp_Explorer countEdge;
				
				for (countEdge.Init(countFace.Current(), TopAbs_EDGE); countEdge.More(); countEdge.Next())
				{
					const TopoDS_Edge &actEdge = TopoDS::Edge(countEdge.Current());
					TopoDS_Vertex vertex = TopExp::FirstVertex(actEdge);
					gp_Pnt pnt = BRep_Tool::Pnt(vertex);
					solidClassifier.Perform(pnt, 0.001);
					Standard_Boolean test1 = (solidClassifier.State()!= TopAbs_OUT);
					if(test1==true){
						//Base::Console().Message("group=%s==%s",Gro.toStdString().c_str(),Groups.toStdString().c_str());
						return true;
					}
				}
			}
		}//for its
		
	}//for it
	return false;
}

static QStringList Sortgeo(QString mlist, int n, QStringList geolist)
{
	if(n==geolist.size()){
		return geolist;
	}
		App::Document* docs = App::GetApplication().getActiveDocument();
	//Base::Console().Message("geolist.size()==%d,n=%d\n",geolist.size(),n);
	std::vector<App::DocumentObject*> sels = docs->getObjectsOfType(App::DocumentObject::getClassTypeId());
	for (std::vector<App::DocumentObject*>::iterator it = sels.begin(); it != sels.end(); ++it)
	{
		std::string Group = (*it)->getNameInDocument();
		QString Groups = QString::fromStdString(Group);
		if (Groups.left(5) == QObject::tr("Group")){
			//Base::Console().Message("Group==Group\n");
			continue;
		}
		if (Groups.left(5) == QObject::tr("Plane")){
			//Base::Console().Message("Group==Plane\n");
			continue;
		}
		std::string line = (*it)->getNameInDocument();
		QString lines = QString::fromStdString(line);
		if (lines.left(4) == QObject::tr("Line")){
			//Base::Console().Message("line==line\n");
			continue;
		}
		Gui::ViewProvider* pcProv = Gui::Application::Instance->getViewProvider(*it);
		bool visibility = true;
		if (pcProv /*&& pcProv->getTypeId().isDerivedFrom(Gui::ViewProviderDocumentObject::getClassTypeId())*/) {
			visibility = ((Gui::ViewProviderDocumentObject*)pcProv)->Visibility.getValue();
		}
		// 读取几何体可见(Visibility)属性值  START  END
		if (visibility == false){// 不可见的跳过
			continue;
		}
		if(mlist==Groups)
		{
			continue;
		}
		//Base::Console().Message("geolist===%s,mlist==%s\n",Group.c_str(),mlist.toStdString().c_str());
		bool tf = CommonFunction::Booleanunionoperation(mlist.toStdString(), Group);
		if(tf){
			//Base::Console().Message("geolist.false\n");
			if(false==geolist.contains(Groups)){
				geolist<<Groups;
				//Base::Console().Message("geolist.size()\n");
				geolist = Sortgeo(Groups, n, geolist);
			}
		}

	}
	return geolist;
}

static void AllPec()
{
	App::Document* docs = App::GetApplication().getActiveDocument();
	//App::DocumentObject *faceObj;
	
	int num=0;
	int allnum=1;
	int max=0;
	QSettings settingsb(QObject::tr("../ini/Boundaries.ini"), QSettings::IniFormat);
	int su = settingsb.value(QObject::tr("PRIORITY/count")).toInt();
	for (int i = 1; i <=su; i++){
		QString pty = QString::fromLatin1("PRIORITY/%1").arg(i);
		QString perfe = settingsb.value(pty).toString();
		QString fivesum = perfe.left(5);
		if((fivesum == QObject::tr("PerfE") == 1)){
			int m= perfe.mid(5).toInt();
			if (m > max){
				max = m;
			}

		}
	}//for
	QString PerfEnum=QObject::tr("PerfE%1").arg(++max);
	std::vector<App::DocumentObject*> sels = docs->getObjectsOfType(App::DocumentObject::getClassTypeId());
	for (std::vector<App::DocumentObject*>::iterator it = sels.begin(); it != sels.end(); ++it)
	{
		std::string Group = (*it)->getNameInDocument();
		QString Groups = QString::fromStdString(Group);
		if (Groups.left(5) == QObject::tr("Group")){
			continue;
		}
		std::string line = (*it)->getNameInDocument();
		QString lines = QString::fromStdString(line);
		if (lines.left(4) == QObject::tr("Line")){
			continue;
		}
		Gui::ViewProvider* pcProv = Gui::Application::Instance->getViewProvider(*it);
		bool visibility = true;
		if (pcProv /*&& pcProv->getTypeId().isDerivedFrom(Gui::ViewProviderDocumentObject::getClassTypeId())*/) {
			visibility = ((Gui::ViewProviderDocumentObject*)pcProv)->Visibility.getValue();
		}
		int visibilityflag=0;
		// 读取几何体可见(Visibility)属性值  START  END
		if (visibility == false){// 不可见的跳过
			//continue;
			visibilityflag=1;
		}
		
		App::DocumentObject *faceObj = (*it);

		const TopoDS_Shape& tShape = static_cast<Part::Feature *>(faceObj)->Shape.getValue();
		/////////////获取面
		TopExp_Explorer expFace;
		//int FaceLocalNo = 0;
		int facenum=0;
		//QList<gp_Pnt> gpList;
		for (expFace.Init(tShape, TopAbs_FACE); expFace.More(); expFace.Next())
		{
			num++;
			
			if(visibilityflag==1){
				continue;
			}
			facenum++;
			
			QStringList fonts;
			TopExp_Explorer expVertex;
			std::string bodyName = faceObj->getNameInDocument();
			//Base::Console().Message("name=%s,face=%d\n", bodyName, facenum);
			QString facename = QObject::tr("Face%1").arg(facenum);
			bool faceflag=true;
			
			//QSettings settingsb(QObject::tr("../ini/Boundaries.ini"), QSettings::IniFormat);
			int sumb = settingsb.value(QObject::tr("PRIORITY/count")).toInt();
			for (int i = 1; i <= sumb; i++)
			{
				//Base::Console().Message("name=%s,face=%d,i=%d\n", bodyName, facenum,i);
				QString str = settingsb.value(QObject::tr("PRIORITY/%1").arg(i)).toString();
				int surfacesnum = settingsb.value(QObject::tr("%1/SurfacesNum").arg(str)).toInt();
				int f = 0;
				for (f = 1; f <= surfacesnum; f++){
					//Base::Console().Message("name=%s,face=%d,f=%d\n", bodyName, facenum,f);
					QString face = settingsb.value(QObject::tr("%1/Surface%2").arg(str).arg(f)).toString();
					QString Box = face.section(QObject::tr(","), 2, 2);
					QString facesnum = face.section(QObject::tr(","), 3, 3);
					if ((Box == QString::fromStdString(bodyName)) && (facesnum == facename))
					{
						//Base::Console().Message("33name=%s,face=%d,f=%d\n", bodyName, facenum,f);
						faceflag=false;
						break;
					}
					
				}
				
			}
	
			QSettings settingse(QObject::tr("../ini/Excitation.ini"), QSettings::IniFormat);
			int sume = settingse.value(QObject::tr("ORDER/Count")).toInt();
			for (int i = 1; i <= sume; i++)
			{
				QString str = settingse.value(QObject::tr("ORDER/%1").arg(i)).toString();
				int surfacesnum = settingse.value(QObject::tr("%1/SurfacesNum").arg(str)).toInt();
				int f = 0;
				for (f = 1; f <= surfacesnum; f++){
					QString face = settingse.value(QObject::tr("%1/Surface%2").arg(str).arg(f)).toString();
					QString Box = face.section(QObject::tr(","), 2, 2);
					QString facesnum = face.section(QObject::tr(","), 3, 3);
					if ((Box == QString::fromStdString(bodyName)) && (facesnum == facename))
					{
						faceflag=false;
						break;
					}

				}
			}
             
			if(faceflag){
				TopExp_Explorer expEdge;
				//Base::Console().Message("4name=%s,face=%d\n", bodyName, facenum);
				//for (expVertex.Init(expFace.Current(), TopAbs_VERTEX); expVertex.More(); expVertex.Next())
				//{
				for (expEdge.Init(expFace.Current(), TopAbs_EDGE); expEdge.More(); expEdge.Next())
				{

					//TopoDS_Vertex  vtx = TopoDS::Vertex(expVertex.Current());
					//gp_Pnt pt = BRep_Tool::Pnt(vtx);
					const TopoDS_Edge &actEdge = TopoDS::Edge(expEdge.Current());
					TopoDS_Vertex vertex = TopExp::FirstVertex(actEdge);
					gp_Pnt pnt = BRep_Tool::Pnt(vertex);
					TopoDS_Vertex vertexs = TopExp::LastVertex(actEdge);
					gp_Pnt pnts = BRep_Tool::Pnt(vertexs);

					//gpList.append(pt);

					//Base::Console().Message("gp point:(%f,%f,%f)", pt.X(), pt.Y(), pt.Z());////////

					QString sss = QString::fromLatin1("%1,%2,%3").arg(QString::number(pnt.X())).arg(QString::number(pnt.Y())).arg(QString::number(pnt.Z()));
					fonts << sss;
					QString ss = QString::fromLatin1("%1,%2,%3").arg(QString::number(pnts.X())).arg(QString::number(pnts.Y())).arg(QString::number(pnts.Z()));
					fonts << ss;
				}//dia
				QStringList str;
				//去除重复的
				int i, j, r = 0;
				for (i = 0; i < fonts.size(); i++) {
					for (j = 0; j < r; j++)
					if (fonts[j] == fonts[i]) break;
					if (j == r)
						str << fonts[i];
					r++;
				}

				QString linn = QString::fromAscii(",");

				QString strlist = str.join(linn);
				//Base::Console().Message("2name=%s,face=%d\n", bodyName, facenum);
				string com = ",";
				QString number = QObject::tr("%1").arg(num);
				QString type = QObject::tr("Part::%1").arg(QString::fromStdString(bodyName));
				std::string searchStrnew = number.toStdString() + com +type.toStdString()+com+std::string(faceObj->getNameInDocument()) + com + facename.toStdString() + com + strlist.toStdString() + com;
				int sumb = settingsb.value(QObject::tr("PRIORITY/count")).toInt();
				if(allnum==1){
					settingsb.setValue(QObject::tr("%1/Type").arg(PerfEnum),QObject::tr("PerfectE"));
				}
				settingsb.setValue(QObject::tr("%1/Surface%2").arg(PerfEnum).arg(allnum),QString::fromStdString(searchStrnew));
				allnum++;
			}
		}
	}
	if(allnum!=1){
		int sumb = settingsb.value(QObject::tr("PRIORITY/count")).toInt();
		settingsb.setValue(QObject::tr("PRIORITY/count"),++sumb);
		settingsb.setValue(QObject::tr("PRIORITY/%1").arg(sumb),PerfEnum);
		settingsb.setValue(QObject::tr("%1/SurfacesNum").arg(PerfEnum),--allnum);
		Gui::Application::Instance->AddProjectTreeObject("MySimIIGui::Boundaries", PerfEnum.toStdString().c_str());
	}
}

static int GriddedMaterialsE(int N, int Meshsum)
{
 // Base::Console().Message("name\n");
	int NN = 45 - N;
	int MM = Meshsum / NN;
	QString GridFile = QString::fromLatin1("../ini/GriddedMaterials.ini");
	QFile gridFile(GridFile);
	if (!gridFile.open(QIODevice::WriteOnly))
	{
		return 0;
	}
  // Base::Console().Message("5name\n");
	QTextStream Gridstream(&gridFile);
	QSettings settings(QObject::tr("../ini/Model.ini"), QSettings::IniFormat);
	int count = settings.value(QObject::tr("Name/count")).toInt();
	///没有设置材料或者材料相同的
	int vum = 0;
	for (int i = 1; i <= count; i++){
		QString name = settings.value(QObject::tr("Name/%1").arg(i)).toString();
		QString countname = settings.value(QObject::tr("%1/material").arg(name)).toString();
		if (countname == QObject::tr("vacuum")){
			vum++;
		}
	}
	if (count == 0 || vum == count){
		Base::Console().Message("GriddedNOMaterials\n");
		int p = GriddedNOMaterials(MM, N);
		return p;
	}
   // Base::Console().Message("6name\n");
	QString BoundaryMeshFile = QString::fromLatin1("../ini/mm/mesh.elements");
	QFile boundaryFile(BoundaryMeshFile);
	if (!boundaryFile.open(QIODevice::ReadOnly))
	{
		return 0;
	}
	QFile boundaryFileM(BoundaryMeshFile);
	if (!boundaryFileM.open(QIODevice::ReadOnly))
	{
		return 0;
	}
	//Base::Console().Message("7name\n");
	QTextStream textstream(&boundaryFile);
	QTextStream elementstream(&boundaryFileM);
	//QMap<int, QStringList> Elmments;
	//QMap<int,int> ElmMap;
	QMap<int,QMap<int, int>> Elmments;
	//Base::Console().Message("2name\n");
	while (!textstream.atEnd())
	{
		int buf, buf22, buf1, buf2, buf3, buf4, buf5;
		textstream >> buf;//读取编号Element_i
		if (textstream.atEnd()){
			break;
		}

		textstream >> buf22;
		textstream >> buf5;
		if (buf5 == 503){
			continue;
		}
		//QStringList list;
		//QString model = QObject::tr("%1").arg(buf22);
		//list.append(model);
		textstream >> buf1;//获取第一个节点

		QMap<int, QMap<int, int>>::Iterator it;
		it = Elmments.find(buf1);
		if (it != Elmments.end()){
			QMap<int,int> list1 = it.value();
			int flag = 0;
			QMap<int, int>::iterator it1;
			for (it1 = list1.begin(); it1 != list1.end(); it1++){
				if (buf22 == it1.key()){//说明这个模块的已经有了
					int num=it1.value();
					flag = 1;
					list1[buf22] = ++num;
					Elmments.insert(buf1,list1);
					break;
				}
			}
			if (flag == 0){//没有找到相同的模块，第一次写入这个模块
				list1.insert(buf22,1);
				Elmments.insert(buf1, list1);
			}
		}
		else{//第一次写入
			QMap<int, int> lists;
			lists.insert(buf22,1);
			Elmments.insert(buf1,lists);
		}

		textstream >> buf2;//获取第二个节点
		//QMap<int, QMap<int, int>>::Iterator it;
		it = Elmments.find(buf2);
		if (it != Elmments.end()){
			QMap<int, int> list2 = it.value();
			int flag = 0;
			QMap<int, int>::iterator it2;
			for (it2 = list2.begin(); it2 != list2.end(); it2++){
				if (buf22 == it2.key()){//说明这个模块的已经有了
					int num = it2.value();
					flag = 1;
					list2[buf22] = ++num;
					Elmments.insert(buf2, list2);
					break;
				}
			}
			if (flag == 0){//没有找到相同的模块，第一次写入这个模块
				list2.insert(buf22, 1);
				Elmments.insert(buf2,list2);
			}
		}
		else{//第一次写入
			QMap<int, int> lists;
			lists.insert(buf22, 1);
			Elmments.insert(buf2, lists);
		}

		textstream >> buf3;//获取第三个节点
		it = Elmments.find(buf3);
		if (it != Elmments.end()){
			QMap<int, int> list3 = it.value();
			int flag = 0;
			QMap<int, int>::iterator it3;
			for (it3 = list3.begin(); it3 != list3.end(); it3++){
				if (buf22 == it3.key()){//说明这个模块的已经有了
					int num = it3.value();
					flag = 1;
					list3[buf22] = ++num;
					Elmments.insert(buf3, list3);
					break;
				}
			}
			if (flag == 0){//没有找到相同的模块，第一次写入这个模块
				list3.insert(buf22, 1);
				Elmments.insert(buf3, list3);
			}
		}
		else{//第一次写入
			QMap<int, int> lists;
			lists.insert(buf22, 1);
			Elmments.insert(buf3, lists);
		}
		textstream >> buf4;//获取第四个节点
		it = Elmments.find(buf4);
		if (it != Elmments.end()){
			QMap<int, int> list4 = it.value();
			int flag = 0;
			QMap<int, int>::iterator it4;
			for (it4 = list4.begin(); it4 != list4.end(); it4++){
				if (buf22 == it4.key()){//说明这个模块的已经有了
					int num = it4.value();
					flag = 1;
					list4[buf22] = ++num;
					Elmments.insert(buf4, list4);
					break;
				}
			}
			if (flag == 0){//没有找到相同的模块，第一次写入这个模块
				list4.insert(buf22, 1);
				Elmments.insert(buf4, list4);
			}
		}
		else{//第一次写入
			QMap<int, int> lists;
			lists.insert(buf22, 1);
			Elmments.insert(buf4, lists);
		}


	}
	boundaryFile.close();
	//Base::Console().Message("3name\n");
	QString nu = QObject::tr(" ");
	while (!elementstream.atEnd())
	{
		int buf, buf22, buf1, buf2, buf3, buf4, buf5;
		elementstream >> buf;//读取编号Element_i
		if (elementstream.atEnd()){
			break;
		}
		
		elementstream >> buf22;
		elementstream >> buf5;
		if (buf5 == 503){
			continue;
		}
		
		QMap<int, QMap<int, int>>::Iterator it;
		elementstream >> buf1;//获取第一个节点
		it = Elmments.find(buf1);
		QMap<int, int> list1 = it.value();
		
		elementstream >> buf2;//获取第二个节点
		it = Elmments.find(buf2);
		QMap<int, int> list2 = it.value();
		
		elementstream >> buf3;//获取第三个节点
		it = Elmments.find(buf3);
		QMap<int, int> list3 = it.value();
		
		elementstream >> buf4;//获取第四个节点
		it = Elmments.find(buf4);
		QMap<int, int> list4 = it.value();
		
		//QMap<int, int> mapdoel;
		QMap<int, int>::Iterator its;
		int max1 = 0;
		int modelnum1 = 0;
		int max2 = 0;
		int modelnum2 = 0;
		int max3 = 0;
		int modelnum3 = 0;
		int max4= 0;
		int modelnum4= 0;
		for (its = list1.begin(); its != list1.end(); its++){
		    if (its.value() > max1){
				max1 = its.value();
				modelnum1 = its.key();
			}
		}

		for (its = list2.begin(); its != list2.end(); its++){
			if (its.value() > max2){
				max2 = its.value();
				modelnum2 = its.key();
			}
		}

		for (its = list3.begin(); its != list3.end(); its++){
			if (its.value() > max3){
				max3 = its.value();
				modelnum3 = its.key();
			}
		}

		for (its = list4.begin(); its != list4.end(); its++){
			if (its.value() > max4){
				max4 = its.value();
				modelnum4 = its.key();
			}
		}

		QSettings settingsNo(QObject::tr("../ini/ModelNo.ini"), QSettings::IniFormat);
		//int count = settingsNO.value(QObject::tr("Name/count")).toInt();
		QString Matermodel1 = settingsNo.value(QObject::tr("%1/ModelName").arg(modelnum1)).toString();
		QString Matermodel2 = settingsNo.value(QObject::tr("%1/ModelName").arg(modelnum2)).toString();
		QString Matermodel3 = settingsNo.value(QObject::tr("%1/ModelName").arg(modelnum3)).toString();
		QString Matermodel4 = settingsNo.value(QObject::tr("%1/ModelName").arg(modelnum4)).toString();

		QString cX, cY, cZ;
		QString X, Y, Z;
		double x = 1.0, y = 1.0, z = 0;
		X.sprintf("%16.9e", x);
		Y.sprintf("%16.9e", y);
		Z.sprintf("%16.9e", z);
		QString m1, m2, m3, m4;
		//QString n1 = QObject::tr("1");
		//QString n0 = QObject::tr("0");
		QString RelativePermittivityG1, RelativePermeabilityG1, BulkConductivityG1;
		QString RelativePermittivityG2, RelativePermeabilityG2, BulkConductivityG2;
		QString RelativePermittivityG3, RelativePermeabilityG3, BulkConductivityG3;
		QString RelativePermittivityG4, RelativePermeabilityG4, BulkConductivityG4;
		int flag1 = 0, flag2 = 0, flag3 = 0, flag4 = 0;
		for (int j = 1; j <= count; j++){//获取材料属性
			QString Bname = settings.value(QObject::tr("Name/%1").arg(j)).toString();
			if (Bname == Matermodel1){
				RelativePermittivityG1 = settings.value(QObject::tr("%1/RelativePermittivity").arg(Bname)).toString();
				RelativePermeabilityG1 = settings.value(QObject::tr("%1/RelativePermeability").arg(Bname)).toString();
				BulkConductivityG1 = settings.value(QObject::tr("%1/BulkConductivity").arg(Bname)).toString();
				flag1 = 1;
			}
			if (Bname == Matermodel2){
				RelativePermittivityG2 = settings.value(QObject::tr("%1/RelativePermittivity").arg(Bname)).toString();
				RelativePermeabilityG2 = settings.value(QObject::tr("%1/RelativePermeability").arg(Bname)).toString();
				BulkConductivityG2 = settings.value(QObject::tr("%1/BulkConductivity").arg(Bname)).toString();
				flag2 = 1;
			}
			if (Bname == Matermodel3){
				RelativePermittivityG3 = settings.value(QObject::tr("%1/RelativePermittivity").arg(Bname)).toString();
				RelativePermeabilityG3 = settings.value(QObject::tr("%1/RelativePermeability").arg(Bname)).toString();
				BulkConductivityG3 = settings.value(QObject::tr("%1/BulkConductivity").arg(Bname)).toString();
				flag3 = 1;
			}
			if (Bname == Matermodel4){
				RelativePermittivityG4 = settings.value(QObject::tr("%1/RelativePermittivity").arg(Bname)).toString();
				RelativePermeabilityG4 = settings.value(QObject::tr("%1/RelativePermeability").arg(Bname)).toString();
				BulkConductivityG4 = settings.value(QObject::tr("%1/BulkConductivity").arg(Bname)).toString();
				flag4 = 1;
			}
		}

		if (flag1 == 0){
			m1 = X + nu + Y + nu + Z;
		}
		else{
			cX.sprintf("%16.9e", RelativePermittivityG1.toDouble());
			cY.sprintf("%16.9e", RelativePermeabilityG1.toDouble());
			cZ.sprintf("%16.9e", BulkConductivityG1.toDouble());
			m1 = cX + nu + cY + nu + cZ;
		}

		if (flag2 == 0){
			m2 = X + nu + Y + nu + Z;
		}
		else{
			cX.sprintf("%16.9e", RelativePermittivityG2.toDouble());
			cY.sprintf("%16.9e", RelativePermeabilityG2.toDouble());
			cZ.sprintf("%16.9e", BulkConductivityG2.toDouble());
			m2 = cX + nu + cY + nu + cZ;
		}

		if (flag3 == 0){
			m3 = X + nu + Y + nu + Z;
		}
		else{
			cX.sprintf("%16.9e", RelativePermittivityG3.toDouble());
			cY.sprintf("%16.9e", RelativePermeabilityG3.toDouble());
			cZ.sprintf("%16.9e", BulkConductivityG3.toDouble());
			m3 = cX + nu + cY + nu + cZ;
		}

		if (flag4 == 0){
			m4 = X + nu + Y + nu + Z;
		}
		else{
			cX.sprintf("%16.9e", RelativePermittivityG4.toDouble());
			cY.sprintf("%16.9e", RelativePermeabilityG4.toDouble());
			cZ.sprintf("%16.9e", BulkConductivityG4.toDouble());
			m4 = cX + nu + cY + nu + cZ;
		}
		
		Gridstream << buf << nu << buf1 << nu << buf2 << nu << buf3 << nu << buf4 << nu << m1 << nu << m2 << nu << m3 << nu << m4 << "\n";
		//Gridstream << buf << nu << buf1 << nu << buf2 << nu << buf3 << nu << buf4 << nu << modelnum1 << nu << modelnum2 << nu << modelnum3 << nu << modelnum4 << "\n";
		//Gridstream << buf << " " << buf1 << " " << buf2 << " " << buf3 << " " << buf4 << " " << Matermodel1<< " " << Matermodel2<< " " << Matermodel3 << " " <<Matermodel4<< "\n";
		QCoreApplication::processEvents();
	}//while
  // Base::Console().Message("4name\n");
   boundaryFileM.close();
	gridFile.close();
	return N;

}

};


//===========================================================================
// CmdMySimII_Test THIS IS JUST A TEST COMMAND
//===========================================================================
DEF_STD_CMD(CmdMySimII_Test);

CmdMySimII_Test::CmdMySimII_Test()
  :Command("MySimII_Test")
{
    sAppModule    = "MySimII";
    sGroup        = QT_TR_NOOP("MySimII");
    sMenuText     = QT_TR_NOOP("Hello");
    sToolTipText  = QT_TR_NOOP("MySimII_Test Test function");
    sWhatsThis    = QT_TR_NOOP("MySimII_Test Test function");
    sStatusTip    = QT_TR_NOOP("MySimII_Test Test function");
    sPixmap       = "log09";
    sAccel        = "CTRL+H";
}

void CmdMySimII_Test::activated(int iMsg)
{
    Base::Console().Message("Hello, World!\n");

}

DEF_STD_CMD(CmdBoundaries);

CmdBoundaries::CmdBoundaries()
  :Command("MySimII_Boundaries")
{
    sAppModule    = "MySimII";
    sGroup        = QT_TR_NOOP("MySimII");
    sMenuText     = QT_TR_NOOP("Boundaries");
    sToolTipText  = QT_TR_NOOP("Boundaries Test function");
    sWhatsThis    = QT_TR_NOOP("Boundaries Test function");
    sStatusTip    = QT_TR_NOOP("Boundaries Test function");
    sPixmap       = "log06";
    sAccel        = "CTRL+B";
}

void CmdBoundaries::activated(int iMsg)
{
	std::vector<string> faceStr = CommonFunction::GetFaceSelected(false, true);
	if (faceStr.size()<=0){
		return;
	}
	else{
		MySimIIGui::Boundaries::reviseflag = 0;
		static MySimIIGui::Boundaries p;
		p.show();
	}

}



//...........................Add Solution Setup....................................................................
DEF_STD_CMD(CmdaddSolution);

CmdaddSolution::CmdaddSolution()
  :Command("TimeDomain")
{
	sAppModule = "MySimII";
	sGroup = QT_TR_NOOP("MySimII");
	sMenuText = QT_TR_NOOP("TimeDomain");
	sToolTipText = QT_TR_NOOP("TimeDomain");
	sWhatsThis = "TimeDomain";
	sStatusTip = sToolTipText;
	sPixmap       = "log09";
}

void CmdaddSolution::activated(int iMsg)
{
	static MySimIIGui::SolutionSetup p;
	p.show();

	
}

//.........................................................................................

//...........................Add Frequency Sweep....................................................................
DEF_STD_CMD(CmdaddFrequencySweep);

CmdaddFrequencySweep::CmdaddFrequencySweep()
:Command("FreqDomain")
{
	sAppModule = "MySimII";
	sGroup = QT_TR_NOOP("MySimII");
	sMenuText = QT_TR_NOOP("FreqDomain");
	sToolTipText = QT_TR_NOOP("FreqDomain");
	sWhatsThis = "FreqDomain";
	sStatusTip = sToolTipText;
	sPixmap       = "log09";
}

void CmdaddFrequencySweep::activated(int iMsg)
{
	static MySimIIGui::FrequencySweep p;
	p.show();
	
}

DEF_STD_CMD(CmdAdaptiveMeshing);

CmdAdaptiveMeshing::CmdAdaptiveMeshing()
  :Command("AdaptiveMeshing")
{
    sAppModule    = "MySimII";
    sGroup        = QT_TR_NOOP("MySimII");
    sMenuText     = QT_TR_NOOP("AdaptiveMeshing");
    sToolTipText  = QT_TR_NOOP("AdaptiveMeshing Test function");
    sWhatsThis    = QT_TR_NOOP("AdaptiveMeshing Test function");
    sStatusTip    = QT_TR_NOOP("AdaptiveMeshing Test function");
    sPixmap       = "log09";
    //sAccel        = "CTRL+H";
}

void CmdAdaptiveMeshing::activated(int iMsg)
{
    Base::Console().Message("Hello, World!\n");

}



DEF_STD_CMD(CmdModularMeshing);

CmdModularMeshing::CmdModularMeshing()
  :Command("ModularMeshing")
{
    sAppModule    = "MySimII";
    sGroup        = QT_TR_NOOP("MySimII");
    sMenuText     = QT_TR_NOOP("ModularMeshing");
    sToolTipText  = QT_TR_NOOP("ModularMeshing Test function");
    sWhatsThis    = QT_TR_NOOP("ModularMeshing Test function");
    sStatusTip    = QT_TR_NOOP("ModularMeshing Test function");
    sPixmap       = "log09";
    //sAccel        = "CTRL+H";
}

void CmdModularMeshing::activated(int iMsg)
{
    Base::Console().Message("Hello ModularMeshing!\n");

}



DEF_STD_CMD(CmdMeshSetting);

CmdMeshSetting::CmdMeshSetting()
  :Command("MySimII_MeshSetting")
{
    sAppModule    = "MySimII";
    sGroup        = QT_TR_NOOP("MySimII");
    sMenuText     = QT_TR_NOOP("MeshSetting");
    sToolTipText  = QT_TR_NOOP("MeshSetting Test function");
    sWhatsThis    = QT_TR_NOOP("MeshSetting Test function");
    sStatusTip    = QT_TR_NOOP("MeshSetting Test function");
    sPixmap       = "log02";
    sAccel        = "CTRL+M";
}

void CmdMeshSetting::activated(int iMsg)
{
    static MySimIIGui::InitialMesh p;
	p.exec();

}


DEF_STD_CMD(CmdMeshView);

CmdMeshView::CmdMeshView()
  :Command("MySimII_MeshViews")
{
    sAppModule    = "MySimII";
    sGroup        = QT_TR_NOOP("MySimII");
    sMenuText     = QT_TR_NOOP("MeshView");
    sToolTipText  = QT_TR_NOOP("MeshView Test function");
    sWhatsThis    = QT_TR_NOOP("MeshView Test function");
    sStatusTip    = QT_TR_NOOP("MeshView Test function");
    sPixmap       = "log02";
//    sAccel        = "CTRL+V";
}

void CmdMeshView::activated(int iMsg)
{
    MySimIIGui::MeshView p;
	p.exec();

}


DEF_STD_CMD(CmdRemoteRun);

CmdRemoteRun::CmdRemoteRun()
  :Command("MySimII_RemoteRun")
{
    sAppModule    = "MySimII";
    sGroup        = QT_TR_NOOP("MySimII");
    sMenuText     = QT_TR_NOOP("RemoteRun");
    sToolTipText  = QT_TR_NOOP("RemoteRun Test function");
    sWhatsThis    = QT_TR_NOOP("RemoteRun Test function");
    sStatusTip    = QT_TR_NOOP("RemoteRun Test function");
    sPixmap       = "log04";
    sAccel        = "CTRL+R";
}

void CmdRemoteRun::activated(int iMsg)
{
    QString s = QObject::tr("../SolverConfig/sample.hta");
	QProcess::execute(QObject::tr("../SolverConfig/sample.bat"));
}




//...........................Mag_Real_E....................................................................
DEF_STD_CMD(CmdaddMag_Real_E);

CmdaddMag_Real_E::CmdaddMag_Real_E()
:Command("Mag_Real_E")
{
	sAppModule = "MySimII";
	sGroup = QT_TR_NOOP("MySimII");
	sMenuText = QT_TR_NOOP("Mag_Real_E");
	sToolTipText = QT_TR_NOOP("Mag_Real_E");
	sWhatsThis = "Mag_Real_E";
	sStatusTip = sToolTipText;
}

void CmdaddMag_Real_E::activated(int iMsg)
{

	//std::vector<std::string> searchStr;

	std::vector<string> searchStr = CommonFunction::GetFaceSelected();
	if (searchStr.size())
	{
		MySimIIGui::SelectFrequency p;
		p.flag = 1;
		p.exec();
	}
	else
	{
		
		return;
	} 

}

//.........................................................................................



//...........................Mag_Imag_E....................................................................
DEF_STD_CMD(CmdaddMag_Imag_E);

CmdaddMag_Imag_E::CmdaddMag_Imag_E()
:Command("Mag_Imag_E")
{
	sAppModule = "MySimII";
	sGroup = QT_TR_NOOP("MySimII");
	sMenuText = QT_TR_NOOP("Mag_Imag_E");
	sToolTipText = QT_TR_NOOP("Mag_Imag_E");
	sWhatsThis = "Mag_Imag_E";
	sStatusTip = sToolTipText;
}

void CmdaddMag_Imag_E::activated(int iMsg)
{

	std::vector<string> searchStr = CommonFunction::GetFaceSelected();
	if (searchStr.size())
	{
		MySimIIGui::SelectFrequency p;
		p.flag = 2;
		p.exec();
	}
	else
	{
		
		return;
	} 

}

//.........................................................................................


//...........................Mag_Comp_E....................................................................
DEF_STD_CMD(CmdaddMag_Comp_E);

CmdaddMag_Comp_E::CmdaddMag_Comp_E()
:Command("Mag_Comp_E")
{
	sAppModule = "MySimII";
	sGroup = QT_TR_NOOP("MySimII");
	sMenuText = QT_TR_NOOP("Mag_Comp_E");
	sToolTipText = QT_TR_NOOP("Mag_Comp_E");
	sWhatsThis = "Mag_Comp_E";
	sStatusTip = sToolTipText;
}

void CmdaddMag_Comp_E::activated(int iMsg)
{

	std::vector<string> searchStr = CommonFunction::GetFaceSelected();
	if (searchStr.size())
	{
		MySimIIGui::SelectFrequency p;
		p.flag = 3;
		p.exec();
	}
	else
	{
		
		return;
	} 

}

//.........................................................................................


//...........................Vector_E....................................................................
DEF_STD_CMD(CmdaddVector_E);

CmdaddVector_E::CmdaddVector_E()
:Command("Vector_E")
{
	sAppModule = "MySimII";
	sGroup = QT_TR_NOOP("MySimII");
	sMenuText = QT_TR_NOOP("Vector_E");
	sToolTipText = QT_TR_NOOP("Vector_E");
	sWhatsThis = "Vector_E";
	sStatusTip = sToolTipText;
}

void CmdaddVector_E::activated(int iMsg)
{

	std::vector<string> searchStr = CommonFunction::GetFaceSelected();
	if (searchStr.size())
	{
		MySimIIGui::SelectFrequency p;
		p.flag = 4;
		p.exec();
	}
	else
	{
		
		return;
	} 

}

//.........................................................................................

//...........................Re_Ex....................................................................
DEF_STD_CMD(CmdGlyphs);

CmdGlyphs::CmdGlyphs()
:Command("MySimII_Glyphs")
{
	sAppModule = "MySimII";
	sGroup = QT_TR_NOOP("MySimII");
	sMenuText = QT_TR_NOOP("Glyphs");
	sToolTipText = QT_TR_NOOP("Glyphs");
	sWhatsThis = "Glyphs";
	sStatusTip = sToolTipText;
        sPixmap    = "log10";
        //sAccel     = "CTRL+H";
}

void CmdGlyphs::activated(int iMsg)
{

	static MySimIIGui::Glyphs p;
	p.show();

}

DEF_STD_CMD(CmdContours);

CmdContours::CmdContours()
:Command("MySimII_Contours")
{
	sAppModule = "MySimII";
	sGroup = QT_TR_NOOP("Results");
	sMenuText = QT_TR_NOOP("&Contours");
	sToolTipText = QT_TR_NOOP("Contours");
	sWhatsThis = "MySimII_Contours";
	sStatusTip = sToolTipText;
        sPixmap    = "log10";
        //sAccel     = "CTRL+H";
}

void CmdContours::activated(int iMsg)
{

//    Q_UNUSED(iMsg);
//  Application::Instance->slotUndo();
//  getGuiApplication()->sendMsgToActiveView("Contours");
	static MySimIIGui::Contours p;
	p.show();

}

//.........................................................................................

//...........................Re_Ey....................................................................
DEF_STD_CMD(CmdaddRe_Ey);

CmdaddRe_Ey::CmdaddRe_Ey()
:Command("Re_Ey")
{
	sAppModule = "MySimII";
	sGroup = QT_TR_NOOP("MySimII");
	sMenuText = QT_TR_NOOP("Re_Ey");
	sToolTipText = QT_TR_NOOP("Re_Ey");
	sWhatsThis = "Re_Ey";
	sStatusTip = sToolTipText;
}

void CmdaddRe_Ey::activated(int iMsg)
{

	std::vector<string> searchStr = CommonFunction::GetFaceSelected();
	if (searchStr.size())
	{
		MySimIIGui::SelectFrequency p;
		p.flag = 6;
		p.exec();
	}
	else
	{
		
		return;
	}
 
}

//.........................................................................................
//...........................Re_Ez....................................................................
DEF_STD_CMD(CmdaddRe_Ez);

CmdaddRe_Ez::CmdaddRe_Ez()
:Command("Re_Ez")
{
	sAppModule = "MySimII";
	sGroup = QT_TR_NOOP("MySimII");
	sMenuText = QT_TR_NOOP("Re_Ez");
	sToolTipText = QT_TR_NOOP("Re_Ez");
	sWhatsThis = "Re_Ez";
	sStatusTip = sToolTipText;
}

void CmdaddRe_Ez::activated(int iMsg)
{

	std::vector<string> searchStr = CommonFunction::GetFaceSelected();
	if (searchStr.size())
	{
		MySimIIGui::SelectFrequency p;
		p.flag = 7;
		p.exec();
	}
	else
	{
		
		return;
	} 

}

//.........................................................................................
//...........................Im_Ex....................................................................
DEF_STD_CMD(CmdaddIm_Ex);

CmdaddIm_Ex::CmdaddIm_Ex()
:Command("Im_Ex")
{
	sAppModule = "MySimII";
	sGroup = QT_TR_NOOP("MySimII");
	sMenuText = QT_TR_NOOP("Im_Ex");
	sToolTipText = QT_TR_NOOP("Im_Ex");
	sWhatsThis = "Im_Ex";
	sStatusTip = sToolTipText;
}

void CmdaddIm_Ex::activated(int iMsg)
{

	std::vector<string> searchStr = CommonFunction::GetFaceSelected();
	if (searchStr.size())
	{
		MySimIIGui::SelectFrequency p;
		p.flag = 8;
		p.exec();
	}
	else
	{
		
		return;
	}

}

//.........................................................................................
//...........................Im_Ey....................................................................
DEF_STD_CMD(CmdaddIm_Ey);

CmdaddIm_Ey::CmdaddIm_Ey()
:Command("Im_Ey")
{
	sAppModule = "MySimII";
	sGroup = QT_TR_NOOP("MySimII");
	sMenuText = QT_TR_NOOP("Im_Ey");
	sToolTipText = QT_TR_NOOP("Im_Ey");
	sWhatsThis = "Im_Ey";
	sStatusTip = sToolTipText;
}

void CmdaddIm_Ey::activated(int iMsg)
{

	std::vector<string> searchStr = CommonFunction::GetFaceSelected();
	if (searchStr.size())
	{
		MySimIIGui::SelectFrequency p;
		p.flag = 9;
		p.exec();
	}
	else
	{
		
		return;
	} 
	
}

//.........................................................................................
//...........................Im_Ez....................................................................
DEF_STD_CMD(CmdaddIm_Ez);

CmdaddIm_Ez::CmdaddIm_Ez()
:Command("Im_Ez")
{
	sAppModule = "MySimII";
	sGroup = QT_TR_NOOP("MySimII");
	sMenuText = QT_TR_NOOP("Im_Ez");
	sToolTipText = QT_TR_NOOP("Im_Ez");
	sWhatsThis = "Im_Ez";
	sStatusTip = sToolTipText;
}

void CmdaddIm_Ez::activated(int iMsg)
{
	std::vector<string> searchStr = CommonFunction::GetFaceSelected();
	if (searchStr.size())
	{
		MySimIIGui::SelectFrequency p;
		p.flag = 10;
		p.exec();
	}
	else
	{
		
		return;
	} 
	
}



//...........................Mag_Real_H....................................................................
DEF_STD_CMD(CmdaddMag_Real_H);

CmdaddMag_Real_H::CmdaddMag_Real_H()
:Command("Mag_Real_H")
{
	sAppModule = "MySimII";
	sGroup = QT_TR_NOOP("MySimII");
	sMenuText = QT_TR_NOOP("Mag_Real_H");
	sToolTipText = QT_TR_NOOP("Mag_Real_H");
	sWhatsThis = "Mag_Real_H";
	sStatusTip = sToolTipText;
}

void CmdaddMag_Real_H::activated(int iMsg)
{

	std::vector<string> searchStr = CommonFunction::GetFaceSelected();
	if (searchStr.size())
	{
		MySimIIGui::SelectFrequency p;
		p.flag = 11;
		p.exec();
	}
	else
	{
		
		return;
	} 

}

//.........................................................................................
//...........................Mag_Imag_H....................................................................
DEF_STD_CMD(CmdaddMag_Imag_H);

CmdaddMag_Imag_H::CmdaddMag_Imag_H()
:Command("Mag_Imag_H")
{
	sAppModule = "MySimII";
	sGroup = QT_TR_NOOP("MySimII");
	sMenuText = QT_TR_NOOP("Mag_Imag_H");
	sToolTipText = QT_TR_NOOP("Mag_Imag_H");
	sWhatsThis = "Mag_Imag_H";
	sStatusTip = sToolTipText;
}

void CmdaddMag_Imag_H::activated(int iMsg)
{
	std::vector<string> searchStr = CommonFunction::GetFaceSelected();
	if (searchStr.size())
	{
		MySimIIGui::SelectFrequency p;
		p.flag = 12;
		p.exec();
	}
	else
	{
		
		return;
	}
 
}

//.........................................................................................
//...........................Mag_Comp_H....................................................................
DEF_STD_CMD(CmdaddMag_Comp_H);

CmdaddMag_Comp_H::CmdaddMag_Comp_H()
:Command("Mag_Comp_H")
{
	sAppModule = "MySimII";
	sGroup = QT_TR_NOOP("MySimII");
	sMenuText = QT_TR_NOOP("Mag_Comp_H");
	sToolTipText = QT_TR_NOOP("Mag_Comp_H");
	sWhatsThis = "Mag_Comp_H";
	sStatusTip = sToolTipText;
}

void CmdaddMag_Comp_H::activated(int iMsg)
{

	std::vector<string> searchStr = CommonFunction::GetFaceSelected();
	if (searchStr.size())
	{
		MySimIIGui::SelectFrequency p;
		p.flag = 13;
		p.exec();
	}
	else
	{
	
		return;
	} 

}

//.........................................................................................
//...........................Vector_H....................................................................
DEF_STD_CMD(CmdaddVector_H);

CmdaddVector_H::CmdaddVector_H()
:Command("Vector_H")
{
	sAppModule = "MySimII";
	sGroup = QT_TR_NOOP("MySimII");
	sMenuText = QT_TR_NOOP("Vector_H");
	sToolTipText = QT_TR_NOOP("Vector_H");
	sWhatsThis = "Vector_H";
	sStatusTip = sToolTipText;
}

void CmdaddVector_H::activated(int iMsg)
{
	std::vector<string> searchStr = CommonFunction::GetFaceSelected();
	if (searchStr.size())
	{
		MySimIIGui::SelectFrequency p;
		p.flag = 14;
		p.exec();
	}
	else
	{
		
		return;
	} 
	
}

//.........................................................................................
//...........................Re_Hx....................................................................
DEF_STD_CMD(CmdaddRe_Hx);

CmdaddRe_Hx::CmdaddRe_Hx()
:Command("Re_Hx")
{
	sAppModule = "MySimII";
	sGroup = QT_TR_NOOP("MySimII");
	sMenuText = QT_TR_NOOP("Re_Hx");
	sToolTipText = QT_TR_NOOP("Re_Hx");
	sWhatsThis = "Re_Hx";
	sStatusTip = sToolTipText;
}

void CmdaddRe_Hx::activated(int iMsg)
{
	std::vector<string> searchStr = CommonFunction::GetFaceSelected();
	if (searchStr.size())
	{
		MySimIIGui::SelectFrequency p;
		p.flag = 15;
		p.exec();
	}
	else
	{
		
		return;
	} 
	
}

//.........................................................................................
//...........................Re_Hy....................................................................
DEF_STD_CMD(CmdaddRe_Hy);

CmdaddRe_Hy::CmdaddRe_Hy()
:Command("Re_Hy")
{
	sAppModule = "MySimII";
	sGroup = QT_TR_NOOP("MySimII");
	sMenuText = QT_TR_NOOP("Re_Hy");
	sToolTipText = QT_TR_NOOP("Re_Hy");
	sWhatsThis = "Re_Hy";
	sStatusTip = sToolTipText;
}

void CmdaddRe_Hy::activated(int iMsg)
{
	std::vector<string> searchStr = CommonFunction::GetFaceSelected();
	if (searchStr.size())
	{
		MySimIIGui::SelectFrequency p;
		p.flag = 16;
		p.exec();
	}
	else
	{
		
		return;
	} 
	
}

//.........................................................................................
//...........................Re_Hz....................................................................
DEF_STD_CMD(CmdaddRe_Hz);

CmdaddRe_Hz::CmdaddRe_Hz()
:Command("Re_Hz")
{
	sAppModule = "MySimII";
	sGroup = QT_TR_NOOP("MySimII");
	sMenuText = QT_TR_NOOP("Re_Hz");
	sToolTipText = QT_TR_NOOP("Re_Hz");
	sWhatsThis = "Re_Hz";
	sStatusTip = sToolTipText;
}

void CmdaddRe_Hz::activated(int iMsg)
{
	
	std::vector<string> searchStr = CommonFunction::GetFaceSelected();
	if (searchStr.size())
	{
		MySimIIGui::SelectFrequency p;
		p.flag = 17;
		p.exec();
	}
	else
	{
		
		return;
	} 
	
}

//.........................................................................................
//...........................Im_Hx....................................................................
DEF_STD_CMD(CmdaddIm_Hx);

CmdaddIm_Hx::CmdaddIm_Hx()
:Command("Im_Hx")
{
	sAppModule = "MySimII";
	sGroup = QT_TR_NOOP("MySimII");
	sMenuText = QT_TR_NOOP("Im_Hx");
	sToolTipText = QT_TR_NOOP("Im_Hx");
	sWhatsThis = "Im_Hx";
	sStatusTip = sToolTipText;
}

void CmdaddIm_Hx::activated(int iMsg)
{
	std::vector<string> searchStr = CommonFunction::GetFaceSelected();
	if (searchStr.size())
	{
		MySimIIGui::SelectFrequency p;
		p.flag = 18;
		p.exec();
	}
	else
	{
		
		return;
	} 
	
}

//.........................................................................................

//...........................Im_Hy....................................................................
DEF_STD_CMD(CmdaddIm_Hy);

CmdaddIm_Hy::CmdaddIm_Hy()
:Command("Im_Hy")
{
	sAppModule = "MySimII";
	sGroup = QT_TR_NOOP("MySimII");
	sMenuText = QT_TR_NOOP("Im_Hy");
	sToolTipText = QT_TR_NOOP("Im_Hy");
	sWhatsThis = "Im_Hy";
	sStatusTip = sToolTipText;
}

void CmdaddIm_Hy::activated(int iMsg)
{

	std::vector<string> searchStr = CommonFunction::GetFaceSelected();
	if (searchStr.size())
	{
		MySimIIGui::SelectFrequency p;
		p.flag = 19;
		p.exec();
	}
	else
	{
		
		return;
	} 
	
}

//.........................................................................................

//...........................Im_Hz....................................................................
DEF_STD_CMD(CmdaddIm_Hz);

CmdaddIm_Hz::CmdaddIm_Hz()
:Command("Im_Hz")
{
	sAppModule = "MySimII";
	sGroup = QT_TR_NOOP("MySimII");
	sMenuText = QT_TR_NOOP("Im_Hz");
	sToolTipText = QT_TR_NOOP("Im_Hz");
	sWhatsThis = "Im_Hz";
	sStatusTip = sToolTipText;
}

void CmdaddIm_Hz::activated(int iMsg)
{
	
	std::vector<string> searchStr = CommonFunction::GetFaceSelected();
	if (searchStr.size())
	{
		MySimIIGui::SelectFrequency p;
		p.flag = 20;
		p.exec();
	}
	else
	{
		
		return;
	} 
	
}

//.........................................................................................


DEF_STD_CMD(CmdAnimat);

CmdAnimat::CmdAnimat()
:Command("MySimII_Animation")
{
	sAppModule = "MySimII";
	sGroup = QT_TR_NOOP("MySimII");
	sMenuText = QT_TR_NOOP("Animat");
	sToolTipText = QT_TR_NOOP("Animat");
	sWhatsThis = QT_TR_NOOP("Animat");
	sStatusTip = QT_TR_NOOP("Animat");
	sPixmap = "log10";
	//sAccel        = "CTRL+H";
}

void CmdAnimat::activated(int iMsg)
{
	static MySimIIGui::Animat p;
	p.show();

}

DEF_STD_CMD(CmdPlotScript);

CmdPlotScript::CmdPlotScript() : Command("MySimII_PlotScript")
{
    sAppModule    = "MySimII";
    sGroup        = QT_TR_NOOP("MySimII");
    sMenuText     = QT_TR_NOOP("PlotScript");
    sToolTipText  = QT_TR_NOOP("PlotScript Test function");
    sWhatsThis    = QT_TR_NOOP("PlotScript Test function");
    sStatusTip    = QT_TR_NOOP("PlotScript Test function");
    sPixmap       = "log10";
    //sAccel        = "CTRL+P";
}

void CmdPlotScript::activated(int iMsg)
{
  doCommand(Gui, "import os" );
  doCommand(Gui, "os.chdir(\"../UserDir\")" );
  doCommand(Gui, "import PlotScript" );
  doCommand(Gui, "PlotScript.run()");
}





void CreateMySimIICommands(void)
{
    Gui::CommandManager &rcCmdMgr = Gui::Application::Instance->commandManager();
	rcCmdMgr.addCommand(new CmdMySimII_Test());
	//rcCmdMgr.addCommand(new CmdBoundaries());
	rcCmdMgr.addCommand(new CmdaddSolution());
	rcCmdMgr.addCommand(new CmdaddFrequencySweep());
	rcCmdMgr.addCommand(new CmdAdaptiveMeshing());
	rcCmdMgr.addCommand(new CmdModularMeshing());
	rcCmdMgr.addCommand(new CmdMeshSetting());
	rcCmdMgr.addCommand(new CmdMeshView());
	rcCmdMgr.addCommand(new CmdRemoteRun());
	rcCmdMgr.addCommand(new CmdGlyphs());
	rcCmdMgr.addCommand(new CmdContours());
	rcCmdMgr.addCommand(new CmdAnimat());
	rcCmdMgr.addCommand(new CmdPlotScript());
}
