
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

		for (itRuleDefinition = ruleDefinition.begin(); itRuleDefinition != ruleDefinition.end(); itRuleDefinition++)
		{
			switch (itRuleDefinition->type)
			{
				case LSSubRuleDefinitionType::LSSRDT_UNARY_EXPRESSION:
					switch (itRuleDefinition->unaryExpression.type)
					{
						case LSUnaryExpressionType::LSUET_RULE_IDENTIFIER:
							cout << itRuleDefinition->unaryExpression.ruleIdentifier;
							break;
						case LSUnaryExpressionType::LSUET_TERMINAL_SYMBOL:
							cout << "\"" << itRuleDefinition->unaryExpression.terminalSymbol << "\"";
							break;
                        case LSUnaryExpressionType::LSUET_INTERVAL_SYMBOL:
                            cout << "{" << itRuleDefinition->unaryExpression.intervalSymbol.first << "," << itRuleDefinition->unaryExpression.intervalSymbol.second << "}";
                            break;
					}
					break;
				case LSSubRuleDefinitionType::LSSRDT_GROUP_EXPRESSION:
					cout << "( ";
					printRuleDefinition(itRuleDefinition->ruleDefinition);
					cout << " )";
					break;
				case LSSubRuleDefinitionType::LSSRDT_OPTIONAL_EXPRESSION:
					cout << "[ ";
					printRuleDefinition(itRuleDefinition->ruleDefinition);
					cout << " ]";
					break;
			}

			switch (itRuleDefinition->repetionSymbol)
			{
				case LSRepetitionSymbol::LSRS_ZERO_TO_N:
					cout << "*";
					break;
				case LSRepetitionSymbol::LSRS_ONE_TO_N:
					cout << "+";
					break;
				case LSRepetitionSymbol::LSRS_ZERO_OR_ONE:
					cout << "?";
					break;
			}

			switch (itRuleDefinition->logicalSymbol)
			{
				case LSLogicalSymbol::LSLS_AND:
					cout << " . ";
					break;
				case LSLogicalSymbol::LSLS_OR:
					cout << " | ";
					break;
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
            _checkRuleDefinition(itRules->first, itRules->second);
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

        //check rule definition
        for (itRuleDefinition = ruleDefinition.begin(); itRuleDefinition != ruleDefinition.end(); itRuleDefinition++)
		{
            if (itRuleDefinition->type == LSSubRuleDefinitionType::LSSRDT_UNARY_EXPRESSION)
            {
                if (itRuleDefinition->unaryExpression.type == LSUnaryExpressionType::LSUET_RULE_IDENTIFIER)
                {
                    //si l'identifiant de la règle n'est pas trouvé dans le tableau des règles
                    if (_rules.find(itRuleDefinition->unaryExpression.ruleIdentifier) == _rules.end())
                    {
                        //traitement des erreurs
                        throw LanguageException(_parameters.getLanguage(), tr("la règle \"%0\" contient une règle non définie : \"%1\".", ruleIdentifier, itRuleDefinition->unaryExpression.ruleIdentifier));
                    }
                }
            }
            else
            {
                _checkRuleDefinition(ruleIdentifier, itRuleDefinition->ruleDefinition);
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
			cout << itRules->first << " := ";
			printRuleDefinition(itRules->second);
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
		LSRuleDefinition ruleDefinition;

		if (_parseRuleIdentifier())
		{
			_parseUnnecessaryCharacters();

			if (_parseString("::="))
			{
				_parseUnnecessaryCharacters();

				//ajout d'une définition de règle
				_ruleDefinitionStack.push(ruleDefinition);

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

						//on récupert la dernière definition de la règle
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

						//ajout de la règle dans le tableau des règles
						_rules[ruleIdentifier] = ruleDefinition;

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

	bool Language::_parseRuleDefinition() throw(LanguageException)
	{
		//EBNF => rule-definition ::= ( optional-expression | group-expression | unary-expression ) . [ unnecessary-character . repetition-symbol ] . [ unnecessary-character . logical-symbol . rule-definition ];

		//variables
		LSSubRuleDefinitionType subRuleDefinitionType;
		LSRepetitionSymbol repetitionSymbol;
		LSLogicalSymbol logicalSymbol;
		LSUnaryExpression unaryExpression;
		LSSubRuleDefinition subRuleDefinition;
		LSRuleDefinition ruleDefinition;

		//parse rule definition
		if (_parseOptionalExpression())
		{
			subRuleDefinitionType = LSSubRuleDefinitionType::LSSRDT_OPTIONAL_EXPRESSION;
		}
		else if (_parseGroupExpression())
		{
			subRuleDefinitionType = LSSubRuleDefinitionType::LSSRDT_GROUP_EXPRESSION;
		}
		else if (_parseUnaryExpression())
		{
			subRuleDefinitionType = LSSubRuleDefinitionType::LSSRDT_UNARY_EXPRESSION;
		}
		else
		{
			return false;
		}

		//si le type est une expression optionel ou une expression de groupe
		if ((subRuleDefinitionType == LSSubRuleDefinitionType::LSSRDT_OPTIONAL_EXPRESSION) ||
			(subRuleDefinitionType == LSSubRuleDefinitionType::LSSRDT_GROUP_EXPRESSION))
		{
			//on récupert la dernière definition de règle
			if (_ruleDefinitionStack.size() >= 2)
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
			subRuleDefinition.ruleDefinition = ruleDefinition;
		}

		_parseUnnecessaryCharacters();

		//on récupert le symbole de répétition s'il y en a un
		repetitionSymbol = _parseRepetitionSymbol();

        _parseUnnecessaryCharacters();

		//on récupert le symbole logique
		logicalSymbol = _parseLogicalSymbol();

		//si il y a pas de symbole logique
		if (logicalSymbol != LSLogicalSymbol::LSLS_NO_LOGICAL_SYMBOL)
		{
			_parseUnnecessaryCharacters();

			//et qu'il n'y a pas de définition de règle après, alors il y une erreur
			if (!_parseRuleDefinition())
			{
				throw LanguageException(_parameters.getLanguage(), _currentLine, tr("syntaxe incorrecte, sous-règle non défini après un symbole logique (\"|\" ou \".\")."));
				return false;
			}
		}

		//si le type est une expression unaire
		if (subRuleDefinitionType == LSSubRuleDefinitionType::LSSRDT_UNARY_EXPRESSION)
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
			subRuleDefinition.unaryExpression = unaryExpression;
		}

		//ajout à la définition de la règle courante
		subRuleDefinition.type = subRuleDefinitionType;
		subRuleDefinition.repetionSymbol = repetitionSymbol;
		subRuleDefinition.logicalSymbol = logicalSymbol;
		_ruleDefinitionStack.top().push_front(subRuleDefinition);

		return true;
	}

    bool Language::_parseOptionalExpression() throw(LanguageException)
	{
		//EBNF => optional-expression ::= '[' . unnecessary-character . rule-definition . unnecessary-character . ']';

		//variable
		LSRuleDefinition ruleDefinition;

		if (_parseCharacter('['))
		{
			_parseUnnecessaryCharacters();

			//ajout d'une définition de règle
			_ruleDefinitionStack.push(ruleDefinition);

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

		//variable
		LSRuleDefinition ruleDefinition;

		if (_parseCharacter('('))
		{
			_parseUnnecessaryCharacters();

			//ajout d'une définition de règle
			_ruleDefinitionStack.push(ruleDefinition);

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

    LSLogicalSymbol Language::_parseLogicalSymbol() throw(LanguageException)
    {
		//EBNF => logical-symbol ::= '.' | '|';

        if (_parseCharacter('.'))
		{
			return LSLogicalSymbol::LSLS_AND;
		}
		else if (_parseCharacter('|'))
		{
            return LSLogicalSymbol::LSLS_OR;
		}

        return LSLogicalSymbol::LSLS_NO_LOGICAL_SYMBOL;
    }

    LSRepetitionSymbol Language::_parseRepetitionSymbol() throw(LanguageException)
    {
		//EBNF => repetition-symbol ::= '*' | '+' | '?';

        if (_parseCharacter('*'))
		{
			return LSRepetitionSymbol::LSRS_ZERO_TO_N;
		}
		else if (_parseCharacter('+'))
		{
			return LSRepetitionSymbol::LSRS_ONE_TO_N;
		}
		else if (_parseCharacter('?'))
		{
            return LSRepetitionSymbol::LSRS_ZERO_OR_ONE;
		}

        return LSRepetitionSymbol::LSRS_NO_REPETITION_SYMBOL;
    }

    bool Language::_parseJoker() throw(LanguageException)
    {
        //EBNF => joker-symbol ::= "_";

        //on parse le symbole joker
        if (_parseCharacter('_'))
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
        LSPresetIntervalArray::iterator itPresetIntervalArray;
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
					throw LanguageException(_parameters.getLanguage(), _currentLine, tr("syntaxe incorrecte, symbole \"\'\" absent."));
				}
			}

			//vérification que le symbole terminal n'est pas vide
			if (terminalSymbol.empty())
			{
				throw LanguageException(_parameters.getLanguage(), _currentLine, tr("syntaxe incorrecte, il n'y a pas d'expression entre les symboles \"\'\"."));
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
					throw LanguageException(_parameters.getLanguage(), _currentLine, tr("syntaxe incorrecte, symbole \'\"\' absent."));
				}
			}

            //vérification que le symbole terminal n'est pas vide
			if (terminalSymbol.empty())
			{
				throw LanguageException(_parameters.getLanguage(), _currentLine, tr("syntaxe incorrecte, il n'y a pas d'expression entre les symboles \'\"\'."));
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

