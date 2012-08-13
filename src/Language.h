
#ifndef _AYEAYE_LANGUAGE_H
#define _AYEAYE_LANGUAGE_H

    #include <iostream>
	#include <fstream>

	#include <boost/filesystem.hpp>
	#include <boost/regex.hpp>

	#include "LanguageException.h"
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

		public:
			/* Constructeur */
			Language(Parameters &parameters) throw(LanguageException);

		private:
			void _parseLanguage() throw(LanguageException);
			bool _parseComment() throw(LanguageException);
			bool _parseRule() throw(LanguageException);
			bool _parseRuleIdentifier() throw(LanguageException);
			bool _parseRuleDefinition() throw(LanguageException);
            bool _parseOptionalExpression() throw(LanguageException);
            bool _parseGroupExpression() throw(LanguageException);
            bool _parseUnaryExpression() throw(LanguageException);
            bool _parseLogicalSymbol() throw(LanguageException);
            bool _parseRepetitionSymbol() throw(LanguageException);
            bool _parseTerminalSymbol() throw(LanguageException);
			void _parseUnnecessaryCharacters() throw(LanguageException);
			bool _parseRegex(string str, string rstr);
			bool _parseString(string str) throw(LanguageException);
			bool _parseCharacter(char c) throw(LanguageException);

		};
	}

#endif

