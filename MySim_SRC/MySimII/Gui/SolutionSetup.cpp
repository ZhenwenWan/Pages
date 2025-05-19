#include "PreCompiled.h"
#ifndef _PreComp_
#include <QMessageBox>
#endif
#include "ui_SolutionSetup.h"
#include "SolutionSetup.h"
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
/********ini********/
#include <QtCore/QCoreApplication>
#include <QSettings>
#include <QStringList>
#include <QString>
//#include <QListInterator>
#include <string>
using namespace MySimIIGui;

SolutionSetup::SolutionSetup(QMainWindow *parent) :QMainWindow(parent), ui(new Ui::SolutionSetup)
{
	ui->setupUi(this);
	connect(ui->Cancel, SIGNAL(clicked()), this, SLOT(Cancel()));
	connect(ui->OK, SIGNAL(clicked()), this, SLOT(OK()));
	insertcomboBox();
	
}
void SolutionSetup::Cancel()
{	
	QMainWindow::close();
}
SolutionSetup::~SolutionSetup()
{
	
}
void SolutionSetup::Warning()
{
	//QMessageBox::warning(this, QString::fromLocal8Bit("Warning!"), QString::fromLocal8Bit("Can not be empty!"), QMessageBox::Ok);//中文可用
}



void SolutionSetup::insertcomboBox()
{
	QStringList qsList;
	qsList << QApplication::translate("MySimIIGui::SolutionSetup", "Second", 0, QApplication::UnicodeUTF8)
		<< QApplication::translate("MySimIIGui::SolutionSetup", "Minute", 0, QApplication::UnicodeUTF8)
	    << QApplication::translate("MySimIIGui::SolutionSetup", "Hour", 0, QApplication::UnicodeUTF8);
	QComboBox *comboBox_1 = new QComboBox();
	comboBox_1->insertItems(1, qsList);
	QComboBox *comboBox_2 = new QComboBox();
	comboBox_2->insertItems(1, qsList);
	QComboBox *comboBox_3 = new QComboBox();
	comboBox_3->insertItems(3, qsList);
	ui->tableWidget->setCellWidget(0, 1, comboBox_1);
	ui->tableWidget->setCellWidget(1, 1, comboBox_2);
	ui->tableWidget->setCellWidget(2, 1, comboBox_3);
}

void SolutionSetup::OK()
{
	QSettings settings(QObject::tr("../ini/SolutionSetup.ini"), QSettings::IniFormat);
	settings.setValue(QObject::tr("TimeDomain/StartValue"), ui->tableWidget->item(0, 0)->text());
	QWidget *widget = ui->tableWidget->cellWidget(0, 1);
	QComboBox *combox = (QComboBox*)widget;
	settings.setValue(QObject::tr("TimeDomain/StartUnit"), combox->currentText());
	settings.setValue(QObject::tr("TimeDomain/StopValue"), ui->tableWidget->item(1, 0)->text());
	widget = ui->tableWidget->cellWidget(1, 1);
	QComboBox *combox1 = (QComboBox*)widget;
	settings.setValue(QObject::tr("TimeDomain/StopUnit"), combox1->currentText());
	settings.setValue(QObject::tr("TimeDomain/StepSizeValue"), ui->tableWidget->item(2, 0)->text());
	widget = ui->tableWidget->cellWidget(2, 1);
	QComboBox *combox2 = (QComboBox*)widget;
	settings.setValue(QObject::tr("TimeDomain/StepSizeUnit"), combox2->currentText());
	Gui::Application::Instance->AddProjectTreeObject("MySimIIGui::TimeDomain", "TDSetup");//522
	QMainWindow::close();

}




void SolutionSetup::reviseinit(const char* str)
{
	QString name = QString(QLatin1String(str));
	static MySimIIGui::SolutionSetup p;
	p.show();
	QSettings settings(QObject::tr("../ini/SolutionSetup.ini"), QSettings::IniFormat);
	p.ui->tableWidget->item(0, 0)->setText(settings.value(QObject::tr("TimeDomain/StartValue")).toString());
	QWidget *widget = p.ui->tableWidget->cellWidget(0, 1);
	QComboBox *combox = (QComboBox*)widget;
	QString unit=settings.value(QObject::tr("TimeDomain/StartUnit")).toString();
	if (unit == QObject::tr("Second")){
		combox->setCurrentIndex(0);
	}
	if (unit == QObject::tr("Minute")){
		combox->setCurrentIndex(1);
	}
	if (unit == QObject::tr("Hour")){
		combox->setCurrentIndex(2);
	}
	
		p.ui->tableWidget->item(1, 0)->setText(settings.value(QObject::tr("TimeDomain/StopValue")).toString());
	unit = settings.value(QObject::tr("TimeDomain/StopUnit")).toString();
		widget = p.ui->tableWidget->cellWidget(1, 1);
		combox = (QComboBox*)widget;
		if(unit == QObject::tr("Second")){
			combox->setCurrentIndex(0);
		}
		if (unit == QObject::tr("Minute")){
			combox->setCurrentIndex(1);
		}
		if (unit == QObject::tr("Hour")){
			combox->setCurrentIndex(2);
		}
		p.ui->tableWidget->item(2, 0)->setText(settings.value(QObject::tr("TimeDomain/StepSizeValue")).toString());

		 unit = settings.value(tr("TimeDomain/StepSizeUnit")).toString();
	widget = p.ui->tableWidget->cellWidget(2, 1);
	combox = (QComboBox*)widget;
	if (unit == QObject::tr("Second")){
		combox->setCurrentIndex(0);
	}
	if (unit == QObject::tr("MHz")){
		combox->setCurrentIndex(1);
	}
	if (unit == QObject::tr("Hour")){
		combox->setCurrentIndex(2);
	}
	
}

#include "moc_SolutionSetup.cpp"