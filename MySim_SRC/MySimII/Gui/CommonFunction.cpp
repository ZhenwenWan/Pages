#include "PreCompiled.h"
#ifndef _PreComp_
#include <QMessageBox>
#include <QtCore/QCoreApplication>
#include <QSettings>
#include <QStringList>
#include <QList>
#include <QListIterator>
#include <string>
#endif
#include <Base/Reader.h>
#include <Base/Console.h>
#include <Gui/Application.h>
#include "CommonFunction.h"

using namespace MySimIIGui;
using namespace std;

//face sum
int CommonFunction::FaceSum(){
	int facecount = 0;
	App::Document* doc = App::GetApplication().getActiveDocument();
	std::vector<App::DocumentObject*> sel = doc->getObjectsOfType(App::DocumentObject::getClassTypeId());
	for (std::vector<App::DocumentObject*>::iterator it = sel.begin(); it != sel.end(); ++it)
	{
		Gui::ViewProvider* pcProv = Gui::Application::Instance->getViewProvider(*it);
		bool visibility = true;
		if (pcProv ) {
			visibility = ((Gui::ViewProviderDocumentObject*)pcProv)->Visibility.getValue();
		}
		// 读取几何体可见(Visibility)属性值  START  END
		if (visibility == false){// 不可见的跳过
			continue;
		}
		
		App::DocumentObject *faceObj = (*it);

		const TopoDS_Shape& tShape = static_cast<Part::Feature *>(faceObj)->Shape.getValue();
		TopExp_Explorer countFace;
		
		for (countFace.Init(tShape, TopAbs_FACE); countFace.More(); countFace.Next())
		{
			facecount++;
			
		}
	}
	return facecount;

}

void CommonFunction::ui2SIF(void)
{

	QSettings settings(QObject::tr("../ini/MySimII-ui.ini"), QSettings::IniFormat);

	QString Solver, Type, WorkDirectory, IterativeSolver, DirectiveSolver;
	QString Starting, Ending, Step, ConvergRelatErr;
	QString OutputSteps, MaxIterSteps;

        Solver           = settings.value(QObject::tr("SolverSettings/Solver")         ).toString();
        Type             = settings.value(QObject::tr("SolverSettings/Type")           ).toString();
        WorkDirectory    = settings.value(QObject::tr("SolverSettings/WorkDirectory")  ).toString();
        IterativeSolver  = settings.value(QObject::tr("SolverSettings/IterativeSolver")).toString();
        DirectiveSolver  = settings.value(QObject::tr("SolverSettings/DirectiveSolver")).toString();
        Starting         = settings.value(QObject::tr("SolverSettings/Starting")       ).toString();
        Ending           = settings.value(QObject::tr("SolverSettings/Ending")         ).toString();
        Step             = settings.value(QObject::tr("SolverSettings/Step")           ).toString();
        ConvergRelatErr  = settings.value(QObject::tr("SolverSettings/ConvergRelatErr")).toString();
        OutputSteps      = settings.value(QObject::tr("SolverSettings/OutputSteps")    ).toString();
        MaxIterSteps     = settings.value(QObject::tr("SolverSettings/MaxIterSteps")   ).toString();

	QString Filename, Outputname;
	Filename   = QObject::tr("../UserDir/prototype%s.sif").arg(Solver);
	if(WorkDirectory.isEmpty()) WorkDirectory = QObject::tr("../ini/");
	Outputname = QObject::tr("%s/MySimII.sif").arg(WorkDirectory);
        
	QMap<QString, QString> ui2sif;
	ui2sif[QObject::tr("<<Solver>>")]          = Solver;
	ui2sif[QObject::tr("<<Type>>")]            = Type;
	ui2sif[QObject::tr("<<WorkDirectory>>")]   = WorkDirectory;
	ui2sif[QObject::tr("<<IterativeSolver>>")] = IterativeSolver;
	ui2sif[QObject::tr("<<Starting>>")]        = Starting;
	ui2sif[QObject::tr("<<Ending>>")]          = Ending;
	ui2sif[QObject::tr("<<Step>>")]            = Step;
	ui2sif[QObject::tr("<<ConvergRelatErr>>")] = ConvergRelatErr;
	ui2sif[QObject::tr("<<OutputSteps>>")]     = OutputSteps;
	ui2sif[QObject::tr("<<MaxIterSteps>>")]    = MaxIterSteps;


	QFile SIF_proto(Filename);
	QFile SIF_final(Outputname);
        if( !SIF_proto.open(QFile::ReadWrite) ) {
	    QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Err: missing prototype file "), Filename);
	}
	else {
            QTextStream proto_stream(&SIF_proto);
            QTextStream final_stream(&SIF_final);
            QString line;
            while( ! proto_stream.atEnd() ) {
               proto_stream >> line;
               for(QMap<QString, QString>::iterator iter = ui2sif.begin(); iter != ui2sif.end(); ++iter) {
	           line.replace(iter.key(), iter.value());
	       }
	       final_stream << line;
            }
        }
        SIF_proto.close();
        SIF_final.close();

}

std::vector<int> CommonFunction::GetFaceSelected()
{
	std::string searchStrnew("");
	std::vector<int> searchStr;

	QStringList fonts;
	std::string bodyName("");
	App::DocumentObject *faceObj;

	

	TopoDS_Shape ref;
	QList<gp_Pnt> gpList;

	QString searchStra;
	std::vector<Gui::SelectionObject> selection = Gui::Selection().getSelectionEx();
	if (selection.size() == 0){
		QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Selection error"), QObject::tr("Nothing selected!"));
		return searchStr;
	}
	
	
	for (std::vector<Gui::SelectionObject>::iterator it = selection.begin(); it != selection.end(); ++it){
		if (static_cast<std::string>(it->getTypeName()).substr(0, 4).compare(std::string("Part")) != 0){
			QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Selection error"), QObject::tr("Selected object is not a part!"));
			return searchStr;
		}
		
		std::vector<std::string> subNames = it->getSubNames();
		
		if (subNames.size() == 0){
			App::DocumentObject *obj = it->getObject();
			const TopoDS_Shape& tShape = static_cast<Part::Feature *>(obj)->Shape.getValue();
			TopExp_Explorer expFace;
			int i = 1;
			for (expFace.Init(tShape, TopAbs_FACE); expFace.More(); expFace.Next())
			{
				QString	name = QObject::tr("Face%1").arg(i);
				subNames.push_back(name.toStdString());
				i++;
			}
		}
		
		for (unsigned int subIt = 0; subIt < (subNames.size()); ++subIt){
			if (subNames[subIt].find("Vertex") != std::string::npos)
				continue;
			else if (subNames[subIt].find("Edge") != std::string::npos)	
				continue;
			else{
				gpList.clear();
				fonts.clear();
			
				faceObj = it->getObject();
				Part::Feature* feat = static_cast<Part::Feature*>(faceObj);				
				ref = feat->Shape.getShape().getSubShape(subNames[subIt].c_str());
				TopExp_Explorer expVertex;
				int flag = 0;
				for (expVertex.Init(ref, TopAbs_VERTEX); expVertex.More(); expVertex.Next())
				{

					TopoDS_Vertex  vtx = TopoDS::Vertex(expVertex.Current());
					gp_Pnt pt = BRep_Tool::Pnt(vtx);

					gpList.append(pt);


					
				}//dian

			
				bodyName = faceObj->getNameInDocument();
			}
			
	
			///  code face 
			App::Document* doc = App::GetApplication().getDocument(faceObj->getDocument()->getName());
			std::vector<App::DocumentObject*> sel = doc->getObjectsOfType(App::DocumentObject::getClassTypeId());
			int num = 0;
			bool findface = false;

			for (std::vector<App::DocumentObject*>::iterator it = sel.begin(); it != sel.end(); ++it)
			{
				Gui::ViewProvider* pcProv = Gui::Application::Instance->getViewProvider(*it);
				bool visibility = true;
				if (pcProv ) {
					visibility = ((Gui::ViewProviderDocumentObject*)pcProv)->Visibility.getValue();
				}
				if (visibility == false){
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

					QList<gp_Pnt> vv;
					for (expVertex.Init(expFace.Current(), TopAbs_VERTEX); expVertex.More(); expVertex.Next())
					{
						TopoDS_Vertex  vtx = TopoDS::Vertex(expVertex.Current());
						gp_Pnt pt = BRep_Tool::Pnt(vtx);
						vv << pt;
						
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
							break;
						}
					}
				}
				if (findface == true){
					break;
				}
			}
			if (findface != true){
				num = 0;
			}
			
			searchStr.push_back(num);
			
		}
		
	}
	return searchStr;
}
