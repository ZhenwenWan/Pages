#ifndef MYSIMGUI_CONTOURS_H
#define MYSIMGUI_CONTOURS_H
#include <memory>
#include <Gui/TaskView/TaskDialog.h>
#include <Gui/TaskView/TaskView.h>
#include <Gui/Selection.h>
#include "ui_Contours.h"
#include "RotateXYZ.h"
#include <QtCore>
#include <iostream>
#include <vtkVersion.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkAnnotatedCubeActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkReverseSense.h>
#include <vtkPolyData.h>
#include <vtkSphereSource.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkAxesActor.h>
#include <vtkPropAssembly.h>
#include <vtkSmartPointer.h>
#include <vtkProperty.h>
#include <vtkPoints.h>
#include <vtkAssignAttribute.h>
#include <vtkXMLUnstructuredGridReader.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkPointData.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkCamera.h>
#include <vtkContourFilter.h>
#include <vtkColorTransferFunction.h>
#include <vtkConeSource.h>
#include <vtkDoubleArray.h>
#include <vtkDataSet.h>
#include <vtkDataSetMapper.h>
#include <vtkLine.h>
#include <vtkLookupTable.h>
#include <vtkTriangle.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTextProperty.h>
#include <vtkDistancePolyDataFilter.h>
#include <vtkPolyDataReader.h>
#include <vtkCleanPolyData.h>
#include <vtkProperty.h>
#include <vtkScalarBarActor.h>
#include <vtkSurfaceReconstructionFilter.h>
#include <vtkSuperquadricSource.h>
#include <vtkImplicitPolyDataDistance.h>
#include <vtkFloatArray.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkCaptionActor2D.h>
#include <vtkProperty2D.h>
#include <vtkCubeAxesActor.h>
#include <vtkMath.h>
#include <cmath>
#include <vtkSmartVolumeMapper.h>
#include <vtkImageGradient.h>
#include <vtkImageViewer.h>
#include <vtkImageGaussianSmooth.h>
#include <vtkImageData.h>
#include <vtkImageAccumulate.h>
#include <vtkXYPlotActor.h>
#include <vtkImageEllipsoidSource.h>
#include <vtkImageLogic.h>
#include <vtkPiecewiseFunction.h>
#include <vtkVolume16Reader.h>
#include <vtkColorTransferFunction.h>
#include <vtkVolumeProperty.h>
#include <vtkVolumeRayCastMapper.h>
#include <vtkVolume.h>
#include <vtkVolumeRayCastCompositeFunction.h>
#include <vtkVolumeRayCastMapper.h>
#include <vtkProp3D.h>
#include <vtkPolyLine.h>
#include <vtkUnstructuredGrid.h>
#include <vtkStructuredGrid.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkOutlineFilter.h>
#include <vtkRotationalExtrusionFilter.h>
#include <vtkSampleFunction.h>
#include <vtkImplicitModeller.h>
#include <vtkElevationFilter.h>
#include <vtkShrinkFilter.h>
#include <vtkIdTypeArray.h>
#include <vtkImageCast.h>
#include <vtkImageConvolve.h>
#include <vtkImageGaussianSmooth.h>
#include <vtkImageHybridMedian2D.h>
#include <vtkImageAnisotropicDiffusion3D.h>
#include <vtkImageAnisotropicDiffusion2D.h>
#include <vtkButterflySubdivisionFilter.h>
#include <vtkLinearSubdivisionFilter.h>
#include <vtkTriangleFilter.h>
#include <vtkPolyDataWriter.h>
#include <vtkInteractorStyleUnicam.h>
#include <vtkArrowSource.h>
#include <vtkGlyph3D.h>


#ifdef vtkGenericDataArray_h
#define InsertNextTupleValue InsertNextTypedTuple
#endif
#define vtkRenderingCore_AUTOINIT 4(vtkInteractionStyle,vtkRenderingFreeType,vtkRenderingFreeTypeOpenGL,vtkRenderingOpenGL2)
#define vtkRenderingVolume_AUTOINIT 1(vtkRenderingVolumeOpenGL2)
using namespace std;
namespace MySimIIGui
{
	class Contours : public QWidget
	{
		Q_OBJECT//
	public:
		Contours(QWidget *parent = 0);
		~Contours();
		void Execute();
	void showEvent(QShowEvent *event);
	protected:

	private Q_SLOTS :		
		void PushButtonok();
		void PushButtoncancel();
	public:
		std::auto_ptr<Ui_Contours> ui;
		double StartValue, StopValue, StepValue;
		
	};


}
#endif
