
#ifndef _AYEAYE_PARSER_EXCEPTION_H
#define _AYEAYE_PARSER_EXCEPTION_H

    #include <iostream>
    #include <string>

	#include "Exception.h"

    using namespace std;

	namespace ayeaye
	{
		class ParserException : public Exception
		{
		public:
			/* Constructeur */
			ParserException(const string &message) : Exception(message) {}
		};
	}

#endif

