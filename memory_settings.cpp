#include "memory_settings.h"

MemorySettings::MemorySettings()
{
	totalMemoryDegree = MAX_TOTAL_MEMORY_DEGREE;
	minBlockDegree = 1;
	stepsExecutionSpeed = 1.0;
	autoSaveCmds = true;
}

uint64_t MemorySettings::DegreeToBytes(uint8_t& degree)
{
	uint64_t bytes = 1;
	for (bytes = 1; degree > 0; --degree)
		bytes <<= 1;
	return bytes;
}

QResultStatus MemorySettings::SetMinBlockDegree(uint8_t degree)
{
	QResultStatus resultStatus = QResult_Success;
	if (degree < totalMemoryDegree)
	{
		minBlockDegree = degree;
	}
	else
	{
		resultStatus = QResult_DataOutOfRange;
	}
	return resultStatus;
}

QResultStatus MemorySettings::SetTotalMemoryDegree(uint8_t degree)
{
	QResultStatus resultStatus = QResult_Success;
	if (degree <= MAX_TOTAL_MEMORY_DEGREE)
	{
		totalMemoryDegree = degree;
		if (minBlockDegree > totalMemoryDegree) minBlockDegree = 1;
	}
	else
	{
		resultStatus = QResult_DataOutOfRange;
	}
	return resultStatus;
}

QResultStatus MemorySettings::SetStepsExecutionSpeed(double speed)
{
	QResultStatus resultStatus = QResult_Success;
	if (speed > 0)
	{
		this->stepsExecutionSpeed = speed;
	}
	else
	{
		resultStatus = QResult_DataOutOfRange;
	}
	return resultStatus;
}

void MemorySettings::SetAutoSaveCmds(bool value)
{
	autoSaveCmds = value;
}

uint8_t MemorySettings::GetMinBlockDegree()
{
	return minBlockDegree;
}

uint8_t MemorySettings::GetTotalMemoryDegree()
{
	return totalMemoryDegree;
}

uint8_t MemorySettings::GetStepsExecutionSpeed()
{
	return stepsExecutionSpeed;
}

bool MemorySettings::GetAutoSaveCmds()
{
	return autoSaveCmds;
}

QString MemorySettings::DegreeToString(uint8_t& degree)
{
	uint8_t divider = 0;
	uint16_t bytes = 1;
	QString result = QString("");

	for (divider = 0; degree >= 10; degree -= 10 , ++divider);

	bytes = (uint16_t)DegreeToBytes(degree);

	switch (divider) {
		case 0:
			result = QString("%1 B").arg(bytes);
			break;
		case 1:
			result = QString("%1 KB").arg(bytes);
			break;
		case 2:
			result = QString("%1 MB").arg(bytes);
			break;
		case 3:
			result = QString("%1 GB").arg(bytes);
			break;
		case 4:
			result = QString("%1 TB").arg(bytes);
			break;
		case 5:
			result = QString("%1 PB").arg(bytes);
			break;
		default:
			break;
	}

	return result;
}
