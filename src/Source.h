
#ifndef _AYEAYE_SOURCE_H
#define _AYEAYE_SOURCE_H

    #include <iostream>
	#include <fstream>
    #include <string>

	#include <boost/filesystem.hpp>

    #include "Language.h"
	#include "LanguageStructure.h"
	#include "Localization.h"
	#include "Parameters.h"
	#include "SourceException.h"

    using namespace std;
	using namespace boost::filesystem;

    namespace ayeaye
    {
        class Source
        {
		private:
			/* Attributs */
			ifstream _sourceFile;

        public:
            /* Constructeur */
            Source(Parameters &parameters, Language &language) throw(SourceException);
        };
    }

#endif

