/***************************************************************************
 *   Copyright (c) 2012 Jürgen Riegel <juergen.riegel@web.de>              *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/


#ifndef GUI_ProjectView_H
#define GUI_ProjectView_H

//#include <QTreeView>
#include <map>
#include <App/Document.h>
#include <App/Application.h>

#include <Gui/DockWindow.h>
#include <Gui/Selection.h>
#include <Gui/ViewProvider.h>
#include <QTreeWidget>
#include <QHeaderView>
#include <QTextStream>
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
#include <vtkButterflySubdivisionFilter.h>
#include <vtkLinearSubdivisionFilter.h>
#include <vtkTriangleFilter.h>
#include <vtkPolyDataWriter.h>
#include <vtkInteractorStyleUnicam.h>
#ifdef vtkGenericDataArray_h
#define InsertNextTupleValue InsertNextTypedTuple
#endif

#define vtkRenderingCore_AUTOINIT 4(vtkInteractionStyle,vtkRenderingFreeType,vtkRenderingFreeTypeOpenGL,vtkRenderingOpenGL2)
#define vtkRenderingVolume_AUTOINIT 1(vtkRenderingVolumeOpenGL2)

namespace Gui {

	enum ProjectObjectType{
		ProjectName = 1000,
		DesignName,
		Model,
		MySimII,
		Boundaries,
		Solvers,
		Meshes,
		Results,
		Nonee
	};

/** Tree view that allows drag & drop of document objects.
 * @author Werner Mayer
 */
	class ProjectWidget : public QTreeWidget
{
    Q_OBJECT

public:
    ProjectWidget(QWidget* parent=0);
    ~ProjectWidget();

protected Q_SLOTS:
	void onEdit();
	void onDelete();

public:
	static void Deleteitem();
	static void OpenProjecttree();

protected:
	/// Observer message from the Selection
	//void onSelectionChanged(const SelectionChanges& msg);
	void contextMenuEvent(QContextMenuEvent * e);
	//void drawRow(QPainter *, const QStyleOptionViewItem &, const QModelIndex &) const;
	///** @name Drag and drop */
	////@{
	//void startDrag(Qt::DropActions supportedActions);
	//bool dropMimeData(QTreeWidgetItem *parent, int index, const QMimeData *data,
	//	Qt::DropAction action);
	//Qt::DropActions supportedDropActions() const;
	//QMimeData * mimeData(const QList<QTreeWidgetItem *> items) const;
	//void dragEnterEvent(QDragEnterEvent * event);
	//void dragLeaveEvent(QDragLeaveEvent * event);
	//void dragMoveEvent(QDragMoveEvent *event);
	//void dropEvent(QDropEvent *event);
	////@}
	//bool event(QEvent *e);
	//void keyPressEvent(QKeyEvent *event);
	void mouseDoubleClickEvent(QMouseEvent * event);


private:
	void slotNewObject(const char* classname, const char* objname);
	void slotDeleteObject(const char* classname, const char* objname);
	
	void InsertNewDesign(void);
	ProjectObjectType GetObjectType(const char* classname);
	std::string GetObjectViewProvider(ProjectObjectType objType);
	std::string GetBasicObjecLableName(ProjectObjectType objType);
	bool StrDelete(QString strFile, QString strFlagOne, QString strFlagTwo);
	static void StrOpenTree(QString strFile, QString strFlag);
private:
    //QDirModel *fileModel;
//	QTreeWidget *tree;
	QTreeWidgetItem *rootItem;

	QTreeWidgetItem* contextItem;
	QAction* editAction;
	QAction* deleteAction;

	QTimer* statusTimer;

	std::map<std::string, QTreeWidgetItem*> ProjectObjMap;

	std::set<std::string> nodeTypeNameSet;
//	std::map<const Gui::Document*, DocumentItem*> DocumentMap;
 
};



}


#endif // GUI_ProjectView_H

