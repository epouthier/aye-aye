
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

#ifndef _AYEAYE_PARAMETERS_H
#define _AYEAYE_PARAMETERS_H

    #include <iostream>
    #include <string>
    #include <vector>

    #include <getopt.h>

    #include <boost/filesystem.hpp>

    #include "Localization.h"
    #include "ParametersException.h"

    using namespace std;
    using namespace boost::filesystem;

    namespace ayeaye
    {
        class Parameters
        {
        private:
            /* Attributs */
            vector<string> _sourceDirectories;
            string _sourceLanguage = "";
            vector<string> _sources;
            vector<string> _languageDirectories;


        public:
            /* Constructeur */
            Parameters(int argc, char **argv) throw(ParametersException);


            /* Getters */
            vector<string> &getSourceDirectories() {return _sourceDirectories;}
            string &getSourceLanguage() {return _sourceLanguage;}
            vector<string> &getSources() {return _sources;}
            vector<string> &getLanguageDirectories() {return _languageDirectories;}


        private:
            /* Méthodes privées */
            void _showHelp();
            void _showVersion();

        };
    }

#endif

