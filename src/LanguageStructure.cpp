
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

#include "LanguageStructure.h"

namespace ayeaye
{
    //Interval Symbol
    LSPresetIntervalArray LSPresetInterval::_presetIntervalArray = 
        {{"ALPHA-UC", LSIntervalSymbol(0x41, 0x5a)},
         {"ALPHA-LC", LSIntervalSymbol(0x61, 0x7a)},
         {"DIGIT", LSIntervalSymbol(0x30, 0x39)},
         {"SPACE", LSIntervalSymbol(0x20, 0x20)},
         {"TABULATION-H", LSIntervalSymbol(0x09, 0x09)},
         {"TABULATION-V", LSIntervalSymbol(0x0b, 0x0b)},
         {"END-LINE", LSIntervalSymbol(0x0a, 0x0a)},
         {"JOKER", LSIntervalSymbol(0x00, 0xff)}};
}

