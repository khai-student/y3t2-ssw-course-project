#ifndef DIALOG_SETTINGS_H
#define DIALOG_SETTINGS_H

#include <QDialog>
#include <QDebug>
#include <QDateTime>
#include <QFile>

#include "memory_settings.h"

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

	protected:
		void changeEvent(QEvent *e);

	private:
		Ui::DialogSettings *ui;
		MemorySettings *memorySettings;

		void updateSettingsFromObject();
		void updateLabels();
		void printMessage(const QString& string);

	public slots:
		void changeTab(DialogTab tab);
		void writeLog(const QString& string);

	signals:
		void eventMessage(const QString& string);

	private slots:
		void on_minBlockDegreeSlider_sliderMoved(int position);
		void on_minBlockDegreeSpinBox_valueChanged(int value);
		void on_totalMemorySlider_sliderMoved(int position);
		void on_totalMemorySpinBox_valueChanged(int value);
		void on_restoreDefaultSettings_clicked();
		void on_clearLog_clicked();
		void on_exportLog_clicked();
};

#endif // DIALOG_SETTINGS_H
