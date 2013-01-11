
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

#include "Language.h"

namespace ayeaye
{
    LanguageMetadataParser Language::_languageMetadataParser;
    LanguageStructureParser Language::_languageStructureParser;

    Language::Language(const path &languageFilePath) throw(Exception, LanguageException) :
        _languageFilePath(languageFilePath),
        _languageIdentifier(""),
        _languageStructureLoaded(false)
    {
        //variables
        FileBuffer *languageMetadataBuffer = nullptr;

        //initialisation des variables
        _languageIdentifier = languageFilePath.filename().native();
        _languageIdentifier = _languageIdentifier.substr(0, _languageIdentifier.size() - 7);

        //vérification de l'existence du fichier
        if (!exists(languageFilePath))
        {
            throw LanguageException(tr("Le langage \"%0\" n'existe pas dans le répertoire des langages.", languageFilePath.native()));
        }

        //extraction des meta-données dans le fichier de langage
        languageMetadataBuffer = _extractFilePath("metadata");
        if (languageMetadataBuffer != nullptr)
        {
            //parsage des données
            _languageMetadata = _languageMetadataParser.parseLanguageMetadata(_languageIdentifier, languageMetadataBuffer);

            //déallocation du buffer
            delete languageMetadataBuffer;
        }
        else
        {
            throw LanguageException(tr("Le fichier du langage \"%0\" est invalide.", _languageIdentifier));
        }
    }

    void Language::loadStructure() throw(Exception, LanguageException)
    {
        //variables
        FileBuffer *languageStructureBuffer = nullptr;

        //traitement des erreurs
        if (_languageStructureLoaded == true) {return;}

        //extraction des meta-données dans le fichier de langage
        languageStructureBuffer = _extractFilePath("structure");
        if (languageStructureBuffer != nullptr)
        {
            //parsage des données
            _languageStructure = _languageStructureParser.parseLanguageStructure(_languageIdentifier, languageStructureBuffer);

            //déallocation du buffer
            delete languageStructureBuffer;
        }
        else
        {
            throw LanguageException(tr("Le fichier du langage \"%0\" est invalide.", _languageIdentifier));
        }

        //structure du langage chargé
        _languageStructureLoaded = true;
    }

    FileBuffer *Language::_extractFilePath(const string &extractFilePath) throw(Exception, LanguageException)
    {
        //variables
        archive *languageFile = nullptr;
        archive_entry *languageFileEntry = nullptr;
        string filePath = "";
        size_t fileSize = 0;
        FileBuffer *buffer = nullptr;

        //ouverture du fichier
        languageFile = archive_read_new();
        archive_read_support_filter_all(languageFile);
        archive_read_support_format_all(languageFile);
        if (archive_read_open_filename(languageFile, _languageFilePath.c_str(), 10240) != ARCHIVE_OK)
        {
            throw LanguageException(tr("La lecture du fichier du langage \"%0\" a échoué.", _languageIdentifier));
        }

        //lecture du fichier de langage
        while (archive_read_next_header(languageFile, &languageFileEntry) == ARCHIVE_OK)
        {
            //récupération des informations dans le fichier de langage
            filePath = archive_entry_pathname(languageFileEntry);
            fileSize = archive_entry_size(languageFileEntry);

            if (filePath == extractFilePath)
            {
                //allocation d'un buffer
                buffer = new FileBuffer(fileSize);

                //récupération des données dans le fichier de langage
                if (archive_read_data(languageFile, buffer->getBufferPtr(), buffer->getBufferSize()) < 0)
                {
                    throw LanguageException(tr("La lecture du fichier du langage \"%0\" a échoué.", _languageIdentifier));
                }

                //fermeture du fichier
                archive_read_close(languageFile);
                archive_read_free(languageFile);

                return buffer;
            }
        }

        //fermeture du fichier
        archive_read_close(languageFile);
        archive_read_free(languageFile);

        return nullptr;
    }
}

