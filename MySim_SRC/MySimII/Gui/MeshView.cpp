#include "PreCompiled.h"
#ifndef _PreComp_
#include <QMessageBox>
#endif
#include "ui_MeshView.h"
#include "MeshView.h"
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

using namespace MySimIIGui;
MeshView::MeshView(QDialog *parent) :QDialog(parent), ui(new Ui::MeshViewD)
{
	ui->setupUi(this);
	MeshWrite();
	AddItems();
	connect(ui->EnabledCB, SIGNAL(clicked(bool)), this, SLOT(CheckedEnable()));
	connect(ui->HideCB, SIGNAL(clicked(bool)), this, SLOT(CheckedHide()));
	connect(ui->CancelPB, SIGNAL(clicked(bool)), this, SLOT(Cancel()));
	connect(ui->OkPB, SIGNAL(clicked(bool)), this, SLOT(Ok()));
	connect(ui->SelectFacesLW, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(MultiFacesView()));


}
MeshView::~MeshView()
{

}
void MeshView::MeshWrite()
{
	//mesh文件数据提取
	QFile file(QObject::tr("../ini/mm/mesh.boundary"));
	QVector<int> arrboundary;
	if (!file.open(QIODevice::ReadOnly))
	{
		return;
	}
	QTextStream textstream(&file);
	while (!textstream.atEnd())
	{		
		static int buf;
		textstream >> buf;
		arrboundary.append(buf);
	}
	file.close();
	QFile nodefile(QObject::tr("../ini/mm/mesh.nodes"));
	QVector<double> arrnodes;
	if (!nodefile.open(QIODevice::ReadOnly))
	{
		return;
	}
	QTextStream nodestream(&nodefile);
	while (!nodestream.atEnd())
	{
		static double buf;
		nodestream >> buf;
		arrnodes.append(buf);
	}
	nodefile.close();
	QFile facefile(QObject::tr("../ini/facemap.dat"));
	QVector<int> arrface;
	if (!facefile.open(QIODevice::ReadOnly))
	{
		return;
	}
	QTextStream facestream(&facefile);
	while (!facestream.atEnd())
	{
		static int buf;
		facestream >> buf;
		arrface.append(buf);
	}
	facefile.close();	
	int *triangle;
	int *facenum1;
	int *pointsnum1;
	int *pointsnum2;
	int *pointsnum3;
	int *pointsnum;
	double *x;
	double *y;
	double *z;
	int *face1;
	int *face2;
	triangle = new int[arrboundary.count() / 8];
	facenum1 = new int[arrboundary.count() / 8];
	pointsnum1= new int[arrboundary.count() / 8];
	pointsnum2 = new int[arrboundary.count() / 8];
	pointsnum3 = new int[arrboundary.count() / 8];
	pointsnum = new int[arrnodes.count() / 5];
	x = new double[arrnodes.count() / 5];
	y = new double[arrnodes.count() / 5];
	z = new double[arrnodes.count() / 5];
	face1 = new int[arrface.count() / 2];
	face2 = new int[arrface.count() / 2];
	int *triangleNew;
	int *faceNew;
	int *pointsNew;
	double *xNew;
	double *yNew;
	double *zNew;
	triangleNew = new int[(arrboundary.count() / 8) * 3];
	faceNew = new int[(arrboundary.count() / 8) * 3];
	pointsNew = new int[(arrboundary.count() / 8) * 3];
	xNew = new double[(arrboundary.count() / 8) * 3];
	yNew = new double[(arrboundary.count() / 8) * 3];
	zNew = new double[(arrboundary.count() / 8) * 3];
	for (int i = 0; i != arrboundary.count(); ++i)
	{
		if (i % 8 == 0 && i / 8 < arrboundary.count() / 8)
		{
			triangle[i / 8] = arrboundary[i];			
		}
		if (i % 8 == 1)
		{
			facenum1[i / 8] = arrboundary[i];			
		}
		if (i % 8 == 5)
		{
			pointsnum1[i / 8] = arrboundary[i];			
		}
		if (i % 8 == 6)
		{
			pointsnum2[i / 8] = arrboundary[i];			
		}
		if (i % 8 == 7)
		{
			pointsnum3[i / 8] = arrboundary[i];			
		}
	}
	for (int i = 0; i != arrnodes.count(); ++i)
	{
		if (i % 5 == 0 && i / 5 < arrnodes.count() / 5)
		{
			pointsnum[i / 5] = (int)arrnodes[i];			
		}
		if (i % 5 == 2)
		{
			x[i / 5] = arrnodes[i];		
		}
		if (i % 5 == 3)
		{
			y[i / 5] = arrnodes[i];
		}
		if (i % 5 == 4)
		{
			z[i / 5] = arrnodes[i];
		}		
	}
	for (int i = 0; i != arrface.count(); ++i)
	{
		if (i % 2 == 0 && i / 2 < arrface.count() / 2)
		{
			face1[i / 2] = arrface[i];
		}
		if (i % 2 == 1)
		{
			face2[i / 2] = arrface[i];
		}		
	}
	for (int i = 0; i != arrboundary.count() / 8; ++i)
	{		
		for (int j = 0; j != arrface.count() / 2; ++j)
		{
			if (facenum1[i] == face1[j])
			{
				facenum1[i] = face2[j];
				break;
			}
		}

	}
	QFile savefile(QObject::tr("../ini/Mesh.dat"));
	if (!savefile.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		return;
	}
	QTextStream savetextstream(&savefile);	
	static int savebuf1;
	static double savebuf2;
	for (int i = 0; i != (arrboundary.count() / 8) * 3; ++i)
	{
		triangleNew[i] = triangle[i / 3];
		savebuf1 = triangleNew[i];
		savetextstream << tr("%1").arg(savebuf1, 20);
		faceNew[i] = facenum1[i / 3];
		savebuf1 = faceNew[i];
		savetextstream << tr("%1").arg(savebuf1, 20);
		if (i % 3 == 0)
		{
			pointsNew[i] = pointsnum1[i / 3];			
			savebuf1 = pointsNew[i];
			savetextstream << tr("%1").arg(savebuf1, 20);
		}
		if (i % 3 == 1)
		{
			pointsNew[i] = pointsnum2[i / 3];
			savebuf1 = pointsNew[i];
			savetextstream << tr("%1").arg(savebuf1, 20);
		}
		if (i % 3 == 2)
		{
			pointsNew[i] = pointsnum3[i / 3];
			savebuf1 = pointsNew[i];
			savetextstream << tr("%1").arg(savebuf1, 20);
		}
		for (int j = 0; j != arrnodes.count() / 5; ++j)
		{
			if (pointsNew[i] == pointsnum[j])
			{
				xNew[i] = x[j];
				savebuf2 = xNew[i];
				savetextstream << tr("%1").arg(savebuf2, 20);
				yNew[i] = y[j];
				savebuf2 = yNew[i];
				savetextstream << tr("%1").arg(savebuf2, 20);
				zNew[i] = z[j];
				savebuf2 = zNew[i];
				savetextstream << tr("%1").arg(savebuf2, 20) << endl;
			}
		}
	}
	savefile.close();
	delete triangle;
	delete facenum1;
	delete pointsnum1;
	delete pointsnum2;
	delete pointsnum3;
	delete pointsnum;
	delete x;
	delete y;
	delete z;
	delete face1;
	delete face2;
	delete triangleNew;
	delete faceNew;
	delete pointsNew;
	delete xNew;
	delete yNew;
	delete zNew;
}
void MeshView::Ok()
{
	bool enableflag = ui->EnabledCB->isChecked();
	bool hideflag = ui->HideCB->isChecked();
	if (ui->MeshViewComB->currentIndex() == 0 && enableflag)
	{
		MeshAll();
	}
	else if (ui->MeshViewComB->currentIndex() == 1 && enableflag)
	{
		MeshAndModel();
	}
	else if (ui->MeshViewComB->currentIndex() >= 2 && enableflag)
	{
		int faceNo = 0;
		faceNo = ui->MeshViewComB->currentText().toInt();
		OneFace(faceNo);
	}
	if (hideflag)
	{
		int faceNo = 0;
		faceNo = ui->CheckedFaceComB->currentText().toInt();
		OneFace(faceNo);
	}
	if ((!hideflag) && (!enableflag))
	{
		int faceNo = 0;
		OneFace(faceNo);
	}

}
void MeshView::OneFace(int faceNo)
{
	//画单个面的网格	
	QFile file(QObject::tr("../ini/Mesh.dat"));
	QVector<double> arr;
	if (!file.open(QIODevice::ReadOnly))
	{
		return;
	}
	QTextStream textstream(&file);
	while (!textstream.atEnd())
	{
		static double buf;
		textstream >> buf;
		arr.append(buf);
	}
	file.close();
	double *triangleU;
	double *faceU;
	double *pointU;
	double *x;
	double *y;
	double *z;
	triangleU = new double[arr.count() / 6];
	faceU = new double[arr.count() / 6];
	pointU = new double[arr.count() / 6];
	x = new double[arr.count() / 6];
	y = new double[arr.count() / 6];
	z = new double[arr.count() / 6];
	double *triangleUN;
	double *faceUN;
	double *pointUN;
	double *xN;
	double *yN;
	double *zN;
	int counttriface = 0;
	for (int i = 0; i != arr.count(); ++i)
	{
		if (i % 6 == 0 && i / 6 < arr.count() / 6)
		{
			triangleU[i / 6] = arr[i];
		}
		if (i % 6 == 1)
		{
			faceU[i / 6] = arr[i];
			if (faceU[i / 6] == faceNo && (ui->EnabledCB->isChecked()))
			{
				counttriface++;
			}
			if (faceU[i / 6] != faceNo && (ui->HideCB->isChecked()))
			{
				counttriface++;
			}
			if ((!ui->EnabledCB->isChecked())&&(!ui->HideCB->isChecked()))
			{
				for (int j = 0; j != ui->SelectFacesLW->count(); ++j)
				{
					if (ui->SelectFacesLW->item(j)->isSelected() && (faceU[i / 6] == ui->SelectFacesLW->item(j)->text().toInt()))
					{
						counttriface++;
					}
				}
			}
		}
		if (i % 6 == 2)
		{
			pointU[i / 6] = arr[i];
		}
		if (i % 6 == 3)
		{
			x[i / 6] = arr[i];
		}
		if (i % 6 == 4)
		{
			y[i / 6] = arr[i];
		}
		if (i % 6 == 5)
		{
			z[i / 6] = arr[i];
		}
	}
	triangleUN = new double[counttriface];
	faceUN = new double[counttriface];
	pointUN = new double[counttriface];
	xN = new double[counttriface];
	yN = new double[counttriface];
	zN = new double[counttriface];
	int counttri = 0;
	for (int i = 0; i != arr.count() / 6; ++i)
	{
		if (faceU[i] == faceNo && (ui->EnabledCB->isChecked()))
		{
			triangleUN[counttri] = triangleU[i];
			faceUN[counttri] = faceU[i];
			pointUN[counttri] = pointU[i];
			xN[counttri] = x[i];
			yN[counttri] = y[i];
			zN[counttri] = z[i];
			counttri++;
		}
		if (faceU[i] != faceNo && (ui->HideCB->isChecked()))
		{
			triangleUN[counttri] = triangleU[i];
			faceUN[counttri] = faceU[i];
			pointUN[counttri] = pointU[i];
			xN[counttri] = x[i];
			yN[counttri] = y[i];
			zN[counttri] = z[i];
			counttri++;
		}
		if ((!ui->EnabledCB->isChecked()) && (!ui->HideCB->isChecked()))
		{
			for (int j = 0; j != ui->SelectFacesLW->count(); ++j)
			{
				if (ui->SelectFacesLW->item(j)->isSelected() && (faceU[i] == ui->SelectFacesLW->item(j)->text().toInt()))
				{
					triangleUN[counttri] = triangleU[i];
					faceUN[counttri] = faceU[i];
					pointUN[counttri] = pointU[i];
					xN[counttri] = x[i];
					yN[counttri] = y[i];
					zN[counttri] = z[i];
					counttri++;
				}
			}
		}
	}
	//三维点云隐式曲面重构技术
	vtkSmartPointer<vtkRenderer> ren = vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renWin = vtkSmartPointer<vtkRenderWindow>::New();
	vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> cell = vtkSmartPointer<vtkCellArray>::New();
	vtkSmartPointer<vtkPolyData> pointsPoly = vtkSmartPointer<vtkPolyData>::New();
	vtkSmartPointer<vtkPoints> pointsface = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> cellface = vtkSmartPointer<vtkCellArray>::New();
	vtkSmartPointer<vtkPolyData> pointsPolyface = vtkSmartPointer<vtkPolyData>::New();
	vtkSmartPointer<vtkPolyDataMapper> pointsMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	vtkSmartPointer<vtkPolyDataMapper> pointsMapperColor = vtkSmartPointer<vtkPolyDataMapper>::New();
	vtkSmartPointer<vtkCamera> camera = vtkSmartPointer<vtkCamera>::New();
	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	vtkSmartPointer<vtkActor> actorColor = vtkSmartPointer<vtkActor>::New();
	vtkSmartPointer<vtkAxesActor> axes = vtkSmartPointer<vtkAxesActor>::New();
	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	vtkSmartPointer<vtkOrientationMarkerWidget> widget = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
	vtkSmartPointer<vtkTransformPolyDataFilter> pTransformPoly = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	int umax = 0;
	for (int i = 0; i != arr.count() / 6; ++i)
	{
		points->InsertPoint(pointU[i], x[i], y[i], z[i]);
		umax = umax > triangleU[i] ? umax : triangleU[i];
	}
	vtkIdType(*pts)[3] = new vtkIdType[umax][3];
	for (int i = 0; i != umax; ++i)
	{
		for (int j = 0; j != 3; ++j)
		{
			pts[i][j] = pointU[i * 3 + j];
		}
		cell->InsertNextCell(3, pts[i]);
	}
	int umaxface = 0;
	for (int i = 0; i != counttri; ++i)
	{
		pointsface->InsertPoint(pointUN[i], xN[i], yN[i], zN[i]);
		umaxface = umaxface > triangleUN[i] ? umaxface : triangleUN[i];
	}
	vtkIdType(*ptsface)[3] = new vtkIdType[(counttri / 3)][3];
	for (int i = 0; i != counttri / 3; ++i)
	{
		for (int j = 0; j != 3; ++j)
		{
			ptsface[i][j] = pointUN[i * 3 + j];
		}
		cellface->InsertNextCell(3, ptsface[i]);
	}
	pointsPolyface->SetPoints(pointsface);
	pointsPolyface->SetStrips(cellface);
	pointsPoly->SetPoints(points);
	pointsPoly->SetStrips(cell);
	pointsMapperColor->SetInputData(pointsPoly);
	pointsMapper->SetInputData(pointsPolyface);
	pointsMapperColor->ScalarVisibilityOff();
	camera->SetPosition(1, 0, 0);
	camera->SetFocalPoint(0, 0, 0);
	camera->SetViewUp(0, 0, 1);
	camera->Zoom(0.5);
	actor->SetMapper(pointsMapper);
	actorColor->SetMapper(pointsMapperColor);
	actorColor->GetProperty()->SetRepresentationToSurface();
	actorColor->GetProperty()->SetOpacity(0.5);
	actor->GetProperty()->SetRepresentationToWireframe();
	actor->RotateX(30);
	actor->RotateY(30);
	actor->RotateZ(30);
	actorColor->RotateX(30);
	actorColor->RotateY(30);
	actorColor->RotateZ(30);
	transform->RotateX(30);
	transform->RotateY(30);
	transform->RotateZ(30);
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
	ren->AddActor(actor);
	ren->AddActor(actorColor);
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
	delete triangleU;
	delete pointU;
	delete x;
	delete y;
	delete z;
	delete faceU;
	delete triangleUN;
	delete pointUN;
	delete xN;
	delete yN;
	delete zN;
	delete faceUN;
	delete[] pts;
	delete[] ptsface;
	
}

void MeshView::MeshAll()
{
	//画三维网格	
	QFile file(QObject::tr("../ini/Mesh.dat"));
	QVector<double> arr;
	if (!file.open(QIODevice::ReadOnly))
	{
		return;
	}
	QTextStream textstream(&file);
	while (!textstream.atEnd())
	{
		static double buf;
		textstream >> buf;
		arr.append(buf);
	}
	file.close();
	double *triangleU;
	double *faceU;
	double *pointU;
	double *x;
	double *y;
	double *z;
	triangleU = new double[arr.count() / 6];
	faceU = new double[arr.count() / 6];
	pointU = new double[arr.count() / 6];
	x = new double[arr.count() / 6];
	y = new double[arr.count() / 6];
	z = new double[arr.count() / 6];
	for (int i = 0; i != arr.count(); ++i)
	{
		if (i % 6 == 0 && i / 6 < arr.count() / 6)
		{
			triangleU[i / 6] = arr[i];
		}
		if (i % 6 == 1)
		{
			faceU[i / 6] = arr[i];
		}
		if (i % 6 == 2)
		{
			pointU[i / 6] = arr[i];
		}
		if (i % 6 == 3)
		{
			x[i / 6] = arr[i];
		}
		if (i % 6 == 4)
		{
			y[i / 6] = arr[i];
		}
		if (i % 6 == 5)
		{
			z[i / 6] = arr[i];
		}
	}
	//三维点云隐式曲面重构技术
	vtkSmartPointer<vtkRenderer> ren = vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renWin = vtkSmartPointer<vtkRenderWindow>::New();
	vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> cell = vtkSmartPointer<vtkCellArray>::New();
	vtkSmartPointer<vtkPolyData> pointsPoly = vtkSmartPointer<vtkPolyData>::New();
	vtkSmartPointer<vtkPolyDataMapper> pointsMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	vtkSmartPointer<vtkCamera> camera = vtkSmartPointer<vtkCamera>::New();
	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	vtkSmartPointer<vtkAxesActor> axes = vtkSmartPointer<vtkAxesActor>::New();
	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	vtkSmartPointer<vtkOrientationMarkerWidget> widget = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
	vtkSmartPointer<vtkTransformPolyDataFilter> pTransformPoly = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	int umax = 0;
	for (int i = 0; i != arr.count() / 6; ++i)
	{
		points->InsertPoint(pointU[i], x[i], y[i], z[i]);
		umax = umax > triangleU[i] ? umax : triangleU[i];
	}
	vtkIdType(*pts)[3] = new vtkIdType[umax][3];
	for (int i = 0; i != umax; ++i)
	{
		for (int j = 0; j != 3; ++j)
		{
			pts[i][j] = pointU[i * 3 + j];
		}
		cell->InsertNextCell(3, pts[i]);
	}
	pointsPoly->SetPoints(points);
	pointsPoly->SetStrips(cell);
	pointsMapper->SetInputData(pointsPoly);
	camera->SetPosition(1, 0, 0);
	camera->SetFocalPoint(0, 0, 0);
	camera->SetViewUp(0, 0, 1);
	camera->Zoom(0.5);
	actor->SetMapper(pointsMapper);
	actor->GetProperty()->SetColor(0.0, 1.0, 1.0);
	actor->GetProperty()->SetRepresentationToWireframe();
	actor->RotateX(30);
	actor->RotateY(30);
	actor->RotateZ(30);
	transform->RotateX(30);
	transform->RotateY(30);
	transform->RotateZ(30);
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
	ren->AddActor(actor);
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
	delete triangleU;
	delete pointU;
	delete x;
	delete y;
	delete z;
	delete faceU;
	delete[] pts;

}


void MeshView::MeshAndModel()
{	
	//画网格+模型	
	QFile file(QObject::tr("../ini/Mesh.dat"));
	QVector<double> arr;
	if (!file.open(QIODevice::ReadOnly))
	{
		return;
	}
	QTextStream textstream(&file);
	while (!textstream.atEnd())
	{
		static double buf;
		textstream >> buf;
		arr.append(buf);
	}
	file.close();
	double *triangleU;
	double *faceU;
	double *pointU;
	double *x;
	double *y;
	double *z;
	triangleU = new double[arr.count() / 6];
	faceU = new double[arr.count() / 6];
	pointU = new double[arr.count() / 6];
	x = new double[arr.count() / 6];
	y = new double[arr.count() / 6];
	z = new double[arr.count() / 6];
	double *triangleUN;
	double *faceUN;
	double *pointUN;
	double *xN;
	double *yN;
	double *zN;
	int counttriface = 0;
	for (int i = 0; i != arr.count(); ++i)
	{
		if (i % 6 == 0 && i / 6 < arr.count() / 6)
		{
			triangleU[i / 6] = arr[i];
		}
		if (i % 6 == 1)
		{
			faceU[i / 6] = arr[i];
			if (ui->EnabledCB->isChecked())
			{
				for (int j = 0; j != ui->SelectFacesLW->count(); ++j)
				{
					if (faceU[i / 6] == ui->SelectFacesLW->item(j)->text().toInt())
					{
						counttriface++;
					}
				}				
			}			
		}
		if (i % 6 == 2)
		{
			pointU[i / 6] = arr[i];

		}
		if (i % 6 == 3)
		{
			x[i / 6] = arr[i];
		}
		if (i % 6 == 4)
		{
			y[i / 6] = arr[i];
		}
		if (i % 6 == 5)
		{
			z[i / 6] = arr[i];
		}
	}
	triangleUN = new double[counttriface];
	faceUN = new double[counttriface];
	pointUN = new double[counttriface];
	xN = new double[counttriface];
	yN = new double[counttriface];
	zN = new double[counttriface];
	int counttri = 0;
	for (int i = 0; i != arr.count() / 6; ++i)
	{
		triangleUN[counttri] = triangleU[i];
		faceUN[counttri] = faceU[i];
		pointUN[counttri] = pointU[i];
		xN[counttri] = x[i];
		yN[counttri] = y[i];
		zN[counttri] = z[i];
		counttri++;			
	}
	//三维点云隐式曲面重构技术
	vtkSmartPointer<vtkRenderer> ren = vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renWin = vtkSmartPointer<vtkRenderWindow>::New();
	vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> cell = vtkSmartPointer<vtkCellArray>::New();
	vtkSmartPointer<vtkPolyData> pointsPoly = vtkSmartPointer<vtkPolyData>::New();
	vtkSmartPointer<vtkPoints> pointsface = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> cellface = vtkSmartPointer<vtkCellArray>::New();
	vtkSmartPointer<vtkPolyData> pointsPolyface = vtkSmartPointer<vtkPolyData>::New();
	vtkSmartPointer<vtkPolyDataMapper> pointsMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	vtkSmartPointer<vtkPolyDataMapper> pointsMapperColor = vtkSmartPointer<vtkPolyDataMapper>::New();
	vtkSmartPointer<vtkCamera> camera = vtkSmartPointer<vtkCamera>::New();
	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	vtkSmartPointer<vtkActor> actorColor = vtkSmartPointer<vtkActor>::New();
	vtkSmartPointer<vtkAxesActor> axes = vtkSmartPointer<vtkAxesActor>::New();
	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	vtkSmartPointer<vtkOrientationMarkerWidget> widget = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
	vtkSmartPointer<vtkTransformPolyDataFilter> pTransformPoly = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	int umax = 0;
	for (int i = 0; i != arr.count() / 6; ++i)
	{
		points->InsertPoint(pointU[i], x[i], y[i], z[i]);
		umax = umax > triangleU[i] ? umax : triangleU[i];
	}
	vtkIdType(*pts)[3] = new vtkIdType[umax][3];
	for (int i = 0; i != umax; ++i)
	{
		for (int j = 0; j != 3; ++j)
		{
			pts[i][j] = pointU[i * 3 + j];
		}
		cell->InsertNextCell(3, pts[i]);
	}
	int umaxface = 0;
	for (int i = 0; i != counttri; ++i)
	{
		pointsface->InsertPoint(pointUN[i], xN[i], yN[i], zN[i]);
		umaxface = umaxface > triangleUN[i] ? umaxface : triangleUN[i];
	}
	vtkIdType(*ptsface)[3] = new vtkIdType[(counttri / 3)][3];
	for (int i = 0; i != counttri / 3; ++i)
	{
		for (int j = 0; j != 3; ++j)
		{
			ptsface[i][j] = pointUN[i * 3 + j];
		}
		cellface->InsertNextCell(3, ptsface[i]);
	}
	pointsPolyface->SetPoints(pointsface);
	pointsPolyface->SetStrips(cellface);
	pointsPoly->SetPoints(points);
	pointsPoly->SetStrips(cell);
	pointsMapperColor->SetInputData(pointsPoly);
	pointsMapper->SetInputData(pointsPolyface);
	camera->SetPosition(1, 0, 0);
	camera->SetFocalPoint(0, 0, 0);
	camera->SetViewUp(0, 0, 1);
	camera->Zoom(0.5);
	actor->SetMapper(pointsMapper);
	actorColor->SetMapper(pointsMapperColor);
	actorColor->GetProperty()->SetRepresentationToSurface();
	actorColor->GetProperty()->SetOpacity(0.5);
	actor->GetProperty()->SetRepresentationToWireframe();
	actor->RotateX(30);
	actor->RotateY(30);
	actor->RotateZ(30);
	actorColor->RotateX(30);
	actorColor->RotateY(30);
	actorColor->RotateZ(30);
	transform->RotateX(30);
	transform->RotateY(30);
	transform->RotateZ(30);
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
	ren->AddActor(actor);
	ren->AddActor(actorColor);
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
	delete triangleU;
	delete pointU;
	delete x;
	delete y;
	delete z;
	delete faceU;
	delete triangleUN;
	delete pointUN;
	delete xN;
	delete yN;
	delete zN;
	delete faceUN;
	delete[] pts;
	delete[] ptsface;

}

void MeshView::CheckedEnable()
{
	if (ui->EnabledCB->isChecked())
	{
		ui->CheckedFaceComB->setEnabled(false);
		ui->HideCB->setEnabled(false);
		ui->MultiFacesLE->setEnabled(false);
		ui->SelectFacesLW->setEnabled(false);
	}
	else
	{
		ui->CheckedFaceComB->setEnabled(true);
		ui->HideCB->setEnabled(true);
		ui->MultiFacesLE->setEnabled(true);
		ui->SelectFacesLW->setEnabled(true);
	}
}
void MeshView::MultiFacesView()
{
	ui->MultiFacesLE->clear();
	for (int i = 0; i != ui->SelectFacesLW->count(); ++i)
	{
		if (ui->SelectFacesLW->item(i)->isSelected())
		{
			ui->MultiFacesLE->setText(ui->MultiFacesLE->text().append(ui->SelectFacesLW->item(i)->text()) + tr(";"));
		}		
	}
}
void MeshView::CheckedHide()
{
	if (ui->HideCB->isChecked())
	{
		ui->MultiFacesLE->setEnabled(false);
		ui->SelectFacesLW->setEnabled(false);
		ui->MeshViewComB->setEnabled(false);
		ui->EnabledCB->setEnabled(false);
	}
	else
	{
		ui->MultiFacesLE->setEnabled(true);
		ui->SelectFacesLW->setEnabled(true);
		ui->MeshViewComB->setEnabled(true);
		ui->EnabledCB->setEnabled(true);
	}

}
void MeshView::AddItems()
{
	QFile facefile(QObject::tr("../ini/facemap.dat"));
	QVector<int> arrface;
	if (!facefile.open(QIODevice::ReadOnly))
	{
		return;
	}
	QTextStream facestream(&facefile);
	while (!facestream.atEnd())
	{
		static int buf;
		facestream >> buf;
		arrface.append(buf);
	}
	facefile.close();
	int *face1;
	int *face2;
	face1 = new int[arrface.count() / 2];
	face2 = new int[arrface.count() / 2];
	ui->SelectFacesLW->setSelectionMode(QAbstractItemView::ExtendedSelection);
	for (int i = 0; i != arrface.count(); ++i)
	{
		if (i % 2 == 0 && i / 2 < arrface.count() / 2)
		{
			face1[i / 2] = arrface[i];
			ui->MeshViewComB->insertItem(i + 2, QString::number(face1[i / 2]));
			ui->CheckedFaceComB->insertItem(i, QString::number(face1[i / 2]));
			ui->SelectFacesLW->addItem(QString::number(face1[i / 2]));		    
		}
		if (i % 2 == 1)
		{
			face2[i / 2] = arrface[i];
		}
	}
	delete face1;
	delete face2;

}

void MeshView::Cancel()
{
	QDialog::close();
}
#include "moc_MeshView.cpp"