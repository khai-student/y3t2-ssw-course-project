#ifndef MEMORY_SETTINGS_H
#define MEMORY_SETTINGS_H

#include <QtCore/qglobal.h>
#include <QString>
#include "common.h"
#include "memory_info.h"

enum DrawUtility
{
	dot,
	neato,
	fdp,
	sfdp,
	twopi,
	circo
};

class MemorySettings
{
	public:
		const uint8_t MAX_TOTAL_MEMORY_DEGREE = 20;
		const double MAX_STEPS_EXECUTION_SPEED = 5.0;

		MemorySettings();
		static uint64_t degreeToBytes(uint8_t degree);
		static QString degreeToString(uint8_t degree);
		static QString bytesToString(uint64_t bytes);

		QResultStatus setMinBlockDegree(uint8_t degree);
		QResultStatus setTotalMemoryDegree(uint8_t degree);
		QResultStatus setStepsExecutionSpeed(double speed);
		void setAutoSaveCmds(bool value);
		void setDrawUtility(DrawUtility value);

		uint8_t getMinBlockDegree();
		uint8_t getTotalMemoryDegree();
		double getStepsExecutionSpeed();
		bool getAutoSaveCmds();
		DrawUtility getDrawUtility();

	private:
		uint8_t minBlockDegree;
		uint8_t totalMemoryDegree;
		double stepsExecutionSpeed;
		bool autoSaveCmds;
		DrawUtility drawUtility;
};

#endif // MEMORY_SETTINGS_H
