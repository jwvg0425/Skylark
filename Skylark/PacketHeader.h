#pragma once


namespace skylark
{

template<typename T>
struct PacketHeader
{
	static_assert(std::is_enum<T>::value, "T must be a enum type.");

	T mType;
};

}