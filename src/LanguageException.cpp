
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

#include "LanguageException.h"

namespace ayeaye
{
    LanguageException::LanguageException(const string &language, const int line, const string &message)
    {
        //conversion int en string
        ostringstream oss;
        oss << line;

        //construction du message
        _message = language + ":" + oss.str() + ": " + message;
    }

    LanguageException::LanguageException(const string &language, const string &message)
    {
        //construction du message
        _message = language + ": " + message;
    }

    LanguageException::LanguageException(const string &message) :
        Exception(message)
    {
    }
}

