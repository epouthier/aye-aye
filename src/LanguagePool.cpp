
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

#include "LanguagePool.h"

namespace ayeaye
{
    LanguagePool::LanguagePool(Parameters &parameters) :
        _parameters(parameters)
    {
    }

    LanguagePool::~LanguagePool()
    {
        //variable
        map<string, Language*>::iterator itrLanguage;

        //déchargement des languages
        for (itrLanguage = _pool.begin(); itrLanguage != _pool.end(); itrLanguage++)
        {
            delete itrLanguage;
        }
    }

    Language* LanguagePool::getLanguage(const string &languageIdentifier) throw(Exception, LanguageException)
    {
        //variable
        path testPath, languageFilePath;
        unsigned int nbrLanguage = 0;
        string multiPathError = "";
        Language *language = nullptr;

        //si le langage est dans la pool
        if (_pool.find(languageIdentifier) != _pool.end())
        {
            return _pool[languageIdentifier];
        }
        else
        {
            //scan des répertoires de langages
            for (unsigned int i = 0; i < _parameters.getLanguageDirectories(); i++)
            {
                //construction du chemin
                testPath = AYEAYE_LANGUAGE_DIRECTORY;
                testPath /= languageIdentifier + ".ayeaye";

                //vérification de l'éxistence du langage
                if (exists(testPath))
                {
                    nbrLanguage++;

                    languageFilePath = testPath;
                    multiPathError += ((nbrLanguage == 1) ? (testPath.native()) : (", " + testPath.native()));
                }
            }

            //traitement des erreurs
            if (nbrLanguage == 0)
            {
                throw LanguageException(tr("Le langage \"%0\" n'existe pas dans le répertoire des langages.", languageIdentifier));
            }
            else if (nbrLanguage > 1)
            {
                throw LanguageException(tr("Le language \"%0\" existe dans plusieurs répertoire de language : \"%1\"", languageIdentifier, multiPathError));
            }

            //chargement du langage
            language = new Language(languageFilePath);
            if (language == nullptr)
            {
                throw LanguageException(tr("Le chargement du language \"%0\" a échoué", languageIdentifier));
            }

            //ajout du langage dans la pool
            _pool[languageIdentifier] = language;

            //on retourne le langage
            return language;
        }
    }
}

