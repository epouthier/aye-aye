
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
    LanguagePool::LanguagePool(Parameters &parameters) throw(Exception, LanguageException) :
        _parameters(parameters)
    {
        //scan du répertoires de langages principales
        _scanLanguageDirectory(AYEAYE_LANGUAGE_DIRECTORY);

        //scan des répertoires de langages
        for (unsigned int i = 0; i < _parameters.getLanguageDirectories().size(); i++)
        {
            _scanLanguageDirectory(path(_parameters.getLanguageDirectories()[i]));
        }
    }

    LanguagePool::~LanguagePool()
    {
        //variable
        map<string, Language*>::iterator itrLanguage;

        //déchargement des languages
        for (itrLanguage = _pool.begin(); itrLanguage != _pool.end(); itrLanguage++)
        {
            delete itrLanguage->second;
        }
    }

    Language *LanguagePool::getLanguageWithIdentifier(const string &languageIdentifier) throw(LanguageException)
    {
        if (_pool.find(languageIdentifier) != _pool.end())
        {
            return _pool[languageIdentifier];
        }
        else
        {
            throw LanguageException(tr("Le langage \"%0\" n'existe pas dans le répertoire des langages.", languageIdentifier));
        }

        return nullptr;
    }

    Language *LanguagePool::getLanguageWithExtension(const string &languageExtension) throw(LanguageException)
    {
        //variable
        map<string, Language*>::const_iterator itrLanguage;
        LMRuleDefinition::const_iterator itrExtension;
        int nbLanguage = 0;
        string languageSupport = "";
        Language* language = nullptr;

        //recherche du langage
        for (itrLanguage = _pool.begin(); itrLanguage != _pool.end(); itrLanguage++)
        {
            for (itrExtension = itrLanguage->second->getLanguageMetadata()["extension"].begin();
                 itrExtension != itrLanguage->second->getLanguageMetadata()["extension"].end();
                 itrExtension++)
            {
                if (languageExtension == (*itrExtension))
                {
                    nbLanguage++;
                    languageSupport += ((nbLanguage == 1) ? itrLanguage->first : (", " + itrLanguage->first));
                    language = itrLanguage->second;
                }
            }
        }

        //traitement des erreurs
        if (nbLanguage <= 0)
        {
            throw LanguageException(tr("Il n'y a aucun langage dans les répertoires des langages qui supporte l'extension \"%0\".", languageExtension));
        }
        else if (nbLanguage == 1)
        {
            return language;
        }
        else
        {
            //conversion int en string
            ostringstream oss;
            oss << nbLanguage;

            throw LanguageException(tr("Il y a %0 langages dans les répertoires des langages qui supporte l'extension \"%1\" : %2", oss.str(), languageExtension, languageSupport));
        }

        return nullptr;
    }

    void LanguagePool::_scanLanguageDirectory(const path &languageDirectory) throw(Exception, LanguageException)
    {
        //variables
        directory_iterator itrLanguageDirectory;
        directory_iterator endItr;
        path languageFilePath;
        string languageIdentifier = "";
        Language *language = nullptr;

        //scan du répertoire de langages
        if (exists(languageDirectory) && is_directory(languageDirectory))
        {
            for (itrLanguageDirectory = directory_iterator(languageDirectory); itrLanguageDirectory != endItr; itrLanguageDirectory++)
            {
                //si c'est un fichier .ayeaye
                if (itrLanguageDirectory->path().extension() == ".ayeaye")
                {
                    //construction du chemin
                    languageFilePath = itrLanguageDirectory->path();

                    //extraction de l'identifiant du langage
                    languageIdentifier = languageFilePath.filename().native();
                    languageIdentifier = languageIdentifier.substr(0, languageIdentifier.size() - 7);

                    //on le charge
                    language = nullptr;
                    language = new Language(languageFilePath);
                    if (language == nullptr)
                    {
                        throw LanguageException(tr("Le chargement du language \"%0\" a échoué", languageIdentifier));
                    }

                    //ajout du langage dans la pool
                    _pool[languageIdentifier] = language;
                }
            }
        }
        else
        {
            throw LanguageException(tr("Le répertoire de langage \"%0\" n'existe pas.", languageDirectory.native()));
        }
    }
}

