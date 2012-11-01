
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

#include "Source.h"

namespace ayeaye
{
    Source::Source(Parameters &parameters, Language &language) throw(SourceException) :
		_parameters(parameters),
		_language(language),
		_sourceFilePath(""),
		_currentLine(1)
    {
        //vérification de l'existence des sources
        for (unsigned int i = 0; i < _parameters.getSources().size(); i++)
        {
            _sourceFilePath = _parameters.getSources()[i];

            if (!exists(_sourceFilePath))
	    	{
                throw SourceException(tr("Le fichier source \"%0\" n'existe pas.", _sourceFilePath.native()));
       		}
        }

		//parsage des sources
		for (unsigned int i = 0; i < _parameters.getSources().size(); i++)
		{
			//récupération du chemin de la source
			_sourceFilePath = _parameters.getSources()[i];

            //ouverture du fichier source
			_sourceFile.open(_sourceFilePath.c_str(), ios::in);
			if (_sourceFile.is_open())
			{
                //parsage de la source
                _parseSource();
				_sourceFile.close();
			}
			else
			{
                //traitement d'erreur
				throw SourceException(tr("Le fichier source \"%0\" est invalide.", _sourceFilePath.native()));
			}
		}
    }

	void Source::_parseSource() throw(SourceException)
	{
        //parsage de la règle principale
        if (!_parseRule(_parameters.getLanguage()))
        {
            //traitement des erreurs
            throw SourceException(_sourceFilePath.native(), _currentLine, tr("syntaxe incorrecte, la source ne correspond pas au langage défini."));
        }
	}

    bool Source::_parseRule(const LSRuleIdentifier &ruleIdentifier) throw(SourceException)
    {
        //parsage de la définition de la règle
        return (_parseRuleDefinition(_language.getRules()[ruleIdentifier]));
    }

	bool Source::_parseRuleDefinition(const LSRuleDefinition &ruleDefinition) throw(SourceException)
	{
        //variable
        LSRuleDefinition::const_iterator itRuleDefinition;
        bool result = false;
        bool ignore = false;
        bool joker = false;
        bool jokerZeroOrOne = false;
        int loop = 0;

        //parse rule definition
        for (itRuleDefinition = ruleDefinition.begin(); itRuleDefinition != ruleDefinition.end(); itRuleDefinition++)
        {
            //traitement la sous-définition de la règle
            if (!ignore)
            {
                //initialisation des variables
                joker = false;
                jokerZeroOrOne = false;
                loop = 0;

                //traitement du joker
                if (itRuleDefinition->type == LSSubRuleDefinitionType::LSSRDT_UNARY_EXPRESSION)
                {
                    if (itRuleDefinition->unaryExpression.type == LSUnaryExpressionType::LSUET_JOKER_SYMBOL)
                    {
                        //traitement des symboles de répétition
                        switch (itRuleDefinition->repetionSymbol)
                        {
                            case LSRepetitionSymbol::LSRS_ZERO_TO_N:
                                joker = true;
                                itRuleDefinition++;
                                break;
                            case LSRepetitionSymbol::LSRS_ONE_TO_N:
                                _parseJokerSymbol();
                                joker = true;
                                itRuleDefinition++;
                                break;
                            case LSRepetitionSymbol::LSRS_ZERO_OR_ONE:
                                joker = true;
                                jokerZeroOrOne = true;
                                itRuleDefinition++;
                                break;
                        }
                    }
                }

                //traitement des symboles de répétition
                while (loop >= 0)
                {
                    switch (itRuleDefinition->repetionSymbol)
                    {
                        case LSRepetitionSymbol::LSRS_NO_REPETITION_SYMBOL:
                            result = _parseSubRuleDefinition(*itRuleDefinition);
                            break;
                        case LSRepetitionSymbol::LSRS_ZERO_TO_N:
                            result = true;
                            while (_parseSubRuleDefinition(*itRuleDefinition));
                            break;
                        case LSRepetitionSymbol::LSRS_ONE_TO_N:
                            result = _parseSubRuleDefinition(*itRuleDefinition);
                            if (result)
                            {
                                while (_parseSubRuleDefinition(*itRuleDefinition));
                            }
                            break;
                        case LSRepetitionSymbol::LSRS_ZERO_OR_ONE:
                            result = true;
                            _parseSubRuleDefinition(*itRuleDefinition);
                            break;
                    }

                    //traitement du joker
                    if (joker)
                    {
                        if (result)
                        {
                            loop = -1;
                        }
                        else
                        {
                            if (jokerZeroOrOne && (loop == 1))
                            {
                                loop = -1;
                            }
                            else
                            {
                                _parseJokerSymbol();
                                loop++;
                            }
                        }
                    }
                    else
                    {
                        loop = -1;
                    }
                }
            }

            cout << "result = " << result << ", ignore = " << ignore << endl; //debug

            //traitement des symboles logiques
            switch (itRuleDefinition->logicalSymbol)
            {
                case LSLogicalSymbol::LSLS_AND:
                    if (!result)
                    {
                        ignore = true;
                    }
                    break;
                case LSLogicalSymbol::LSLS_OR:
                    if (ignore)
                    {
                        ignore = false;
                    }
                    else if (result)
                    {
                        return true;
                    }
                    break;
            }
        }

        return result;
	}

    bool Source::_parseSubRuleDefinition(const LSSubRuleDefinition &subRuleDefinition) throw(SourceException)
    {
        //parse sub rule definition
        switch (subRuleDefinition.type)
        {
            case LSSubRuleDefinitionType::LSSRDT_UNARY_EXPRESSION:
                return _parseUnaryExpression(subRuleDefinition.unaryExpression);
                break;
            case LSSubRuleDefinitionType::LSSRDT_GROUP_EXPRESSION:
            case LSSubRuleDefinitionType::LSSRDT_OPTIONAL_EXPRESSION:
                return _parseRuleDefinition(subRuleDefinition.ruleDefinition);
                break;
        }

        return false;
    }

	bool Source::_parseUnaryExpression(const LSUnaryExpression &unaryExpression) throw(SourceException)
	{
		//parse unary expression
		switch (unaryExpression.type)
		{
			case LSUnaryExpressionType::LSUET_RULE_IDENTIFIER:
				return _parseRule(unaryExpression.ruleIdentifier);
				break;
			case LSUnaryExpressionType::LSUET_TERMINAL_SYMBOL:
				return _parseTerminalSymbol(unaryExpression.terminalSymbol);
				break;
			case LSUnaryExpressionType::LSUET_INTERVAL_SYMBOL:
				return _parseIntervalSymbol(unaryExpression.intervalSymbol);
				break;
            case LSUnaryExpressionType::LSUET_JOKER_SYMBOL:
				return _parseJokerSymbol();
				break;
		}

		return false;
	}

    bool Source::_parseJokerSymbol() throw(SourceException)
    {
        cout << "_parseJokerSymbol()" << endl; //debug

        //traitement des erreurs
		if (_sourceFile.eof())
		{
			throw SourceException(_sourceFilePath.native(), _currentLine, tr("syntaxe incorrecte, fin de fichier inattendu."));
		}

        //parse joker symbol
        _sourceFile.get();

        return true;
    }

    bool Source::_parseIntervalSymbol(const LSIntervalSymbol &intervalSymbol) throw(SourceException)
    {
        cout << "_parseIntervalSymbol(" << intervalSymbol.first << ", " << intervalSymbol.second << ")" << endl; //debug

        //variable
        char c;

        //traitement des erreurs
		if (_sourceFile.eof())
		{
			throw SourceException(_sourceFilePath.native(), _currentLine, tr("syntaxe incorrecte, fin de fichier inattendu."));
		}

        //parse interval symbol
        c = _sourceFile.get();

        if (!((c >= intervalSymbol.first) && (c <= intervalSymbol.second)))
        {
            _sourceFile.unget();
            return false;
        }

        return true;
    }

	bool Source::_parseTerminalSymbol(const LSTerminalSymbol &terminalSymbol) throw(SourceException)
	{
        cout << "_parseTerminalSymbol(\"" << terminalSymbol << "\")" << endl; //debug

		//parse terminal symbol
		for (unsigned int i = 0; i < terminalSymbol.size(); i++)
		{
			if (!_parseCharacter(terminalSymbol[i]))
			{
				for (unsigned int j = 0; j < i; j++)
				{
					_sourceFile.unget();
				}

				return false;
			}
		}

		return true;
	}

	bool Source::_parseCharacter(const char c) throw(SourceException)
	{
		//traitement des erreurs
		if (_sourceFile.eof())
		{
			throw SourceException(_sourceFilePath.native(), _currentLine, tr("syntaxe incorrecte, fin de fichier inattendu."));
		}

		if (_sourceFile.get() != c)
		{            
			_sourceFile.unget();
			return false;
		}

		return true;
	}
}
