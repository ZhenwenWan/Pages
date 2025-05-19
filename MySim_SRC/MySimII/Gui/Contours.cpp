#include "PreCompiled.h"
#ifndef _PreComp_
#include <QMessageBox>
#endif
#include "ui_Contours.h"
#include "Contours.h"
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
#include <vtkDataArray.h>
#include <vtkDataArraySelection.h>
#include <vtkAlgorithmOutput.h>


using namespace MySimIIGui;
using namespace std;
Contours::Contours(QWidget *parent) : QWidget(parent), ui(new Ui::Contours)
{
	ui->setupUi(this);
	connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(PushButtonok()));
	connect(ui->pushButton_Cancel, SIGNAL(clicked()), this, SLOT(PushButtoncancel()));
}


void Contours::PushButtonok(void)
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
	vtkSmartPointer<vtkXMLUnstructuredGridReader> ReaderFile  = vtkSmartPointer<vtkXMLUnstructuredGridReader>::New();
	vtkSmartPointer<vtkRenderer> ren                          = vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renWin                   = vtkSmartPointer<vtkRenderWindow>::New();
	vtkSmartPointer<vtkRenderWindowInteractor> iren           = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	vtkSmartPointer<vtkPoints> points                         = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> cells                       = vtkSmartPointer<vtkCellArray>::New();
	vtkSmartPointer<vtkPolyData> pointsPoly                   = vtkSmartPointer<vtkPolyData>::New();
	vtkSmartPointer<vtkPolyDataMapper> pointsMapper           = vtkSmartPointer<vtkPolyDataMapper>::New();
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
	vtkSmartPointer<vtkDoubleArray> scalarcomponent           = vtkSmartPointer<vtkDoubleArray>::New();

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

	int vcount = 0;
	if( arrayname.mid(arrayname.length()-2,1).compare(QObject::tr("C"))==0 ) {
	   vcount = arrayname.right(1).toInt();
	   arrayname = arrayname.left(arrayname.length()-2);
        }
	else if( arrayname.mid(arrayname.length()-2,1).compare(QObject::tr("M"))==0 ) {
	   vcount = 4;
	   arrayname = arrayname.left(arrayname.length()-2);
        }
	
	scalar = pointdata->GetArray(arrayname.toStdString().c_str() );
        
	pointsPoly->SetPoints(points);
	pointsPoly->SetStrips(cells);
        if (scalar->GetNumberOfComponents() == 1)
        {
	    pointsPoly->GetPointData()->SetScalars( scalar );
	}
        if (scalar->GetNumberOfComponents() == 3)
        {
	    vtkIdType j=scalar->GetNumberOfTuples();
	    for( vtkIdType i=0; i < ( scalar->GetNumberOfTuples() ); i++){
		double *xTuple = scalar->GetTuple3(i);
		if(vcount<4) {
	           scalarcomponent->InsertTuple1(i, xTuple[vcount-1]);
		}
		else {
	           double xt = pow(xTuple[0],2.0) + pow(xTuple[1],2.0) + pow(xTuple[2],2.0);
	           scalarcomponent->InsertTuple1(i, pow(xt,0.5));
		}
            }
	    pointsPoly->GetPointData()->SetScalars( scalarcomponent );
	}

        variable->SetInputConnection(ReaderFile->GetOutputPort());
        variable->Assign(arrayname.toStdString().c_str(), vtkDataSetAttributes::SCALARS, vtkAssignAttribute::POINT_DATA);


        if ( ! ui->CheckBoxCuttingProf->isChecked() ) {
          filter->SetInputData(pointsPoly);
	  filter->Update();
	  linear->SetInputData(filter->GetOutput());
	  linear->SetNumberOfSubdivisions(2);
	  linear->Update();
	}
	else {
          plane->SetOrigin(ui->lineEdit_CFPX->text().toDouble(),
                           ui->lineEdit_CFPY->text().toDouble(),
                           ui->lineEdit_CFPZ->text().toDouble()  );
          plane->SetNormal(ui->lineEdit_CFDX->text().toDouble(),
                           ui->lineEdit_CFDY->text().toDouble(),
                           ui->lineEdit_CFDZ->text().toDouble()  );
          planeCut->SetInputConnection(variable->GetOutputPort());
          planeCut->SetCutFunction(plane);
	}

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

        if ( ! ui->CheckBoxCuttingProf->isChecked() ) {
       	  pointsMapper->SetInputData(linear->GetOutput());
	}
	else {
	  pointsMapper->SetInputConnection(planeCut->GetOutputPort());
	}
        if ( ui->LineEdit_VMax->text().isEmpty() || ui->LineEdit_VMin->text().isEmpty() ) {
	  pointsMapper->SetScalarRange(scalar->GetRange());
	}
	else {
	  pointsMapper->SetScalarRange(ui->LineEdit_VMin->text().toDouble(),
	                               ui->LineEdit_VMax->text().toDouble());
	}
	pointsMapper->SetLookupTable(pColorTable);
	bar->SetLookupTable(pColorTable);
	bar->SetNumberOfLabels( ui->LineEdit_NumOfContours->text().toInt() );
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
	vtkSmartPointer<vtkGlyph3D> glyph = vtkSmartPointer<vtkGlyph3D>::New();
        glyph->SetSourceConnection(arrow->GetOutputPort());
        if ( ! ui->CheckBoxCuttingProf->isChecked() ) {
       	  glyph->SetInputData(pointsPoly);
	}
	else {
  	  glyph->SetInputData(planeCut->GetOutput());
	}
        glyph->OrientOn();
        glyph->SetVectorModeToUseVector();
        glyph->Update();

        vtkSmartPointer<vtkPolyDataMapper> glyphMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        glyphMapper->SetInputConnection(glyph->GetOutputPort());
        glyphMapper->SetLookupTable(pColorTable);
        glyphMapper->ScalarVisibilityOn();
        glyphMapper->SetScalarRange(scalar->GetRange());

        actor->SetMapper(pointsMapper);	

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

void Contours::PushButtoncancel(void)
{
	QWidget::close();
}


void Contours::Execute()
{

}

Contours::~Contours()
{

}

void Contours::showEvent(QShowEvent *)
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
    for (int j=1; j<icmp+1; j++) {
       if(icmp>1) {
	  QString addtext = QObject::tr("C%1").arg(j);
          QString VarName = QObject::tr(dataset->GetPointData()->GetArray(i)->GetName());
          ui->SelectVariable->addItem(VarName.append(addtext));    // inserting components of vector
	  if(j==icmp) {
             QString addtext = QObject::tr("Ma");
             QString VarName = QObject::tr(dataset->GetPointData()->GetArray(i)->GetName());
             ui->SelectVariable->addItem(VarName.append(addtext)); // inserting magnitude of vector
	  }
       }
       else {
          QString VarName = QObject::tr(dataset->GetPointData()->GetArray(i)->GetName());
          ui->SelectVariable->addItem(VarName); // inserting scalar
       }
    }
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

#include "moc_Contours.cpp"
