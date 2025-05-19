#include "PreCompiled.h"
#ifndef _PreComp_
#include <QMessageBox>
#endif
#include "ui_Glyphs.h"
#include "Glyphs.h"
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
#include <vtkInteractorStyleTrackballActor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkWindowToImageFilter.h>
#include <vtkPNGWriter.h>
#include <vtkInteractorStyleSwitch.h>
#include <vtkInteractorStyleUser.h>
#include <vtkInteractorStyleFlight.h>
#include <vtkInteractorStyleRubberBandZoom.h>
#include <vtkJPEGWriter.h>

#include <vtkSmartPointer.h>
#include <vtkGlyph2D.h>
#include <vtkAnimationCue.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkConeSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkCommand.h>
#include <vtkAnimationScene.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCamera.h>
#include <vtkPlane.h>
#include <vtkCutter.h>
#include <vtkProperty.h>

#include <vtkNamedColors.h>
#include <vtkColor.h>
#include <vtkProgrammableFilter.h>
#include <vtkCallbackCommand.h>
#include <vtkContourFilter.h>
#include <vtkDataArraySelection.h>
#include <vtkAlgorithmOutput.h>

#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Compound.hxx>
#include <TopExp_Explorer.hxx>
#include <gp_Pnt.hxx>
#include <gp_Circ.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Elips.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <gp_Vec.hxx>
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepClass_FaceClassifier.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <Precision.hxx>
#include <TColgp_Array2OfPnt.hxx>

using namespace MySimIIGui;
using namespace std;
Glyphs::Glyphs(QWidget *parent) : QWidget(parent), ui(new Ui::Glyphs)
{
	ui->setupUi(this);
	connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(PushButtonok()));
	connect(ui->pushButton_Cancel, SIGNAL(clicked()), this, SLOT(PushButtoncancel()));
}


void Glyphs::PushButtonok(void)
{
        //directory to pick up model results
	QString dirstr = QString::fromUtf8(App::GetApplication().getHomePath()) + QString::fromUtf8("ini/Mesh_dir/");
	//Variable to display
        QString arrayname = ui->SelectVariable->currentText();
        QString filename  = ui->SN_Start->currentText();
 
        if (filename.length()==1)
	{
           filename = tr("%1case000%2.vtu").arg(dirstr).arg(filename);
	}
	else if (filename.length()==2)
	{
           filename = tr("%1case00%2.vtu").arg(dirstr).arg(filename);
	}
	else if (filename.length()==3)
	{
           filename = tr("%1case0%2.vtu").arg(dirstr).arg(filename);
	}
	else if (filename.length()==4)
	{
           filename = tr("%1case%2.vtu").arg(dirstr).arg(filename);
	}
	else
	{
           QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong"),
                                QObject::tr("Sequential No. %1 is too large.").arg(filename));
           return;
	}

	//--define vtk stuff
	//--define vtk stuff
	vtkSmartPointer<vtkXMLUnstructuredGridReader> ReaderFile  = vtkSmartPointer<vtkXMLUnstructuredGridReader>::New();
	vtkSmartPointer<vtkRenderer> ren                          = vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renWin                   = vtkSmartPointer<vtkRenderWindow>::New();
	vtkSmartPointer<vtkRenderWindowInteractor> iren           = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	vtkSmartPointer<vtkPoints> points                         = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> cells                       = vtkSmartPointer<vtkCellArray>::New();
	vtkSmartPointer<vtkPolyData> pointsPoly                   = vtkSmartPointer<vtkPolyData>::New();
	vtkSmartPointer<vtkCamera> camera                         = vtkSmartPointer<vtkCamera>::New();
	vtkSmartPointer<vtkLookupTable> pColorTable               = vtkSmartPointer<vtkLookupTable>::New();
	vtkSmartPointer<vtkActor> actor                           = vtkSmartPointer<vtkActor>::New();
	vtkSmartPointer<vtkScalarBarActor> bar                    = vtkSmartPointer<vtkScalarBarActor>::New();
	vtkSmartPointer<vtkAxesActor> axes                        = vtkSmartPointer<vtkAxesActor>::New();  
	vtkSmartPointer<vtkTransform> transform                   = vtkSmartPointer<vtkTransform>::New();
	vtkSmartPointer<vtkTriangleFilter> filter                 = vtkSmartPointer<vtkTriangleFilter>::New();
	vtkSmartPointer<vtkLinearSubdivisionFilter> linear        = vtkSmartPointer<vtkLinearSubdivisionFilter>::New();
	vtkSmartPointer<vtkInteractorStyleTrackballCamera> style1 = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
	vtkSmartPointer<vtkOrientationMarkerWidget> widget        = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
	vtkSmartPointer<vtkWindowToImageFilter> wif               = vtkSmartPointer<vtkWindowToImageFilter>::New();
	vtkSmartPointer<vtkJPEGWriter> writer                     = vtkSmartPointer<vtkJPEGWriter>::New();
	vtkSmartPointer<vtkPlane> plane                           = vtkSmartPointer<vtkPlane>::New();
        vtkSmartPointer<vtkCutter> planeCut                       = vtkSmartPointer<vtkCutter>::New();
	vtkSmartPointer<vtkAssignAttribute> variable              = vtkSmartPointer<vtkAssignAttribute>::New();

	//--read in data set
        ReaderFile->SetFileName(filename.toStdString().c_str());
        ReaderFile->Update();
        ReaderFile->GetOutput()->Register(ReaderFile);
        vtkDataSet   *dataset         = vtkDataSet::SafeDownCast(ReaderFile->GetOutput());
	vtkPointData *pointdata       = dataset->GetPointData();
	vtkCellData  *cellsdata       = dataset->GetCellData();
        vtkIdType NumberOfPoints      = dataset->GetNumberOfPoints();
        vtkIdType NumberOfCells       = dataset->GetNumberOfCells();
        vtkIdType NumberOfPointArrays = pointdata->GetNumberOfArrays();
        vtkIdType NumberOfCellArrays  = cellsdata->GetNumberOfArrays();
	vtkDataArray *scalar;

	//--transfer to PolyData
        for (int i = 0; i < NumberOfPoints; i++)
	{
           points->InsertNextPoint(dataset->GetPoint(i));
	}
        for (int i = 0; i < NumberOfCells; i++)
	{
           cells->InsertNextCell(dataset->GetCell(i));
	}
	scalar = pointdata->GetArray(arrayname.toStdString().c_str() );
        pointsPoly->SetPoints(points);
	pointsPoly->SetStrips(cells);
        pointsPoly->GetPointData()->SetScalars( scalar );
        pointsPoly->GetPointData()->SetAttribute(scalar, 1);

        variable->SetInputConnection(ReaderFile->GetOutputPort());
        variable->Assign(arrayname.toStdString().c_str(), vtkDataSetAttributes::VECTORS, vtkAssignAttribute::POINT_DATA);

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

	bar->SetLookupTable(pColorTable);
	bar->SetTitle(arrayname.toStdString().c_str());
        camera->SetPosition(
                   ui->lineEdit_CameraPX->text().toDouble(),
                   ui->lineEdit_CameraPY->text().toDouble(),
                   ui->lineEdit_CameraPZ->text().toDouble()  );
        camera->SetFocalPoint(
                   ui->lineEdit_FocusPX->text().toDouble(),
                   ui->lineEdit_FocusPY->text().toDouble(),
                   ui->lineEdit_FocusPZ->text().toDouble()  );
        camera->SetViewUp(
                   ui->lineEdit_CameraDX->text().toDouble(),
                   ui->lineEdit_CameraDY->text().toDouble(),
                   ui->lineEdit_CameraDZ->text().toDouble()  );
        camera->Zoom(
                   ui->lineEdit_FocusZoom->text().toDouble()  );
        vtkSmartPointer<vtkArrowSource> arrow = vtkSmartPointer<vtkArrowSource>::New();
	vtkSmartPointer<vtkGlyph3D> glyph3D = vtkSmartPointer<vtkGlyph3D>::New();
        glyph3D->SetSourceConnection(arrow->GetOutputPort());
       	glyph3D->SetInputData(pointsPoly);
        glyph3D->OrientOn();
        glyph3D->SetVectorModeToUseVector();
        glyph3D->SetScaleModeToScaleByVector();
	glyph3D->SetScaleFactor(ui->LineEdit_ScalingFactor->text().toDouble());
        glyph3D->Update();

        vtkSmartPointer<vtkPolyDataMapper> glyphMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        glyphMapper->SetInputConnection(glyph3D->GetOutputPort());
        glyphMapper->SetLookupTable(pColorTable);
        glyphMapper->ScalarVisibilityOn();
        glyphMapper->SetScalarRange(scalar->GetRange());

	actor->SetMapper(glyphMapper);	

	actor->GetProperty()->SetRepresentationToSurface();
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
	renWin->SetSize(600, 450);
	renWin->Render();
	renWin->SetWindowName(arrayname.toStdString().c_str());
	renWin->SetPosition(600,400);
	iren->SetRenderWindow(renWin);
	iren->SetInteractorStyle(style1);
	widget->SetOutlineColor(0.9300, 0.5700, 0.1300);
	widget->SetOrientationMarker(axes);
	widget->SetInteractor(iren);
	widget->SetViewport(0.0, 0.0, 0.4, 0.4);
	widget->SetEnabled(1);
	widget->InteractiveOff();
	iren->Initialize();
	iren->Start();
	QWidget::close();
}

void Glyphs::PushButtoncancel(void)
{
	QWidget::close();
}


void Glyphs::Execute()
{

}

Glyphs::~Glyphs()
{

}

void Glyphs::showEvent(QShowEvent *)
{
  if( ui->SN_Start->count() > 0 ) return;

  QString dirstr = QString::fromUtf8(App::GetApplication().getHomePath()) + QString::fromUtf8("ini/Mesh_dir/");
  QString filename;

  for (int i = 1; i <= 9999; ++i)
  {
    //directory to pick up model results
    if ( i < 10 ) 
      { filename = QString::fromLatin1("%1case000%2.vtu").arg(dirstr).arg(i); }
    else if ( i < 100 ) 
      { filename = QString::fromLatin1("%1case00%2.vtu").arg(dirstr).arg(i); }
    else if ( i < 1000 ) 
      { filename = QString::fromLatin1("%1case0%2.vtu").arg(dirstr).arg(i); }
    else 
      { filename = QString::fromLatin1("%1case%2.vtu").arg(dirstr).arg(i); }

    QFile file2read(filename);
    if ( !file2read.exists() ) {
      break;
    }

    ui->SN_Start->addItem(QString::number(i));		    

  }
 
  vtkSmartPointer<vtkXMLUnstructuredGridReader> ReaderFile = vtkSmartPointer<vtkXMLUnstructuredGridReader>::New();
  vtkDataSet *dataset;
  ReaderFile->SetFileName((QString::fromLatin1("%1case0001.vtu").arg(dirstr)).toStdString().c_str());
  ReaderFile->Update();
  ReaderFile->GetOutput()->Register(ReaderFile);
  dataset = vtkDataSet::SafeDownCast(ReaderFile->GetOutput());
  vtkIdType NumberOfPointArrays = dataset->GetPointData()->GetNumberOfArrays();
  for (vtkIdType i = 0; i < NumberOfPointArrays; i++)
  {
   int icmp = dataset->GetPointData()->GetArray(i)->GetNumberOfComponents();
   if (icmp != 3) continue;
    Base::Console().Message("Variable %s is listed. \n", dataset->GetPointData()->GetArray(i)->GetName());
    ui->SelectVariable->addItem(QObject::tr(dataset->GetPointData()->GetArray(i)->GetName()));
  }

    ui->lineEdit_CFPX->setText(QObject::tr("0.0"));
    ui->lineEdit_CFPY->setText(QObject::tr("0.0"));
    ui->lineEdit_CFPZ->setText(QObject::tr("0.0"));
    ui->lineEdit_CFDX->setText(QObject::tr("1.0"));
    ui->lineEdit_CFDY->setText(QObject::tr("0.0"));
    ui->lineEdit_CFDZ->setText(QObject::tr("0.0"));
    ui->lineEdit_CameraPX->setText(QObject::tr("1.0"));
    ui->lineEdit_CameraPY->setText(QObject::tr("1.0"));
    ui->lineEdit_CameraPZ->setText(QObject::tr("1.0"));
    ui->lineEdit_CameraDX->setText(QObject::tr("0.0"));
    ui->lineEdit_CameraDY->setText(QObject::tr("0.0"));
    ui->lineEdit_CameraDZ->setText(QObject::tr("1.0"));
    ui->lineEdit_FocusPX->setText(QObject::tr("0.0"));
    ui->lineEdit_FocusPY->setText(QObject::tr("0.0"));
    ui->lineEdit_FocusPZ->setText(QObject::tr("0.0"));
    ui->lineEdit_FocusZoom->setText(QObject::tr("1.0"));
}

#include "moc_Glyphs.cpp"
