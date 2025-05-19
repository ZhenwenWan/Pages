#include "PreCompiled.h"
#ifndef _PreComp_
#include <QMessageBox>
#include <string>
#endif
#include <Base/Reader.h>
#include <Base/Console.h>
#include "Geometry.h"

using namespace MySimIIGui;
using namespace std;
int MySimIIGui::Geometry::NDocObj;
int MySimIIGui::Geometry::NShape;
int MySimIIGui::Geometry::NFace;
int MySimIIGui::Geometry::NEdge;
int MySimIIGui::Geometry::vNShape;
int MySimIIGui::Geometry::vNFace;
int MySimIIGui::Geometry::vNEdge;
int MySimIIGui::Geometry::ModelFace;

void Geometry::GeometrySum(){

  NDocObj=0, NShape=0, NFace=0, NEdge=0;
            vNShape=0,vNFace=0,vNEdge=0;
	    ModelFace=0;

  int DocLastFace=0; // recording the face count of last DocumentObj as Model in case of no Model label

  App::Document* pDoc = App::GetApplication().getActiveDocument();
  std::vector<App::DocumentObject*> vpDocObj = pDoc->getObjectsOfType(App::DocumentObject::getClassTypeId());

//scanning DocumentObject for counting the total visible shapes  
  for (std::vector<App::DocumentObject*>::iterator it = vpDocObj.begin(); it != vpDocObj.end(); ++it)
  {
    App::DocumentObject *pDocObj = (*it);
    NDocObj++;
    std::string label = (*it)->Label.getValue();

    const TopoDS_Shape& tShape = static_cast<Part::Feature *>(pDocObj)->Shape.getValue();
    NShape++;

//scanning tShape for tFace -- Brep-surface
    TopExp_Explorer expFace;
    DocLastFace=0;
    for (expFace.Init(tShape, TopAbs_FACE); expFace.More(); expFace.Next())
    {
      NFace++;
      if(label=="Model") ModelFace++;
      DocLastFace++;
      const TopoDS_Face& tFace = TopoDS::Face(expFace.Current());

//scanning tFace for tEdge -- BRep-curve  
      TopExp_Explorer expEdge;
      for (expEdge.Init(tFace, TopAbs_EDGE); expEdge.More(); expEdge.Next())
      {
        NEdge++;
      }
    }

    if(ModelFace==0) ModelFace=DocLastFace;


//scanning for visible Shape, Face, Edge
    std::string Name = (*it)->getNameInDocument();
    QString QName = QString::fromStdString(Name);
    
    if (QName.left(5) == QObject::tr("Group")){
    	continue;
    }
    if (QName.left(4) == QObject::tr("Line")){
    	continue;
    }
    Gui::ViewProvider* pcProv = Gui::Application::Instance->getViewProvider(*it);
    bool visibility = true;
    if (pcProv) {
    	visibility = ((Gui::ViewProviderDocumentObject*)pcProv)->Visibility.getValue();
    }
    if (visibility == false){
    	continue;
    }

    vNShape++;

//scanning tShape for tFace -- Brep-surface
    expFace;
    for (expFace.Init(tShape, TopAbs_FACE); expFace.More(); expFace.Next())
    {
      vNFace++;
      const TopoDS_Face& tFace = TopoDS::Face(expFace.Current());

//scanning tFace for tEdge -- BRep-curve  
      TopExp_Explorer expEdge;
      for (expEdge.Init(tFace, TopAbs_EDGE); expEdge.More(); expEdge.Next())
      {
        vNEdge++;
      }
    }

  }
}

//Geometry::~Geometry(){
//
//}

QString Geometry::getFaceInfo(int FaceSN ){
  int iFace = 0;
  double xt = 0.0, Length, Width, Radius1, Radius2;
  std::string type;
  gp_XYZ dir(0, 0, 0);
  gp_Dir direction;
  Base::Vector3d FaceDirection, FaceCenter, RectEdge1, RectEdge2, RectEdge3;
  TopoDS_Vertex tVertex1, tVertex2, tVertex3, tVertex4;
      
  QString result;
  result = QObject::tr("");
  result.append(QObject::tr("FUNCTION<j>(t,x,y,z){\n"));
  result.append(QObject::tr("## all parameters below are usable ##\n"));
  result.append(QObject::tr("## for example:                    \n"));
  result.append(QObject::tr("## xl= x - LCoordX;                \n"));
  result.append(QObject::tr("## FUNCTION<1> = xl*ONormalY + 5;  \n"));
  result.append(QObject::tr("}\n"));
  result.append(QObject::tr("\n"));

  App::Document* pDoc = App::GetApplication().getActiveDocument();
  std::vector<App::DocumentObject*> vpDocObj = pDoc->getObjectsOfType(App::DocumentObject::getClassTypeId());

  for (std::vector<App::DocumentObject*>::iterator it = vpDocObj.begin(); it != vpDocObj.end(); ++it)
  {
    App::DocumentObject *pDocObj = (*it);
    const TopoDS_Shape& tShape = static_cast<Part::Feature *>(pDocObj)->Shape.getValue();
    std::string Name = (*it)->getNameInDocument();
    QString QName = QString::fromStdString(Name);
    
    //if (QName.left(5) == QObject::tr("Group")){
    //	continue;
    //}
    //if (QName.left(4) == QObject::tr("Line")){
    //	continue;
    //}
    
    //Gui::ViewProvider* pcProv = Gui::Application::Instance->getViewProvider(*it);
    //bool visibility = true;
    //if (pcProv) {
    //	visibility = ((Gui::ViewProviderDocumentObject*)pcProv)->Visibility.getValue();
    //}
    //if (visibility == false){
    //	continue;
    //}

//scanning tShape for tFace -- Brep-surface
    TopExp_Explorer expFace;
    for (expFace.Init(tShape, TopAbs_FACE); expFace.More(); expFace.Next())
    {
      iFace++;
      if( iFace < FaceSN ) continue;

      const TopoDS_Face& tFace = TopoDS::Face(expFace.Current());
      BRepAdaptor_Surface surface(tFace);

      if (surface.GetType() != GeomAbs_Plane) {
        result = QObject::tr("Err:UndefinedFaceType1");
	return result; 
      }

      int isNormal = surface.Face().Orientation();
      direction = surface.Plane().Axis().Direction();
      if (isNormal){
        FaceDirection.Set(0 - direction.Dot(gp::DX()), 0 - direction.Dot(gp::DY()), 0 - direction.Dot(gp::DZ()));
      }
      else{
        FaceDirection.Set(0 + direction.Dot(gp::DX()), 0 + direction.Dot(gp::DY()), 0 + direction.Dot(gp::DZ()));
      }
      
//identifying the type by nCircle, nLine
      int nCircle=0, nLine=0;
      TopExp_Explorer expEdge;
      for (expEdge.Init(tFace, TopAbs_EDGE); expEdge.More(); expEdge.Next())
      {
        const TopoDS_Edge& tEdge = TopoDS::Edge(expEdge.Current());
        BRepAdaptor_Curve curve(tEdge);

        if (curve.GetType() == GeomAbs_Line) {
           nLine++;
        }
        else if (curve.GetType() == GeomAbs_Circle) {
           nCircle++;
        }
        else {
        	continue;
        }

      }
      if( nLine == 4   ) result.append(QObject::tr("BType=%1;\n").arg(QObject::tr("Rect")));
      if( nCircle == 1 ) result.append(QObject::tr("BType=%1;\n").arg(QObject::tr("Circ")));
      if( nCircle == 2 ) result.append(QObject::tr("BType=%1;\n").arg(QObject::tr("Ring")));

      if ( !(nLine==4||nCircle==1||nCircle==2) || (nLine+nCircle>4) ) {
//delimit to three types 
        result = QObject::tr("Err:UndefinedFaceType2");
	return result; 
      }

//Rectangular 
      std::vector<gp_Pnt> vertexlist;
      if(nLine==4) {
	int mt=0, lt=0;
	FaceCenter.Set(0.0, 0.0, 0.0);
        for (expEdge.Init(tFace, TopAbs_EDGE); expEdge.More(); expEdge.Next())
        {
	  lt++;
          const TopoDS_Edge& tEdge = TopoDS::Edge(expEdge.Current());
          BRepAdaptor_Curve curve(tEdge);
          
	  TopoDS_Vertex tVertex1 = TopExp::FirstVertex(tEdge);
          TopoDS_Vertex tVertex2 = TopExp:: LastVertex(tEdge);
          gp_Pnt tPnt1 = BRep_Tool::Pnt(tVertex1);
          gp_Pnt tPnt2 = BRep_Tool::Pnt(tVertex2);
	  FaceCenter.x += tPnt1.X()/8;
	  FaceCenter.y += tPnt1.Y()/8;
	  FaceCenter.z += tPnt1.Z()/8;
	  FaceCenter.x += tPnt2.X()/8;
	  FaceCenter.y += tPnt2.Y()/8;
	  FaceCenter.z += tPnt2.Z()/8;
        
	  if(lt==1) {
            vertexlist.push_back( tPnt1 );
            vertexlist.push_back( tPnt2 );
	  }
	  if(lt>1) {
            int nt=vertexlist.size();
            if( tPnt1.IsEqual(vertexlist[nt-1], 1.0e-10) ){
              vertexlist.push_back( tPnt2 );
	    }
	    else if( tPnt2.IsEqual(vertexlist[nt-1], 1.0e-10) ){
              vertexlist.push_back( tPnt1 );
	    }
	    else if( tPnt1.IsEqual(vertexlist[nt-2], 1.0e-10) ){
              vertexlist.push_back( tPnt2 );
	      //mt represent the order of vetexlist either 0-1-2 or 2-0-1
              mt++;
	    }
	    else if( tPnt2.IsEqual(vertexlist[nt-2], 1.0e-10) ){
              vertexlist.push_back( tPnt1 );
	      //mt represent the order of vetexlist either 0-1-2 or 2-0-1
              mt++;
	    }
	  }
          if(vertexlist.size()==3) break;
        }
	//mt represent the order of vetexlist either 0-1-2 or 2-0-1
        if(mt == 0) { 
	   RectEdge1.Set(vertexlist[1].X() - vertexlist[0].X(),
	                 vertexlist[1].Y() - vertexlist[0].Y(),
	                 vertexlist[1].Z() - vertexlist[0].Z());
	   RectEdge2.Set(vertexlist[2].X() - vertexlist[1].X(),
	                 vertexlist[2].Y() - vertexlist[1].Y(),
	                 vertexlist[2].Z() - vertexlist[1].Z());
	}
	else {
	   RectEdge1.Set(vertexlist[0].X() - vertexlist[2].X(),
	                 vertexlist[0].Y() - vertexlist[2].Y(),
	                 vertexlist[0].Z() - vertexlist[2].Z());
	   RectEdge2.Set(vertexlist[1].X() - vertexlist[0].X(),
	                 vertexlist[1].Y() - vertexlist[0].Y(),
	                 vertexlist[1].Z() - vertexlist[0].Z());
	}

        xt = RectEdge1.Cross(RectEdge2).Dot(FaceDirection);
	if( xt < 0.0) {
//adjusting the order and direction of two edges RectEdge1 and RectEdge2
          RectEdge3 =  RectEdge1; //RectEdge3 for temporay usage only
          RectEdge1 = -RectEdge2;
          RectEdge2 = -RectEdge3;
	}
	Length = RectEdge1.Length();
	Width  = RectEdge2.Length();
	RectEdge1.Normalize();
	RectEdge2.Normalize();
      }

      int nt=0;
      if(nCircle<=2)
      {
        for (expEdge.Init(tFace, TopAbs_EDGE); expEdge.More(); expEdge.Next())
        {
          const TopoDS_Edge& tEdge = TopoDS::Edge(expEdge.Current());
          BRepAdaptor_Curve curve(tEdge);
          nt++;
          if (curve.GetType() == GeomAbs_Circle) {
            gp_Pnt location = curve.Circle().Location();
	    FaceCenter.x = location.X();
	    FaceCenter.y = location.Y();
	    FaceCenter.z = location.Z();
	    if( nt==1 )
	    { Radius1 = curve.Circle().Radius(); }
	    else
	    { Radius2 = curve.Circle().Radius(); }
          } 
        
        }
      }

      QString sFaceCenter, sFaceDirection, sRectEdge1, sRectEdge2;
      QString strX, strY, strZ;

      strX.sprintf("%14.6e", FaceCenter.x);
      strY.sprintf("%14.6e", FaceCenter.y);
      strZ.sprintf("%14.6e", FaceCenter.z);
      result.append(QObject::tr("LCoordX=%1;\n").arg(strX));
      result.append(QObject::tr("LCoordY=%1;\n").arg(strY));
      result.append(QObject::tr("LCoordZ=%1;\n").arg(strZ));


      strX.sprintf("%14.6e", FaceDirection.x);
      strY.sprintf("%14.6e", FaceDirection.y);
      strZ.sprintf("%14.6e", FaceDirection.z);
      result.append(QObject::tr("ONormalX=%1;\n").arg(strX));
      result.append(QObject::tr("ONormalY=%1;\n").arg(strY));
      result.append(QObject::tr("ONormalZ=%1;\n").arg(strZ));
      
      if( nLine==4 )
      {
        strX.sprintf("%14.6e", RectEdge1.x);
        strY.sprintf("%14.6e", RectEdge1.y);
        strZ.sprintf("%14.6e", RectEdge1.z);
        result.append(QObject::tr("Edge1X=%1;\n").arg(strX));
        result.append(QObject::tr("Edge1Y=%1;\n").arg(strY));
        result.append(QObject::tr("Edge1Z=%1;\n").arg(strZ));
        
        strX.sprintf("%14.6e", RectEdge2.x);
        strY.sprintf("%14.6e", RectEdge2.y);
        strZ.sprintf("%14.6e", RectEdge2.z);
        result.append(QObject::tr("Edge2X=%1;\n").arg(strX));
        result.append(QObject::tr("Edge2Y=%1;\n").arg(strY));
        result.append(QObject::tr("Edge2Z=%1;\n").arg(strZ));
      
        strX.sprintf("%14.6e", Length);
        strY.sprintf("%14.6e", Width);
        result.append(QObject::tr("Length1=%1;\n").arg(strX));
        result.append(QObject::tr("Length2=%1;\n").arg(strY));
      }
      else if( nCircle==1 )
      {
        strX.sprintf("%14.6e", Radius1);
        result.append(QObject::tr("Radius=%1;\n").arg(strX));
      }
      else
      {
        strX.sprintf("%14.6e", Radius1);
        strY.sprintf("%14.6e", Radius2);
        result.append(QObject::tr("Radius1=%1;\n").arg(strX));
        result.append(QObject::tr("Radius2=%1;\n").arg(strY));
      }

      if( iFace == FaceSN ) return result;

    }
  }
}

void Geometry::GeometryScanning(){

  int NDocObj=0, NShape=0, NFace=0, NEdge=0, NVertex=0;
  double cx, cy, cz;

//scanning DocumentObject for tShape  
  App::Document* pDoc = App::GetApplication().getActiveDocument();
  std::vector<App::DocumentObject*> vpDocObj = pDoc->getObjectsOfType(App::DocumentObject::getClassTypeId());
  for (std::vector<App::DocumentObject*>::iterator it = vpDocObj.begin(); it != vpDocObj.end(); ++it)
  {
    App::DocumentObject *pDocObj = (*it);
    const TopoDS_Shape& tShape = static_cast<Part::Feature *>(pDocObj)->Shape.getValue();

//scanning tShape for tFace -- Brep-surface
    TopExp_Explorer expFace;
    for (expFace.Init(tShape, TopAbs_FACE); expFace.More(); expFace.Next())
    {
      const TopoDS_Face& tFace = TopoDS::Face(expFace.Current());
      BRepAdaptor_Surface face(tFace);

      QString planDir;
      gp_XYZ dir(0, 0, 0);
      BRepAdaptor_Surface surface(tFace);
      int isNormal = surface.Face().Orientation();
      if (surface.GetType() == GeomAbs_Plane) {
      
      	dir = surface.Plane().Axis().Direction().XYZ();
      	QString cX, cY, cZ;
      	if (isNormal){
          cX.sprintf("%14.6e", 0 - dir.X());
          cY.sprintf("%14.6e", 0 - dir.Y());
          cZ.sprintf("%14.6e", 0 - dir.Z());
          planDir = QObject::tr("FaceDir(%1,%2,%3)\n").arg(cX).arg(cY).arg(cZ);
      	}
      	else{
          cX.sprintf("%14.6e", dir.X());
          cY.sprintf("%14.6e", dir.Y());
          cZ.sprintf("%14.6e", dir.Z());
          planDir = QObject::tr("FaceDir(%1,%2,%3)\n").arg(cX).arg(cY).arg(cZ);
      	}
      }
      else {
	continue;
      }

//scanning tFace for tEdge -- BRep-curve  
      TopExp_Explorer expEdge;
      for (expEdge.Init(tFace, TopAbs_EDGE); expEdge.More(); expEdge.Next())
      {
        const TopoDS_Edge& tEdge = TopoDS::Edge(expEdge.Current());
        BRepAdaptor_Curve curve(tEdge);
  
        if (curve.GetType() == GeomAbs_Line) {
          	dir = curve.Line().Direction().XYZ();
        }
        else if (curve.GetType() == GeomAbs_Circle) {
        	gp_Pnt location = curve.Circle().Location();
        	cx = location.X();
        	cy = location.Y();
        	cz = location.Z();
        }
        else {
        	continue;
        }

        TopoDS_Vertex tVertex1 = TopExp::FirstVertex(tEdge);
        gp_Pnt tPnt1 = BRep_Tool::Pnt(tVertex1);
        TopoDS_Vertex tVertex2 = TopExp::LastVertex(tEdge);
        gp_Pnt tPnt2 = BRep_Tool::Pnt(tVertex2);

        QString strX, strY, strZ;
        strX.sprintf("%14.6e", tPnt1.X());
        strY.sprintf("%14.6e", tPnt1.Y());
        strZ.sprintf("%14.6e", tPnt1.Z());
        QString fipnt = QObject::tr("Curve point#1: %1,%2,%3").arg(strX).arg(strY).arg(strZ);
        strX.sprintf("%14.6e", tPnt2.X());
        strY.sprintf("%14.6e", tPnt2.Y());
        strZ.sprintf("%14.6e", tPnt2.Z());
                fipnt = QObject::tr("Curve point#2: %1,%2,%3").arg(strX).arg(strY).arg(strZ);

      }
    }
  }
}


void Geometry::GeometryReader(){

  double cx, cy, cz, cr;
  QString cX, cY, cZ;
  gp_Pnt location;
  gp_XYZ dir(0, 0, 0);
  gp_Dir direction;
  Base::Vector3d FaceDirection, FaceCenter, RectEdge1, RectEdge2;

  std::vector<Gui::SelectionObject> selection = Gui::Selection().getSelectionEx();
  if (selection.size() != 1){
  	QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Selection error"), 
  			     QObject::tr("Nothing or multiple objects selected!"));
	return;
  }
  	
//selecting DocumentObject for tShape  
  for (std::vector<Gui::SelectionObject>::iterator it = selection.begin(); it != selection.end(); ++it){
    if (static_cast<std::string>(it->getTypeName()).substr(0, 4).compare(std::string("Part")) != 0){
    	QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Selection error"), 
    			     QObject::tr("No Topo selected!"));
    }

    if (it->getSubNames().size()!=1){
    	QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Selection error"), 
    			     QObject::tr("One face must be selected! Only one!!"));
	return;
    }

    App::DocumentObject* pDocObj =it->getObject();
    std::vector<std::string> subNames = it->getSubNames();
    const TopoDS_Shape& tShape = static_cast<Part::Feature *>(pDocObj)->Shape.getShape().
                                 getSubShape(subNames[0].c_str());

//scanning tShape for tFace -- Brep-surface
    TopExp_Explorer expFace;
    for (expFace.Init(tShape, TopAbs_FACE); expFace.More(); expFace.Next())
    {
      const TopoDS_Face& tFace = TopoDS::Face(expFace.Current());
      BRepAdaptor_Surface face(tFace);

      QString planDir;
      BRepAdaptor_Surface surface(tFace);
      int isNormal = surface.Face().Orientation();
      if (surface.GetType() == GeomAbs_Plane) {
      
      	direction = surface.Plane().Axis().Direction();
      	if (isNormal){
          FaceDirection.Set(0 - direction.Dot(gp::DX()), 0 - direction.Dot(gp::DY()), 0 - direction.Dot(gp::DZ()));
      	}
      	else{
          FaceDirection.Set(0 + direction.Dot(gp::DX()), 0 + direction.Dot(gp::DY()), 0 + direction.Dot(gp::DZ()));
      	}
      }
      else {
	continue;
      }
      cX.sprintf("%14.6e", FaceDirection.x);
      cY.sprintf("%14.6e", FaceDirection.y);
      cZ.sprintf("%14.6e", FaceDirection.z);
      planDir = QObject::tr("FaceDirection(%1,%2,%3)\n").arg(cX).arg(cY).arg(cZ);
      Base::Console().Message(planDir.toStdString().c_str());

      std::vector<gp_Pnt> vertexlist;
	int nt=0;
        TopExp_Explorer expVertex;
        for (expVertex.Init(tFace, TopAbs_VERTEX); expVertex.More(); expVertex.Next())
        {
          const TopoDS_Vertex& tVertex = TopoDS::Vertex(expVertex.Current());
          vertexlist.push_back( BRep_Tool::Pnt(tVertex) );
          nt++;
      Base::Console().Message("vertex.X()= %f \n", BRep_Tool::Pnt(tVertex).X());
      Base::Console().Message("vertex.Y()= %f \n", BRep_Tool::Pnt(tVertex).Y());
      Base::Console().Message("vertex.Z()= %f \n", BRep_Tool::Pnt(tVertex).Z());

	}
      Base::Console().Message("nt= %d \n", nt);
      Base::Console().Message("vertexlist.size()= %d \n", vertexlist.size());

//scanning tFace for tEdge -- BRep-curve  
      TopExp_Explorer expEdge;
      for (expEdge.Init(tFace, TopAbs_EDGE); expEdge.More(); expEdge.Next())
      {
        const TopoDS_Edge& tEdge = TopoDS::Edge(expEdge.Current());
        BRepAdaptor_Curve curve(tEdge);
  
        if (curve.GetType() == GeomAbs_Line) {
          direction = curve.Line().Direction();
	  cr = 0.0;
        }
        else if (curve.GetType() == GeomAbs_Circle) {
          location = curve.Circle().Location();
          cr = curve.Circle().Radius();
        }
        else {
        	continue;
        }
        cx = direction.X();
        cy = direction.Y();
        cz = direction.Z();
        QString strX, strY, strZ, strR;
        strX.sprintf("%14.6e", cx);
        strY.sprintf("%14.6e", cy);
        strZ.sprintf("%14.6e", cz);
        strR.sprintf("%14.6e", isNormal);
        QString fipnt = QObject::tr("Direction: %1,%2,%3; isNormal: %4 \n").arg(strX).arg(strY).arg(strZ).arg(strR);
      Base::Console().Message(fipnt.toStdString().c_str());

        TopoDS_Vertex tVertex1 = TopExp::FirstVertex(tEdge);
        gp_Pnt tPnt1 = BRep_Tool::Pnt(tVertex1);
        TopoDS_Vertex tVertex2 = TopExp::LastVertex(tEdge);
        gp_Pnt tPnt2 = BRep_Tool::Pnt(tVertex2);

        strX.sprintf("%14.6e", tPnt1.X());
        strY.sprintf("%14.6e", tPnt1.Y());
        strZ.sprintf("%14.6e", tPnt1.Z());
        fipnt = QObject::tr("Curve point#1: %1,%2,%3 \n").arg(strX).arg(strY).arg(strZ);
      Base::Console().Message(fipnt.toStdString().c_str());
        strX.sprintf("%14.6e", tPnt2.X());
        strY.sprintf("%14.6e", tPnt2.Y());
        strZ.sprintf("%14.6e", tPnt2.Z());
        fipnt = QObject::tr("Curve point#2: %1,%2,%3 \n").arg(strX).arg(strY).arg(strZ);
      Base::Console().Message(fipnt.toStdString().c_str());

      }
    }
  }
}


std::vector<int> Geometry::SelectedModelFaces(){

  std::vector<int> ReturnFaces;

  std::vector<Gui::SelectionObject> selection = Gui::Selection().getSelectionEx();
  if (selection.size() != 1){
  	QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Selection error"), 
  			     QObject::tr("One object MUST be selected! Only ONE!!"));
	return ReturnFaces;
  }
  	
  for (std::vector<Gui::SelectionObject>::iterator it = selection.begin(); it != selection.end(); ++it){
    if (static_cast<std::string>(it->getTypeName()).substr(0, 4).compare(std::string("Part")) != 0){
    	QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Selection error"), 
    			     QObject::tr("No Topo selected!"));
    }

    App::DocumentObject* pDocObj =it->getObject();
    std::vector<std::string> subNames = it->getSubNames();
    
    int itSubMax=subNames.size();
    itSubMax=std::max(1,itSubMax);
    for (int itSub=0; itSub<itSubMax; itSub++)
    {
      TopoDS_Shape tFace;
      if(subNames.size()!=0) 
      { 
         tFace = static_cast<Part::Feature *>(pDocObj)->Shape.getShape().
    	                      getSubShape(subNames[itSub].c_str());
      }
      else
      {
         tFace = static_cast<Part::Feature *>(pDocObj)->Shape.getValue();
      }

      std::vector<gp_Pnt> vertexlist;
      TopExp_Explorer expVertex;
      for (expVertex.Init(tFace, TopAbs_VERTEX); expVertex.More(); expVertex.Next())
      {
        const TopoDS_Vertex& tVertex = TopoDS::Vertex(expVertex.Current());
        gp_Pnt pnt = BRep_Tool::Pnt(tVertex);
        vertexlist.push_back( pnt );
      }
      int ModelFace = getModelFace(vertexlist);
      ReturnFaces.push_back( ModelFace );
    }
  }
  return ReturnFaces;
}

int Geometry::getModelFace( std::vector<gp_Pnt> vertexlist ){
  
  int ModelFace = 0;
  int YN = 1;

  App::Document* pDoc = App::GetApplication().getActiveDocument();
  std::vector<App::DocumentObject*> vpDocObj = pDoc->getObjectsOfType(App::DocumentObject::getClassTypeId());
  
  // find out the model from DocumentObjects
  int NModel =0;
  std::string Name = "";
  for (std::vector<App::DocumentObject*>::iterator it = vpDocObj.begin(); it != vpDocObj.end(); ++it)
  {
    NModel++;
    Name = (*it)->Label.getValue();
    if(Name == "Model") break;
  }
  if(Name != "Model") {
     int NModel =0;
     for (std::vector<App::DocumentObject*>::iterator it = vpDocObj.begin(); it != vpDocObj.end(); ++it)
     {
       NModel++;
     }
  }

  int tModel =0;
  for (std::vector<App::DocumentObject*>::iterator it = vpDocObj.begin(); it != vpDocObj.end(); ++it)
  {
    tModel++;
    if(tModel!=NModel) continue;

    App::DocumentObject* pDocObj = (*it);
    const TopoDS_Shape& tShape = static_cast<Part::Feature *>(pDocObj)->Shape.getValue();

    //Gui::ViewProvider* pcProv = Gui::Application::Instance->getViewProvider(*it);
    //bool visibility = true;
    //if (pcProv) {
    //	visibility = ((Gui::ViewProviderDocumentObject*)pcProv)->Visibility.getValue();
    //}
    //if (visibility == false){
    //    QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Model error"), 
    //      QObject::tr("Geometrical object labeled 'Model' MUST be visible!"));
    //   continue;
    //}

    TopExp_Explorer expFace;
    for (expFace.Init(tShape, TopAbs_FACE); expFace.More(); expFace.Next())
    {
      ModelFace++;
      const TopoDS_Face& tFace = TopoDS::Face(expFace.Current());

      std::vector<gp_Pnt> vertexes;
      TopExp_Explorer expVertex;
      for (expVertex.Init(expFace.Current(), TopAbs_VERTEX); expVertex.More(); expVertex.Next())
      {
         TopoDS_Vertex  vtx = TopoDS::Vertex(expVertex.Current());
         gp_Pnt pt = BRep_Tool::Pnt(vtx);
        
	 YN=0;
         for(int n=0; n<vertexlist.size(); n++) {
           if(abs(pt.X()-vertexlist[n].X())<1.0e-10&&
              abs(pt.Y()-vertexlist[n].Y())<1.0e-10&&
              abs(pt.Z()-vertexlist[n].Z())<1.0e-10  ) {
	      YN = 1;
	      break;
	   }
	 }
         if(YN==1) vertexes.push_back(pt);
      }

      if( vertexlist.size()==vertexes.size() ) return ModelFace;
    }

    if(tModel==NModel) break;
  }

  QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Selection error"), 
    QObject::tr("No model face matches the selected face!"));
}


