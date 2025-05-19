#ifndef MYSIMGUI_SOLUTIONSETUP_H
#define MYSIMGUI_SOLUTIONSETUP_H
#include <memory>
#include <Gui/TaskView/TaskDialog.h>
#include <Gui/TaskView/TaskView.h>
#include <Gui/Selection.h>
#include "ui_SolutionSetup.h"
#include <QtCore>
namespace MySimIIGui
{
	class SolutionSetup :public QMainWindow
	{
		Q_OBJECT//
	public:
		SolutionSetup(QMainWindow *parent = 0);
		~SolutionSetup();
		
	
		void insertcomboBox();
	protected:

		public Q_SLOTS :
			void OK();
			void Cancel();
			void Warning();

	public:
		static void reviseinit(const char* str);
		std::auto_ptr<Ui_SolutionSetup> ui;
	

	};
}//namespace 
#endif