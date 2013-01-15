
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

#ifndef _AYEAYE_LANGUAGE_H
#define _AYEAYE_LANGUAGE_H

    #include <iostream>
    #include <string>

    #include <archive.h>
    #include <archive_entry.h>

    #include <boost/filesystem.hpp>

    #include "Exception.h"
    #include "FileBuffer.h"
    #include "LanguageException.h"
    #include "LanguageMetadata.h"
    #include "LanguageMetadataParser.h"
    #include "LanguageStructure.h"
    #include "LanguageStructureParser.h"
    #include "Localization.h"
    #include "Parameters.h"

    using namespace std;
    using namespace boost::filesystem;

    namespace ayeaye
    {
        class Language
        {
        private:
            /* Attributs */
            static LanguageMetadataParser _languageMetadataParser;
            static LanguageStructureParser _languageStructureParser;
            path _languageFilePath;
            string _languageIdentifier = "";
            LMRules _languageMetadata;
            LSRules _languageStructure;
            bool _languageStructureLoaded = false;

        public:
            /* Constructeur */
            Language(Parameters &parameters, const string &languageIdentifier) throw(Exception, LanguageException);
            Language(const path &languageFilePath) throw(Exception, LanguageException);

            /* Méthode publique */
            LSRules &getLanguageStructure() throw(Exception, LanguageException);

            /* Getters */
            string &getLanguageIdentifier() {return _languageIdentifier;}
            LMRules &getLanguageMetadata() {return _languageMetadata;}

        private:
            /* Méthodes privées */
            FileBuffer *_extractFilePath(const string &extractFilePath) throw(Exception, LanguageException);
            void _loadStructure() throw(Exception, LanguageException);

        };
    }

#endif

