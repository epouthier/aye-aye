
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

    #include <iostream>
    #include <string>
    #include <vector>

    #include <boost/filesystem.hpp>

    #include "Exception.h"
    #include "Language.h"
    #include "LanguageException.h"
    #include "LanguagePool.h"
    #include "Localization.h"
    #include "Parameters.h"
    #include "Source.h"
    #include "SourceException.h"

    using namespace std;
    using namespace boost::filesystem;

	namespace ayeaye
	{
		class AyeAye
		{
        private:
            /* Attributs */
            Language *_sourceLanguage = nullptr;
            LanguagePool *_languagePool = nullptr;

		public:
            /* Constructeur */
            AyeAye();

			/* Méthode publique */
			void run(int argc, char **argv) throw(Exception, LanguageException, SourceException);

        private:
            /* Méthodes privées */
            void _scanSourceDirectory(const path &sourceDirectory) throw(Exception, LanguageException, SourceException);
            void _parseSource(const path &sourceFilePath) throw(LanguageException, SourceException);
		};
	}

#endif

