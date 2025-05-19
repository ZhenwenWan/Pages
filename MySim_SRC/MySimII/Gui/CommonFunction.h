#ifndef MYSIMGUI_COMMONFUNCTION_H
#define MYSIMGUI_COMMONFUNCTION_H
#include <memory>
#include <QtCore>
#include <QSettings>
#include <QStringList>
#include <QString>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Pln.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <App/Document.h>
#include <App/Application.h>
#include <App/DocumentObject.h>
#include <Gui/ViewProvider.h>
#include <Gui/Application.h>
#include <Gui/Selection.h>
#include <Gui/MainWindow.h>
#include <Gui/ViewProviderDocumentObject.h>
#include <Mod/Part/App/PartFeature.h>
#include <BRep_Builder.hxx>
#include <vector>


namespace MySimIIGui {

class CommonFunction
{
public:
  
	static int FaceSum(void);
	static void ui2SIF(void);
	static std::vector<int> GetFaceSelected(void);

protected:
	 
public:

};

}//namespace 	
#endif
