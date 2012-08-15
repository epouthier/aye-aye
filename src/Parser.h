
#ifndef _AYEAYE_PARSER_H
#define _AYEAYE_PARSER_H

    #include <iostream>
	#include <fstream>
    #include <string>

	#include <boost/filesystem.hpp>

    #include "Language.h"
    #include "ParserException.h"
	#include "Parameters.h"

    using namespace std;
	using namespace boost::filesystem;

    namespace ayeaye
    {
        class Parser
        {
        public:
            /* Constructeur */
            Parser(Parameters &parameters, Language &language) throw(ParserException);
        };
    }

#endif

