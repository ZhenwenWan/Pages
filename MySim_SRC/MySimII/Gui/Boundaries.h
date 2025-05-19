#ifndef MYSIMGUI_BOUNDARIES_H
#define MYSIMGUI_BOUNDARIES_H
#include <memory>

#include <Gui/TaskView/TaskDialog.h>
#include <Gui/TaskView/TaskView.h>
#include <Gui/Selection.h>
#include "ui_Boundaries.h"
#include <QtCore>

namespace MySimIIGui {

class Boundaries : public QWidget
{
  Q_OBJECT //
public:
  
    Boundaries(QWidget *parent = 0);
    ~Boundaries();
	
	void showEvent(QShowEvent *event);
	static int FaceSum;
	static void reviseinit(const char* str);
	static int reviseflag, NVisual;
	static int revisecount;//¼ÇÂ¼editÏî

protected:


	private Q_SLOTS :
		void PushButtoncancel(void);
		void PushButtonok(void);
	  
	 
public:
	std::auto_ptr<Ui_Boundariess> ui;
};
}//namespace 
#endif
