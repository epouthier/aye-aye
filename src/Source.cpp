
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

    //debug
	//===================================================================================
	void printRootNode(SSNode *rootNode, unsigned int heightNode)
	{
        for (unsigned int i = heightNode; i > 0; i--) { cout << " "; }
        cout << rootNode->getRuleIdentifier();
        if (! rootNode->getValue().empty()) { cout << " = \"" << rootNode->getValue() << "\""; }
        cout << endl;

        list<SSNode*>::iterator itrNode;
        for (itrNode = rootNode->getChildren().begin(); itrNode != rootNode->getChildren().end(); itrNode++)
        {
            printRootNode(*itrNode, heightNode + 1);
        }
	}
	//===================================================================================

	void Source::_parseSource() throw(SourceException)
	{
        //variable
        SSNode* rootNode = nullptr;

        //initialisation du noeud principale
        rootNode = new SSNode(_sourceFilePath.native());

        //parsage de la règle principale
        if (!_parseRule(rootNode, _parameters.getLanguage()))
        {
            //traitement des erreurs
            throw SourceException(_sourceFilePath.native(), _currentLine, tr("syntaxe incorrecte, la source ne correspond pas au langage défini."));
        }

        //debug
		//======================================================
        printRootNode(rootNode, 0);
		//======================================================

        //destruction du noeud principale
        delete rootNode;
	}

    bool Source::_parseRule(SSNode *rootNode, const LSRuleIdentifier &ruleIdentifier) throw(SourceException)
    {
        //cout << "_parseRule(" << ruleIdentifier << ")" << endl; //debug

        //variable
        SSNode *ruleNode = nullptr;
        SSValue ruleValue = "";

        //initialisation du noeud
        ruleNode = new SSNode(ruleIdentifier);

        if (_parseRuleDefinition(ruleNode, ruleValue, _language.getRules()[ruleIdentifier].ruleDefinition))
        {
            //ajout de la valeur au noeud
            if (_language.getRules()[ruleIdentifier].ruleParameters.isValue)
            {
                ruleNode->attachValue(ruleValue);
            }

            //ajout du noeud au noeud parent
            rootNode->attachChildNode(ruleNode);

            return true;
        }

        //destruction du noeud
        delete ruleNode;

        return false;
    }

    bool Source::_parseRuleDefinition(SSNode *ruleNode, SSValue &ruleValue, const LSRuleDefinition &ruleDefinition) throw(SourceException)
    {
        //cout << " _parseRuleDefinition()" << endl; //debug

        //variables
        LSRuleDefinition::const_iterator itRuleDefinition;

        //parse rule definition
        for (itRuleDefinition = ruleDefinition.begin(); itRuleDefinition != ruleDefinition.end(); itRuleDefinition++)
        {
            //parse expression list
            if (_parseExpressionList(ruleNode, ruleValue, *itRuleDefinition))
            {
                return true;
            }
        }

        return false;
    }

    bool Source::_parseExpressionList(SSNode *ruleNode, SSValue &ruleValue, const LSExpressionList &expressionList) throw(SourceException)
    {
        //cout << "  _parseExpressionList()" << endl; //debug

        //variables
        LSExpressionList::const_iterator itExpression;
        bool result, joker = false;
        streampos positionSourceFileSave;

        //sauvegarde de la position dans le fichier
        positionSourceFileSave = _sourceFile.tellg();

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
                            _parseJokerSymbol(ruleValue);
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
                        result = _parseExpression(ruleNode, ruleValue, *itExpression, !joker);
                        break;
                    case LSRepetitionSymbol::LSRS_ZERO_TO_N:
                        result = true;
                        while (_parseExpression(ruleNode, ruleValue, *itExpression, !joker));
                        break;
                    case LSRepetitionSymbol::LSRS_ONE_TO_N:
                        result = _parseExpression(ruleNode, ruleValue, *itExpression, !joker);
                        if (result)
                        {
                            while (_parseExpression(ruleNode, ruleValue, *itExpression, !joker));
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
                        _parseJokerSymbol(ruleValue);
                    }
                }
            } while (joker);

            //si on a pas arrivé à parser une expression, alors continuer le parsage des autres expressions ne sert à rien
            if (!result)
            {
                //retour à la position sauvegardé dans le fichier
                _sourceFile.seekg(positionSourceFileSave);

                return false;
            }
        }

        return true;
    }

    bool Source::_parseExpression(SSNode *ruleNode, SSValue &ruleValue, const LSExpression &expression, bool withSeparator) throw(SourceException)
    {
        //cout << "   _parseExpression()" << endl; //debug

        //parse expression
        switch (expression.type)
	    {
            case LSExpressionType::LSET_OPTIONAL_EXPRESSION:
                _parseRuleDefinition(ruleNode, ruleValue, expression.ruleDefinition);
                return true;
                break;
            case LSExpressionType::LSET_GROUP_EXPRESSION:
                return _parseRuleDefinition(ruleNode, ruleValue, expression.ruleDefinition);
			    break;
		    case LSExpressionType::LSET_UNARY_EXPRESSION:
			    return _parseUnaryExpression(ruleNode, ruleValue, expression.unaryExpression, withSeparator);
		        break;
	    }

        return false;
    }

	bool Source::_parseUnaryExpression(SSNode *ruleNode, SSValue &ruleValue, const LSUnaryExpression &unaryExpression, bool withSeparator) throw(SourceException)
	{
        //cout << "    _parseUnaryExpression()" << endl; //debug

        //parse separator
        if (withSeparator && !_separatorParsing)
        {
            while (_parseSeparator(ruleNode, ruleValue));
        }

		//parse unary expression
		switch (unaryExpression.type)
		{
			case LSUnaryExpressionType::LSUET_RULE_IDENTIFIER:
				return _parseRule(ruleNode, unaryExpression.ruleIdentifier);
				break;
			case LSUnaryExpressionType::LSUET_TERMINAL_SYMBOL:
				return _parseTerminalSymbol(ruleValue, unaryExpression.terminalSymbol);
				break;
			case LSUnaryExpressionType::LSUET_INTERVAL_SYMBOL:
				return _parseIntervalSymbol(ruleValue, unaryExpression.intervalSymbol);
				break;
            case LSUnaryExpressionType::LSUET_JOKER_SYMBOL:
				return _parseJokerSymbol(ruleValue);
				break;
		}

		return false;
	}

    bool Source::_parseJokerSymbol(SSValue &ruleValue) throw(SourceException)
    {
        //cout << "     _parseJokerSymbol()" << endl; //debug

        //variable
        char c;

        //traitement des erreurs
		if (_sourceFile.eof())
		{
			throw SourceException(_sourceFilePath.native(), _currentLine, tr("syntaxe incorrecte, fin de fichier inattendu."));
		}

        //parse joker symbol
        c = _sourceFile.get();

        //construction de la valeur
        ruleValue += c;

        return true;
    }

    bool Source::_parseIntervalSymbol(SSValue &ruleValue, const LSIntervalSymbol &intervalSymbol) throw(SourceException)
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

        //construction de la valeur
        ruleValue += c;

        return true;
    }

	bool Source::_parseTerminalSymbol(SSValue &ruleValue, const LSTerminalSymbol &terminalSymbol) throw(SourceException)
	{
        //cout << "     _parseTerminalSymbol(\"" << terminalSymbol << "\")" << endl; //debug
        //cout << "      " << (char) _sourceFile.get() << endl; _sourceFile.unget(); //debug;

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

        //construction de la valeur
        ruleValue += terminalSymbol;

		return true;
	}

    bool Source::_parseSeparator(SSNode *ruleNode, SSValue &ruleValue) throw(SourceException)
    {
        //cout << "_parseSeparator()" << endl; //debug

        //variable
        bool result = false;

        //lock separator parsing
        _separatorParsing = true;

        //traitement du separator custom
        if (_language.getRules().find("separator") != _language.getRules().end())
        {
            result = _parseRuleDefinition(ruleNode, ruleValue, _language.getRules()["separator"].ruleDefinition);
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

