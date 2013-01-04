
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

#include "Parameters.h"

namespace ayeaye
{
    Parameters::Parameters(int argc, char **argv) throw(ParametersException) :
        _sourceDirectory(""),
        _languageDirectory("")
    {
        //variable
        bool withoutParams = true;

        //analyse des paramètres
        try
        {
            //les sources
            options_description sources ("Sources");
            sources.add_options()
                ("sources", value<vector<string>>(), "Sources");


            //les options
            options_description options("Options");
            options.add_options()
                ("help,h", "")
                ("version,v", "")
                ("source-directory,S", value<string>(), "")
                ("language-directory,L", value<string>(), "");


            //configuration des paramètres
            options_description params;
            params.add(sources);
            params.add(options);

            positional_options_description positional;
            positional.add("sources", -1);


            //récupération des paramètres
            variables_map variablesMap;
            store(command_line_parser(argc, argv).options(params).positional(positional).run(), variablesMap);
            notify(variablesMap);


            //analyse des paramètres
            if (variablesMap.count("help")) //--help ou -h
            {
                _showHelp();
                exit(0);
            }
            else if (variablesMap.count("version")) //--version ou -v
            {
                _showVersion();
                exit(0);
            }
            else
            {
                if (variablesMap.count("source-directory")) //--source-directory=<directory> ou -S<directory>
                {
                    _sourceDirectory = variablesMap["source-directory"].as<string>();
                    withoutParams = false;
                }
                
                if (variablesMap.count("sources")) //SOURCES
                {
                    _sources = variablesMap["sources"].as<vector<string>>();
                    withoutParams = false;
                }

                if (variablesMap.count("language-directory")) //--language-directory=<directory> ou -L<directory>
                {
                    _languageDirectory = variablesMap["language-directory"].as<string>();
                    withoutParams = false;
                }
                else
                {
                    _languageDirectory = AYEAYE_LANGUAGE_DIRECTORY;
                }

                if (withoutParams)
                {
                    _showHelp();
                    exit(0);
                }
            }
        }
        catch (error &e) //traitement des exceptions
        {
            throw ParametersException(e.what());
        }
    }

    void Parameters::_showHelp()
    {
        cout << tr("Utilisation : ayeaye [OPTIONS] SOURCES") << endl
             << tr("Options :") << endl
             << " -h, --help                                        " << tr("Affiche l'aide") << endl
             << " -v, --version                                     " << tr("Affiche le numéro de version") << endl
             << " -S<directory>, --source-directory=<directory>     " << tr("Définie le répertoire des sources") << endl
             << " -L<directory>, --language-directory=<directory>   " << tr("Définie le répertoire des langages") << endl;
    }

    void Parameters::_showVersion()
    {
        cout << "Aye-Aye version " << AYEAYE_VERSION << endl
             << "Copyright (C) 2013 Emmanuel Pouthier" << endl
             << endl
             << "Aye-Aye is free software: you can redistribute it and/or modify" << endl
             << "it under the terms of the GNU General Public License as published by" << endl
             << "the Free Software Foundation, either version 3 of the License, or" << endl
             << "(at your option) any later version." << endl
             << endl
             << "Aye-Aye is distributed in the hope that it will be useful," << endl
             << "but WITHOUT ANY WARRANTY; without even the implied warranty of" << endl
             << "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the" << endl
             << "GNU General Public License for more details." << endl
             << endl
             << "You should have received a copy of the GNU General Public License" << endl
             << "along with this program.  If not, see <http://www.gnu.org/licenses/>." << endl;
    }
}

