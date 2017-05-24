#ifndef WINDOW_MAIN_H
#define WINDOW_MAIN_H

#include <QMainWindow>
#include <QLabel>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QHorizontalPercentBarSeries>

#include "dialog_settings.h"

QT_CHARTS_USE_NAMESPACE

namespace Ui {
	class WindowMain;
	}

class WindowMain : public QMainWindow
{
		Q_OBJECT

	public:
		explicit WindowMain(QWidget *parent = 0);
		~WindowMain();
		DialogSettings *dialog;

	protected:
		void changeEvent(QEvent *e);

	private slots:
		void on_redrawButton_clicked();

		void on_actionCommandWindow_triggered();

		void on_actionSettingsWindow_triggered();

		void on_actionLogWindow_triggered();

		void on_viewTypeBinaryTree_clicked();

		void on_viewTypeHorizontalBar_clicked();

	private:
		const uint16_t MESSAGE_TIMEOUT = 2000;

		Ui::WindowMain *ui;
		QLabel *viewLabel;
		QChartView *viewChart;

	public slots:
		void showMessage(const QString& string);

	signals:
		void chooseDialogTab(DialogTab tab);
};

#endif // WINDOW_MAIN_H
