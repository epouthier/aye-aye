
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

#include "Source.h"

namespace ayeaye
{
    SourceParser Source::_sourceParser;

    Source::Source(const path &sourceFilePath) throw(SourceException)
    {
        //variables
        Language *sourceLanguage = nullptr;
        ifstream sourceFile;
        FileBuffer *sourceBuffer = nullptr;
        size_t sourceFileSize = 0;

        //vérification de l'existence du fichier
        if (!exists(sourceFilePath))
        {
            throw SourceException(tr("Le fichier source \"%0\" n'existe pas.", sourceFilePath.native()));
        }

        //taille du fichier
        sourceFileSize = file_size(sourceFilePath);

        //récupération du langage
        sourceLanguage = nullptr;

        //ouverture du fichier
        sourceFile.open(sourceFilePath.c_str(), ios::in);
        if (sourceFile.is_open())
        {
            //allocation d'un buffer
            sourceBuffer = new FileBuffer(sourceFileSize);

            //récupération des données dans le fichier source
            sourceFile.read(sourceBuffer->getBufferPtr(), sourceBuffer->getBufferSize());
            if (sourceFile.bad())
            {
                throw SourceException(tr("La lecture du fichier source \"%0\" a échoué.", sourceFilePath.native()));
            }

            //fermeture du fichier
            sourceFile.close();

            //parsage des données
            _sourceParser.parseSource(sourceFilePath.native(), sourceBuffer, sourceLanguage);

            //déallocation du buffer
            delete sourceBuffer;
        }
        else
        {
            //traitement des erreurs
            throw SourceException(tr("La lecture du fichier source \"%0\" a échoué.", sourceFilePath.native()));
        }
    }
}

