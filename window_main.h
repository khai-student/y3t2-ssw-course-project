#ifndef WINDOW_MAIN_H
#define WINDOW_MAIN_H

#include <QtWidgets/QMainWindow>

namespace Ui {
	class WindowMain;
	}

class WindowMain : public QMainWindow
{
		Q_OBJECT

	public:
		explicit WindowMain(QWidget *parent = 0);
		~WindowMain();

	protected:
		void changeEvent(QEvent *e);

	private:
		Ui::WindowMain *ui;
};

#endif // WINDOW_MAIN_H
