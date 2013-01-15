
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
    AyeAye::AyeAye() :
        _sourceLanguage(nullptr),
        _languagePool(nullptr)
    {
    }

	void AyeAye::run(int argc, char **argv) throw(Exception, LanguageException, SourceException)
	{
		//analyse de paramètres d'éxécutions
		Parameters parameters(argc, argv);

        //chargement des langages
        if (parameters.getSourceLanguage() != "")
        {
            _sourceLanguage = new Language(parameters, parameters.getSourceLanguage());
        }
        else
        {
            //création d'une pool à langage
            _languagePool = new LanguagePool(parameters);
        }

        //analyse des sources
        for (unsigned int i = 0; i < parameters.getSourceDirectories().size(); i++) //scan des répertoires des sources et parsage
        {
            _scanSourceDirectory(parameters.getSourceDirectories()[i]);
        }

        for (unsigned int i = 0; i < parameters.getSources().size(); i++) //parsage des sources
        {
            _parseSource(parameters.getSources()[i]);
        }
	}

    void AyeAye::_scanSourceDirectory(const path &sourceDirectory) throw(Exception, LanguageException, SourceException)
    {
        //variables
        directory_iterator itrSourceDirectory;
        directory_iterator endItr;
        path sourceFilePath;

        //scan du répertoire des sources
        if (exists(sourceDirectory) && is_directory(sourceDirectory))
        {
            for (itrSourceDirectory = directory_iterator(sourceDirectory); itrSourceDirectory != endItr; itrSourceDirectory++)
            {
                //construction du chemin
                sourceFilePath = itrSourceDirectory->path();

                //parse source
                _parseSource(sourceFilePath);
            }
        }
        else
        {
            throw Exception(tr("Le répertoire de sources \"%0\" n'existe pas.", sourceDirectory.native()));
        }
    }

    void AyeAye::_parseSource(const path &sourceFilePath) throw(LanguageException, SourceException)
    {
        //variable
        Source *source = nullptr;

        //parsage source
        if (_sourceLanguage != nullptr)
        {
            source = new Source(_sourceLanguage, sourceFilePath);
        }
        else
        {
            source = new Source(_languagePool, sourceFilePath);
        }
    }
}

