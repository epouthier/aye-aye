
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

#include "Language.h"

namespace ayeaye
{
	Language::Language(Parameters &parameters) throw(LanguageException) :
		_parameters(parameters),
		_currentLine(1)
	{
		//variables
		path languageFilePath;


		//vérification que le langage existe
		if (_parameters.getLanguage() == "")
		{
			throw LanguageException(tr("Le langage des fichiers sources n'est pas spécifié."));
		}

		if (_parameters.getLanguageDirectory() == "")
		{
			languageFilePath = AYEAYE_LANGUAGE_DIRECTORY;
		}
		else
		{
			languageFilePath = _parameters.getLanguageDirectory();
		}

        languageFilePath /= _parameters.getLanguage() + ".aye";

		if (!exists(languageFilePath))
		{
            throw LanguageException(tr("Le langage \"%0\" n'existe pas dans le répertoire des langages.", _parameters.getLanguage()));
		}


		//chargement du langage
		_languageFile.open(languageFilePath.c_str(), ios::in);

		if (_languageFile.is_open())
		{
			_parseLanguage();
            _checkLanguage();
			_languageFile.close();
		}
		else
		{
			throw LanguageException(tr("Le fichier du langage \"%0\" est invalide.", _parameters.getLanguage()));
		}
	}

	//debug
	//===================================================================================
	/*void printRuleDefinition(LSRuleDefinition ruleDefinition)
	{
		LSRuleDefinition::iterator itRuleDefinition;
        LSExpressionList::iterator itExpression;

		for (itRuleDefinition = ruleDefinition.begin(); itRuleDefinition != ruleDefinition.end(); )
		{
            for (itExpression = itRuleDefinition->begin(); itExpression != itRuleDefinition->end(); itExpression++)
            {
                cout << " ";

			    switch (itExpression->type)
			    {
				    case LSExpressionType::LSET_UNARY_EXPRESSION:
					    switch (itExpression->unaryExpression.type)
					    {
						    case LSUnaryExpressionType::LSUET_RULE_IDENTIFIER:
							    cout << itExpression->unaryExpression.ruleIdentifier;
							    break;
						    case LSUnaryExpressionType::LSUET_TERMINAL_SYMBOL:
							    cout << "\"" << itExpression->unaryExpression.terminalSymbol << "\"";
							    break;
                            case LSUnaryExpressionType::LSUET_INTERVAL_SYMBOL:
                                cout << "{" << itExpression->unaryExpression.intervalSymbol.first << "," << itExpression->unaryExpression.intervalSymbol.second << "}";
                                break;
                            case LSUnaryExpressionType::LSUET_JOKER_SYMBOL:
                                cout << ".";
                                break;
					    }
					    break;
				    case LSExpressionType::LSET_GROUP_EXPRESSION:
					    cout << "(";
					    printRuleDefinition(itExpression->ruleDefinition);
					    cout << " )";
					    break;
				    case LSExpressionType::LSET_OPTIONAL_EXPRESSION:
					    cout << "[";
					    printRuleDefinition(itExpression->ruleDefinition);
					    cout << " ]";
					    break;
			    }

			    switch (itExpression->repetitionSymbol)
			    {
				    case LSRepetitionSymbol::LSRS_ZERO_TO_N:
					    cout << "*";
					    break;
				    case LSRepetitionSymbol::LSRS_ONE_TO_N:
					    cout << "+";
					    break;
			    }
            }

            itRuleDefinition++;

            if (itRuleDefinition != ruleDefinition.end())
            {
                cout << " |";
            }
		}
	}*/
	//===================================================================================

    void Language::_checkLanguage() throw(LanguageException)
    {
        //variable
        LSRules::const_iterator itRules;

        //vérifie si les règles sont toutes définies
        for (itRules = _rules.begin(); itRules != _rules.end(); itRules++)
        {
            _checkRuleDefinition(itRules->first, itRules->second.ruleDefinition);
        }

        //vérification que la règle principale est définie
        if (_rules.find(_parameters.getLanguage()) == _rules.end())
        {
            //traitement des erreurs
            throw LanguageException(_parameters.getLanguage(), tr("la règle principale \"%0\" n'est pas définie.", _parameters.getLanguage()));
        }
    }

    void Language::_checkRuleDefinition(const LSRuleIdentifier &ruleIdentifier, const LSRuleDefinition &ruleDefinition) throw(LanguageException)
    {
        //variable
        LSRuleDefinition::const_iterator itRuleDefinition;
        LSExpressionList::const_iterator itExpression;

        //check rule definition
        for (itRuleDefinition = ruleDefinition.begin(); itRuleDefinition != ruleDefinition.end(); itRuleDefinition++)
		{
            for (itExpression = itRuleDefinition->begin(); itExpression != itRuleDefinition->end(); itExpression++)
            {
                if (itExpression->type == LSExpressionType::LSET_UNARY_EXPRESSION)
                {
                    //vérification que chaque ne contient pas une règle non définie
                    if (itExpression->unaryExpression.type == LSUnaryExpressionType::LSUET_RULE_IDENTIFIER)
                    {
                        //si l'identifiant de la règle n'est pas "separator"
                        if (itExpression->unaryExpression.ruleIdentifier == "separator")
                        {
                            //traitement des erreurs
                            throw LanguageException(_parameters.getLanguage(), tr("la règle \"%0\" contient une règle \"separator\", c'est interdit.", ruleIdentifier));
                        }
                        else
                        {
                            //si l'identifiant de la règle n'est pas trouvé dans le tableau des règles
                            if (_rules.find(itExpression->unaryExpression.ruleIdentifier) == _rules.end())
                            {
                                //traitement des erreurs
                                throw LanguageException(_parameters.getLanguage(), tr("la règle \"%0\" contient une règle non définie : \"%1\".", ruleIdentifier, itExpression->unaryExpression.ruleIdentifier));
                            }
                        }
                    }
                    else if (itExpression->unaryExpression.type == LSUnaryExpressionType::LSUET_JOKER_SYMBOL) //vérication que tous les joker sont bien suivi
                    {
                        itExpression++;

                        if (itExpression == itRuleDefinition->end())
                        {
                            //traitement des erreurs
                            throw LanguageException(_parameters.getLanguage(), tr("la règle \"%0\" contient un joker non suivi.", ruleIdentifier));
                        }
                        else if (itExpression->type != LSExpressionType::LSET_UNARY_EXPRESSION)
                        {
                            //traitement des erreurs
                            throw LanguageException(_parameters.getLanguage(), tr("la règle \"%0\" contient un joker non suivi d'une expression simple.", ruleIdentifier));
                        }
                        else if (itExpression->unaryExpression.type == LSUnaryExpressionType::LSUET_JOKER_SYMBOL)
                        {
                            //traitement des erreurs
                            throw LanguageException(_parameters.getLanguage(), tr("la règle \"%0\" contient un joker suivi d'un joker, c'est interdit.", ruleIdentifier));
                        }
                        else if (itExpression->unaryExpression.type == LSUnaryExpressionType::LSUET_RULE_IDENTIFIER)
                        {
                            //traitement des erreurs
                            throw LanguageException(_parameters.getLanguage(), tr("la règle \"%0\" contient un joker suivi d'un identifiant de règle, c'est interdit.", ruleIdentifier));
                        }

                        itExpression--;
                    }
                }
                else
                {
                    _checkRuleDefinition(ruleIdentifier, itExpression->ruleDefinition);
                }
            }
        }
    }

	void Language::_parseLanguage() throw(LanguageException)
	{
		//EBNF => language ::= (unnecessary-character . (comment | rule))*;

		while (_languageFile.good())
		{
			_parseUnnecessaryCharacters();

			if (!_parseComment())
			{
				_parseRule();
			}
		}
        


		//debug
		//======================================================
		/*LSRules::iterator itRules;
		
		for (itRules = _rules.begin(); itRules != _rules.end(); itRules++)
		{
			cout << itRules->first << " ::=";
			printRuleDefinition(itRules->second.ruleDefinition);
			cout << ";" << endl;
		}*/
		//======================================================
	}

	bool Language::_parseComment() throw(LanguageException)
	{
		//EBNF => comment ::= '(*' . ... . '*)';

        //variable
        char c;

        //parse comment
		if (_parseString("(*"))
		{
			while (!_parseString("*)"))
			{
				c = _languageFile.get();

				if (!_languageFile.good())
				{
					throw LanguageException(_parameters.getLanguage(), _currentLine, tr("syntaxe incorrecte, symbole \"*)\" absent."));
				}

                //si c'est un caractère de fin de ligne, on incrémente le compteur de ligne
				if (c == '\n')
				{
					_currentLine++;
				}
			}

			return true;
		}

		return false;
	}

	bool Language::_parseRule() throw(LanguageException)
	{
		//EBNF => rule ::= rule-identifier . unnecessary-character . '::=' . unnecessary-character . rule-definition . unnecessary-character . ';';

		//variables
		LSRuleIdentifier ruleIdentifier;
        LSRule rule;

		if (_parseRuleIdentifier())
		{
			_parseUnnecessaryCharacters();

            //on parse les paramètres de la règle
            _parseRuleParameters();

            _parseUnnecessaryCharacters();

			if (_parseString("::="))
			{
				_parseUnnecessaryCharacters();

                //on parse la définition de la règle
				if (_parseRuleDefinition())
				{
					_parseUnnecessaryCharacters();

					if (_parseString(";"))
					{
						//on récupert le dernier identifiant de règle
						if (_ruleIdentifierStack.size() >= 1)
						{
							ruleIdentifier = _ruleIdentifierStack.top();
							_ruleIdentifierStack.pop();
						}
						else
						{
							//traitement des erreurs
							throw LanguageException(_parameters.getLanguage(), tr("euh ..."));
						}

                        //on récupert les derniers paramètres
						if (_ruleParametersStack.size() >= 1)
						{
							rule.ruleParameters = _ruleParametersStack.top();
							_ruleParametersStack.pop();
						}

						//on récupert la dernière definition de la règle
						if (_ruleDefinitionStack.size() >= 1)
						{
							rule.ruleDefinition = _ruleDefinitionStack.top();
							_ruleDefinitionStack.pop();
						}
						else
						{
							//traitement des erreurs
							throw LanguageException(_parameters.getLanguage(), tr("euh ..."));
						}

						//ajout de la règle dans le tableau des règles
						_rules[ruleIdentifier] = rule;

						return true;
					}
					else
					{
						throw LanguageException(_parameters.getLanguage(), _currentLine - 1, tr("syntaxe incorrecte, symbole \";\" absent."));
					}
				}
				else
				{
					throw LanguageException(_parameters.getLanguage(), _currentLine, tr("syntaxe incorrecte, règle non défini après le symbole \"::=\"."));
				}
			}
			else
			{
				throw LanguageException(_parameters.getLanguage(), _currentLine, tr("syntaxe incorrecte, symbole \"::=\" absent."));
			}
		}

		return false;
	}

	bool Language::_parseRuleIdentifier() throw(LanguageException)
	{
		//EBNF => rule-identifier ::= regex([a-z]+(-[a-z]+)*);

		//variables
		LSRuleIdentifier ruleIdentifier = "";
		char c;

		//on parse l'identifiant de la règle
		while (true)
		{
			ruleIdentifier += _languageFile.get();

			if (!_parseRegex(ruleIdentifier, "[a-z-]+"))
			{
				ruleIdentifier = ruleIdentifier.substr(0, ruleIdentifier.size() - 1);
				_languageFile.unget();
				break;
			}
		}

		//vérification que l'identifiant n'est pas vide
		if (ruleIdentifier.empty())
		{
			return false;
		}

		//vérification que l'identifiant est correcte
		if (!_parseRegex(ruleIdentifier, "[a-z]+(-[a-z]+)*"))
		{
			throw LanguageException(_parameters.getLanguage(), _currentLine, tr("syntaxe incorrecte, \"%0\", la convention de nommage des identifiants des règles est un ensemble de mots (composé de une ou plusieurs lettres minuscules de a à z) séparé par des traits d'unions, ex: \"rule-identifier\".", ruleIdentifier));
		}

		//ajout à la pile d'indentifiants de règles
		_ruleIdentifierStack.push(ruleIdentifier);

		//tout c'est bien passé !!!
		return true;
	}

    void Language::_parseRuleParameters() throw(LanguageException)
    {
        //EBNF => ruleParameters ::= "(" . "value" . ")";

        //variable
        LSRuleParameters ruleParameters;

        //parse rule parameters
        if (_parseCharacter('('))
        {
            _parseUnnecessaryCharacters();

            if (_parseString("value"))
            {
                ruleParameters.isValue = true;
            }

            _parseUnnecessaryCharacters();

            if (_parseCharacter(')'))
            {
                //ajout à la pile de paramètres de règles
                _ruleParametersStack.push(ruleParameters);
            }
            else
            {
                //traitement des erreurs
                throw LanguageException(_parameters.getLanguage(), _currentLine, tr("syntaxe incorrecte, symbole \")\" absent."));
            }
        }
    }

	bool Language::_parseRuleDefinition() throw(LanguageException)
    {
        //EBNF => ruleDefinition ::= expression-list . ( unnecessary-character . "|" . unnecessary-character . expression-list )*;

        //variables
        LSRuleDefinition ruleDefinition;
        LSExpressionList expressionList;

        //parse rule definition
        if (_parseExpressionList())
        {
            //on récupert la dernière liste d'expression
			if (_expressionListStack.size() >= 1)
			{
				expressionList = _expressionListStack.top();
				_expressionListStack.pop();
			}
			else
			{
				//traitement des erreurs
				throw LanguageException(_parameters.getLanguage(), tr("euh ..."));
			}

            //ajout de l'expression à la liste d'expression
            ruleDefinition.push_back(expressionList);
        }
        else
        {
            return false;
        }

        _parseUnnecessaryCharacters();

        while (_parseCharacter('|'))
        {
            _parseUnnecessaryCharacters();

            if (_parseExpressionList())
            {
                //on récupert la dernière liste d'expression
			    if (_expressionListStack.size() >= 1)
			    {
			    	expressionList = _expressionListStack.top();
			    	_expressionListStack.pop();
			    }
			    else
			    {
			    	//traitement des erreurs
				    throw LanguageException(_parameters.getLanguage(), tr("euh ..."));
			    }

                //ajout de l'expression à la liste d'expression
                ruleDefinition.push_back(expressionList);
            }
            else
            {
                throw LanguageException(_parameters.getLanguage(), _currentLine, tr("syntaxe incorrecte, sous-règle non défini après un symbole logique \"|\"."));
                return false;
            }

            _parseUnnecessaryCharacters();
        }

        //ajout à la pile de définition de règle
		_ruleDefinitionStack.push(ruleDefinition);

        return true;
    }

    bool Language::_parseExpressionList() throw(LanguageException)
    {
        //EBNF => expression-list ::= expression . ( unnecessary-character . expression )*;

        //variables
        LSExpressionList expressionList;
        LSExpression expression;

        //parse expression list
        if (_parseExpression())
        {
            //on récupert la dernière expression
			if (_expressionStack.size() >= 1)
			{
				expression = _expressionStack.top();
				_expressionStack.pop();
			}
			else
			{
				//traitement des erreurs
				throw LanguageException(_parameters.getLanguage(), tr("euh ..."));
			}

            //ajout de l'expression à la liste d'expression
            expressionList.push_back(expression);
        }
        else
        {
            return false;
        }

        _parseUnnecessaryCharacters();

        while (_parseExpression())
        {
            //on récupert la dernière expression
			if (_expressionStack.size() >= 1)
			{
				expression = _expressionStack.top();
				_expressionStack.pop();
			}
			else
			{
				//traitement des erreurs
				throw LanguageException(_parameters.getLanguage(), tr("euh ..."));
			}

            //ajout de l'expression à la liste d'expression
            expressionList.push_back(expression);

            _parseUnnecessaryCharacters();
        }

        //ajout à la pile de liste d'expression
		_expressionListStack.push(expressionList);

        return true;
    }

    bool Language::_parseExpression() throw(LanguageException)
    {
        //EBNF => expression ::= ( optional-expression | group-expression | unary-expression ) . [ unnecessary-character . repetition-symbol ];

        //variables
        LSExpressionType expressionType;
        LSRepetitionSymbol repetitionSymbol;
		LSUnaryExpression unaryExpression;
		LSExpression expression;
		LSRuleDefinition ruleDefinition;

        //parse expression
		if (_parseOptionalExpression())
		{
			expressionType = LSExpressionType::LSET_OPTIONAL_EXPRESSION;
		}
		else if (_parseGroupExpression())
		{
			expressionType = LSExpressionType::LSET_GROUP_EXPRESSION;
		}
		else if (_parseUnaryExpression())
		{
			expressionType = LSExpressionType::LSET_UNARY_EXPRESSION;
		}
		else
		{
			return false;
		}

        //si le type est une expression optionel ou une expression de groupe
		if ((expressionType == LSExpressionType::LSET_OPTIONAL_EXPRESSION) ||
			(expressionType == LSExpressionType::LSET_GROUP_EXPRESSION))
		{
			//on récupert la dernière definition de règle
			if (_ruleDefinitionStack.size() >= 1)
			{
				ruleDefinition = _ruleDefinitionStack.top();
				_ruleDefinitionStack.pop();
			}
			else
			{
				//traitement des erreurs
				throw LanguageException(_parameters.getLanguage(), tr("euh ..."));
			}

			//définition de la définition de règle
			expression.ruleDefinition = ruleDefinition;
		}
        else if (expressionType == LSExpressionType::LSET_UNARY_EXPRESSION) //si le type est une expression unaire
		{
			//on récupert la dernière expression unaire
			if (_unaryExpressionStack.size() >= 1)
			{
				unaryExpression = _unaryExpressionStack.top();
				_unaryExpressionStack.pop();
			}
			else
			{
				//traitement des erreurs
				throw LanguageException(_parameters.getLanguage(), tr("euh ..."));
			}

			//définition de l'expression unaire
			expression.unaryExpression = unaryExpression;
		}

		_parseUnnecessaryCharacters();

		//on récupert le symbole de répétition s'il y en a un
		repetitionSymbol = _parseRepetitionSymbol();
		
        //ajout à la pile d'expression
		expression.type = expressionType;
		expression.repetitionSymbol = repetitionSymbol;
        _expressionStack.push(expression);

		return true;
    }

    bool Language::_parseOptionalExpression() throw(LanguageException)
	{
		//EBNF => optional-expression ::= '[' . unnecessary-character . rule-definition . unnecessary-character . ']';

        //pârse optional expression
		if (_parseCharacter('['))
		{
			_parseUnnecessaryCharacters();

			if (_parseRuleDefinition())
			{
				_parseUnnecessaryCharacters();

				if (_parseCharacter(']'))
				{
					return true;
				}
				else
				{
					throw LanguageException(_parameters.getLanguage(), _currentLine, tr("syntaxe incorrecte, symbole \"]\" absent."));
				}
			}
			else
			{
				throw LanguageException(_parameters.getLanguage(), _currentLine, tr("syntaxe incorrecte, sous-règle non défini entre les symboles \"[\" et \"]\"."));
			}
		}

		return false;
	}

    bool Language::_parseGroupExpression() throw(LanguageException)
	{
		//EBNF => group-expression ::= '(' . unnecessary-character . rule-definition . unnecessary-character . ')';

        //parse group expression
		if (_parseCharacter('('))
		{
			_parseUnnecessaryCharacters();

			if (_parseRuleDefinition())
			{
				_parseUnnecessaryCharacters();

				if (_parseCharacter(')'))
				{
					return true;
				}
				else
				{
					throw LanguageException(_parameters.getLanguage(), _currentLine, tr("syntaxe incorrecte, symbole \")\" absent."));
				}
			}
			else
			{
				throw LanguageException(_parameters.getLanguage(), _currentLine, tr("syntaxe incorrecte, sous-règle non défini entre les symboles \"(\" et \")\"."));
			}
		}

		return false;
	}

    bool Language::_parseUnaryExpression() throw(LanguageException)
	{
		//EBNF => unary-expression ::= ( rule-identifier | terminal-symbol | interval-symbol );

		//variables
		LSRuleIdentifier ruleIdentifier;
		LSTerminalSymbol terminalSymbol;
        LSIntervalSymbol intervalSymbol;
		LSUnaryExpression unaryExpression;

		//parse unary expression
		if (_parseRuleIdentifier())
		{
			//on récupert le dernier identifiant de règle
			if (_ruleIdentifierStack.size() >= 1)
			{
				ruleIdentifier = _ruleIdentifierStack.top();
				_ruleIdentifierStack.pop();
			}
			else
			{
				//traitement des erreurs
				throw LanguageException(_parameters.getLanguage(), tr("euh ..."));
			}

			//ajout à la pile d'expression unaire
			unaryExpression.type = LSUnaryExpressionType::LSUET_RULE_IDENTIFIER;
			unaryExpression.ruleIdentifier = ruleIdentifier;
			_unaryExpressionStack.push(unaryExpression);

			return true;
		}
		else if (_parseTerminalSymbol())
        {
			//on récupert le dernier symbole terminal
			if (_terminalSymbolStack.size() >= 1)
			{
				terminalSymbol = _terminalSymbolStack.top();
				_terminalSymbolStack.pop();
			}
			else
			{
				//traitement des erreurs
				throw LanguageException(_parameters.getLanguage(), tr("euh ..."));
			}

			//ajout à la pile d'expression unaire
			unaryExpression.type = LSUnaryExpressionType::LSUET_TERMINAL_SYMBOL;
			unaryExpression.terminalSymbol = terminalSymbol;
			_unaryExpressionStack.push(unaryExpression);

			return true;
        }
        else if (_parseIntervalSymbol())
        {
			//on récupert le dernier symbole interval
			if (_intervalSymbolStack.size() >= 1)
			{
				intervalSymbol = _intervalSymbolStack.top();
				_intervalSymbolStack.pop();
			}
			else
			{
				//traitement des erreurs
				throw LanguageException(_parameters.getLanguage(), tr("euh ..."));
			}

			//ajout à la pile d'expression unaire
			unaryExpression.type = LSUnaryExpressionType::LSUET_INTERVAL_SYMBOL;
			unaryExpression.intervalSymbol = intervalSymbol;
			_unaryExpressionStack.push(unaryExpression);

			return true;
        }
        else if (_parseJoker())
        {
            //ajout à la pile d'expression unaire
			unaryExpression.type = LSUnaryExpressionType::LSUET_JOKER_SYMBOL;
			_unaryExpressionStack.push(unaryExpression);

            return true;
        }

		return false;
	}

    LSRepetitionSymbol Language::_parseRepetitionSymbol() throw(LanguageException)
    {
		//EBNF => repetition-symbol ::= '*' | '+';

        if (_parseCharacter('*'))
		{
			return LSRepetitionSymbol::LSRS_ZERO_TO_N;
		}
		else if (_parseCharacter('+'))
		{
			return LSRepetitionSymbol::LSRS_ONE_TO_N;
		}

        return LSRepetitionSymbol::LSRS_NO_REPETITION_SYMBOL;
    }

    bool Language::_parseJoker() throw(LanguageException)
    {
        //EBNF => joker-symbol ::= ".";

        //on parse le symbole joker
        if (_parseCharacter('.'))
        {
            return true;
        }

        return false;
    }

    bool Language::_parseIntervalSymbol() throw(LanguageException)
    {
        //EBNF => interval-symbol ::= "{" . unnecessary-character . ( intervalePredefini | ( character-code . ( unnecessary-character . "," . unnecessary-character . character-code )? ) . unnecessary-character . "}"

        //variable
        LSIntervalSymbol intervalSymbol = LSIntervalSymbol(0x00, 0x00);
        LSPresetIntervalArray::const_iterator itPresetIntervalArray;
        bool isPresetInterval = false;

        //on parse le symbole intervale
        if (_parseCharacter('{'))
        {
            _parseUnnecessaryCharacters();

            //si c'est un intervale prédéfini
            for (itPresetIntervalArray = LSPresetInterval::getPresetIntervalArray().begin(); itPresetIntervalArray != LSPresetInterval::getPresetIntervalArray().end(); itPresetIntervalArray++)
            {
                if (_parseString(itPresetIntervalArray->first))
                {
                    //ajout à la pile du symbole intervale
                    intervalSymbol = itPresetIntervalArray->second;

                    isPresetInterval = true;
                    break;
                }
            }

            if (!isPresetInterval)
            {
                //si c'est un code de caractère
                if (_parseCharacterCode())
                {
                    _parseUnnecessaryCharacters();

                    //caractère séparateur
                    if (_parseCharacter(','))
                    {
                        _parseUnnecessaryCharacters();

                        if (_parseCharacterCode())
                        {
                            //on récupert les deux derniers code de caractères
                            if (_characterCodeStack.size() >= 2)
                            {
                                intervalSymbol.second = _characterCodeStack.top();
                                _characterCodeStack.pop();
                                intervalSymbol.first = _characterCodeStack.top();
                                _characterCodeStack.pop();

                                //traitement des erreurs
                                if (intervalSymbol.first > intervalSymbol.second)
                                {
                                    throw LanguageException(_parameters.getLanguage(), _currentLine, tr("semantique incorrecte, l'intervale est incorrecte, le premier code de caractère est supérieur au deuxième."));
                                }
                            }
                			else
                			{
                				//traitement des erreurs
                				throw LanguageException(_parameters.getLanguage(), tr("euh ..."));
                			}
                        }
                        else
                        {
                            //traitement des erreurs
                            throw LanguageException(_parameters.getLanguage(), _currentLine, tr("syntaxe incorrecte, code du caractère de fin d'intervale manquant après le symbole \",\"."));
                        }
                    }
                    else
                    {
                        //on récupert le dernier code de caractère
                        if (_characterCodeStack.size() >= 1)
                        {
                            intervalSymbol.first = _characterCodeStack.top();
                            intervalSymbol.second = _characterCodeStack.top();                            
                            _characterCodeStack.pop();
                        }
                        else
                        {
                            //traitement des erreurs
                            throw LanguageException(_parameters.getLanguage(), tr("euh ..."));
                        }
                    }
                }
                else
                {
                    throw LanguageException(_parameters.getLanguage(), _currentLine, tr("syntaxe incorrecte, intervale ou constante d'intervale non défini entre les symboles \"{\" et \"}\"."));
                }
            }
            
            _parseUnnecessaryCharacters();

            //caractère de fin
            if (_parseCharacter('}'))
            {
                //ajout à la pile du symbole intervale
                _intervalSymbolStack.push(intervalSymbol);

                return true;
            }
            else
            {
                throw LanguageException(_parameters.getLanguage(), _currentLine, tr("syntaxe incorrecte, symbole \"}\" absent."));
            }
        }

        return false;
    }

    bool Language::_parseCharacterCode() throw(LanguageException)
    {
        //EBNF => character-identifier ::= regex([0-9]+);

		//variables
        LSCharacterCode characterCode = 0x00;
        string str = "";
		char c;

		//on parse le code du caractère
		while (true)
		{
			str += _languageFile.get();

			if (!_parseRegex(str, "[0-9]+"))
			{
				str = str.substr(0, str.size() - 1);
				_languageFile.unget();
				break;
			}
		}

		//vérification que le code du caractère n'est pas vide
		if (str.empty())
		{
			return false;
		}

		//vérification que le code du caractère est correcte
		if (!_parseRegex(str, "[0-9]+"))
		{
			throw LanguageException(_parameters.getLanguage(), _currentLine, tr("syntaxe incorrecte, \"%0\", la convention de nommage des codes de caractères est un ensemble de chiffres, ex: \"123\".", str));
		}

        //conversion chaine de caractères vers entier
        istringstream iss(str);
        iss >> characterCode;

		//ajout à la pile de codes de caractères
		_characterCodeStack.push(characterCode);

		//tout c'est bien passé !!!
		return true;
    }

    bool Language::_parseTerminalSymbol() throw(LanguageException)
	{
		//EBNF => terminal-symbol ::= "'" . ... . "'" | '"' . ... . '"';

		//variable
		LSTerminalSymbol terminalSymbol = "";

		//on parse le symbole terminal
		if (_parseCharacter('\'')) //EBNF => "'" . ... . "'"
		{
			while (!_parseCharacter('\''))
			{
				terminalSymbol += _languageFile.get();

				//traitement des erreurs
				if (!_languageFile.good())
				{
					throw LanguageException(_parameters.getLanguage(), _currentLine, tr("syntaxe incorrecte, symbole \"'\" absent."));
				}
			}

			//vérification que le symbole terminal n'est pas vide
			if (terminalSymbol.empty())
			{
				throw LanguageException(_parameters.getLanguage(), _currentLine, tr("syntaxe incorrecte, il n'y a pas d'expression entre les symboles \"'\"."));
			}

			//ajout à la pile de symbole terminal
			_terminalSymbolStack.push(terminalSymbol);

			return true;
		}
		else if (_parseCharacter('"')) //EBNF => '"' . ... . '"'
		{
			while (!_parseCharacter('"'))
			{
				terminalSymbol += _languageFile.get();

				//traitement des erreurs
				if (!_languageFile.good())
				{
					throw LanguageException(_parameters.getLanguage(), _currentLine, tr("syntaxe incorrecte, symbole '\"' absent."));
				}
			}

            //vérification que le symbole terminal n'est pas vide
			if (terminalSymbol.empty())
			{
				throw LanguageException(_parameters.getLanguage(), _currentLine, tr("syntaxe incorrecte, il n'y a pas d'expression entre les symboles '\"'."));
			}

			//ajout à la pile de symbole terminal
			_terminalSymbolStack.push(terminalSymbol);

			return true;
		}

		return false;
	}

	void Language::_parseUnnecessaryCharacters() throw(LanguageException)
	{
		//EBNF => unnecessary-character ::= ( ' ' | '\t' | '\n' )*;

		while (_languageFile.good())
		{
			if ((!_parseCharacter(' ')) &&
				(!_parseCharacter('\t')))
			{
				//si c'est un caractère de fin de ligne, on incrémente le compteur de ligne
				if (_parseCharacter('\n'))
				{
					_currentLine++;
					continue;
				}

				break;
			}
		}
	}

	bool Language::_parseRegex(const string &str, const string &rstr)
	{
		boost::regex r(rstr);

		return (regex_match(str, r));
	}

	bool Language::_parseString(const string &str) throw(LanguageException)
	{
		for (unsigned int i = 0; i < str.size(); i++)
		{
			if (!_parseCharacter(str[i]))
			{
				for (unsigned int j = 0; j < i; j++)
				{
					_languageFile.unget();
				}

				return false;
			}
		}

		return true;
	}

	bool Language::_parseCharacter(const char c) throw(LanguageException)
	{
		//traitement des erreurs
		if (_languageFile.eof())
		{
			throw LanguageException(_parameters.getLanguage(), _currentLine, tr("syntaxe incorrecte, fin de fichier inattendu."));
		}

		if (_languageFile.get() != c)
		{            
			_languageFile.unget();
			return false;
		}

		return true;
	}
}

