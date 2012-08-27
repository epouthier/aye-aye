
#include "Source.h"

namespace ayeaye
{
    Source::Source(Parameters &parameters, Language &language) throw(SourceException)
    {
        //variable
        path sourceFilePath;

        //vérification de l'existence des sources
        for (unsigned int i = 0; i < parameters.getSources().size(); i++)
        {
            sourceFilePath = parameters.getSources()[i];

            if (!exists(sourceFilePath))
	    	{
                throw SourceException(tr("Le fichier source \"%0\" n'existe pas.", sourceFilePath.native()));
       		}
        }

		//parsage des sources
		for (unsigned int i = 0; i < parameters.getSources().size(); i++)
		{
			//récupération du chemin de la source
			sourceFilePath = parameters.getSources()[i];

			_sourceFile.open(sourceFilePath.c_str(), ios::in);

			if (_sourceFile.is_open())
			{
				_sourceFile.close();
			}
			else
			{
				throw SourceException(tr("Le fichier source \"%0\" est invalide.", sourceFilePath.native()));
			}
		}
    }
}

