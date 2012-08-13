
#include <iostream>
#include <stdexcept>

#include "AyeAye.h"

using namespace std;
using namespace ayeaye;

int main(int argc, char **argv)
{
	try
	{
		AyeAye::run(argc, argv);
	}
	catch (const exception &e)
	{
		cerr << e.what() << endl;
		return (-1);
	}

	return (0);
}

