
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

#ifndef _AYEAYE_SOURCE_EXCEPTION_H
#define _AYEAYE_SOURCE_EXCEPTION_H

    #include <iostream>
	#include <sstream>
    #include <string>

	#include "Exception.h"

    using namespace std;

	namespace ayeaye
	{
		class SourceException : public Exception
		{
		public:
			/* Constructeur */
			SourceException(const string &source, const int line, const string &message);
			SourceException(const string &source, const string &message);
			SourceException(const string &message);
		};
	}

#endif

