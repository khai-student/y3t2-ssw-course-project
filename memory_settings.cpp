#include "memory_settings.h"

MemorySettings::MemorySettings()
{
	totalMemoryDegree = MAX_TOTAL_MEMORY_DEGREE;
	minBlockDegree = 1;
	stepsExecutionSpeed = 1.0;
	autoSaveCmds = true;
}

uint64_t MemorySettings::degreeToBytes(uint8_t degree)
{
	uint64_t bytes = 1;
	for (bytes = 1; degree > 0; --degree)
		bytes <<= 1;
	return bytes;
}

QResultStatus MemorySettings::setMinBlockDegree(uint8_t degree)
{
	QResultStatus resultStatus = QResult_Success;
	if (degree <= totalMemoryDegree)
	{
		minBlockDegree = degree;
	}
	else
	{
		resultStatus = QResult_DataOutOfRange;
	}
	return resultStatus;
}
QResultStatus MemorySettings::setTotalMemoryDegree(uint8_t degree)
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

QResultStatus MemorySettings::setStepsExecutionSpeed(double speed)
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

void MemorySettings::setAutoSaveCmds(bool value)
{
	autoSaveCmds = value;
}

void MemorySettings::setDrawUtility(DrawUtility value)
{
	drawUtility = value;
}

uint8_t MemorySettings::getMinBlockDegree()
{
	return minBlockDegree;
}

uint8_t MemorySettings::getTotalMemoryDegree()
{
	return totalMemoryDegree;
}

double MemorySettings::getStepsExecutionSpeed()
{
	return stepsExecutionSpeed;
}

bool MemorySettings::getAutoSaveCmds()
{
	return autoSaveCmds;
}

DrawUtility MemorySettings::getDrawUtility()
{
	return drawUtility;
}

QString MemorySettings::degreeToString(uint8_t degree)
{
	uint8_t divider = 0;
	uint16_t bytes = 1;
	QString result = QString("");

	for (divider = 0; degree >= 10; degree -= 10 , ++divider);

	bytes = (uint16_t)degreeToBytes(degree);

	switch (divider) {
		case 0:
			result = QString("%1B").arg(bytes);
			break;
		case 1:
			result = QString("%1KB").arg(bytes);
			break;
		case 2:
			result = QString("%1MB").arg(bytes);
			break;
		case 3:
			result = QString("%1GB").arg(bytes);
			break;
		case 4:
			result = QString("%1TB").arg(bytes);
			break;
		case 5:
			result = QString("%1PB").arg(bytes);
			break;
		default:
			break;
	}

	return result;
}

QString MemorySettings::bytesToString(uint64_t bytes)
{
	uint8_t divider = 0;
	QString result = QString("");
	double value = bytes;

	for (divider = 0; value >= 1024; value /= 1024 , ++divider);

	switch (divider) {
		case 0:
			result = QString("%1B").arg(QString::number(value));
			break;
		case 1:
			result = QString("%1KB").arg(QString::number(value));
			break;
		case 2:
			result = QString("%1MB").arg(QString::number(value));
			break;
		case 3:
			result = QString("%1GB").arg(QString::number(value));
			break;
		case 4:
			result = QString("%1TB").arg(QString::number(value));
			break;
		case 5:
			result = QString("%1PB").arg(QString::number(value));
			break;
		default:
			break;
	}

	return result;
}
