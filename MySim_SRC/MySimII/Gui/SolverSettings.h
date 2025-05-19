#ifndef MYSIMGUI_SOLVERSETTINGS_H
#define MYSIMGUI_SOLVERSETTINGS_H
#include <memory>

#include <QtCore>
#include <Gui/TaskView/TaskDialog.h>
#include <Gui/TaskView/TaskView.h>
#include <Gui/Selection.h>
#include "ui_SolverSettings.h"

namespace MySimIIGui {

class SolverSettings : public QWidget
{
  Q_OBJECT
public:
  
    SolverSettings(QWidget *parent = 0);
    ~SolverSettings();
	
    static void reviseinit(const char* str);
	void showEvent(QShowEvent *event);

protected:


	private Q_SLOTS :
		void PushButtonok(void);
		void EnableIterativeSolver();
	  
	 
public:
	std::auto_ptr<Ui_SolverSettings> ui;
};

}//namespace 
#endif
