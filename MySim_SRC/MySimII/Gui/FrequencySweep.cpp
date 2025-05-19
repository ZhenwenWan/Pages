#include "PreCompiled.h"
#ifndef _PreComp_
#include <QMessageBox>
#endif
#include "ui_FrequencySweep.h"
#include "FrequencySweep.h"
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
using namespace MySimIIGui;
FrequencySweep::FrequencySweep(QMainWindow *parent) :QMainWindow(parent), ui(new Ui::FrequencySweep)
{
	ui->setupUi(this);
	connect(ui->OK, SIGNAL(clicked(bool)), this, SLOT(Ok()));
	connect(ui->Cancel, SIGNAL(clicked(bool)), this, SLOT(Cancel()));
	play();
	

}
FrequencySweep::~FrequencySweep()
{

}
void FrequencySweep::Warning()
{
	//QMessageBox::warning(this, QString::fromLocal8Bit("Warning!"), QString::fromLocal8Bit("Can not be empty!"), QMessageBox::Ok);//中文可用
}

void FrequencySweep::play()
{	
	QStringList qsList;
	qsList << QApplication::translate("MySimIIGui::SolutionSetup", "GHz", 0, QApplication::UnicodeUTF8)
		<< QApplication::translate("MySimIIGui::SolutionSetup", "MHz", 0, QApplication::UnicodeUTF8)
		<< QApplication::translate("MySimIIGui::SolutionSetup", "KHz", 0, QApplication::UnicodeUTF8)
		<< QApplication::translate("MySimIIGui::SolutionSetup", "Hz", 0, QApplication::UnicodeUTF8);
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

void FrequencySweep::Ok()
{
	
	QSettings settings(QObject::tr("../ini/SolutionSetup.ini"), QSettings::IniFormat);
	settings.setValue(QObject::tr("FrequencyDomain/StartValue"), ui->tableWidget->item(0, 0)->text());
	QWidget *widget=ui->tableWidget->cellWidget(0, 1);
	QComboBox *combox = (QComboBox*)widget;
	settings.setValue(QObject::tr("FrequencyDomain/StartUnit"), combox->currentText());
	settings.setValue(QObject::tr("FrequencyDomain/StopValue"), ui->tableWidget->item(1, 0)->text());
	widget = ui->tableWidget->cellWidget(1, 1);
	QComboBox *combox1 = (QComboBox*)widget;
	settings.setValue(QObject::tr("FrequencyDomain/StopUnit"), combox1->currentText());
	settings.setValue(QObject::tr("FrequencyDomain/StepSizeValue"), ui->tableWidget->item(2, 0)->text());
	widget = ui->tableWidget->cellWidget(2, 1);
	QComboBox *combox2 = (QComboBox*)widget;
	settings.setValue(QObject::tr("FrequencyDomain/StepSizeUnit"), combox2->currentText());
	Gui::Application::Instance->AddProjectTreeObject("MySimIIGui::FrequencyDomain", "FDSetup");//522
	QMainWindow::close();
}
void FrequencySweep::Cancel()
{
	QMainWindow::close();
}

void FrequencySweep::reviseinit(const char* str)
{
	QString name = QString(QLatin1String(str));
	static MySimIIGui::FrequencySweep p;
	p.show();
	QSettings settings(QObject::tr("../ini/SolutionSetup.ini"), QSettings::IniFormat);
	p.ui->tableWidget->item(0, 0)->setText(settings.value(QObject::tr("FrequencyDomain/StartValue")).toString());
	QWidget *widget = p.ui->tableWidget->cellWidget(0, 1);
	QComboBox *combox = (QComboBox*)widget;
	QString unit=settings.value(QObject::tr("FrequencyDomain/StartUnit")).toString();
	if (unit == QObject::tr("GHz")){
		combox->setCurrentIndex(0);
	}
	if (unit == QObject::tr("MHz")){
		combox->setCurrentIndex(1);
	}
	if (unit == QObject::tr("KHz")){
		combox->setCurrentIndex(2);
	}
	if (unit == QObject::tr("Hz")){
		combox->setCurrentIndex(3);
	}
	
	
		p.ui->tableWidget->item(1, 0)->setText(settings.value(QObject::tr("FrequencyDomain/StopValue")).toString());
	unit = settings.value(QObject::tr("FrequencyDomain/StopUnit")).toString();
		widget = p.ui->tableWidget->cellWidget(1, 1);
		combox = (QComboBox*)widget;
		if(unit == QObject::tr("GHz")){
			combox->setCurrentIndex(0);
		}
		if (unit == QObject::tr("MHz")){
			combox->setCurrentIndex(1);
		}
		if (unit == QObject::tr("KHz")){
			combox->setCurrentIndex(2);
		}
		if (unit == QObject::tr("Hz")){
			combox->setCurrentIndex(3);
		}
		p.ui->tableWidget->item(2, 0)->setText(settings.value(QObject::tr("FrequencyDomain/StepSizeValue")).toString());

		 unit = settings.value(tr("FrequencyDomain/StepSizeUnit")).toString();
	widget = p.ui->tableWidget->cellWidget(2, 1);
	combox = (QComboBox*)widget;
	if (unit == QObject::tr("GHz")){
		combox->setCurrentIndex(0);
	}
	if (unit == QObject::tr("MHz")){
		combox->setCurrentIndex(1);
	}
	if (unit == QObject::tr("KHz")){
		combox->setCurrentIndex(2);
	}
	if (unit == QObject::tr("Hz")){
		combox->setCurrentIndex(3);
	}
	
	

	
	
}
#include "moc_FrequencySweep.cpp"