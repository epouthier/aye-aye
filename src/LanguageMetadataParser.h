
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

#ifndef _AYEAYE_LANGUAGE_METADATA_PARSER_H
#define _AYEAYE_LANGUAGE_METADATA_PARSER_H

    #include <iostream>
    #include <iterator>
    #include <stack>
    #include <string>

    #include <boost/regex.hpp>

    #include "FileBuffer.h"
    #include "LanguageMetadata.h"
    #include "LanguageException.h"
    #include "Localization.h"

    using namespace std;

    namespace ayeaye
    {
        class LanguageMetadataParser
        {
        private:
            /* Atrributs */
            string _languageIdentifier = "";
            FileBuffer *_languageMetadataBuffer = nullptr;
            stack<LMRuleIdentifier> _ruleIdentifierStack;
            stack<LMRuleDefinition> _ruleDefinitionStack;
            stack<LMExpression> _expressionStack;
            LMRules _rules;

        public:
            /* Constructeur */
            LanguageMetadataParser();

            /* Méthodes publiques */
            LMRules parseLanguageMetadata(const string &languageIdentifier, FileBuffer *languageMetadataBuffer) throw(LanguageException);

        private:
            void _checkLanguageMetadata() throw(LanguageException);
            void _parseLanguageMetadata() throw(LanguageException);
            bool _parseComment() throw(LanguageException);
            bool _parseRule() throw(LanguageException);
            bool _parseRuleIdentifier() throw(LanguageException);
            bool _parseRuleDefinition() throw(LanguageException);
            bool _parseExpression() throw(LanguageException);
            void _parseUnnecessaryCharacters() throw(LanguageException);
            bool _parseRegex(const string &str, const string &rstr);
            bool _parseString(const string &str) throw(LanguageException);
            bool _parseCharacter(const char c) throw(LanguageException);

        };
    }

#endif

