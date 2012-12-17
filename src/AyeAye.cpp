
/*
 * Copyright (C) 2013 Emmanuel Pouthier
 *
 * This file is part of Aye-Aye.
 *
 * Aye-Aye is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Aye-Aye is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
        Source source(parameters, language);

        //analyse sémantique
        

        //génération de code
	}
}

