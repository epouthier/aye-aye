
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

#ifndef _AYEAYE_LANGUAGE_H
#define _AYEAYE_LANGUAGE_H

    #include <iostream>
	#include <iterator>
	#include <fstream>
    #include <sstream>
	#include <stack>
    #include <string>

	#include <boost/filesystem.hpp>
	#include <boost/regex.hpp>

	#include "LanguageException.h"
	#include "LanguageStructure.h"
	#include "Localization.h"
	#include "Parameters.h"

    using namespace std;
	using namespace boost::filesystem;

	namespace ayeaye
	{
		class Language
		{
		private:
			/* Attributs */
			Parameters &_parameters;
			ifstream _languageFile;
			int _currentLine = 1;
			stack<LSRuleIdentifier> _ruleIdentifierStack;
			stack<LSRuleDefinition> _ruleDefinitionStack;
            stack<LSExpressionList> _expressionListStack;
            stack<LSExpression> _expressionStack;
			stack<LSUnaryExpression> _unaryExpressionStack;
			stack<LSTerminalSymbol> _terminalSymbolStack;
            stack<LSIntervalSymbol> _intervalSymbolStack;
            stack<LSCharacterCode> _characterCodeStack;
			LSRules _rules;


		public:
			/* Constructeur */
			Language(Parameters &parameters) throw(LanguageException);


			/* Getters */
			LSRules &getRules() {return _rules;}


		private:
			/* Méthodes privées */
            void _checkLanguage() throw(LanguageException);
            void _checkRuleDefinition(const LSRuleIdentifier &ruleIdentifier, const LSRuleDefinition &ruleDefinition) throw(LanguageException);
			void _parseLanguage() throw(LanguageException);
			bool _parseComment() throw(LanguageException);
			bool _parseRule() throw(LanguageException);
			bool _parseRuleIdentifier() throw(LanguageException);
			bool _parseRuleDefinition() throw(LanguageException);
            bool _parseExpressionList() throw(LanguageException);
            bool _parseExpression() throw(LanguageException);
            bool _parseOptionalExpression() throw(LanguageException);
            bool _parseGroupExpression() throw(LanguageException);
            bool _parseUnaryExpression() throw(LanguageException);
            LSRepetitionSymbol _parseRepetitionSymbol() throw(LanguageException);
            bool _parseJoker() throw(LanguageException);
            bool _parseIntervalSymbol() throw(LanguageException);
            bool _parseCharacterCode() throw(LanguageException);
            bool _parseTerminalSymbol() throw(LanguageException);
			void _parseUnnecessaryCharacters() throw(LanguageException);
			bool _parseRegex(const string &str, const string &rstr);
			bool _parseString(const string &str) throw(LanguageException);
			bool _parseCharacter(const char c) throw(LanguageException);

		};
	}

#endif

