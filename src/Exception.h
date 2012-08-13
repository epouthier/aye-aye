
#ifndef _AYEAYE_EXCEPTION_H
#define _AYEAYE_EXCEPTION_H

	#include <iostream>
	#include <stdexcept>
	#include <string>

	using namespace std;

	namespace ayeaye
	{
		class Exception : public std::exception
		{
		protected:
			/* Attributs */
			string _message = "";

		public:
			/* Constructeur et destructeur */
			Exception(string message) : _message(message) {}
			virtual ~Exception() throw() {}

			/* Méthode virtuelle */
			virtual const char *what() const throw() {return _message.c_str();}
		};
	}

#endif

