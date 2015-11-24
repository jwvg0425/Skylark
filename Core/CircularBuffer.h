#pragma once

namespace skylark
{
class CircularBuffer
{
public:
	CircularBuffer(std::size_t capacity) :
		mRegionBPointer(nullptr),
		mRegionASize(0), mRegionBSize(0),
		mCapacity(capacity)
	{
		mBuffer = new char[mCapacity];
		mBufferEnd = mBuffer + mCapacity;
		mRegionAPointer = mBuffer;
	}

	~CircularBuffer()
	{
		delete[] mBuffer;
	}

	void bufferReset()
	{
		mRegionBPointer = nullptr;
		mRegionASize = 0;
		mRegionBSize = 0;

		memset(mBuffer, 0, mCapacity);

		mBufferEnd = mBuffer + mCapacity;
		mRegionAPointer = mBuffer;
	}

	bool peek(OUT char* destbuf, std::size_t bytes) const;
	bool read(OUT char* destbuf, std::size_t bytes);
	bool write(const char* data, std::size_t bytes);

	void remove(std::size_t len);

	std::size_t getFreeSpaceSize()
	{
		if (mRegionBPointer != nullptr)
		{
			return getBFreeSpace();
		}
		else
		{
			if (getAFreeSpace() < getSpaceBeforeA())
			{
				allocateB();
				return getBFreeSpace();
			}
			else
			{
				return getAFreeSpace();
			}
		}
	}

	std::size_t getStoredSize() const
	{
		return mRegionASize + mRegionBSize;
	}

	std::size_t getContiguiousBytes() const
	{
		if (mRegionASize > 0)
		{
			return mRegionASize;
		}
		else
		{
			return mRegionBSize;
		}
	}

	char* getBuffer() const
	{
		if (mRegionBPointer != nullptr)
		{
			return mRegionBPointer + mRegionBSize;
		}
		else
		{
			return mRegionAPointer + mRegionASize;
		}
	}

	void commit(std::size_t len)
	{
		if (mRegionBPointer != nullptr)
		{
			mRegionBSize += len;
		}
		else
		{
			mRegionASize += len;
		}
	}

	char* getBufferStart() const
	{
		if (mRegionASize > 0)
		{
			return mRegionAPointer;
		}
		else
		{
			return mRegionBPointer;
		}
	}

private:
	void allocateB()
	{
		mRegionBPointer = mBuffer;
	}

	std::size_t getAFreeSpace() const
	{
		return (mBufferEnd - mRegionAPointer - mRegionASize);
	}

	std::size_t getSpaceBeforeA() const
	{
		return (mRegionAPointer - mBuffer);
	}

	std::size_t getBFreeSpace() const
	{
		if (mRegionBPointer == nullptr)
			return 0;

		return (mRegionAPointer - mRegionBPointer - mRegionBSize);
	}

private:
	char* mBuffer;
	char* mBufferEnd;

	char* mRegionAPointer;
	std::size_t  mRegionASize;

	char* mRegionBPointer;
	std::size_t  mRegionBSize;

	std::size_t mCapacity;
};
}