#ifndef MYSIMGUI_INITIALMESH_H
#define MYSIMGUI_INITIALMESH_H
#include <memory>
#include <Gui/TaskView/TaskDialog.h>
#include <Gui/TaskView/TaskView.h>
#include <Gui/Selection.h>
#include "ui_InitialMesh.h"
#include <QtCore>
using namespace std;
namespace MySimIIGui
{
	class InitialMesh :public QDialog
	{
		Q_OBJECT//
	public:
		InitialMesh(QDialog *parent = 0);
		~InitialMesh();
		void Boundaryini();
		void modeini();
		void MeshPartFace();
		int GetFaceCodeWithGenCodeFromData(int genFaceCode);
		static int flagWarning;
	protected:

	private Q_SLOTS:
		//void Warning();
		void Warning1();
		void Warning2();
		void Warning3();
		void Warning4();
		void Warning5();
		void Warning6();
		void Warning7();
		void Ok();
		void Cancel();
		int WriteIni();
		void tablebem();
		void Tableboundary();
		void Tablemodel();
		void showEvent(QShowEvent *event);
	public:
		//std::auto_ptr<Ui::InitialMesh> ui;
		static void reviseinit(const char* str);
		std::auto_ptr<Ui_InitialMesh> ui;
		bool flag = true;
	};
}
#endif