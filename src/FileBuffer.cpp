
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
        _bufferIndex(0),
        _bufferLine(1)
    {
        //allocation du buffer
        if (_bufferSize > 0)
        {
            _buffer = new char[_bufferSize];
            if (_buffer == nullptr)
            {
                throw Exception(tr("Erreur d'allocation de mémoire."));
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
    }

    bool FileBuffer::hasData()
    {
        return (_bufferIndex < _bufferSize);
    }

    char FileBuffer::nextData()
    {
        char data;

        if (hasData())
        {
            data = _buffer[_bufferIndex];

            _bufferIndex++;

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
        if (_bufferIndex > 0)
        {
            _bufferIndex--;
        }
    }

    void FileBuffer::seekIndex(unsigned long index)
    {
        if ((index >= 0) && (index < _bufferSize))
        {
            _bufferIndex = index;
        }
    }

    void FileBuffer::resetIndexAndLine()
    {
        _bufferIndex = 0;
        _bufferLine = 1;
    }
}

