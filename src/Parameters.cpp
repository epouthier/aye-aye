
/*
 * Copyright (C) 2012 Emmanuel Pouthier
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Parameters.h"

namespace ayeaye
{
	Parameters::Parameters(int argc, char **argv) throw(ParametersException) :
		_language(""),
		_languageDirectory(""),
		_sourceDirectory("")
	{
		try
		{
			//configuration des paramètres
			options_description genericOptions(tr("Options génériques"));
			genericOptions.add_options()
				("help", "")
				("version", tr("Affiche le numéro de version").c_str());

			options_description languageOptions(tr("Options pour les langages"));
			languageOptions.add_options()
				("language", value<string>(), tr("Pour indiquer le langage des fichiers sources").c_str())
				("language-dir", value<string>(), tr("Pour indiquer dans quelle répertoire où est le langage").c_str());

			options_description sourceOptions(tr("Options pour les sources"));
			sourceOptions.add_options()
				("source-dir", value<string>(), tr("Pour indiquer dans quelle répertoire où sont les sources").c_str());

			options_description sourceOptionsHidden("Options cachées pour les sources");
			sourceOptionsHidden.add_options()
				("sources", value<vector<string>>(), "Les fichiers sources");

			positional_options_description positionalOptions;
			positionalOptions.add("sources", -1);


			//récupération des paramètres
			options_description cmdLineOptions;
			cmdLineOptions.add(genericOptions);
			cmdLineOptions.add(sourceOptions).add(sourceOptionsHidden);
			cmdLineOptions.add(languageOptions);

			variables_map variablesMap;
			store(command_line_parser(argc, argv).options(cmdLineOptions).positional(positionalOptions).run(), variablesMap);
			notify(variablesMap);


			//analyse des paramètres
			if (variablesMap.count("help")) //--help
			{
				options_description helpOptions;
				helpOptions.add(genericOptions);
				helpOptions.add(sourceOptions);
				helpOptions.add(languageOptions);

				cout << tr("Utilisation: ayeaye [OPTIONS] SOURCES") << endl
					 << helpOptions << endl;
			}
			else if (variablesMap.count("version")) //--version
			{
				cout << "Aye-Aye" << endl
					 << "Version : 0.1" << endl;
			}
			else
			{
				if (variablesMap.count("language")) //--language=
				{
					_language = variablesMap["language"].as<string>();
				}

				if (variablesMap.count("language-dir")) //--language-dir=
				{
					_languageDirectory = variablesMap["language-dir"].as<string>();
				}

				if (variablesMap.count("source-dir")) //--source-dir=
				{
					_sourceDirectory = variablesMap["source-dir"].as<string>();
				}

				if (variablesMap.count("sources")) //SOURCES
				{
					_sources = variablesMap["sources"].as<vector<string>>();
				}
				else
				{
					throw ParametersException(tr("Il n'y a pas de fichier source spécifié."));
				}
			}
		}
		catch (error &e) //traitement des exceptions
		{
			throw ParametersException(e.what());
		}
	}
}

