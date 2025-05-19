#include "PreCompiled.h"
#ifndef _PreComp_
#include <QMessageBox>
#endif
//#include "ui_Boundaries.h"
#include "Boundaries.h"
#include<Base/Reader.h>
#include <Base/Console.h>
/*******ini********/
#include <QtCore/QCoreApplication>
#include <QSettings>
#include <QStringList>
#include <QList>
#include <QListIterator>
#include <string>
#include "CommonFunction.h"
#include "Geometry.h"
using namespace MySimIIGui;
using namespace std;
int MySimIIGui::Boundaries::reviseflag = 0;
int MySimIIGui::Boundaries::NVisual = 0;
int MySimIIGui::Boundaries::revisecount = 0;
int MySimIIGui::Boundaries::FaceSum=0;
Boundaries::Boundaries(QWidget *parent) :
QWidget(parent), 
ui(new Ui::Boundariess)
{
	ui->setupUi(this);
	connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(PushButtonok()));
	connect(ui->pushButton_Cancel, SIGNAL(clicked()), this, SLOT(PushButtoncancel()));
}

void Boundaries::PushButtonok(void)
{
	QSettings settings(QObject::tr("../ini/MySimII-ui.ini"), QSettings::IniFormat);
	int count_setting=0;
	if( ! settings.value(tr("BSettingHead/Count")).toString().isEmpty() )
	{ count_setting = settings.value(tr("BSettingHead/Count")).toInt(); }

        if(reviseflag != 1) { 
	   ++count_setting ;
	   settings.setValue(QObject::tr("BSettingHead/Count"), count_setting );
	   int MaxVisual = settings.value(tr("BSettingHead/MaxVisual")).toInt();
	   ++MaxVisual ;
	   settings.setValue(QObject::tr("BSettingHead/MaxVisual"), MaxVisual );
	   settings.setValue(QObject::tr("BSettingHead/BSetting%1").arg(count_setting), QObject::tr("BSetting%1").arg(MaxVisual) );
	   NVisual = MaxVisual;
	}
	else{
	}

	MySimIIGui::Geometry geo;
	geo.GeometrySum();
	int FaceSum = geo.ModelFace;
	if( FaceSum == 0) // No faces are selected
	{
  	  QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Selection error"), 
  			     QObject::tr("One object MUST be selected! Only ONE!!"));
	  return;
        }
	//int FaceSum = CommonFunction::FaceSum();
	
	int count = 0;
	QString BFaces = QObject::tr("");
	std::vector<int> BFaceList;

	for(int n = 0; n < FaceSum; n++)
	{
	    int i=n/3;
	    int j=n%3;
            if(ui->TabApplyTo->item(i, j)->checkState() == Qt::Checked)
	    {
	       ++count;
	       BFaceList.push_back(n+1);
	       QString tStr   = QObject::tr("");
	       tStr.sprintf("%d ",n+1);
	       BFaces.append(tStr);
	    }
	}
	settings.setValue(QObject::tr("BSetting%1/Count").arg(NVisual), count );
	settings.setValue(QObject::tr("BSetting%1/BFaces").arg(NVisual), BFaces);

	// correcting other faces which are being set by the setting NVisual
        for ( int ns = 1; ns <= count_setting; ns++ )
        {
              if ( ns == NVisual ) continue;

	      // to check out faces which are not covered by the setting NVisual
	      QString TFaces= settings.value(QObject::tr("BSetting%1/BFaces").arg(ns)).toString();
	      QString TFacesNew = QObject::tr("");

	      count=0;
	      QStringList qsl = TFaces.split(QObject::tr(" "));
	      for( int n=0; n<qsl.size(); n++ ) 
              {
		    if(qsl[n].isEmpty()) {continue;}
		    int m = qsl[n].toInt();
                    if( std::find(BFaceList.begin(), BFaceList.end(), m) == BFaceList.end())
		    { 
	               QString tStr = QObject::tr("");
	               tStr.sprintf("%d ",m);
	               TFacesNew.append(tStr);
		       count++;
		    }
	      }

	      settings.setValue(QObject::tr("BSetting%1/Count").arg(ns), count );
	      settings.setValue(QObject::tr("BSetting%1/BFaces").arg(ns), TFacesNew);
	}


//CF--Computational Fluid Dynamics
	QVariant Value;
	settings.setValue(QObject::tr("BSetting%1/CF_LCoord").arg(NVisual), ui->Coordinates->isChecked() == 1 ? tr("TRUE") : tr("FALSE"));
	settings.setValue(QObject::tr("BSetting%1/CF_Wall").arg(NVisual), ui->Wall->isChecked() == 1 ? tr("TRUE") : tr("FALSE"));
	settings.setValue(QObject::tr("BSetting%1/CF_FreeSurf").arg(NVisual), ui->FreeSurf->isChecked() == 1 ? tr("TRUE") : tr("FALSE"));
	settings.setValue(QObject::tr("BSetting%1/CF_FreeMoving").arg(NVisual), ui->FreeMoving->isChecked() == 1 ? tr("TRUE") : tr("FALSE"));
	Value=ui->lineEdit_VelX->text().toDouble();
	if( ! ui->lineEdit_VelX->text().isEmpty() )
	{ settings.setValue(QObject::tr("BSetting%1/CF_Vel1").arg(NVisual), Value); }
	Value=ui->lineEdit_VelY->text().toDouble();
	if( ! ui->lineEdit_VelY->text().isEmpty() )
	{ settings.setValue(QObject::tr("BSetting%1/CF_Vel2").arg(NVisual), Value); }
	Value=ui->lineEdit_VelZ->text().toDouble();
	if( ! ui->lineEdit_VelZ->text().isEmpty() )
	{ settings.setValue(QObject::tr("BSetting%1/CF_Vel3").arg(NVisual), Value); }
	Value=ui->lineEdit_Pressure->text().toDouble();
	if( ! ui->lineEdit_Pressure->text().isEmpty() )
	{ settings.setValue(QObject::tr("BSetting%1/CF_Pres").arg(NVisual), Value); }
	Value=ui->lineEdit_SlipCoeff->text().toDouble();
	if( ! ui->lineEdit_SlipCoeff->text().isEmpty() )
	{ settings.setValue(QObject::tr("BSetting%1/CF_SlipCoeff").arg(NVisual), Value); }
	
	QVariant NValue;
	NValue=ui->lineEdit_Temperature->text().toDouble();
	if( ! ui->lineEdit_Temperature->text().isEmpty() )
	{ settings.setValue(QObject::tr("BSetting%1/Heat_Temp").arg(NVisual), NValue); }
	NValue=ui->lineEdit_HeatFlx->text().toDouble();
	if( ! ui->lineEdit_HeatFlx->text().isEmpty() )
	{ settings.setValue(QObject::tr("BSetting%1/Heat_Flux").arg(NVisual), NValue); }
	
	settings.setValue(QObject::tr("BSetting%1/SelfDef1").arg(NVisual), ui->TextEdit_SelfDefinition1->toPlainText());
	settings.setValue(QObject::tr("BSetting%1/SelfDef2").arg(NVisual), ui->TextEdit_SelfDefinition2->toPlainText());

	settings.setValue(QObject::tr("BSetting%1/EM_Type").arg(NVisual), ui->ComB_EM->currentText());
	Value=ui->lineEdit_IncWaveX->text().toDouble();
	if( ! ui->lineEdit_IncWaveX->text().isEmpty() )
	{ settings.setValue(QObject::tr("BSetting%1/IncWaveX").arg(NVisual), Value); }
	Value=ui->lineEdit_IncWaveY->text().toDouble();
	if( ! ui->lineEdit_IncWaveY->text().isEmpty() )
	{ settings.setValue(QObject::tr("BSetting%1/IncWaveY").arg(NVisual), Value); }
	Value=ui->lineEdit_IncWaveZ->text().toDouble();
	if( ! ui->lineEdit_IncWaveZ->text().isEmpty() )
	{ settings.setValue(QObject::tr("BSetting%1/IncWaveZ").arg(NVisual), Value); }
	Value=ui->lineEdit_IncPropX->text().toDouble();
	if( ! ui->lineEdit_IncPropX->text().isEmpty() )
	{ settings.setValue(QObject::tr("BSetting%1/IncPropX").arg(NVisual), Value); }
	Value=ui->lineEdit_IncPropY->text().toDouble();
	if( ! ui->lineEdit_IncPropY->text().isEmpty() )
	{ settings.setValue(QObject::tr("BSetting%1/IncPropY").arg(NVisual), Value); }
	Value=ui->lineEdit_IncPropZ->text().toDouble();
	if( ! ui->lineEdit_IncPropZ->text().isEmpty() )
	{ settings.setValue(QObject::tr("BSetting%1/IncPropZ").arg(NVisual), Value); }

        if(reviseflag != 1) { 
	   QString BSetting = tr("BSetting%1").arg(NVisual);
	   Gui::Application::Instance->AddProjectTreeObject("MySimIIGui::Boundaries", BSetting.toStdString().c_str());
	}

	QWidget::close();

}

void Boundaries::PushButtoncancel(void)
{
	QWidget::close();
	
}

void Boundaries::showEvent(QShowEvent *)
{

	MySimIIGui::Geometry geo;
	geo.GeometrySum();
	int facenum = geo.ModelFace;
	if( facenum == 0) // No faces are selected
	{
  	  QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Selection error"), 
  			     QObject::tr("One face MUST be selected at least!!!"));
	  return;
        }

	QSettings settings(QObject::tr("../ini/MySimII-ui.ini"), QSettings::IniFormat);
	FaceSum = facenum;
	std::vector<int> faceStr;
	if (reviseflag != 1) // new setting
	{ 
	  faceStr=geo.SelectedModelFaces();
	  
	  for(int n=0; n<faceStr.size(); n++) {
	     Base::Console().Message("Selected face: %d \n", faceStr[n]);
	  }

	  if( faceStr.size() == 0) // No faces are selected
	  {
	    Base::Console().Message("Faces must be selected, please CANCEL and redo selection !!!");
	    return;
          }
  	  // static MySimIIGui::Geometry p;
  	  QString Info;
  	  if(faceStr.size()==1) 
	    { Info = Geometry::getFaceInfo(faceStr[0]); }
  	  else
            { Info = QObject::tr("NOT a single face selected!!!"); }
  	  ui->TextEdit_SelfDefinition1->setText(Info);
  
          Info =       QObject::tr("## to include as Elmer MATC functions ##\n");
          Info.append( QObject::tr("## arbitrary <i> <j> stand for loops: ##\n") );
          Info.append( QObject::tr("                                        \n") );
          Info.append( QObject::tr("variable<i> = FUNCTION<j>(t,x,y,z)      \n") );
          Info.append( QObject::tr("                                        \n") );
          Info.append( QObject::tr("## FUNCTION<j> must be defined above. ##\n") );
  	  ui->TextEdit_SelfDefinition2->setText(Info);
	}
	else
	{
	  faceStr.clear();
	  QString BFaces = settings.value(QObject::tr("BSetting%1/BFaces").arg(NVisual)).toString();
	  QStringList qsl = BFaces.split(QObject::tr(" "));
	  for( int n=0; n<qsl.size(); n++ ) 
          {
	     int m = qsl[n].toInt();
	     faceStr.push_back(m);
	  }

	  QString tStr;
	  tStr = settings.value(QObject::tr("BSetting%1/CF_LCoord").arg(NVisual)).toString();
	  ui->Coordinates->setChecked(FALSE) ;
	  if(tStr == QObject::tr("TRUE")) { ui->Coordinates->setChecked(TRUE) ; }
	  tStr = settings.value(QObject::tr("BSetting%1/CF_Wall").arg(NVisual)).toString();
	  ui->Wall->setChecked(FALSE) ;
	  if(tStr == QObject::tr("TRUE")) { ui->Wall->setChecked(TRUE) ; }
	  tStr = settings.value(QObject::tr("BSetting%1/CF_FreeSurf").arg(NVisual)).toString();
	  ui->FreeSurf->setChecked(FALSE) ;
	  if(tStr == QObject::tr("TRUE")) { ui->FreeSurf->setChecked(TRUE) ; }
	  tStr = settings.value(QObject::tr("BSetting%1/CF_FreeMoving").arg(NVisual)).toString();
	  ui->FreeMoving->setChecked(FALSE) ;
	  if(tStr == QObject::tr("TRUE")) { ui->FreeMoving->setChecked(TRUE) ; }
	  tStr = settings.value(QObject::tr("BSetting%1/CF_Vel1").arg(NVisual)).toString();
	  ui->lineEdit_VelX->setText(tStr);
	  tStr = settings.value(QObject::tr("BSetting%1/CF_Vel2").arg(NVisual)).toString();
	  ui->lineEdit_VelY->setText(tStr);
	  tStr = settings.value(QObject::tr("BSetting%1/CF_Vel3").arg(NVisual)).toString();
	  ui->lineEdit_VelZ->setText(tStr);
	  tStr = settings.value(QObject::tr("BSetting%1/CF_Pres").arg(NVisual)).toString();
	  ui->lineEdit_Pressure->setText(tStr);
	  tStr = settings.value(QObject::tr("BSetting%1/CF_SlipCoeff").arg(NVisual)).toString();
	  ui->lineEdit_SlipCoeff->setText(tStr);
	  
	  tStr = settings.value(QObject::tr("BSetting%1/Heat_Temp").arg(NVisual)).toString();
	  ui->lineEdit_Temperature->setText(tStr);
	  tStr = settings.value(QObject::tr("BSetting%1/Heat_Flux").arg(NVisual)).toString();
	  ui->lineEdit_HeatFlx->setText(tStr);
	  
	  tStr = settings.value(QObject::tr("BSetting%1/EM_Type").arg(NVisual)).toString();
	  int index = ui->ComB_EM->findText(tStr);
	  ui->ComB_EM->setItemText(index, tStr);
	  tStr = settings.value(QObject::tr("BSetting%1/IncWaveX").arg(NVisual)).toString();
	  ui->lineEdit_IncWaveX->setText(tStr);
	  tStr = settings.value(QObject::tr("BSetting%1/IncWaveY").arg(NVisual)).toString();
	  ui->lineEdit_IncWaveY->setText(tStr);
	  tStr = settings.value(QObject::tr("BSetting%1/IncWaveZ").arg(NVisual)).toString();
	  ui->lineEdit_IncWaveZ->setText(tStr);
	  tStr = settings.value(QObject::tr("BSetting%1/IncPropX").arg(NVisual)).toString();
	  ui->lineEdit_IncPropX->setText(tStr);
	  tStr = settings.value(QObject::tr("BSetting%1/IncPropY").arg(NVisual)).toString();
	  ui->lineEdit_IncPropY->setText(tStr);
	  tStr = settings.value(QObject::tr("BSetting%1/IncPropZ").arg(NVisual)).toString();
	  ui->lineEdit_IncPropZ->setText(tStr);
	  
	  tStr = settings.value(QObject::tr("BSetting%1/SelfDef1").arg(NVisual)).toString();
	  ui->TextEdit_SelfDefinition1->setText(tStr);
	  tStr = settings.value(QObject::tr("BSetting%1/SelfDef2").arg(NVisual)).toString();
	  ui->TextEdit_SelfDefinition2->setText(tStr);
	}

  	ui->TabApplyTo->clear();
  	ui->TabApplyTo->setColumnCount(3);
  	int num=facenum%3==0?facenum/3:facenum/3+1;
  	ui->TabApplyTo->setRowCount(num);
  	ui->TabApplyTo->horizontalHeader()->hide();
  	ui->TabApplyTo->verticalHeader()->hide();
  	ui->TabApplyTo->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
  	int j = 0;
  	for (int i = 0; i<facenum; i++)
  	{
  		QString value=QObject::tr("Face%1").arg(i+1);
  		QTableWidgetItem *item = new QTableWidgetItem(value);
  		if(std::find(faceStr.begin(),faceStr.end(),i+1)==faceStr.end()){
  			item->setCheckState(Qt::Unchecked);
  		}
  		else{
  			item->setCheckState(Qt::Checked);
  		}
  		if (i % 3 == 0){
  			ui->TabApplyTo->setItem(j, 0, item);
  		}
  		if (i % 3 == 1){
  			ui->TabApplyTo->setItem(j, 1, item);
  		}
  		if (i % 3 == 2){
  			ui->TabApplyTo->setItem(j, 2, item);
  			j++;
  		}
  	}
  
        ui->TabWid_1->setCurrentIndex(0);

}


void Boundaries::reviseinit(const char* str)
{
	reviseflag = 1;
	QString name = QString(QLatin1String(str));
	NVisual=name.mid(8).toInt();
	static MySimIIGui::Boundaries p;
	p.show();
}


Boundaries::~Boundaries()
{

}

#include "moc_Boundaries.cpp"
