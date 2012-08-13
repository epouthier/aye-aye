
#ifndef _AYEAYE_LANGUAGE_EXCEPTION_H
#define _AYEAYE_LANGUAGE_EXCEPTION_H

    #include <iostream>
	#include <sstream>
    #include <string>

	#include "Exception.h"

	using namespace std;

	namespace ayeaye
	{
		class LanguageException : public Exception
		{
		public:
			/* Constructeur */
			LanguageException(const string &language, const int line, const string &message);
			LanguageException(const string &message);
		};
	}

#endif

