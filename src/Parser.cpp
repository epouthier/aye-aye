
#include "Parser.h"

namespace ayeaye
{
    Parser::Parser(Parameters &parameters, Language &language) throw(ParserException)
    {
        //variable
        path sourceFilePath;

        //vérification de l'existence des sources
        for (unsigned int i = 0; i < parameters.getSources().size(); i++)
        {
            sourceFilePath = parameters.getSources()[i];

            if (!exists(sourceFilePath))
	    	{
                throw ParserException(tr("Le fichier source \"%0\" n'existe pas.", sourceFilePath.native()));
       		}
        }
    }
}

