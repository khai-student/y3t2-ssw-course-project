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
	enum class MessageStatus;
	}

enum class DialogTab
{
	Settings,
	Commands,
	Log
};

enum class MessageStatus
{
	Info,
	Error,
	None
};

class DialogSettings : public QDialog
{
		Q_OBJECT

	public:
		explicit DialogSettings(QWidget *parent = 0);
		~DialogSettings();
		void printMessage(const QString& string, const MessageStatus& status = MessageStatus::Info);

	protected:
		void changeEvent(QEvent *e);

	private:
		Ui::DialogSettings* ui;
		MemorySettings* memorySettings;
		QShortcut* hotkeyDeleteCmd;
		CommandProcessor* processor;
		QTimer* execTimer;
		QTimer* saveTimer;
		bool updateInProgress;
		bool lastCmdError;
		QString cmdFilePath;

		void updateSettingsFromObject();
		void updateLabels();	
		void updateCmdsTable();
		void highlightNextCommand();
		void resetProcessor();

	public slots:
		void changeTab(DialogTab tab);
		void writeLog(const QString& string);
		void queryChart();
		void querySvg(const QString& filePath);
		void queryInfo();

	signals:
		void eventMessage(const QString& string);
		void sendChart(QChartView* chartView);
		void sendSvg(const QString& filePath);
		void redraw();

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
		void on_cmdOperation_currentIndexChanged(const QString &string);
		void on_addCmd_clicked();
		void on_autoSave_clicked();
		void on_stepsExecSpeedSpinBox_valueChanged(double value);
		void on_drawingTool_currentIndexChanged(const QString &str);
		void on_stepsExecSpeedSlider_sliderMoved(int position);
		void on_saveCmds_clicked();
		void on_loadCmds_clicked();
		void saveCmdsToFile();
		void on_execAll_clicked();
};

#endif // DIALOG_SETTINGS_H
