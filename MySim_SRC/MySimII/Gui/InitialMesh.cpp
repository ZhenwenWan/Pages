#include "PreCompiled.h"
#ifndef _PreComp_
#include <QMessageBox>
#endif
#include "ui_InitialMesh.h"
#include "InitialMesh.h"
//#include "MeshView.h"
#include <Base/Console.h>
#include <Base/Exception.h>
#include <App/Application.h>
#include <App/Document.h>
#include <Gui/Application.h>
#include <Gui/Command.h>
#include <Gui/Document.h>
#include <Gui/BitmapFactory.h>
#include <Gui/Selection.h>
#include <Gui/ViewProvider.h>
#include <Gui/WaitCursor.h>
#include <App/DocumentObjectGroup.h>
#include <App/DocumentObject.h>
#include <Gui/MainWindow.h>
#include <Gui/Control.h>
#include <Gui/FileDialog.h>
#include <Gui/MouseSelection.h>
#include <Gui/View3DInventor.h>
#include <Gui/View3DInventorViewer.h>
#include <Gui/NavigationStyle.h>
#include <CXX/Objects.hxx>
#include <Gui/Action.h>
#include <Base/Interpreter.h>
#include <Base/Tools.h>
#include <QMessageBox>
/********ini********/
#include <QtCore/QCoreApplication>
#include <QSettings>
#include <QStringList>
#include <QString>
//#include <QListInterator>
#include <string>
#include <Gui/ViewProviderDocumentObject.h>
#include <TopoDS_Face.hxx>
#include <Mod/Part/App/PartFeature.h>
//#include<Mod/Part/App/PartFeature.h>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
using namespace MySimIIGui;
int InitialMesh::flagWarning = 0;
map<string, string> M;
int editflag = 0;//记录edit的界面处理
InitialMesh::InitialMesh(QDialog *parent) :QDialog(parent), ui(new Ui::InitialMesh)
{
	ui->setupUi(this);
	//Tableboundary();//显示边界
	//Tableexciation();//显示激励
	//Tablemodel();
	//ui->total->setText(tr("10000"));
	ui->InitialMeshTW->setCurrentIndex(0);
	ui->lineEdit_bcgt->setText(tr("1"));
	ui->lineEdit_10->setText(tr("0"));
	connect(ui->CancelPB, SIGNAL(clicked(bool)), this, SLOT(Cancel()));
	tablebem();
	connect(ui->OKPB, SIGNAL(clicked(bool)), this, SLOT(Ok()));
	
}
InitialMesh::~InitialMesh()
{
	
}
void InitialMesh::Cancel()
{
	QDialog::close();
}
void InitialMesh::Warning1()
{
	QMessageBox::warning(this, QString::fromLocal8Bit("Warning!"), QString::fromLocal8Bit("The boundary object parameter is empty or invalid!"), QMessageBox::Ok);//中文可用
	//flagWarning = 0;
}
void InitialMesh::Warning2()
{
	QMessageBox::warning(this, QString::fromLocal8Bit("Warning!"), QString::fromLocal8Bit("The parameter Finest Mesh Times of the Boundary object must be greater than or equal to the Coarest Mesh Times and must not be less than 1!"), QMessageBox::Ok);//中文可用
	//flagWarning = 0;
}
void InitialMesh::Warning3()
{
	QMessageBox::warning(this, QString::fromLocal8Bit("Warning!"), QString::fromLocal8Bit("The Coarest Distance of the Boundary object must be greater than or equal to the Finest Distance and must not be less than 0!"), QMessageBox::Ok);//中文可用
	//flagWarning = 0;
}
void InitialMesh::Warning4()
{
	QMessageBox::warning(this, QString::fromLocal8Bit("Warning!"), QString::fromLocal8Bit("The excitation object parameter is empty or invalid!"), QMessageBox::Ok);//中文可用
	//flagWarning = 0;
}
void InitialMesh::Warning5()
{
	QMessageBox::warning(this, QString::fromLocal8Bit("Warning!"), QString::fromLocal8Bit("The parameter Finest Mesh Times of the Excitation object must be greater than or equal to the Coarest Mesh Times and must not be less than 1!"), QMessageBox::Ok);//中文可用
	//flagWarning = 0;
}
void InitialMesh::Warning6()
{
	QMessageBox::warning(this, QString::fromLocal8Bit("Warning!"), QString::fromLocal8Bit("The Coarest Distance of the Excitation object must be greater than or equal to the Finest Distance and must not be less than 0!"), QMessageBox::Ok);//中文可用
	//flagWarning = 0;
}

void InitialMesh::Warning7()
{
	QMessageBox::warning(this, QString::fromLocal8Bit("Warning!"), QString::fromLocal8Bit("The module parameter is empty or invalid!"), QMessageBox::Ok);//中文可用
	//flagWarning = 0;
}

void InitialMesh::Tableboundary()
{
	QSettings settings(QObject::tr("../ini/MySimII-ui.ini"), QSettings::IniFormat);
	int sum = settings.value(QObject::tr("BSettingHead/Count")).toInt();
	ui->tableWidget_boundary->setColumnCount(1);
	ui->tableWidget_boundary->setRowCount(sum);
	ui->tableWidget_boundary->horizontalHeader()->hide();
	ui->tableWidget_boundary->verticalHeader()->hide();
	for (int i = 0; i <sum; i++)
	{
		QString pty = QString::fromLatin1("Boundaries/%1").arg(i+1);
		QString value = settings.value(pty).toString();
		QTableWidgetItem *item = new QTableWidgetItem(value);
		item->setCheckState(Qt::Unchecked);
		ui->tableWidget_boundary->setItem(i, 0, item);
	}
	
}

void InitialMesh::Boundaryini()
{
	//if (ui->Auto->isChecked()){ return; }
	//没有选择不进行提示
	int nums = ui->tableWidget_boundary->rowCount();
	int jj = 0;
	for (int i = 0; i < nums; i++)
	{

		if (ui->tableWidget_boundary->item(i, 0)->checkState() == Qt::Checked){
			jj++;
		}
	}
	if (!jj){
		return;
	}

	if (ui->lineEdit_bcgt->text().toDouble()>=0 && ui->lineEdit_bcd->text().toDouble()>=0 && ui->lineEdit_bgt->text().toDouble()>=0 && ui->lineEdit_10->text().toDouble()>=0){
		if ((ui->lineEdit_bgt->text().toDouble() <ui->lineEdit_bcgt->text().toDouble())||(ui->lineEdit_bcgt->text().toDouble()<1.0)){
			flagWarning = 1;
			Warning2();
			return;
		}
		if ((ui->lineEdit_10->text().toDouble() >ui->lineEdit_bcd->text().toDouble()) || (ui->lineEdit_10->text().toDouble()<0.0)){
			Warning3();
			flagWarning = 1;
			return;
		}
		QSettings settings(QObject::tr("../ini/MeshSetting.ini"), QSettings::IniFormat);
		QString str = QObject::tr("Boundary");
		settings.remove(str);
		int num = ui->tableWidget_boundary->rowCount();
		int j = 0;
		for (int i = 0; i < num; i++)
		{

			if (ui->tableWidget_boundary->item(i, 0)->checkState() == Qt::Checked){
				QString name = ui->tableWidget_boundary->item(i, 0)->text();//获取内容
				settings.setValue(tr("/") + str + tr("/Boundary%1").arg(j + 1), name);
				j++;
			}
		}
		settings.setValue(tr("/") + str + tr("/Boundarysum"), j);
		settings.setValue(tr("/") + str + tr("/CoarestGridTimes"), ui->lineEdit_bcgt->text());
		settings.setValue(tr("/") + str + tr("/CoarsetDistance"), ui->lineEdit_bcd->text());
		settings.setValue(tr("/") + str + tr("/FinestGridTimes"), ui->lineEdit_bgt->text());
		settings.setValue(tr("/") + str + tr("/FinesetDistance"), ui->lineEdit_10->text());
	}
	else{
		Warning1();
		flagWarning = 1;
	}
}

void InitialMesh::Tablemodel()
{
	ui->tableWidget_model->setColumnCount(1);
	ui->tableWidget_model->horizontalHeader()->hide();
	ui->tableWidget_model->verticalHeader()->hide();
	App::Document* pdocs = App::GetApplication().getActiveDocument();
	std::vector<App::DocumentObject*> psels = pdocs->getObjectsOfType(App::DocumentObject::getClassTypeId());
	for (std::vector<App::DocumentObject*>::iterator it = psels.begin(); it != psels.end(); ++it)
	{
		
		std::string Group = (*it)->getNameInDocument();
		QString Groups = QString::fromStdString(Group);
		//if (Groups.left(5) == QObject::tr("Plane")){
			//continue;
		//}
		if (Groups.left(4)== QObject::tr("Line")){
			continue;
		}
		if (Groups.left(5) == QObject::tr("Group")){
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
		std::string label = (*it)->Label.getValue();
		QString Label = QString::fromStdString(label);
		QTableWidgetItem *item = new QTableWidgetItem(Label);
		M.insert(pair<string, string>(label,Group));
		//518label/QTableWidgetItem *item = new QTableWidgetItem(Groups);
		item->setCheckState(Qt::Unchecked);
		int row = ui->tableWidget_model->rowCount();
		ui->tableWidget_model->insertRow(row);
		ui->tableWidget_model->setItem(row, 0, item);

	}//for
	
}

void InitialMesh::modeini()
{
	//if (ui->Auto->isChecked()){ return; }
	int nums = ui->tableWidget_model->rowCount();
	int jj = 0;
	for (int i = 0; i < nums; i++)
	{

		if (ui->tableWidget_model->item(i, 0)->checkState() == Qt::Checked){
			jj++;
		}
	}
	if (!jj){ return; }

	if (ui->lineEdit_meshPart->text().toDouble()){
		QSettings settings(QObject::tr("../ini/MeshSetting.ini"), QSettings::IniFormat);
		QString str = QObject::tr("Model");
		settings.remove(str);
		int num = ui->tableWidget_model->rowCount();
		int j = 0;
		for (int i = 0; i < num; i++)
		{

			if (ui->tableWidget_model->item(i, 0)->checkState() == Qt::Checked){
				QString name = ui->tableWidget_model->item(i, 0)->text();//获取内容
				map<string, string >::iterator l_it;
				l_it = M.find(name.toStdString());
				//settings.setValue(tr("/") + str + tr("/Model%1").arg(j + 1), name);518label
				settings.setValue(tr("/") + str + tr("/Model%1").arg(j + 1), QString::fromStdString(l_it->second));
				settings.setValue(tr("/") + str + tr("/Modelname%1").arg(j + 1), QString::fromStdString(l_it->first));
				j++;
			}
		}
		
			settings.setValue(tr("/") + str + tr("/Modelsum"), j);
			settings.setValue(tr("/") + str + tr("/MeshSizeParts"), ui->lineEdit_meshPart->text());
			MeshPartFace();///使用facemap.dat时调用这句。
		
		
			
		
	}
	else{
		Warning7();
		flagWarning = 1;
	}
}
int InitialMesh::WriteIni()
{

	Boundaryini();
	modeini();
	if (!ui->wavelength->text().toDouble()){
		QMessageBox::warning(this, QString::fromLocal8Bit("Warning!"), QString::fromLocal8Bit("The global coarsest parameter is empty or invalid!"), QMessageBox::Ok);//中文可用
		flagWarning = 0;
		return 0;
	}
	//if (!ui->total->text().toDouble()){
	//	QMessageBox::warning(this, QString::fromLocal8Bit("Warning!"), QString::fromLocal8Bit("The mesh total parameter cannot be empty or invalid!"), QMessageBox::Ok);//中文可用
	//	flagWarning = 0;
	//	return 0;
	//}
	if (!flagWarning){
		QSettings settings(QObject::tr("../ini/MeshSetting.ini"), QSettings::IniFormat);
		QString str = QObject::tr("Mesh");
		settings.setValue(tr("/") + str + tr("/Gridwavelength"), ui->wavelength->text());
		//settings.setValue(tr("/") + str + tr("/totalgrids"), ui->total->text());
		settings.setValue(tr("MeshSettings/Name"), tr("MeshSettings"));
		return 1;
	}
	else{
		flagWarning = 0;
		//Warning();
		DeleteFile("..\\ini\\MeshSetting.ini");
		return 0;
	}
	
}

void InitialMesh::Ok()
{	
	DeleteFile("..\\ini\\MeshSetting.ini");
		int f=WriteIni();
		if (f){ 
			editflag = 0;
			QDialog::close();
			M.clear();//518label
			Gui::Application::Instance->AddProjectTreeObject("MySimIIGui::Meshes", "MSetting");
		}

}

void InitialMesh::tablebem()
{
	
	//int index=ui->InitialMeshTW->currentIndex();
	//if (index != 1){//如果是1已经进行设置过的就不需要再改变
	//	return;
	//}
	for (int i=ui->tableWidget_model->rowCount(); i>0; i--)
	{
		ui->tableWidget_model->removeRow(0);
	}
	for (int i = ui->tableWidget_boundary->rowCount(); i>0; i--)
	{
		ui->tableWidget_boundary->removeRow(0);
	}
	Tableboundary();
	Tablemodel();
	
}


// 从文件取到所要的面的编号
int InitialMesh::GetFaceCodeWithGenCodeFromData(int genFaceCode)
{

	QFile facefile(QObject::tr("../ini/facemap.dat"));
	int totalLine = 0;

	QVector<int> faceVec;
	if (!facefile.open(QIODevice::ReadOnly))
	{
		return 0;
	}

	QTextStream textstream(&facefile);

	while (!textstream.atEnd())
	{
		int buf;

		textstream >> buf;;
		if (textstream.atEnd()){
			break;
		}
		totalLine++;

		faceVec.append(buf);
		textstream >> buf;
		faceVec.append(buf);
	}
	facefile.close();

	for (int i = 0; i < totalLine; i++){
		if (faceVec[i * 2] == genFaceCode){
			return faceVec[i * 2 + 1];
		}
	}
	return 0;
}

void InitialMesh::MeshPartFace()
{
	QString strs = QObject::tr("");
	QSettings settings(QObject::tr("../ini/MeshSetting.ini"), QSettings::IniFormat);
	QString str = QObject::tr("Model");
	int num = ui->tableWidget_model->rowCount();
	int j = 0;
	App::Document* pdocs = App::GetApplication().getActiveDocument();
	std::vector<App::DocumentObject*> psels = pdocs->getObjectsOfType(App::DocumentObject::getClassTypeId());
	for (int i = 0; i < num; i++)
	{
		
		if (ui->tableWidget_model->item(i, 0)->checkState() == Qt::Checked){
			QString name = ui->tableWidget_model->item(i, 0)->text();//获取内容
			//settings.setValue(tr("/") + str + tr("/Model%1").arg(i + 1), name);
			map<string, string >::iterator l_it;//518label
			l_it = M.find(name.toStdString());

			j++;
			//}
			//}
			int count = 0;
			for (std::vector<App::DocumentObject*>::iterator it = psels.begin(); it != psels.end(); ++it)
			{
				int flag = 0;
				std::string Group = (*it)->getNameInDocument();
				QString Groups = QString::fromStdString(Group);
				if (Groups.left(5) == QObject::tr("Plane")){
					continue;
				}
				if (Groups == QString::fromStdString(l_it->second)){
					flag = 1;
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
				App::DocumentObject *pfaceObj = (*it);
				const TopoDS_Shape& ptShape = static_cast<Part::Feature *>(pfaceObj)->Shape.getValue();
				TopExp_Explorer pexpFace;
				for (pexpFace.Init(ptShape, TopAbs_FACE); pexpFace.More(); pexpFace.Next())
				{
					count++;
					if (flag){
						int s=GetFaceCodeWithGenCodeFromData(count);
						strs = strs + QObject::tr("%1,").arg(s);
					}
				}//forface

			}//forli
		}//if
	}//for
	settings.setValue(tr("/") + str + tr("/facesum"), strs.left(strs.length() - 1));
	//GetFaceCodeWithGenCodeFromData();
}


void InitialMesh::showEvent(QShowEvent *)
{
	ui->InitialMeshTW->setCurrentIndex(0);
	tablebem();
}

void InitialMesh::reviseinit(const char* str)
{
	editflag = 1;
	static MySimIIGui::InitialMesh p;
	p.show();
	QSettings settings(QObject::tr("../ini/MeshSetting.ini"), QSettings::IniFormat);
	QString tf;
	
	tf = settings.value(QObject::tr("Mesh/Gridwavelength")).toString();
	p.ui->wavelength->setText(tf);
	tf = settings.value(QObject::tr("Boundary/CoarestGridTimes")).toString();
	p.ui->lineEdit_bcgt->setText(tf);
	tf = settings.value(QObject::tr("Boundary/CoarsetDistance")).toString();
	p.ui->lineEdit_bcd->setText(tf);
	tf = settings.value(QObject::tr("Boundary/FinestGridTimes")).toString();
	p.ui->lineEdit_bgt->setText(tf);
	tf = settings.value(QObject::tr("Boundary/FinesetDistance")).toString();
	p.ui->lineEdit_10->setText(tf);

	tf = settings.value(QObject::tr("Model/MeshSizeParts")).toString();
	p.ui->lineEdit_meshPart->setText(tf);
	int num = settings.value(QObject::tr("Model/Modelsum")).toInt();
	for (int i = 1; i <=num; i++){
		tf = settings.value(QObject::tr("Model/Modelname%1").arg(i)).toString();
		int sum = p.ui->tableWidget_model->rowCount();
		for (int i = 0; i < sum; i++)
		{

			if (p.ui->tableWidget_model->item(i, 0)->text().toStdString() == tf.toStdString()){
				p.ui->tableWidget_model->item(i, 0)->setCheckState(Qt::Checked);
				
			}
		}
	}


     num = settings.value(QObject::tr("Boundary/Boundarysum")).toInt();
	for (int i = 1; i <=num; i++){
		tf = settings.value(QObject::tr("Boundary/Boundary%1").arg(i)).toString();
		int sum = p.ui->tableWidget_boundary->rowCount();
		for (int i = 0; i < sum; i++)
		{

			if (p.ui->tableWidget_boundary->item(i, 0)->text().toStdString() == tf.toStdString()){
				p.ui->tableWidget_boundary->item(i, 0)->setCheckState(Qt::Checked);
			}
		}
	}

}

#include "moc_InitialMesh.cpp"
