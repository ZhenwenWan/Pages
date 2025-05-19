#ifndef ENGINEGUI_ROTATEXYZ_H
#define ENGINEGUI_ROTATEXYZ_H
#include <memory>
#include <Gui/TaskView/TaskDialog.h>
#include <Gui/TaskView/TaskView.h>
#include <Gui/Selection.h>
#include "ui_RotateXYZ.h"
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
#include <vtkXMLPolyDataWriter.h>
#include <vtkPointData.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkCamera.h>
#include <vtkContourFilter.h>
#include <vtkColorTransferFunction.h>
#include <vtkConeSource.h>
#include <vtkDoubleArray.h>
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
#ifdef vtkGenericDataArray_h
#define InsertNextTupleValue InsertNextTypedTuple
#endif
#include <vtkPolyDataWriter.h>
#include <vtkLinearSubdivisionFilter.h>
#include <vtkLoopSubdivisionFilter.h>
#include <vtkButterflySubdivisionFilter.h>
#include <vtkPolyDataReader.h>
#include <vtkTriangleFilter.h>
#include <vtkIdTypeArray.h>
#include <vtkCallbackCommand.h>
#include <vtkCommand.h>
#include <QtCore>
#include <iostream>

using namespace std;
namespace MySimIIGui
{
	class RotateXYZ :public QDialog
	{
		Q_OBJECT//
	public:
		RotateXYZ(QDialog *parent = 0);
		~RotateXYZ();
	protected:

		private Q_SLOTS :
		void WarningXYZ();
		void RotXYZ(int rotx1, int roty1, int rotz1, int rotx2, int roty2, int rotz2, int flag);
		void Ok();
		void Cancel();
	private:
		std::auto_ptr<Ui_RotateXYZDialog> ui;
		
	public:
		int Rflag = 0;
		int RotX=0;
		int RotY=0;
		int RotZ=0;
		int RotXR = 0;
		int RotYR = 0;
		int RotZR = 0;
		vtkSmartPointer<vtkActor> actor ;
		vtkSmartPointer<vtkTransform> transform1 ;
		vtkSmartPointer<vtkRenderer> ren ;
		vtkSmartPointer<vtkRenderWindow> renWin ;
		vtkSmartPointer<vtkRenderWindowInteractor> iren;
		vtkSmartPointer<vtkPoints> points ;
		vtkSmartPointer<vtkCellArray> cell ;
		vtkSmartPointer<vtkFloatArray> scalars ;
		vtkSmartPointer<vtkPolyData> pointsPoly ;
		vtkSmartPointer<vtkPolyDataMapper> pointsMapper ;
		vtkSmartPointer<vtkCamera> camera ;
		vtkSmartPointer<vtkLookupTable> pColorTable ;		
		vtkSmartPointer<vtkCubeAxesActor> cube ;
		vtkSmartPointer<vtkScalarBarActor> bar ;
		vtkSmartPointer<vtkAxesActor> axes ;
		vtkSmartPointer<vtkOrientationMarkerWidget> widget ;
		vtkSmartPointer<vtkTransformPolyDataFilter> pTransformPoly ;		
		vtkSmartPointer<vtkLinearSubdivisionFilter> linear;
		vtkSmartPointer<vtkTriangleFilter> filter;

	};
}
#endif