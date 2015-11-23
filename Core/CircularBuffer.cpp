#include "stdafx.h"
#include "CircularBuffer.h"
#include "Exception.h"

bool skylark::CircularBuffer::peek(OUT char * destbuf, std::size_t bytes) const
{
	crashAssert(mBuffer != nullptr);

	if (mRegionASize + mRegionBSize < bytes)
	{
		return false;
	}

	std::size_t cnt = bytes;
	std::size_t readA = 0;

	if (mRegionASize > 0)
	{
		readA = (cnt > mRegionASize) ? mRegionASize : cnt;
		memcpy(destbuf, mRegionAPointer, readA);
		cnt -= readA;
	}

	if (cnt > 0 && mRegionBSize > 0)
	{
		crashAssert(cnt <= mRegionBSize);

		std::size_t readB = cnt;

		memcpy(destbuf + readA, mRegionBPointer, readB);
		cnt -= readB;
	}

	crashAssert(cnt == 0);

	return true;
}

bool skylark::CircularBuffer::read(OUT char * destbuf, std::size_t bytes)
{
	crashAssert(mBuffer != nullptr);

	if (mRegionASize + mRegionBSize < bytes)
		return false;

	std::size_t cnt = bytes;
	std::size_t readA = 0;

	if (mRegionASize > 0)
	{
		readA = (cnt > mRegionASize) ? mRegionASize : cnt;
		memcpy(destbuf, mRegionAPointer, readA);
		mRegionASize -= readA;
		mRegionAPointer += readA;
		cnt -= readA;
	}

	if (cnt > 0 && mRegionBSize > 0)
	{
		crashAssert(cnt <= mRegionBSize);

		std::size_t readB = cnt;

		memcpy(destbuf + readA, mRegionBPointer, readB);
		mRegionBSize -= readB;
		mRegionBPointer += readB;
		cnt -= readB;
	}

	crashAssert(cnt == 0);

	if (mRegionASize == 0)
	{
		if (mRegionBSize > 0)
		{
			if (mRegionBPointer != mBuffer)
			{
				memmove(mBuffer, mRegionBPointer, mRegionBSize);
			}

			mRegionAPointer = mBuffer;
			mRegionASize = mRegionBSize;
			mRegionBPointer = nullptr;
			mRegionBSize = 0;
		}
		else
		{
			mRegionBPointer = nullptr;
			mRegionBSize = 0;
			mRegionAPointer = mBuffer;
			mRegionASize = 0;
		}
	}

	return true;
}

bool skylark::CircularBuffer::write(const char * data, std::size_t bytes)
{
	crashAssert(mBuffer != nullptr);

	if (mRegionBPointer != nullptr)
	{
		if (getBFreeSpace() < bytes)
			return false;

		memcpy(mRegionBPointer + mRegionBSize, data, bytes);
		mRegionBSize += bytes;

		return true;
	}

	if (getAFreeSpace() < getSpaceBeforeA())
	{
		allocateB();

		if (getBFreeSpace() < bytes)
			return false;

		memcpy(mRegionBPointer + mRegionBSize, data, bytes);
		mRegionBSize += bytes;

		return true;
	}
	else
	{
		if (getAFreeSpace() < bytes)
			return false;

		memcpy(mRegionAPointer + mRegionASize, data, bytes);
		mRegionASize += bytes;

		return true;
	}
}

void skylark::CircularBuffer::remove(std::size_t len)
{
	std::size_t cnt = len;

	if (mRegionASize > 0)
	{
		std::size_t removeA = (cnt > mRegionASize) ? mRegionASize : cnt;

		mRegionASize -= removeA;
		mRegionAPointer += removeA;

		cnt -= removeA;
	}

	if (cnt > 0 && mRegionBSize > 0)
	{
		std::size_t removeB = (cnt > mRegionBSize) ? mRegionBSize : cnt;

		mRegionBSize -= removeB;
		mRegionBPointer += removeB;

		cnt -= removeB;
	}

	if (mRegionASize == 0)
	{
		if (mRegionBSize > 0)
		{
			if (mRegionBPointer != mBuffer)
				memmove(mBuffer, mRegionBPointer, mRegionBSize);

			mRegionAPointer = mBuffer;
			mRegionASize = mRegionBSize;
			mRegionBPointer = nullptr;
			mRegionBSize = 0;
		}
		else
		{
			mRegionBPointer = nullptr;
			mRegionBSize = 0;
			mRegionAPointer = mBuffer;
			mRegionASize = 0;
		}
	}
}