#include "stdafx.h"
#include "Lock.h"

skylark::Lock::Lock(Order order)
	:mOrder(order)
{
}

bool skylark::Lock::operator<(const Lock & rhs)
{
	return mOrder < rhs.mOrder;
}

bool skylark::Lock::operator>(const Lock & rhs)
{
	return mOrder > rhs.mOrder;
}
