
#ifndef _AYEAYE_AYEAYE_H
#define _AYEAYE_AYEAYE_H

	#include "Language.h"
	#include "Parameters.h"
    #include "Parser.h"

	namespace ayeaye
	{
		class AyeAye
		{
		public:
			/* Méthode statique */
			static void run(int argc, char **argv) throw(Exception);
		};
	}

#endif

