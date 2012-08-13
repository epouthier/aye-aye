
#include "LanguageException.h"

namespace ayeaye
{
	LanguageException::LanguageException(string language, int line, string message)
	{
		//conversion int en string
		ostringstream oss;
    	oss << line;

		//construction du message
		_message = language + ":" + oss.str() + ": " + message;
	}

	LanguageException::LanguageException(string message) :
		Exception(message)
	{
	}
}

