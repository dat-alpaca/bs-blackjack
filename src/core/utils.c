#include "utils.h"

void swap(int* lhs, int* rhs)
{
	int temp = *lhs;
	*lhs = *rhs;
	*rhs = temp;
}