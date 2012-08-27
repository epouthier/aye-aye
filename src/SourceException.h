
#ifndef _AYEAYE_SOURCE_EXCEPTION_H
#define _AYEAYE_SOURCE_EXCEPTION_H

    #include <iostream>
    #include <string>

	#include "Exception.h"

    using namespace std;

	namespace ayeaye
	{
		class SourceException : public Exception
		{
		public:
			/* Constructeur */
			SourceException(const string &message) : Exception(message) {}
		};
	}

#endif

