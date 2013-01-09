
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

#ifndef _AYEAYE_SOURCE_PARSER_H
#define _AYEAYE_SOURCE_PARSER_H

    #include <iostream>
    #include <iterator>
    #include <string>

    #include "FileBuffer.h"
    #include "Language.h"
    #include "LanguageStructure.h"
    #include "Localization.h"
    #include "SourceException.h"
    #include "SourceNode.h"

    using namespace std;

    namespace ayeaye
    {
        class SourceParser
        {
        private:
            /* Attributs */
            string _sourceIdentifier = "";
            FileBuffer *_sourceBuffer = nullptr;
            Language *_sourceLanguage = nullptr;
            bool _separatorParsing = false;

        public:
            /* Constructeur */
            SourceParser();

            /* Méthodes publiques */
            void parseSource(const string &sourceIdentifier, FileBuffer *sourceBuffer, Language *sourceLanguage) throw(SourceException);

        private:
            /* Méthodes privés */
            void _parseSource() throw(SourceException);
            bool _parseRule(SourceNode *rootNode, const LSRuleIdentifier &ruleIdentifier) throw(SourceException);
            bool _parseRuleDefinition(SourceNode *ruleNode, SourceNodeValue &ruleValue, const LSRuleDefinition &ruleDefinition) throw(SourceException);
            bool _parseExpressionList(SourceNode *ruleNode, SourceNodeValue &ruleValue, const LSExpressionList &expressionList) throw(SourceException);
            bool _parseExpression(SourceNode *ruleNode, SourceNodeValue &ruleValue, const LSExpression &expression, bool withSeparator = true) throw(SourceException);
            bool _parseUnaryExpression(SourceNode *ruleNode, SourceNodeValue &ruleValue, const LSUnaryExpression &unaryExpression, bool withSeparator = true) throw(SourceException);
            bool _parseJokerSymbol(SourceNodeValue &ruleValue) throw(SourceException);
            bool _parseIntervalSymbol(SourceNodeValue &ruleValue, const LSIntervalSymbol &intervalSymbol) throw(SourceException);
            bool _parseTerminalSymbol(SourceNodeValue &ruleValue, const LSTerminalSymbol &terminalSymbol) throw(SourceException);
            bool _parseSeparator(SourceNode *ruleNode, SourceNodeValue &ruleValue) throw(SourceException);
            bool _parseCharacter(const char c) throw(SourceException);
        };
    }

#endif

