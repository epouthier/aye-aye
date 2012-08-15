
#include "AyeAye.h"

namespace ayeaye
{
	void AyeAye::run(int argc, char **argv) throw(Exception)
	{
		//analyse de paramètres d'éxécutions
		Parameters parameters(argc, argv);
        if (parameters.getSources().size() == 0)
            return;

		//chargement du language
		Language language(parameters);

        //parsage des sources
        Parser parser(parameters, language);

        //analyse sémantique
        

        //génération de code
	}
}

