
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
    #include "SourceStructure.h"

    using namespace std;
	using namespace boost::filesystem;

    namespace ayeaye
    {
        class Source
        {
		private:
			/* Attributs */
			Parameters &_parameters;
			Language &_language;
			path _sourceFilePath;
			ifstream _sourceFile;
			int _currentLine = 1;
            bool _separatorParsing = false;

        public:
            /* Constructeur */
            Source(Parameters &parameters, Language &language) throw(SourceException);

		private:
			/* Méthodes privées */
			void _parseSource() throw(SourceException);
            bool _parseRule(SSNode *rootNode, const LSRuleIdentifier &ruleIdentifier) throw(SourceException);
			bool _parseRuleDefinition(SSNode *ruleNode, SSValue &ruleValue, const LSRuleDefinition &ruleDefinition) throw(SourceException);
            bool _parseExpressionList(SSNode *ruleNode, SSValue &ruleValue, const LSExpressionList &expressionList) throw(SourceException);
            bool _parseExpression(SSNode *ruleNode, SSValue &ruleValue, const LSExpression &expression, bool withSeparator = true) throw(SourceException);
			bool _parseUnaryExpression(SSNode *ruleNode, SSValue &ruleValue, const LSUnaryExpression &unaryExpression, bool withSeparator = true) throw(SourceException);
            bool _parseJokerSymbol(SSValue &ruleValue) throw(SourceException);
            bool _parseIntervalSymbol(SSValue &ruleValue, const LSIntervalSymbol &intervalSymbol) throw(SourceException);
			bool _parseTerminalSymbol(SSValue &ruleValue, const LSTerminalSymbol &terminalSymbol) throw(SourceException);
            bool _parseSeparator(SSNode *ruleNode, SSValue &ruleValue) throw(SourceException);
			bool _parseCharacter(const char c) throw(SourceException);
        };
    }

#endif

