
#include "LanguageException.h"

namespace ayeaye
{
	LanguageException::LanguageException(const string &language, const int line, const string &message)
	{
		//conversion int en string
		ostringstream oss;
    	oss << line;

		//construction du message
		_message = language + ":" + oss.str() + ": " + message;
	}

	LanguageException::LanguageException(const string &message) :
		Exception(message)
	{
	}
}

