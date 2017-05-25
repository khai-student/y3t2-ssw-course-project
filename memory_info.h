#ifndef MEMORY_INFO_H
#define MEMORY_INFO_H

#include "QString"

class MemoryInfo
{
	public:
		static QString usedMemory;
		static uint64_t blocksQuantity;
		static QString smallestBlockSize;
		static double usedPercent;
};

#endif // MEMORY_INFO_H
