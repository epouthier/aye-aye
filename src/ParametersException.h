
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

#ifndef _AYEAYE_PARAMETERS_EXCEPTION_H
#define _AYEAYE_PARAMETERS_EXCEPTION_H

    #include <iostream>
    #include <string>

    #include "Exception.h"

    using namespace std;

    namespace ayeaye
    {
        class ParametersException : public Exception
        {
        public:
            /* Constructeur */
            ParametersException(const string &message) : Exception(message) {}
        };
    }

#endif

