
#include "runtime.h"

int main()
{
	bool appShouldClose = false;

	while (!appShouldClose)
	{
		Runtime r(&appShouldClose);
	}


}