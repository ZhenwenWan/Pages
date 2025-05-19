#include "PreCompiled.h"
#ifndef _PreComp_
#include <QMessageBox>
#endif
#include "ui_RotateXYZ.h"
#include "RotateXYZ.h"
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
#include <string>
#include <QTime>
#define vtkRenderingCore_AUTOINIT 4(vtkInteractionStyle,vtkRenderingFreeType,vtkRenderingFreeTypeOpenGL,vtkRenderingOpenGL2)
#define vtkRenderingVolume_AUTOINIT 1(vtkRenderingVolumeOpenGL2)
using namespace MySimIIGui;
RotateXYZ::RotateXYZ(QDialog *parent) :QDialog(parent), ui(new Ui::RotateXYZDialog)
{
	ui->setupUi(this);	
	actor = vtkSmartPointer<vtkActor>::New();
	transform1 = vtkSmartPointer<vtkTransform>::New();
	ren = vtkSmartPointer<vtkRenderer>::New();
	renWin = vtkSmartPointer<vtkRenderWindow>::New();
	iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	points = vtkSmartPointer<vtkPoints>::New();
	cell = vtkSmartPointer<vtkCellArray>::New();
	scalars = vtkSmartPointer<vtkFloatArray>::New();
	pointsPoly = vtkSmartPointer<vtkPolyData>::New();
	pointsMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	camera = vtkSmartPointer<vtkCamera>::New();
	pColorTable = vtkSmartPointer<vtkLookupTable>::New();
	cube = vtkSmartPointer<vtkCubeAxesActor>::New();
	bar = vtkSmartPointer<vtkScalarBarActor>::New();
	axes = vtkSmartPointer<vtkAxesActor>::New();
	widget = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
	pTransformPoly = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	linear = vtkSmartPointer<vtkLinearSubdivisionFilter>::New();
	filter = vtkSmartPointer<vtkTriangleFilter>::New();	
	connect(ui->CancelPB, SIGNAL(clicked(bool)), this, SLOT(Cancel()));	
	QRegExp regExp(tr("[0-9]{0,3}"));
	QRegExpValidator *regExpValidator = new QRegExpValidator(regExp, this);
	ui->RotateXLE->setValidator(regExpValidator);
	ui->RotateYLE->setValidator(regExpValidator);
	ui->RotateZLE->setValidator(regExpValidator);
	delete regExpValidator;
	connect(ui->OkPB, SIGNAL(clicked(bool)), this, SLOT(Ok()));

}
RotateXYZ::~RotateXYZ()
{
	

}

void RotateXYZ::WarningXYZ()
{
	if (ui->RotateXLE->text().isEmpty() || ui->RotateYLE->text().isEmpty() || ui->RotateZLE->text().isEmpty())
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("Warning!"), QString::fromLocal8Bit("Can not be empty!"), QMessageBox::Ok);//中文可用
	}

}
void RotateXYZ::Ok()
{
	RotXR = -RotX;
	RotYR = -RotY;
	RotZR = -RotZ;
	RotX = ui->RotateXLE->text().toInt();
	RotY = ui->RotateYLE->text().toInt();
	RotZ = ui->RotateZLE->text().toInt();
	RotXYZ(RotX, RotY, RotZ, RotXR, RotYR, RotZR, Rflag);
	
}
void RotateXYZ::Cancel()
{

	iren->GetRenderWindow()->Finalize();
	iren->TerminateApp();
	QDialog::close();
}

void RotateXYZ::RotXYZ(int rotx1, int roty1, int rotz1, int rotx2, int roty2, int rotz2, int flag)
{
	
	//画三维伪彩色图	
	QFile file(QObject::tr("../ini/3D_rectangle.dat"));
	QVector<double> arr;
	if (!file.open(QIODevice::ReadOnly))
	{
		return;
	}
	QTextStream textstream(&file);
	int tmp1 = 0;
	QStringList ss;
	QString s;
	while (!textstream.atEnd())
	{
		if (tmp1 < 1)
		{
			textstream >> s;
			ss.append(s + textstream.readLine());
		}
		else
		{
			static double buf;
			textstream >> buf;
			arr.append(buf);
		}
		tmp1++;
	}
	file.close();	
	int pp = 0;
	QString str1, str2, str3;
	for each (QString var in ss)
	{
		if (pp == 0)
		{
			str1 = var.mid(0, 3);
			str2 = var.mid(17, 3);
			str3 = var.mid(34, 3);
		}
	}
	double *u;
	double *p;
	double *x;
	double *y;
	double *z;
	double *v;
	u = new double[arr.count() / 19];
	p = new double[arr.count() / 19];
	x = new double[arr.count() / 19];
	y = new double[arr.count() / 19];
	z = new double[arr.count() / 19];
	v = new double[arr.count() / 19];

	for (int i = 0; i != arr.count(); ++i)
	{
		if (i % 19 == 0 && i / 19 < arr.count() / 19)
		{
			u[i / 19] = arr[i];			
		}
		if (i % 19 == 1)
		{
			p[i / 19] = arr[i];			
		}
		if (i % 19 == 2)
		{
			x[i / 19] = arr[i];			
		}
		if (i % 19 == 3)
		{
			y[i / 19] = arr[i];			
		}
		if (i % 19 == 4)
		{
			z[i / 19] = arr[i];			
		}
		switch (flag)
		{
		case 1://Mag_E  第三列
			if (i % 19 == 5)
			{
				v[i / 19] = arr[i];
			}
			break;
		case 2://Vector_E
			if (i % 19 == 6)
			{
				v[i / 19] = arr[i];				
			}
			break;
		case 3://Re_Ex
			if (i % 19 == 6)
			{
				v[i / 19] = arr[i];				
			}
			break;
		case 4://Re_Ey
			if (i % 19 == 7)
			{
				v[i / 19] = arr[i];				
			}
			break;
		case 5://Re_Ez
			if (i % 19 == 8)
			{
				v[i / 19] = arr[i];				
			}
			break;
		case 6://Im_Ex
			if (i % 19 == 9)
			{
				v[i / 19] = arr[i];				
			}
			break;
		case 7://Im_Ey
			if (i % 19 == 10)
			{
				v[i / 19] = arr[i];				
			}
			break;
		case 8://Im_Ez
			if (i % 19 == 11)
			{
				v[i / 19] = arr[i];				
			}
			break;
		case 9://Mag_H  第三列
			if (i % 19 == 12)
			{
				v[i / 19] = arr[i];				
			}
			break;
		case 10://Vector_H
			if (i % 19 == 13)
			{
				v[i / 19] = arr[i];				
			}
			break;
		case 11://Re_Hx
			if (i % 19 == 13)
			{
				v[i / 19] = arr[i];				
			}
			break;
		case 12://Re_Hy
			if (i % 19 == 14)
			{
				v[i / 19] = arr[i];				
			}
			break;
		case 13://Re_Hz
			if (i % 19 == 15)
			{
				v[i / 19] = arr[i];				
			}
			break;
		case 14://Im_Hx
			if (i % 19 == 16)
			{
				v[i / 19] = arr[i];				
			}
			break;
		case 15://Im_Hy
			if (i % 19 == 17)
			{
				v[i / 19] = arr[i];				
			}
			break;
		case 16://Im_Hz
			if (i % 19 == 18)
			{
				v[i / 19] = arr[i];				
			}
			break;
		default:
			break;
		}
	}
	
	double vmax, vmin;
	vmax = v[0];
	vmin = v[0];
	int umax = 0;
	for (int i = 0; i != arr.count(); ++i)
	{
		if (i % 19 == 0 && i / 19 < arr.count() / 19)
		{
			points->InsertPoint(p[i / 19], x[i / 19], y[i / 19], z[i / 19]);
			scalars->InsertTuple1(p[i / 19], v[i / 19]);
			vmax = vmax > v[i / 19] ? vmax : v[i / 19];
			vmin = vmin < v[i / 19] ? vmin : v[i / 19];
			umax = umax > u[i / 19] ? umax : u[i / 19];
		}
	}
	vtkIdType(*pts)[3] = new vtkIdType[umax][3];
	for (int i = 0; i != umax; ++i)
	{
		for (int j = 0; j != 3; ++j)
		{
			pts[i][j] = p[i * 3 + j];
		}
		cell->InsertNextCell(3, pts[i]);
	}
	pointsPoly->SetPoints(points);
	pointsPoly->SetStrips(cell);
	pointsPoly->GetPointData()->SetScalars(scalars);
	filter->SetInputData(pointsPoly);
	filter->Update();
	linear->SetInputData(filter->GetOutput());
	linear->SetNumberOfSubdivisions(4);
	linear->Update();

	pColorTable->SetNumberOfColors(33);
	pColorTable->SetTableValue(0, 0.0, 0.0, 1.0, 1.0);
	pColorTable->SetTableValue(1, 0.0, 0.125, 1.0, 1.0);
	pColorTable->SetTableValue(2, 0.0, 0.25, 1.0, 1.0);
	pColorTable->SetTableValue(3, 0.0, 0.375, 1.0, 1.0);
	pColorTable->SetTableValue(4, 0.0, 0.5, 1.0, 1.0);//
	pColorTable->SetTableValue(5, 0.0, 0.625, 1.0, 1.0);
	pColorTable->SetTableValue(6, 0.0, 0.75, 1.0, 1.0);//4
	pColorTable->SetTableValue(7, 0.0, 0.875, 1.0, 1.0);
	pColorTable->SetTableValue(8, 0.0, 1.0, 1.0, 1.0);
	pColorTable->SetTableValue(9, 0.0, 1.0, 0.875, 1.0);
	pColorTable->SetTableValue(10, 0.0, 1.0, 0.75, 1.0);
	pColorTable->SetTableValue(11, 0.0, 1.0, 0.625, 1.0);
	pColorTable->SetTableValue(12, 0.0, 1.0, 0.5, 1.0);//
	pColorTable->SetTableValue(13, 0.0, 1.0, 0.375, 1.0);
	pColorTable->SetTableValue(14, 0.0, 1.0, 0.25, 1.0);//4
	pColorTable->SetTableValue(15, 0.0, 1.0, 0.125, 1.0);
	pColorTable->SetTableValue(16, 0.0, 1.0, 0.0, 1.0);
	pColorTable->SetTableValue(17, 0.125, 1.0, 0.0, 1.0);
	pColorTable->SetTableValue(18, 0.25, 1.0, 0.0, 1.0);
	pColorTable->SetTableValue(19, 0.375, 1.0, 0.0, 1.0);
	pColorTable->SetTableValue(20, 0.5, 1.0, 0.0, 1.0);//
	pColorTable->SetTableValue(21, 0.625, 1.0, 0.0, 1.0);
	pColorTable->SetTableValue(22, 0.75, 1.0, 0.0, 1.0);
	pColorTable->SetTableValue(23, 0.875, 1.0, 0.0, 1.0);
	pColorTable->SetTableValue(24, 1.0, 1.0, 0.0, 1.0);
	pColorTable->SetTableValue(25, 1.0, 0.875, 0.0, 1.0);
	pColorTable->SetTableValue(26, 1.0, 0.75, 0.0, 1.0);//
	pColorTable->SetTableValue(27, 1.0, 0.625, 0.0, 1.0);
	pColorTable->SetTableValue(28, 1.0, 0.5, 0.0, 1.0);//
	pColorTable->SetTableValue(29, 1.0, 0.375, 0.0, 1.0);
	pColorTable->SetTableValue(30, 1.0, 0.25, 0.0, 1.0);//
	pColorTable->SetTableValue(31, 1.0, 0.125, 0.0, 1.0);
	pColorTable->SetTableValue(32, 1.0, 0.0, 0.0, 1.0);
	pColorTable->Build();
	pointsMapper->SetInputData(linear->GetOutput());
	pointsMapper->SetScalarRange(vmin, vmax);
	pointsMapper->SetLookupTable(pColorTable);
	bar->SetLookupTable(pColorTable);
	bar->SetNumberOfLabels(16);
	bar->SetTitle("3D Rectangular Color");
	camera->SetPosition(1, 0, 0);
	camera->SetFocalPoint(0, 0, 0);
	camera->SetViewUp(0, 0, 1);
	camera->Zoom(0.5);
	actor->SetOrigin(0, 0, 0);
	actor->SetMapper(pointsMapper);
	actor->GetProperty()->SetRepresentationToSurface();
	actor->RotateX(rotx1 + rotx2);
	actor->RotateY(roty1 + roty2);
	actor->RotateZ(rotz1 + rotz2);
	transform1->RotateX(rotx1 + rotx2);
	transform1->RotateY(roty1 + roty2);
	transform1->RotateZ(rotz1 + rotz2);
	pTransformPoly->SetInputData(linear->GetOutput());
	pTransformPoly->SetTransform(transform1);
	pTransformPoly->Update();
	axes->SetUserTransform(transform1);
	axes->SetTotalLength(1.0, 1.0, 1.0);
	axes->GetXAxisCaptionActor2D()->GetCaptionTextProperty()->SetColor(1.0, 0.0, 0.0);
	axes->GetYAxisCaptionActor2D()->GetCaptionTextProperty()->SetColor(0.0, 1.0, 0.0);
	axes->GetZAxisCaptionActor2D()->GetCaptionTextProperty()->SetColor(0.0, 0.0, 1.0);
	axes->SetXAxisLabelText("X");
	axes->SetYAxisLabelText("Y");
	axes->SetZAxisLabelText("Z");
	ren->AddActor(actor);
	ren->AddActor2D(bar);
	ren->SetBackground(0.2, 0.3, 0.4);
	ren->SetActiveCamera(camera);
	ren->ResetCamera();
	renWin->AddRenderer(ren);
	renWin->SetSize(320, 320);
	renWin->Render();
	renWin->SetWindowName("3D Rectangular Color Plot");
	iren->SetRenderWindow(renWin);
	widget->SetOutlineColor(0.9300, 0.5700, 0.1300);
	widget->SetOrientationMarker(axes);
	widget->SetInteractor(iren);
	widget->SetViewport(0.0, 0.0, 0.4, 0.4);
	widget->SetEnabled(1);
	widget->InteractiveOff();
	iren->Start();
	delete u;
	delete p;
	delete x;
	delete y;
	delete z;
	delete v;
	delete[] pts;


}
#include "moc_RotateXYZ.cpp"