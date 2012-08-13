
#ifndef _AYEAYE_LANGUAGE_EXCEPTION_H
#define _AYEAYE_LANGUAGE_EXCEPTION_H

    #include <iostream>
    #include <string>

	#include "Exception.h"

	using namespace std;

	namespace ayeaye
	{
		class LanguageException : public Exception
		{
		public:
			/* Constructeur */
			LanguageException(string type, string message) : Exception(type + " : " + message) {}
			LanguageException(string message) : Exception(message) {}
		};
	}

#endif

