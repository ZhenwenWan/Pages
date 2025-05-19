#include "PreCompiled.h"
#ifndef _PreComp_
#include <QMessageBox>
#endif
#include "ui_Animat.h"
#include "Animat.h"
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
#include <vtkDataArraySelection.h>
#include <vtkAlgorithmOutput.h>

using namespace MySimIIGui;
using namespace std;
Animat::Animat(QWidget *parent) : QWidget(parent), ui(new Ui::Animat)
{
	ui->setupUi(this);
	connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(PushButtonok()));
	connect(ui->pushButton_Cancel, SIGNAL(clicked()), this, SLOT(PushButtoncancel()));
}

void Animat::PushButtoncancel(void)
{
  QWidget::close();
}


void Animat::Execute()
{

}

Animat::~Animat()
{

}

void Animat::showEvent(QShowEvent *)
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
    ui->SN_End  ->addItem(QString::number(i));		    

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
          ui->SelectVariable->addItem(VarName.append(addtext));     // inserting component of vector
	  if(j==icmp) {
             QString addtext = QObject::tr("Ma");
             QString VarName = QObject::tr(dataset->GetPointData()->GetArray(i)->GetName());
             ui->SelectVariable->addItem(VarName);                  // inserting vector itself
             ui->SelectVariable->addItem(VarName.append(addtext));  // inserting magnitude og vector
	  }
       }
       else {
          QString VarName = QObject::tr(dataset->GetPointData()->GetArray(i)->GetName());
          ui->SelectVariable->addItem(VarName);                     // inserting scalar
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
    ui->lineEdit_WindowX->setText(QObject::tr("300"));
    ui->lineEdit_WindowY->setText(QObject::tr("300"));
    ui->lineEdit_WRate->setText(QObject::tr("500"));
}

void Animat::PushButtonok(void)
{

  QString arrayname = ui->SelectVariable->currentText();
  int vcount = 0;
  if( arrayname.mid(arrayname.length()-2,1).compare(QObject::tr("C"))==0 ) {
     vcount = arrayname.right(1).toInt();
     arrayname = arrayname.left(arrayname.length()-2);
  }
  else if( arrayname.mid(arrayname.length()-2,1).compare(QObject::tr("M"))==0 ) {
     vcount = 4;
     arrayname = arrayname.left(arrayname.length()-2);
  }
  int Nstart        = ui->SN_Start->currentText().toInt();
  int Nend          = ui->SN_End->currentText().toInt();
  int NumOfContours = ui->LineEdit_NumOfContours->text().toInt();
  
  //directory to pick up model results
  QString dirstr = QString::fromUtf8(App::GetApplication().getHomePath()) + QString::fromUtf8("ini/Mesh_dir/");
  QStringList filenamevector;
  QString filename;
  for(int i = Nstart; i <= Nend; i++) {
    filename = QObject::tr("%1").arg(i);
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
                           QObject::tr("SN_Start %1 is too large.").arg(filename));
      return;
    }
    filenamevector << filename;
  } 

  vtkSmartPointer<vtkLookupTable> pColorTable = vtkSmartPointer<vtkLookupTable>::New();
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
    
  vtkSmartPointer<vtkXMLUnstructuredGridReader> reader = vtkSmartPointer<vtkXMLUnstructuredGridReader>::New();
  vtkSmartPointer<vtkAssignAttribute> variable         = vtkSmartPointer<vtkAssignAttribute>::New();
  vtkSmartPointer<vtkScalarBarActor> bar               = vtkSmartPointer<vtkScalarBarActor>::New();
  vtkSmartPointer<vtkAxesActor> axes                   = vtkSmartPointer<vtkAxesActor>::New();  
  vtkSmartPointer<vtkOrientationMarkerWidget> widget   = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
  vtkSmartPointer<vtkPlane> plane                      = vtkSmartPointer<vtkPlane>::New();
  vtkSmartPointer<vtkCutter> planeCut                  = vtkSmartPointer<vtkCutter>::New();
  vtkSmartPointer<vtkPoints> points                    = vtkSmartPointer<vtkPoints>::New();
  vtkSmartPointer<vtkCellArray> cells                  = vtkSmartPointer<vtkCellArray>::New();
  vtkSmartPointer<vtkTriangleFilter> filter            = vtkSmartPointer<vtkTriangleFilter>::New();
  vtkSmartPointer<vtkLinearSubdivisionFilter> linear   = vtkSmartPointer<vtkLinearSubdivisionFilter>::New();
  vtkSmartPointer<vtkPolyData> pointsPoly              = vtkSmartPointer<vtkPolyData>::New();
  vtkSmartPointer<vtkArrowSource> arrow                = vtkSmartPointer<vtkArrowSource>::New();
  vtkSmartPointer<vtkGlyph3D> glyph3D                  = vtkSmartPointer<vtkGlyph3D>::New();
  vtkSmartPointer<vtkPolyDataMapper> mapper            = vtkSmartPointer<vtkPolyDataMapper>::New();
  vtkSmartPointer<vtkActor> actor                      = vtkSmartPointer<vtkActor>::New();
  vtkSmartPointer<vtkRenderer> renderer                = vtkSmartPointer<vtkRenderer>::New();
  vtkSmartPointer<vtkRenderWindow> renderWindow        = vtkSmartPointer<vtkRenderWindow>::New();
    
  plane->SetOrigin(ui->lineEdit_CFPX->text().toDouble(),
                   ui->lineEdit_CFPY->text().toDouble(),
                   ui->lineEdit_CFPZ->text().toDouble()  );
  plane->SetNormal(ui->lineEdit_CFDX->text().toDouble(),
                   ui->lineEdit_CFDY->text().toDouble(),
                   ui->lineEdit_CFDZ->text().toDouble()  );
  renderer->GetActiveCamera()->SetPosition(
                   ui->lineEdit_CameraPX->text().toDouble(),
                   ui->lineEdit_CameraPY->text().toDouble(),
                   ui->lineEdit_CameraPZ->text().toDouble()  );
  renderer->GetActiveCamera()->SetFocalPoint(
                   ui->lineEdit_FocusPX->text().toDouble(),
                   ui->lineEdit_FocusPY->text().toDouble(),
                   ui->lineEdit_FocusPZ->text().toDouble()  );
  renderer->GetActiveCamera()->SetViewUp(
                   ui->lineEdit_CameraDX->text().toDouble(),
                   ui->lineEdit_CameraDY->text().toDouble(),
                   ui->lineEdit_CameraDZ->text().toDouble()  );
  renderer->GetActiveCamera()->Zoom(
                   ui->lineEdit_FocusZoom->text().toDouble()  );
  //QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Warning:WZW "), QObject::tr("004"));

  for(int i = 0; i <= Nend - Nstart; i++)
  {
    reader->SetFileName(filenamevector[i].toStdString().c_str());
    reader->Update();
    vtkDataSet   *dataset         = vtkDataSet::SafeDownCast(reader->GetOutput());
    vtkDataArray *scalar          = dataset->GetPointData()->GetArray(arrayname.toStdString().c_str() );
    vtkIdType NumberOfPoints      = dataset->GetNumberOfPoints();
    vtkIdType NumberOfCells       = dataset->GetNumberOfCells();
    vtkSmartPointer<vtkPoints> points               = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkCellArray> cells             = vtkSmartPointer<vtkCellArray>::New();
    vtkSmartPointer<vtkDoubleArray> scalarcomponent = vtkSmartPointer<vtkDoubleArray>::New();
    
    for (int i = 0; i < NumberOfPoints; i++)
    {
       points->InsertNextPoint(dataset->GetPoint(i));
    }
    for (int i = 0; i < NumberOfCells; i++)
    {
       cells->InsertNextCell(dataset->GetCell(i));
    }
    pointsPoly->SetPoints(points);
    pointsPoly->SetStrips(cells);
    variable->SetInputConnection(reader->GetOutputPort());

    if ( scalar->GetNumberOfComponents() == 1 ) {
      variable->Assign(arrayname.toStdString().c_str(), vtkDataSetAttributes::SCALARS, vtkAssignAttribute::POINT_DATA);
      planeCut->SetInputConnection(variable->GetOutputPort());
      planeCut->SetCutFunction(plane);
      planeCut->Update();
      mapper->SetInputConnection(planeCut->GetOutputPort());
    }
    else if ( vcount == 0 ) {
      pointsPoly->GetPointData()->SetScalars( scalar );
      pointsPoly->GetPointData()->SetAttribute(scalar, 1);
      glyph3D->SetSourceConnection(arrow->GetOutputPort());
      glyph3D->SetInputData(pointsPoly);
      glyph3D->OrientOn();
      glyph3D->SetVectorModeToUseVector();
      glyph3D->SetScaleModeToScaleByVector();
      glyph3D->SetScaleFactor(ui->LineEdit_ScaleFactor->text().toDouble());
      glyph3D->Update();
      mapper->SetInputConnection(glyph3D->GetOutputPort());
    }
    else if ( ui->CheckBoxCuttingProf->isChecked() )  {
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
      filter->SetInputData(pointsPoly);
      filter->Update();
      planeCut->SetInputConnection(filter->GetOutputPort());
      planeCut->SetCutFunction(plane);
      planeCut->Update();
      mapper->SetInputConnection(planeCut->GetOutputPort());
    }
    else  {
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
      filter->SetInputData(pointsPoly);
      filter->Update();
      linear->SetInputData(filter->GetOutput());
      linear->SetNumberOfSubdivisions(1);
      linear->Update();
      mapper->SetInputData(linear->GetOutput());
    }

    // mapper
    mapper->SetLookupTable(pColorTable);
    mapper->ScalarVisibilityOn();
    if ( ui->LineEdit_VMax->text().isEmpty() || ui->LineEdit_VMin->text().isEmpty() ) {
      mapper->SetScalarRange(scalar->GetRange());
    }
    else {
      mapper->SetScalarRange(ui->LineEdit_VMin->text().toDouble(),
                             ui->LineEdit_VMax->text().toDouble());
    }
    bar->SetLookupTable(pColorTable);
    bar->SetNumberOfLabels( ui->LineEdit_NumOfContours->text().toInt() );
    bar->SetTitle(arrayname.toStdString().data());
  
    // actor
    actor->SetMapper(mapper);
  
    axes->SetTotalLength(1.0, 1.0, 1.0);
    axes->GetXAxisCaptionActor2D()->GetCaptionTextProperty()->SetColor(1.0, 0.0, 0.0);
    axes->GetYAxisCaptionActor2D()->GetCaptionTextProperty()->SetColor(0.0, 1.0, 0.0);
    axes->GetZAxisCaptionActor2D()->GetCaptionTextProperty()->SetColor(0.0, 0.0, 1.0);
    axes->SetXAxisLabelText("X");
    axes->SetYAxisLabelText("Y");
    axes->SetZAxisLabelText("Z");
    // renderer
    renderer->AddActor(actor);
    renderer->AddActor2D(bar);
    renderer->SetBackground(0.0,0.0,0.0); // Background color white

    // renderwindow
    renderWindow->SetSize( ui->lineEdit_WindowX->text().toDouble(),
                           ui->lineEdit_WindowY->text().toDouble()  );
    renderWindow->AddRenderer(renderer);

    renderWindow->Render();
    Sleep( ui->lineEdit_WRate->text().toDouble() );
  }
    
  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
  renderWindowInteractor->SetRenderWindow(renderWindow);
  widget->SetOutlineColor(0.9300, 0.5700, 0.1300);
  widget->SetOrientationMarker(axes);
  widget->SetInteractor(renderWindowInteractor);
  widget->SetViewport(0.0, 0.0, 0.4, 0.4);
  widget->SetEnabled(1);
  widget->InteractiveOff();
  renderWindowInteractor->Start();

}


#include "moc_Animat.cpp"
