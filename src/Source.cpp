
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
        //cout << "_parseRule(" << ruleIdentifier << ")" << endl; //debug

        //parsage de la définition de la règle
        if (ruleIdentifier == "separator")
        {
            return _parseSeparator();
        }

        return (_parseRuleDefinition(_language.getRules()[ruleIdentifier]));
    }

    bool Source::_parseRuleDefinition(const LSRuleDefinition &ruleDefinition) throw(SourceException)
    {
        //cout << " _parseRuleDefinition()" << endl; //debug

        //variables
        LSRuleDefinition::const_iterator itRuleDefinition;

        //parse rule definition
        for (itRuleDefinition = ruleDefinition.begin(); itRuleDefinition != ruleDefinition.end(); itRuleDefinition++)
        {
            if (_parseExpressionList(*itRuleDefinition))
            {
                return true;
            }
        }

        return false;
    }

    bool Source::_parseExpressionList(const LSExpressionList &expressionList) throw(SourceException)
    {
        //cout << "  _parseExpressionList()" << endl; //debug

        //variables
        LSExpressionList::const_iterator itExpression;
        bool result, joker = false;

        //parse expression list
        for (itExpression = expressionList.begin(); itExpression != expressionList.end(); itExpression++)
        {
            //initialisation des variables
            joker = false;

            //traitement du joker
            if (itExpression->type == LSExpressionType::LSET_UNARY_EXPRESSION)
            {
                if (itExpression->unaryExpression.type == LSUnaryExpressionType::LSUET_JOKER_SYMBOL)
                {
                    //traitement des symboles de répétition
                    switch (itExpression->repetitionSymbol)
                    {
                        case LSRepetitionSymbol::LSRS_ZERO_TO_N:
                            joker = true;
                            itExpression++;
                            break;
                        case LSRepetitionSymbol::LSRS_ONE_TO_N:
                            _parseJokerSymbol();
                            joker = true;
                            itExpression++;
                            break;
                    }
                }
            }
            
            do
            {
                //traitement des symboles de répétition
                switch (itExpression->repetitionSymbol)
                {
                    case LSRepetitionSymbol::LSRS_NO_REPETITION_SYMBOL:
                        result = _parseExpression(*itExpression, !joker);
                        break;
                    case LSRepetitionSymbol::LSRS_ZERO_TO_N:
                        result = true;
                        while (_parseExpression(*itExpression, !joker));
                        break;
                    case LSRepetitionSymbol::LSRS_ONE_TO_N:
                        result = _parseExpression(*itExpression, !joker);
                        if (result)
                        {
                            while (_parseExpression(*itExpression, !joker));
                        }
                        break;
                }

                //traitement du joker
                if (joker)
                {
                    if (result)
                    {
                        joker = false;
                    }
                    else
                    {
                        _parseJokerSymbol();
                    }
                }
            } while (joker);

            //si on a pas arrivé à parser une expression, alors continuer le parsage des autres expressions ne sert à rien
            if (!result)
            {
                return false;
            }
        }

        return true;
    }

    bool Source::_parseExpression(const LSExpression &expression, bool withSeparator) throw(SourceException)
    {
        //cout << "   _parseExpression()" << endl; //debug

        //parse expression
        switch (expression.type)
	    {
            case LSExpressionType::LSET_OPTIONAL_EXPRESSION:
                _parseRuleDefinition(expression.ruleDefinition);
                return true;
                break;
            case LSExpressionType::LSET_GROUP_EXPRESSION:
                return _parseRuleDefinition(expression.ruleDefinition);
			    break;
		    case LSExpressionType::LSET_UNARY_EXPRESSION:
			    return _parseUnaryExpression(expression.unaryExpression, withSeparator);
		        break;
	    }

        return false;
    }

	bool Source::_parseUnaryExpression(const LSUnaryExpression &unaryExpression, bool withSeparator) throw(SourceException)
	{
        //cout << "    _parseUnaryExpression()" << endl; //debug

        //parse separator
        if (withSeparator && !_separatorParsing)
        {
            while (_parseSeparator());
        }

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
        //cout << "     _parseJokerSymbol()" << endl; //debug

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
        //cout << "     _parseIntervalSymbol(" << intervalSymbol.first << ", " << intervalSymbol.second << ")" << endl; //debug

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
        //cout << "     _parseTerminalSymbol(\"" << terminalSymbol << "\")" << endl; //debug

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

    bool Source::_parseSeparator() throw(SourceException)
    {
        //cout << "_parseSeparator()" << endl; //debug

        //variable
        bool result = false;

        //lock separator parsing
        _separatorParsing = true;

        //traitement du separator custom
        if (_language.getRules().find("separator") != _language.getRules().end())
        {
            result = _parseRuleDefinition(_language.getRules()["separator"]);
        }
        else //traitement du separator default
        {
            //traitement des erreurs
    		if (_sourceFile.eof())
    		{
    			throw SourceException(_sourceFilePath.native(), _currentLine, tr("syntaxe incorrecte, fin de fichier inattendu."));
    		}

            //parsage du separator default
        	if ((!_parseCharacter(' ')) &&
        		(!_parseCharacter('\t')))
        	{
        		//si c'est un caractère de fin de ligne, on incrémente le compteur de ligne
        		if (_parseCharacter('\n'))
        		{
        			_currentLine++;
                    result = true;
                }
            }
            else
            {
                result = true;
            }
        }

        //unlock separator parsing
        _separatorParsing = false;

        return result;
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

