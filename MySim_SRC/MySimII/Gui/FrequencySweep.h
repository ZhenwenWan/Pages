#ifndef MYSIMGUI_FREQUENCYSWEEP_H
#define MYSIMGUI_FREQUENCYSWEEP_H
#include <memory>
#include <Gui/TaskView/TaskDialog.h>
#include <Gui/TaskView/TaskView.h>
#include <Gui/Selection.h>
#include "ui_FrequencySweep.h"
#include <QtCore>
using namespace std;
namespace MySimIIGui
{
	class FrequencySweep :public QMainWindow
	{
		Q_OBJECT//
	public:
		FrequencySweep(QMainWindow *parent = 0);
		~FrequencySweep();
		void play();
	protected:

	public Q_SLOTS:
	
		
		void Warning();
		void Ok();
		void Cancel();
	public:
		static void reviseinit(const char* str);
		std::auto_ptr<Ui_FrequencySweep> ui;
	};
}//namespace EngineGui
#endif