
#ifndef _AYEAYE_PARAMETERS_H
#define _AYEAYE_PARAMETERS_H

	#include <iostream>
	#include <string>
	#include <vector>

	#include <boost/program_options.hpp>

    #include "Localization.h"
	#include "ParametersException.h"

	using namespace std;
	using namespace boost::program_options;

	namespace ayeaye
	{
		class Parameters
		{
		private:
			/* Attributs */
			string _language = "";
			string _languageDirectory = "";
			string _sourceDirectory = "";
			vector<string> _sources;


		public:
			/* Constructeur */
			Parameters(int argc, char **argv) throw(ParametersException);


			/* Getters */
			string &getLanguage() {return _language;}
			string &getLanguageDirectory() {return _languageDirectory;}
			string &getSourceDirectory() {return _sourceDirectory;}
            vector<string> &getSources() {return _sources;}
		};
	}

#endif

