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
#include "SolverSettings.h"

using namespace MySimIIGui;
using namespace std;

SolverSettings::SolverSettings(QWidget *parent) :
QWidget(parent), 
ui(new Ui::SolverSettings)
{
	ui->setupUi(this);
	ui->IterativeSolver->setChecked(true);
	connect(ui->PushButton_Ok, SIGNAL(clicked()), this, SLOT(PushButtonok()));
	connect(ui->IterativeSolver, SIGNAL(clicked()), this, SLOT(EnableIterativeSolver()));
}


void SolverSettings::PushButtonok(void)
{
	QSettings settings(QObject::tr("../ini/MySimII-ui.ini"), QSettings::IniFormat);
	
	settings.setValue(QObject::tr("SolverSettings/SolverName"), ui->ComboBox_Solvers->currentText());
	settings.setValue(QObject::tr("SolverSettings/SolverType"), ui->ComboBox_Type->currentText());
	settings.setValue(QObject::tr("SolverSettings/WorkDirectory"), ui->FileChooser_Dir->fileName());
	settings.setValue(QObject::tr("SolverSettings/IterativeSolver"), ui->IterativeSolver->isChecked() == 1 ? tr("TRUE") : tr("FALSE"));
	settings.setValue(QObject::tr("SolverSettings/DirectiveSolver"), ui->DirectiveSolver->isChecked() == 1 ? tr("TRUE") : tr("FALSE"));
	if( ! ui->LineEdit_Starting->text().isEmpty() ) {
	   settings.setValue(QObject::tr("SolverSettings/Starting"), ui->LineEdit_Starting->text().toDouble());
	}
	if( ! ui->LineEdit_Ending->text().isEmpty() ) {
	   settings.setValue(QObject::tr("SolverSettings/Ending"), ui->LineEdit_Ending->text().toDouble());
	}
	if( ! ui->LineEdit_Step->text().isEmpty() ) {
	   settings.setValue(QObject::tr("SolverSettings/StepSize"), ui->LineEdit_Step->text().toDouble());
	}
	if( ! ui->LineEdit_OutputSteps->text().isEmpty() ) {
	   settings.setValue(QObject::tr("SolverSettings/OutputSteps"), ui->LineEdit_OutputSteps->text().toInt());
	}
	if( ! ui->LineEdit_MaxIterSteps->text().isEmpty() ) {
	   settings.setValue(QObject::tr("SolverSettings/MaxIterSteps"), ui->LineEdit_MaxIterSteps->text().toInt());
	}
	if( ! ui->LineEdit_ConvergRelatErr->text().isEmpty() ) {
	   settings.setValue(QObject::tr("SolverSettings/ConvergRelatErr"), ui->LineEdit_ConvergRelatErr->text().toDouble());
	}
	if( ! ui->LineEdit_Scaling->text().isEmpty() ) {
	   settings.setValue(QObject::tr("SolverSettings/Scaling"), ui->LineEdit_Scaling->text().toDouble());
	}

	Gui::Application::Instance->AddProjectTreeObject("MySimIIGui::Solvers", ui->ComboBox_Solvers->currentText().toStdString().c_str());
	QWidget::close();
}

void SolverSettings::showEvent(QShowEvent *)
{
        ui->FileChooser_Dir->setFileName(QObject::tr("../ini/"));
        ui->TabWidget_1->setCurrentIndex(0);
}

void SolverSettings::reviseinit(const char* str)
{
	QSettings settings(QObject::tr("../ini/MySimII-ui.ini"), QSettings::IniFormat);

	static MySimIIGui::SolverSettings p;
	p.show();

	QString Solver, Type, WorkDirectory, IterativeSolver, DirectiveSolver;
	QString Starting, Ending, Step, ConvergRelatErr;
	QString OutputSteps, MaxIterSteps;
	QString Scaling;
        int     index;

        Solver = settings.value(tr("SolverSettings/SolverName")).toString();
        index  = p.ui->ComboBox_Solvers->findText(Solver);
        p.ui->ComboBox_Solvers->setCurrentIndex(index);
	
        Type  = settings.value(tr("SolverSettings/SolverType")).toString();
        index = p.ui->ComboBox_Type->findText(Type);
        p.ui->ComboBox_Type->setCurrentIndex(index);
	
        WorkDirectory    = settings.value(tr("SolverSettings/WorkDirectory")).toString();
        IterativeSolver  = settings.value(tr("SolverSettings/IterativeSolver")).toString();
        DirectiveSolver  = settings.value(tr("SolverSettings/DirectiveSolver")).toString();
        Starting         = settings.value(tr("SolverSettings/Starting")).toString();
        Ending           = settings.value(tr("SolverSettings/Ending")).toString();
        Step             = settings.value(tr("SolverSettings/StepSize")).toString();
        ConvergRelatErr  = settings.value(tr("SolverSettings/ConvergRelatErr")).toString();
        OutputSteps      = settings.value(tr("SolverSettings/OutputSteps")).toString();
        MaxIterSteps     = settings.value(tr("SolverSettings/MaxIterSteps")).toString();
        Scaling          = settings.value(tr("SolverSettings/Scaling")).toString();
	
	if (IterativeSolver == tr("TRUE") || DirectiveSolver == tr("FALSE")){
		p.ui->IterativeSolver->setChecked(true);
		p.ui->DirectiveSolver->setChecked(false);
	}
	else{
		p.ui->IterativeSolver->setChecked(false);
		p.ui->DirectiveSolver->setChecked(true);
	}

        p.ui->FileChooser_Dir         ->setFileName(WorkDirectory);
	p.ui->LineEdit_Starting       ->setText(Starting);
	p.ui->LineEdit_Ending         ->setText(Ending);
	p.ui->LineEdit_Step           ->setText(Step);
	p.ui->LineEdit_ConvergRelatErr->setText(ConvergRelatErr);
	p.ui->LineEdit_OutputSteps    ->setText(OutputSteps);
	p.ui->LineEdit_MaxIterSteps   ->setText(MaxIterSteps);
	p.ui->LineEdit_Scaling        ->setText(Scaling);
	
	Gui::Application::Instance->DeleteProjectTreeObject("MySimIIGui::Solvers", Solver.toStdString().c_str());
	
}


void SolverSettings::EnableIterativeSolver()
{
	if (!ui->IterativeSolver->isChecked())
	{
	}
	else
	{
	}
}


SolverSettings::~SolverSettings()
{

}


#include "moc_SolverSettings.cpp"
