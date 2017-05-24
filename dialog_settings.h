#ifndef DIALOG_SETTINGS_H
#define DIALOG_SETTINGS_H

#include <QDialog>
#include <QDebug>
#include <QDateTime>
#include <QFile>
#include <QFileDialog>
#include <QShortcut>
#include <QList>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QDateTime>
#include <QTimer>
#include <QtCharts/QChartView>

QT_CHARTS_USE_NAMESPACE

#include "memory_settings.h"
#include "command_processor.h"

namespace Ui {
	class DialogSettings;
	enum class DialogTab;
	}

enum class DialogTab
{
	Settings,
	Commands,
	Log
};

class DialogSettings : public QDialog
{
		Q_OBJECT

	public:
		explicit DialogSettings(QWidget *parent = 0);
		~DialogSettings();
		void printMessage(const QString& string);

	protected:
		void changeEvent(QEvent *e);

	private:
		Ui::DialogSettings* ui;
		MemorySettings* memorySettings;
		QShortcut* hotkeyDeleteCmd;
		CommandProcessor* processor;
		QTimer* execTimer;

		void updateSettingsFromObject();
		void updateLabels();	

	public slots:
		void changeTab(DialogTab tab);
		void writeLog(const QString& string);
		void queryChart();


	signals:
		void eventMessage(const QString& string);
		void sendChart(QChartView* chartView);

	private slots:
		void on_minBlockDegreeSlider_sliderMoved(int position);
		void on_minBlockDegreeSpinBox_valueChanged(int value);
		void on_totalMemorySlider_sliderMoved(int position);
		void on_totalMemorySpinBox_valueChanged(int value);
		void on_restoreDefaultSettings_clicked();
		void on_clearLog_clicked();
		void on_exportLog_clicked();
		void on_deleteCmd();
		void on_autoExec_clicked();
		void on_execNextCmd_clicked();
		void on_resetExec_clicked();
};

#endif // DIALOG_SETTINGS_H
