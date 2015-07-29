#pragma once

namespace skylark
{
	class CircularBuffer
	{
	public:
		CircularBuffer(std::size_t capacity_) :
			regionBPointer(nullptr),
			regionASize(0), regionBSize(0),
			capacity(capacity_)
		{
			buffer = new char[capacity];
			bufferEnd = buffer + capacity;
			regionAPointer = buffer;
		}

		~CircularBuffer()
		{
			delete[] buffer;
		}

		void bufferReset()
		{
			regionBPointer = nullptr;
			regionASize = 0;
			regionBSize = 0;

			memset(buffer, 0, capacity);

			bufferEnd = buffer + capacity;
			regionAPointer = buffer;
		}

		bool peek(OUT char* destbuf, std::size_t bytes) const;
		bool read(OUT char* destbuf, std::size_t bytes);
		bool write(const char* data, std::size_t bytes);

		void remove(std::size_t len);

		std::size_t getFreeSpaceSize()
		{
			if (regionBPointer != nullptr)
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
			return regionASize + regionBSize;
		}

		std::size_t getContiguiousBytes() const
		{
			if (regionASize > 0)
			{
				return regionASize;
			}
			else
			{
				return regionBSize;
			}
		}

		char* getBuffer() const
		{
			if (regionBPointer != nullptr)
			{
				return regionBPointer + regionBSize;
			}
			else
			{
				return regionAPointer + regionASize;
			}
		}

		void commit(std::size_t len)
		{
			if (regionBPointer != nullptr)
			{
				regionBSize += len;
			}
			else
			{
				regionASize += len;
			}
		}

		char* getBufferStart() const
		{
			if (regionASize > 0)
			{
				return regionAPointer;
			}
			else
			{
				return regionBPointer;
			}
		}

	private:
		void allocateB()
		{
			regionBPointer = buffer;
		}

		std::size_t getAFreeSpace() const
		{
			return (bufferEnd - regionAPointer - regionASize);
		}

		std::size_t getSpaceBeforeA() const
		{
			return (regionAPointer - buffer);
		}

		std::size_t getBFreeSpace() const
		{
			if (regionBPointer == nullptr)
				return 0;

			return (regionAPointer - regionBPointer - regionBSize);
		}

	private:
		char* buffer;
		char* bufferEnd;

		char* regionAPointer;
		std::size_t  regionASize;

		char* regionBPointer;
		std::size_t  regionBSize;

		std::size_t capacity;
	};
}