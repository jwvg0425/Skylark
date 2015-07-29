#include "stdafx.h"
#include "CircularBuffer.h"

bool skylark::CircularBuffer::peek(OUT char * destbuf, std::size_t bytes) const
{
	assert(buffer != nullptr);

	if (regionASize + regionBSize < bytes)
	{
		return false;
	}

	std::size_t cnt = bytes;
	std::size_t readA = 0;

	if (regionASize > 0)
	{
		readA = (cnt > regionASize) ? regionASize : cnt;
		memcpy(destbuf, regionAPointer, readA);
		cnt -= readA;
	}

	if (cnt > 0 && regionBSize > 0)
	{
		assert(cnt <= regionBSize);

		std::size_t readB = cnt;

		memcpy(destbuf + readA, regionBPointer, readB);
		cnt -= readB;
	}

	assert(cnt == 0);

	return true;
}

bool skylark::CircularBuffer::read(OUT char * destbuf, std::size_t bytes)
{
	assert(buffer != nullptr);

	if (regionASize + regionBSize < bytes)
		return false;

	std::size_t cnt = bytes;
	std::size_t readA = 0;

	if (regionASize > 0)
	{
		readA = (cnt > regionASize) ? regionASize : cnt;
		memcpy(destbuf, regionAPointer, readA);
		regionASize -= readA;
		regionAPointer += readA;
		cnt -= readA;
	}

	if (cnt > 0 && regionBSize > 0)
	{
		assert(cnt <= regionBSize);

		std::size_t readB = cnt;

		memcpy(destbuf + readA, regionBPointer, readB);
		regionBSize -= readB;
		regionBPointer += readB;
		cnt -= readB;
	}

	assert(cnt == 0);

	if (regionASize == 0)
	{
		if (regionBSize > 0)
		{
			if (regionBPointer != buffer)
			{
				memmove(buffer, regionBPointer, regionBSize);
			}

			regionAPointer = buffer;
			regionASize = regionBSize;
			regionBPointer = nullptr;
			regionBSize = 0;
		}
		else
		{
			regionBPointer = nullptr;
			regionBSize = 0;
			regionAPointer = buffer;
			regionASize = 0;
		}
	}

	return true;
}

bool skylark::CircularBuffer::write(const char * data, std::size_t bytes)
{
	assert(buffer != nullptr);

	if (regionBPointer != nullptr)
	{
		if (getBFreeSpace() < bytes)
			return false;

		memcpy(regionBPointer + regionBSize, data, bytes);
		regionBSize += bytes;

		return true;
	}

	if (getAFreeSpace() < getSpaceBeforeA())
	{
		allocateB();

		if (getBFreeSpace() < bytes)
			return false;

		memcpy(regionBPointer + regionBSize, data, bytes);
		regionBSize += bytes;

		return true;
	}
	else
	{
		if (getAFreeSpace() < bytes)
			return false;

		memcpy(regionAPointer + regionASize, data, bytes);
		regionASize += bytes;

		return true;
	}
}

void skylark::CircularBuffer::remove(std::size_t len)
{
	std::size_t cnt = len;

	if (regionASize > 0)
	{
		std::size_t removeA = (cnt > regionASize) ? regionASize : cnt;

		regionASize -= removeA;
		regionAPointer += removeA;

		cnt -= removeA;
	}

	if (cnt > 0 && regionBSize > 0)
	{
		std::size_t removeB = (cnt > regionBSize) ? regionBSize : cnt;
		
		regionBSize -= removeB;
		regionBPointer += removeB;
		
		cnt -= removeB;
	}

	if (regionASize == 0)
	{
		if (regionBSize > 0)
		{
			if (regionBPointer != buffer)
				memmove(buffer, regionBPointer, regionBSize);

			regionAPointer = buffer;
			regionASize = regionBSize;
			regionBPointer = nullptr;
			regionBSize = 0;
		}
		else
		{
			regionBPointer = nullptr;
			regionBSize = 0;
			regionAPointer = buffer;
			regionASize = 0;
		}
	}
}
