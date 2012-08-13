
#ifndef _AYEAYE_PARAMETERS_EXCEPTION_H
#define _AYEAYE_PARAMETERS_EXCEPTION_H

    #include <iostream>
    #include <string>

	#include "Exception.h"

    using namespace std;

	namespace ayeaye
	{
		class ParametersException : public Exception
		{
		public:
			/* Constructeur */
			ParametersException(string message) : Exception(message) {}
		};
	}

#endif

