
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

#ifndef _AYEAYE_AYEAYE_H
#define _AYEAYE_AYEAYE_H

    #include "Exception.h"
    #include "LanguagePool.h"
    #include "Parameters.h"

	namespace ayeaye
	{
		class AyeAye
		{
        private:
            /* Attributs */
            LanguagePool *_languagePool = nullptr;

		public:
            /* Constructeur */
            AyeAye();

			/* Méthode statique */
			void run(int argc, char **argv) throw(Exception);
		};
	}

#endif

