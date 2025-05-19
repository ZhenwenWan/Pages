#include "PreCompiled.h"
#ifndef _PreComp_
#include <QMessageBox>
#endif
#include "ui_SelectFrequency.h"
#include "SelectFrequency.h"
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
#include <QtCore/QCoreApplication>
#include <QSettings>
#include <QStringList>
#include <QString>
#include <string>
#include <QTime> 
#include <vtkInteractorStyleTrackballActor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkWindowToImageFilter.h>
#include <vtkPNGWriter.h>
#include <vtkInteractorStyleSwitch.h>
#include <vtkInteractorStyleUser.h>
#include <vtkInteractorStyleFlight.h>
#include <vtkInteractorStyleRubberBandZoom.h>
#include <vtkJPEGWriter.h>

using namespace MySimIIGui;
SelectFrequency::SelectFrequency(QDialog *parent) :QDialog(parent), ui(new Ui::SelectFrequency)
{
	ui->setupUi(this);	
	connect(ui->CancelPB, SIGNAL(clicked(bool)), this, SLOT(Cancel()));
	connect(ui->OKPB, SIGNAL(clicked(bool)), this, SLOT(Ok()));
	QSettings *SolutionFreqIniRead = new QSettings(QObject::tr("../ini/SolutionSetup.ini"), QSettings::IniFormat);
	ui->SelectFreqComB->addItem(SolutionFreqIniRead->value(tr("/Setup1/SolutionFreq")).toString());
	delete SolutionFreqIniRead;	
	ui->RotateXL->setVisible(false);
	ui->RotateXLE->setVisible(false);
	ui->RotateXDegL->setVisible(false);
	ui->RotateYL->setVisible(false);
	ui->RotateYLE->setVisible(false);
	ui->RotateYDegL->setVisible(false);
	ui->RotateZL->setVisible(false);
	ui->RotateZLE->setVisible(false);
	ui->RotateZDegL->setVisible(false);


}
SelectFrequency::~SelectFrequency()
{
	
}

void SelectFrequency::Excute()
{
	QSettings *DefaultIniRead = new QSettings(QObject::tr("../ini/Default.ini"), QSettings::IniFormat);
	QString FieldOverlayExe = DefaultIniRead->value(QObject::tr("/EXEFILE/FieldOverlay")).toString();
	delete DefaultIniRead;
	if (!FieldOverlayExe.isEmpty())
	{   // 阻塞方式
		Base::Console().Message(" Start EXEFILE/FieldOverlay\n");
		QProcess::execute(FieldOverlayExe);
		Base::Console().Message(" End EXEFILE/FieldOverlay\n");
	}
	else
	{
		QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong"), QObject::tr("This file does not exist!"));
		Base::Console().Message(" EXEFILE/FieldOverlay, This file does not exist!\n");
	}
	Base::Console().Message(" Plot ...\n");
}


void SelectFrequency::PlotThreeDimColor()
{

	QDialog::close();
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
	QString strflag = 0;
	QDir dir(QObject::tr("../ini/3DRectangular"));
	if (!dir.exists())
	{
		dir.mkdir(QObject::tr("../ini/3DRectangular"));
	}
	QSettings *colorsplot = new QSettings(QObject::tr("../ini/3DRectangular/GlobalVar.ini"), QSettings::IniFormat);
	switch (flag)
	{
	case 1://Mag_Real_E  
		strflag = tr("Mag_Real_E");
		colorplot = colorsplot->value(tr("/GlobalVar/cntMag_Real_E")).toInt();
		colorplot++;
		colorsplot->setValue(tr("/GlobalVar/cntMag_Real_E"), colorplot);
		Gui::Application::Instance->AddProjectTreeObject("EngineGui::FieldOverlays", "Mag_Real_E " + QString::number(colorplot).toLatin1());
		break;
	case 2://Mag_Imag_E  
		strflag = tr("Mag_Imag_E");
		colorplot = colorsplot->value(tr("/GlobalVar/cntMag_Imag_E")).toInt();
		colorplot++;
		colorsplot->setValue(tr("/GlobalVar/cntMag_Imag_E"), colorplot);
		Gui::Application::Instance->AddProjectTreeObject("EngineGui::FieldOverlays", "Mag_Imag_E " + QString::number(colorplot).toLatin1());
		break;
	case 3://Mag_Comp_E  
		strflag = tr("Mag_Comp_E");
		colorplot = colorsplot->value(tr("/GlobalVar/cntMag_Comp_E")).toInt();
		colorplot++;
		colorsplot->setValue(tr("/GlobalVar/cntMag_Comp_E"), colorplot);
		Gui::Application::Instance->AddProjectTreeObject("EngineGui::FieldOverlays", "Mag_Comp_E " + QString::number(colorplot).toLatin1());
		break;
	case 4://Vector_E
		strflag = tr("Vector_E");
		colorplot = colorsplot->value(tr("/GlobalVar/cntVector_E")).toInt();
		colorplot++;
		colorsplot->setValue(tr("/GlobalVar/cntVector_E"), colorplot);
		Gui::Application::Instance->AddProjectTreeObject("EngineGui::FieldOverlays", "Vector_E " + QString::number(colorplot).toLatin1());
		break;
	case 5://Re_Ex
		strflag = tr("Re_Ex");
		colorplot = colorsplot->value(tr("/GlobalVar/cntRe_Ex")).toInt();
		colorplot++;
		colorsplot->setValue(tr("/GlobalVar/cntRe_Ex"), colorplot);
		Gui::Application::Instance->AddProjectTreeObject("EngineGui::FieldOverlays", "Re_Ex " + QString::number(colorplot).toLatin1());
		break;
	case 6://Re_Ey
		strflag = tr("Re_Ey");
		colorplot = colorsplot->value(tr("/GlobalVar/cntRe_Ey")).toInt();
		colorplot++;
		colorsplot->setValue(tr("/GlobalVar/cntRe_Ey"), colorplot);
		Gui::Application::Instance->AddProjectTreeObject("EngineGui::FieldOverlays", "Re_Ey " + QString::number(colorplot).toLatin1());
		break;
	case 7://Re_Ez
		strflag = tr("Re_Ez");
		colorplot = colorsplot->value(tr("/GlobalVar/cntRe_Ez")).toInt();
		colorplot++;
		colorsplot->setValue(tr("/GlobalVar/cntRe_Ez"), colorplot);
		Gui::Application::Instance->AddProjectTreeObject("EngineGui::FieldOverlays", "Re_Ez " + QString::number(colorplot).toLatin1());
		break;
	case 8://Im_Ex
		strflag = tr("Im_Ex");
		colorplot = colorsplot->value(tr("/GlobalVar/cntIm_Ex")).toInt();
		colorplot++;
		colorsplot->setValue(tr("/GlobalVar/cntIm_Ex"), colorplot);
		Gui::Application::Instance->AddProjectTreeObject("EngineGui::FieldOverlays", "Im_Ex " + QString::number(colorplot).toLatin1());
		break;
	case 9://Im_Ey
		strflag = tr("Im_Ey");
		colorplot = colorsplot->value(tr("/GlobalVar/cntIm_Ey")).toInt();
		colorplot++;
		colorsplot->setValue(tr("/GlobalVar/cntIm_Ey"), colorplot);
		Gui::Application::Instance->AddProjectTreeObject("EngineGui::FieldOverlays", "Im_Ey " + QString::number(colorplot).toLatin1());
		break;
	case 10://Im_Ez
		strflag = tr("Im_Ez");
		colorplot = colorsplot->value(tr("/GlobalVar/cntIm_Ez")).toInt();
		colorplot++;
		colorsplot->setValue(tr("/GlobalVar/cntIm_Ez"), colorplot);
		Gui::Application::Instance->AddProjectTreeObject("EngineGui::FieldOverlays", "Im_Ez " + QString::number(colorplot).toLatin1());
		break;
	case 11://Mag_Real_H  
		strflag = tr("Mag_Real_H");
		colorplot = colorsplot->value(tr("/GlobalVar/cntMag_Real_H")).toInt();
		colorplot++;
		colorsplot->setValue(tr("/GlobalVar/cntMag_Real_H"), colorplot);
		Gui::Application::Instance->AddProjectTreeObject("EngineGui::FieldOverlays", "Mag_Real_H " + QString::number(colorplot).toLatin1());
		break;
	case 12://Mag_Imag_H  
		strflag = tr("Mag_Imag_H");
		colorplot = colorsplot->value(tr("/GlobalVar/cntMag_Imag_H")).toInt();
		colorplot++;
		colorsplot->setValue(tr("/GlobalVar/cntMag_Imag_H"), colorplot);
		Gui::Application::Instance->AddProjectTreeObject("EngineGui::FieldOverlays", "Mag_Imag_H " + QString::number(colorplot).toLatin1());
		break;
	case 13://Mag_Comp_H  
		strflag = tr("Mag_Comp_H");
		colorplot = colorsplot->value(tr("/GlobalVar/cntMag_Comp_H")).toInt();
		colorplot++;
		colorsplot->setValue(tr("/GlobalVar/cntMag_Comp_H"), colorplot);
		Gui::Application::Instance->AddProjectTreeObject("EngineGui::FieldOverlays", "Mag_Comp_H " + QString::number(colorplot).toLatin1());
		break;
	case 14://Vector_H
		strflag = tr("Vector_H");
		colorplot = colorsplot->value(tr("/GlobalVar/cntVector_H")).toInt();
		colorplot++;
		colorsplot->setValue(tr("/GlobalVar/cntVector_H"), colorplot);
		Gui::Application::Instance->AddProjectTreeObject("EngineGui::FieldOverlays", "Vector_H " + QString::number(colorplot).toLatin1());
		break;
	case 15://Re_Hx
		strflag = tr("Re_Hx");
		colorplot = colorsplot->value(tr("/GlobalVar/cntRe_Hx")).toInt();
		colorplot++;
		colorsplot->setValue(tr("/GlobalVar/cntRe_Hx"), colorplot);
		Gui::Application::Instance->AddProjectTreeObject("EngineGui::FieldOverlays", "Re_Hx " + QString::number(colorplot).toLatin1());
		break;
	case 16://Re_Hy
		strflag = tr("Re_Hy");
		colorplot = colorsplot->value(tr("/GlobalVar/cntRe_Hy")).toInt();
		colorplot++;
		colorsplot->setValue(tr("/GlobalVar/cntRe_Hy"), colorplot);
		Gui::Application::Instance->AddProjectTreeObject("EngineGui::FieldOverlays", "Re_Hy " + QString::number(colorplot).toLatin1());
		break;
	case 17://Re_Hz
		strflag = tr("Re_Hz");
		colorplot = colorsplot->value(tr("/GlobalVar/cntRe_Hz")).toInt();
		colorplot++;
		colorsplot->setValue(tr("/GlobalVar/cntRe_Hz"), colorplot);
		Gui::Application::Instance->AddProjectTreeObject("EngineGui::FieldOverlays", "Re_Hz " + QString::number(colorplot).toLatin1());
		break;
	case 18://Im_Hx
		strflag = tr("Im_Hx");
		colorplot = colorsplot->value(tr("/GlobalVar/cntIm_Hx")).toInt();
		colorplot++;
		colorsplot->setValue(tr("/GlobalVar/cntIm_Hx"), colorplot);
		Gui::Application::Instance->AddProjectTreeObject("EngineGui::FieldOverlays", "Im_Hx " + QString::number(colorplot).toLatin1());
		break;
	case 19://Im_Hy
		strflag = tr("Im_Hy");
		colorplot = colorsplot->value(tr("/GlobalVar/cntIm_Hy")).toInt();
		colorplot++;
		colorsplot->setValue(tr("/GlobalVar/cntIm_Hy"), colorplot);
		Gui::Application::Instance->AddProjectTreeObject("EngineGui::FieldOverlays", "Im_Hy " + QString::number(colorplot).toLatin1());
		break;
	case 20://Im_Hz
		strflag = tr("Im_Hz");
		colorplot = colorsplot->value(tr("/GlobalVar/cntIm_Hz")).toInt();
		colorplot++;
		colorsplot->setValue(tr("/GlobalVar/cntIm_Hz"), colorplot);
		Gui::Application::Instance->AddProjectTreeObject("EngineGui::FieldOverlays", "Im_Hz " + QString::number(colorplot).toLatin1());
		break;
	default:
		break;
	}
	delete colorsplot;
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

	u = new double[arr.count() / 23];
	p = new double[arr.count() / 23];
	x = new double[arr.count() / 23];
	y = new double[arr.count() / 23];
	z = new double[arr.count() / 23];
	v = new double[arr.count() / 23];

	QString sf = QString::number(colorplot);
	QFile strfile(QObject::tr("../ini/3DRectangular/ItemsVar.txt"));
	if (!strfile.open(QIODevice::WriteOnly | QIODevice::Text|QIODevice::Append))
	{
		return;
	}
	QTextStream strtextstream(&strfile);
	QString strbuf;
	strbuf = strflag + tr(" ") + sf;
	strtextstream << tr("%1").arg(strbuf) << endl;
	strfile.close();
	QFile savefile(QObject::tr("../ini/3DRectangular/") + strflag + sf + tr(".txt"));
	if (!savefile.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		return;
	}
	QTextStream savetextstream(&savefile);
	savetextstream << tr("%1").arg(tr("U"), 20) << tr("%1").arg(tr("P"), 20) << tr("%1").arg(tr("X"), 20) << tr("%1").arg(tr("Y"), 20) << tr("%1").arg(tr("Z"), 20) << tr("%1").arg(tr("V"), 20) << endl;
	static double savebuf;
	for (int i = 0; i != arr.count(); ++i)
	{
		if (i % 23 == 0 && i / 23 < arr.count() / 23)
		{
			u[i / 23] = arr[i];
			savebuf = arr[i];
			savetextstream << tr("%1").arg(savebuf, 20);			
		}
		if (i % 23 == 1)
		{
			p[i / 23] = arr[i];
			savebuf = arr[i];
			savetextstream << tr("%1").arg(savebuf, 20);			
		}
		if (i % 23 == 2)
		{
			x[i / 23] = arr[i];
			savebuf = arr[i];
			savetextstream << tr("%1").arg(savebuf, 20);
		}
		if (i % 23 == 3)
		{
			y[i / 23] = arr[i];
			savebuf = arr[i];
			savetextstream << tr("%1").arg(savebuf, 20);			
		}
		if (i % 23 == 4)
		{
			z[i / 23] = arr[i];
			savebuf = arr[i];
			savetextstream << tr("%1").arg(savebuf, 20);			
		}
		switch (flag)
		{
		case 1://Mag_Real_E  
			if (i % 23 == 5)
			{
				v[i / 23] = arr[i];
				savebuf = arr[i];
				savetextstream << tr("%1").arg(savebuf, 20) << endl;				
			}
			break;
		case 2://Mag_Imag_E  
			if (i % 23 == 6)
			{
				v[i / 23] = arr[i];
				savebuf = arr[i];
				savetextstream << tr("%1").arg(savebuf, 20) << endl;
			}
			break;
		case 3://Mag_Comp_E  
			if (i % 23 == 7)
			{
				v[i / 23] = arr[i];
				savebuf = arr[i];
				savetextstream << tr("%1").arg(savebuf, 20) << endl;
			}
			break;
		
		case 4://Vector_E
			if (i % 23 == 7)
			{
				v[i / 23] = arr[i];
				savebuf = arr[i];
				savetextstream << tr("%1").arg(savebuf, 20) << endl;				
			}
			break;
		case 5://Re_Ex
			if (i % 23 == 8)
			{
				v[i / 23] = arr[i];
				savebuf = arr[i];
				savetextstream << tr("%1").arg(savebuf, 20) << endl;				
			}
			break;
		case 6://Re_Ey
			if (i % 23 == 9)
			{
				v[i / 23] = arr[i];
				savebuf = arr[i];
				savetextstream << tr("%1").arg(savebuf, 20) << endl;				
			}
			break;
		case 7://Re_Ez
			if (i % 23 == 10)
			{
				v[i / 23] = arr[i];
				savebuf = arr[i];
				savetextstream << tr("%1").arg(savebuf, 20) << endl;				
			}
			break;
		case 8://Im_Ex
			if (i % 23 == 11)
			{
				v[i / 23] = arr[i];
				savebuf = arr[i];
				savetextstream << tr("%1").arg(savebuf, 20) << endl;				
			}
			break;
		case 9://Im_Ey
			if (i % 23 == 12)
			{
				v[i / 23] = arr[i];
				savebuf = arr[i];
				savetextstream << tr("%1").arg(savebuf, 20) << endl;				
			}
			break;
		case 10://Im_Ez
			if (i % 23 == 13)
			{
				v[i / 23] = arr[i];
				savebuf = arr[i];
				savetextstream << tr("%1").arg(savebuf, 20) << endl;				
			}
			break;
		case 11://Mag_Real_H  
			if (i % 23 == 14)
			{
				v[i / 23] = arr[i];
				savebuf = arr[i];
				savetextstream << tr("%1").arg(savebuf, 20) << endl;				
			}
			break;
		case 12://Mag_Imag_H  
			if (i % 23 == 15)
			{
				v[i / 23] = arr[i];
				savebuf = arr[i];
				savetextstream << tr("%1").arg(savebuf, 20) << endl;
			}
			break;
		case 13://Mag_Comp_H  
			if (i % 23 == 16)
			{
				v[i / 23] = arr[i];
				savebuf = arr[i];
				savetextstream << tr("%1").arg(savebuf, 20) << endl;
			}
			break;
		case 14://Vector_H
			if (i % 23 == 16)
			{
				v[i / 23] = arr[i];
				savebuf = arr[i];
				savetextstream << tr("%1").arg(savebuf, 20) << endl;				
			}
			break;
		case 15://Re_Hx
			if (i % 23 == 17)
			{
				v[i / 23] = arr[i];
				savebuf = arr[i];
				savetextstream << tr("%1").arg(savebuf, 20) << endl;				
			}
			break;
		case 16://Re_Hy
			if (i % 23 == 18)
			{
				v[i / 23] = arr[i];
				savebuf = arr[i];
				savetextstream << tr("%1").arg(savebuf, 20) << endl;				
			}
			break;
		case 17://Re_Hz
			if (i % 23 == 19)
			{
				v[i / 23] = arr[i];
				savebuf = arr[i];
				savetextstream << tr("%1").arg(savebuf, 20) << endl;				
			}
			break;
		case 18://Im_Hx
			if (i % 23 == 20)
			{
				v[i / 23] = arr[i];
				savebuf = arr[i];
				savetextstream << tr("%1").arg(savebuf, 20) << endl;				
			}
			break;
		case 19://Im_Hy
			if (i % 23 == 21)
			{
				v[i / 23] = arr[i];
				savebuf = arr[i];
				savetextstream << tr("%1").arg(savebuf, 20) << endl;				
			}
			break;
		case 20://Im_Hz
			if (i % 23 == 22)
			{
				v[i / 23] = arr[i];
				savebuf = arr[i];
				savetextstream << tr("%1").arg(savebuf, 20) << endl;				
			}
			break;
		default:
			break;
		}
	}
	savefile.close();
	//三维点云隐式曲面重构技术
	vtkSmartPointer<vtkRenderer> ren = vtkSmartPointer<vtkRenderer>::New();//负责管理场景的渲染过程
	vtkSmartPointer<vtkRenderWindow> renWin = vtkSmartPointer<vtkRenderWindow>::New();//将操作系统与Vtk渲染引擎连接到一起
	vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();//提供平台独立的响应鼠标，键盘，时钟事件的交互机制
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> cell = vtkSmartPointer<vtkCellArray>::New();
	vtkSmartPointer<vtkFloatArray> scalars = vtkSmartPointer<vtkFloatArray>::New();
	vtkSmartPointer<vtkPolyData> pointsPoly = vtkSmartPointer<vtkPolyData>::New();
	vtkSmartPointer<vtkPolyDataMapper> pointsMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	vtkSmartPointer<vtkCamera> camera = vtkSmartPointer<vtkCamera>::New();
	vtkSmartPointer<vtkLookupTable> pColorTable = vtkSmartPointer<vtkLookupTable>::New();
	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();//派生自vtkPorp类，数据的可视化
	vtkSmartPointer<vtkCubeAxesActor> cube = vtkSmartPointer<vtkCubeAxesActor>::New();
	vtkSmartPointer<vtkScalarBarActor> bar = vtkSmartPointer<vtkScalarBarActor>::New();
	vtkSmartPointer<vtkAxesActor> axes = vtkSmartPointer<vtkAxesActor>::New();//同一个视口中显示坐标系，直接添加到renderer中  
	vtkSmartPointer<vtkAxesActor> axes1 = vtkSmartPointer<vtkAxesActor>::New();
	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	vtkSmartPointer<vtkOrientationMarkerWidget> widget = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
	vtkSmartPointer<vtkTransformPolyDataFilter> pTransformPoly = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	vtkSmartPointer<vtkTriangleFilter> filter = vtkSmartPointer<vtkTriangleFilter>::New();
	vtkSmartPointer<vtkLinearSubdivisionFilter> linear = vtkSmartPointer<vtkLinearSubdivisionFilter>::New();
	//vtkSmartPointer<vtkInteractorStyleUnicam> style1 = vtkSmartPointer<vtkInteractorStyleUnicam>::New();//只有放大和平移的功能
	//vtkSmartPointer<vtkInteractorStyleTrackballActor> style1 = vtkSmartPointer<vtkInteractorStyleTrackballActor>::New();
	vtkSmartPointer<vtkInteractorStyleTrackballCamera> style1 = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();//ok不能平移
	//vtkSmartPointer<vtkInteractorStyleSwitch> style1 = vtkSmartPointer<vtkInteractorStyleSwitch>::New();//ok 默认的交互样式
	//vtkSmartPointer<vtkInteractorStyleUser> style1 = vtkSmartPointer<vtkInteractorStyleUser>::New();// no没有鼠标响应 主要用于用户自定义的操作 
	//vtkSmartPointer<vtkInteractorStyleFlight> style1 = vtkSmartPointer<vtkInteractorStyleFlight>::New();//no
	//vtkSmartPointer<vtkInteractorStyleRubberBandZoom> style1 = vtkSmartPointer<vtkInteractorStyleRubberBandZoom>::New();//局部放大
	vtkSmartPointer<vtkOutlineFilter> outline = vtkSmartPointer<vtkOutlineFilter>::New();//后面没有使用这个和outlineactor，这个是把按形状大小框在一个范围。
	vtkSmartPointer<vtkPolyDataMapper> outlinemapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	vtkSmartPointer<vtkActor> outlineactor = vtkSmartPointer<vtkActor>::New();
	//vtk实现保存窗口为图像
	vtkSmartPointer<vtkWindowToImageFilter> wif = vtkSmartPointer<vtkWindowToImageFilter>::New();
	//vtkSmartPointer<vtkPNGWriter> pngw = vtkSmartPointer<vtkPNGWriter>::New();
	
	vtkSmartPointer<vtkJPEGWriter> writer=vtkSmartPointer<vtkJPEGWriter>::New();

	double vmax, vmin;
	vmax = v[0];
	vmin = v[0];
	int umax = 0;
	for (int i = 0; i != arr.count(); ++i)
	{
		if (i % 23 == 0 && i / 23 < arr.count() / 23)
		{
			points->InsertPoint(p[i / 23], x[i / 23], y[i / 23], z[i / 23]);
			scalars->InsertTuple1(p[i / 23], v[i / 23]);			
			vmax = vmax > v[i / 23] ? vmax : v[i / 23];
			vmin = vmin < v[i / 23] ? vmin : v[i / 23];
			umax = umax > u[i / 23] ? umax : u[i / 23];
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
	outline->SetInputData(pointsPoly);
	outline->Update();
	outlinemapper->SetInputData(outline->GetOutput());
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
	bar->SetTitle(strflag.toStdString().data());
	camera->SetPosition(1, 0, 0);
	camera->SetFocalPoint(0, 0, 0);
	camera->SetViewUp(0, 0, 1);
	camera->Zoom(0.5);
	actor->SetMapper(pointsMapper);	
	actor->GetProperty()->SetRepresentationToSurface();
	//actor->GetProperty()->SetOpacity(0.5);
	outlineactor->SetMapper(outlinemapper);
	outlineactor->GetProperty()->SetColor(0, 1, 0);
	outlineactor->GetProperty()->SetLineWidth(3);
	pTransformPoly->SetInputData(pointsPoly);
	pTransformPoly->SetTransform(transform);
	pTransformPoly->Update();
	axes->SetUserTransform(transform);	
	axes->SetTotalLength(1.0, 1.0, 1.0);
	axes->GetXAxisCaptionActor2D()->GetCaptionTextProperty()->SetColor(1.0, 0.0, 0.0);
	axes->GetYAxisCaptionActor2D()->GetCaptionTextProperty()->SetColor(0.0, 1.0, 0.0);
	axes->GetZAxisCaptionActor2D()->GetCaptionTextProperty()->SetColor(0.0, 0.0, 1.0);
	axes->SetXAxisLabelText("X");
	axes->SetYAxisLabelText("Y");
	axes->SetZAxisLabelText("Z");
	axes1->SetUserTransform(transform);
	axes1->GetXAxisCaptionActor2D()->GetProperty()->SetOpacity(1);
	axes1->GetYAxisCaptionActor2D()->GetProperty()->SetOpacity(1);
	axes1->GetZAxisCaptionActor2D()->GetProperty()->SetOpacity(1);
	axes1->GetXAxisCaptionActor2D()->GetCaptionTextProperty()->SetColor(1.0, 0.0, 0.0);
	axes1->GetYAxisCaptionActor2D()->GetCaptionTextProperty()->SetColor(0.0, 1.0, 0.0);
	axes1->GetZAxisCaptionActor2D()->GetCaptionTextProperty()->SetColor(0.0, 0.0, 1.0);
	ren->AddActor(actor);//将VtkProp类型的对象添加到渲染场景中
	ren->AddActor2D(bar);
	//ren->AddActor(axes1);
	//ren->AddActor(outlineactor);
	ren->SetBackground(0.2, 0.3, 0.4);
	//ren->SetViewport(0.0, 0.0, 0.5, 1.0);//设置view显示的位置
	ren->SetActiveCamera(camera);
	ren->ResetCamera();
	//ren->GetActiveCamera()->Azimuth(90);
	renWin->AddRenderer(ren);
	renWin->SetSize(600, 450);
	renWin->Render();
	renWin->SetWindowName(strflag.toStdString().data());
	renWin->SetPosition(600,400);
	iren->SetRenderWindow(renWin);
	iren->SetInteractorStyle(style1);//图像的交互功能
	widget->SetOutlineColor(0.9300, 0.5700, 0.1300);
	widget->SetOrientationMarker(axes);//在左下角的视口中显示坐标系
	widget->SetInteractor(iren);
	widget->SetViewport(0.0, 0.0, 0.4, 0.4);
	widget->SetEnabled(1);
	widget->InteractiveOff();
	iren->Start();
	//输出为图片
	//wif->SetInput(renWin);
	//pngw->SetInputConnection(wif->GetOutput());
	//pngw->SetFileName("aa");
	//pngw->SetFilePattern("bmp");
	//pngw->Write();
	//writer->SetFileName("lin");
	//writer->SetInputConnection(wif->GetOutputPort());
   // writer->Write();
	delete u;
	delete p;
	delete x;
	delete y;
	delete z;
	delete v;
	delete[] pts;
		
}

void SelectFrequency::Ok()
{
	Excute();	
	PlotThreeDimColor();//三维伪彩色	

	
}
void SelectFrequency::Cancel()
{
	QDialog::close();
}

#include "moc_SelectFrequency.cpp"
