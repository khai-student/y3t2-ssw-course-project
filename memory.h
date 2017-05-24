#ifndef MEMORY_H
#define MEMORY_H

#include <QtCore/qglobal.h>
#include <QString>
#include <QPair>
#include <QVector>
#include <QSet>
#include <QFile>
#include <QObject>
#include <QTextStream>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QHorizontalPercentBarSeries>

QT_CHARTS_USE_NAMESPACE

#include "common.h"
#include "memory_settings.h"
#include "block.h"

enum DrawUtility
{
	dot,
	neato,
	fdp,
	sfdp,
	twopi,
	circo
};

class Memory
{
	public:
		typedef QPair<Block*, Block*> pair_t;
		typedef QVector<pair_t*> level_t;
		typedef QVector<level_t*> tree_t;
		~Memory();
		Memory(MemorySettings* settings);

		QResultStatus allocate(const uint64_t bytes, const QString& procName);
		QResultStatus free(const QString& procName);
		uint8_t query(const QString& procName);
		QResultStatus toSvg(const QString& pathToFile, const DrawUtility& algo);
		QChartView* toChart();

	private:
		MemorySettings* settings;
		tree_t* blocks;
		pair_t* rootPair;

		Memory();
		Block* splitUntilDegree(const uint8_t degree);
		QResultStatus free(Block* block);
		QResultStatus memToDot(QString* result);
		QResultStatus dotToSvg(const QString& pathToFile, const DrawUtility& util);
};

#endif // MEMORY_H
