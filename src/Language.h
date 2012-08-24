
#ifndef _AYEAYE_LANGUAGE_H
#define _AYEAYE_LANGUAGE_H

    #include <iostream>
	#include <iterator>
	#include <fstream>
	#include <stack>
    #include <string>

	#include <boost/filesystem.hpp>
	#include <boost/regex.hpp>

	#include "LanguageException.h"
	#include "LanguageStructure.h"
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
			stack<LSUnaryExpression> _unaryExpressionStack;
			stack<LSTerminalSymbol> _terminalSymbolStack;
			LSRules _rules;


		public:
			/* Constructeur */
			Language(Parameters &parameters) throw(LanguageException);


			/* Getters */
			LSRules &getRules() {return _rules;}


		private:
			/* Méthodes privées */
			void _parseLanguage() throw(LanguageException);
			bool _parseComment() throw(LanguageException);
			bool _parseRule() throw(LanguageException);
			bool _parseRuleIdentifier() throw(LanguageException);
			bool _parseRuleDefinition() throw(LanguageException);
            bool _parseOptionalExpression() throw(LanguageException);
            bool _parseGroupExpression() throw(LanguageException);
            bool _parseUnaryExpression() throw(LanguageException);
            LSLogicalSymbol _parseLogicalSymbol() throw(LanguageException);
            LSRepetitionSymbol _parseRepetitionSymbol() throw(LanguageException);
            bool _parseTerminalSymbol() throw(LanguageException);
			void _parseUnnecessaryCharacters() throw(LanguageException);
			bool _parseRegex(const string &str, const string &rstr);
			bool _parseString(const string &str) throw(LanguageException);
			bool _parseCharacter(const char c) throw(LanguageException);

		};
	}

#endif

