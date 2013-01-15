
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

#ifndef _AYEAYE_FILE_BUFFER_H
#define _AYEAYE_FILE_BUFFER_H

    #include "Exception.h"
    #include "Localization.h"

    namespace ayeaye
    {
        class FileBuffer
        {
        private:
            /* Attributs */
            size_t _bufferSize = 0;
            char *_buffer = nullptr;
            bool *_bufferBlackList = nullptr;
            unsigned long _bufferIndex = 0;
            unsigned int _bufferLine = 1;

        public:
            /* Constructeur et destructeur */
            FileBuffer(size_t bufferSize) throw(Exception);
            ~FileBuffer();

            /* Getters */
            size_t getBufferSize() {return _bufferSize;}
            char *getBufferPtr() {return _buffer;}
            unsigned long getCurrentIndex() {return _bufferIndex;}
            unsigned int getCurrentLine() {return _bufferLine;}

            /* Méthodes publiques */
            bool hasData();
            char nextData();
            void decrementIndex();
            pair<unsigned long, unsigned int> saveState();
            void restoreState(const pair<unsigned long, unsigned int> &saveStateBuffer);
            void reset();
            void blacklistData(unsigned long bufferIndexMin, unsigned long bufferIndexMax);

        };
    }

#endif

