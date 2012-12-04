
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

#ifndef _AYEAYE_SOURCE_H
#define _AYEAYE_SOURCE_H

    #include <iostream>
	#include <iterator>
	#include <fstream>
    #include <sstream>
    #include <string>

	#include <boost/filesystem.hpp>

    #include "Language.h"
	#include "LanguageStructure.h"
	#include "Localization.h"
	#include "Parameters.h"
	#include "SourceException.h"

    using namespace std;
	using namespace boost::filesystem;

    namespace ayeaye
    {
        class Source
        {
		private:
			/* Attributs */
			/*Parameters &_parameters;
			Language &_language;
			path _sourceFilePath;
			ifstream _sourceFile;
			int _currentLine = 1;*/

        public:
            /* Constructeur */
            /*Source(Parameters &parameters, Language &language) throw(SourceException);*/

		private:
			/* Méthodes privées */
			/*void _parseSource() throw(SourceException);
            bool _parseRule(const LSRuleIdentifier &ruleIdentifier) throw(SourceException);
			bool _parseRuleDefinition(const LSRuleDefinition &ruleDefinition) throw(SourceException);
            bool _parseSubRuleDefinition(const LSSubRuleDefinition &subRuleDefinition) throw(SourceException);
			bool _parseUnaryExpression(const LSUnaryExpression &unaryExpression) throw(SourceException);
            bool _parseJokerSymbol() throw(SourceException);
            bool _parseIntervalSymbol(const LSIntervalSymbol &intervalSymbol) throw(SourceException);
			bool _parseTerminalSymbol(const LSTerminalSymbol &terminalSymbol) throw(SourceException);
			bool _parseCharacter(const char c) throw(SourceException);*/
        };
    }

#endif

