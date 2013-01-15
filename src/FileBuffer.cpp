
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

#include "FileBuffer.h"

namespace ayeaye
{
    FileBuffer::FileBuffer(size_t bufferSize) throw(Exception) :
        _bufferSize(bufferSize),
        _buffer(nullptr),
        _bufferBlackList(nullptr),
        _bufferIndex(0),
        _bufferLine(1)
    {
        if (_bufferSize > 0)
        {
            //allocation du buffer
            _buffer = new char[_bufferSize];
            if (_buffer == nullptr)
            {
                throw Exception(tr("Erreur d'allocation de mémoire."));
            }

            //allocation de la liste noire
            _bufferBlackList = new bool[_bufferSize];
            if (_bufferBlackList == nullptr)
            {
                throw Exception(tr("Erreur d'allocation de mémoire."));
            }

            //initialisation de la liste noire
            for (unsigned long i = 0; i < _bufferSize; i++)
            {
                _bufferBlackList[i] = false;
            }
        }
        else
        {
            throw Exception(tr("La taille du buffer doit être supérieur à zéro"));
        }
    }

    FileBuffer::~FileBuffer()
    {
        //libération du buffer
        if (_buffer != nullptr)
        {
            delete[] _buffer;
        }

        //libération de la liste noire
        if (_bufferBlackList != nullptr)
        {
            delete[] _bufferBlackList;
        }
    }

    bool FileBuffer::hasData()
    {
        //variable
        unsigned long i = _bufferIndex;

        //vérifie si il y a encore des données
        while (true)
        {
            if (i < _bufferSize)
            {
                if (_bufferBlackList[i])
                {
                    i++;
                }
                else
                {
                    return true;
                }
            }
            else
            {
                return false;
            }
        }
    }

    char FileBuffer::nextData()
    {
        //variable
        char data;

        //vérifie si il y a des données
        if (hasData())
        {
            //on ignore tant qu'il y a des données sur liste noire
            while (_bufferBlackList[_bufferIndex])
            {
                //on récupert la donnée
                data = _buffer[_bufferIndex];

                //on avance le curseur
                _bufferIndex++;

                //si c'est un caractère de fin de ligne on met à jour le compteur de ligne
                if (data == '\n')
                {
                    _bufferLine++;
                }
            }

            //on récupert la donnée
            data = _buffer[_bufferIndex];

            //on avance le curseur
            _bufferIndex++;

            //si c'est un caractère de fin de ligne on met à jour le compteur de ligne
            if (data == '\n')
            {
                _bufferLine++;
            }

            return data;
        }

        return 0;
    }

    void FileBuffer::decrementIndex()
    {
        //variable
        char data;

        if (_bufferIndex > 0)
        {
            _bufferIndex--;

            data = _buffer[_bufferIndex];

            if (data == '\n')
            {
                _bufferLine--;
            }
        }
    }

    pair<unsigned long, unsigned int> FileBuffer::saveState()
    {
        return pair<unsigned long, unsigned int>(_bufferIndex, _bufferLine);
    }

    void FileBuffer::restoreState(const pair<unsigned long, unsigned int> &saveStateBuffer)
    {
        if ((saveStateBuffer.first >= 0) && (saveStateBuffer.first < _bufferSize))
        {
            _bufferIndex = saveStateBuffer.first;
            _bufferLine = saveStateBuffer.second;
        }
    }

    void FileBuffer::reset()
    {
        _bufferIndex = 0;
        _bufferLine = 1;
    }

    void FileBuffer::blacklistData(unsigned long bufferIndexMin, unsigned long bufferIndexMax)
    {
        if (bufferIndexMin >= _bufferSize)
        {
            bufferIndexMin = _bufferSize - 1;
        }

        if (bufferIndexMax >= _bufferSize)
        {
            bufferIndexMax = _bufferSize - 1;
        }

        if (bufferIndexMin < bufferIndexMax)
        {
            for (bufferIndexMin; bufferIndexMin < bufferIndexMax; bufferIndexMin++)
            {
                _bufferBlackList[bufferIndexMin] = true;
            }
        }
    }
}

