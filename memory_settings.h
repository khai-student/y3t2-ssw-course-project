#ifndef MEMORY_SETTINGS_H
#define MEMORY_SETTINGS_H

#include <QtCore/qglobal.h>
#include <QString>
#include "common.h"

class MemorySettings
{
	public:
		const uint8_t MAX_TOTAL_MEMORY_DEGREE = 20;
		const double MAX_STEPS_EXECUTION_SPEED = 5;

		MemorySettings();
		static uint64_t DegreeToBytes(uint8_t degree);
		static QString DegreeToString(uint8_t degree);

		QResultStatus SetMinBlockDegree(uint8_t degree);
		QResultStatus SetTotalMemoryDegree(uint8_t degree);
		QResultStatus SetStepsExecutionSpeed(double speed);
		void SetAutoSaveCmds(bool value);

		uint8_t GetMinBlockDegree();
		uint8_t GetTotalMemoryDegree();
		uint8_t GetStepsExecutionSpeed();
		bool GetAutoSaveCmds();

	private:
		uint8_t minBlockDegree;
		uint8_t totalMemoryDegree;
		double stepsExecutionSpeed;
		bool autoSaveCmds;
};

#endif // MEMORY_SETTINGS_H
