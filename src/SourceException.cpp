
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

#include "SourceException.h"

namespace ayeaye
{
	SourceException::SourceException(const string &source, const int line, const string &message)
	{
		//conversion int en string
		ostringstream oss;
    	oss << line;

		//construction du message
		_message = source + ":" + oss.str() + ": " + message;
	}

	SourceException::SourceException(const string &source, const string &message)
	{
		//construction du message
		_message = source + ": " + message;
	}

	SourceException::SourceException(const string &message) :
		Exception(message)
	{
	}
}

