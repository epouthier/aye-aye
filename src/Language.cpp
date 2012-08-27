
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
			_languageFile.close();
		}
		else
		{
			throw LanguageException(tr("Le fichier du langage \"%0\" est invalide.", _parameters.getLanguage()));
		}
	}

	//debug
	//===================================================================================
	void printRuleDefinition(LSRuleDefinition ruleDefinition)
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
						case LSUnaryExpressionType::LSUET_REGULAR_EXPRESSION:
							cout << "{" << itRuleDefinition->unaryExpression.regularExpression.str() << "}";
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
	}
	//===================================================================================

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
		LSRules::iterator itRules;
		
		for (itRules = _rules.begin(); itRules != _rules.end(); itRules++)
		{
			cout << itRules->first << " := ";
			printRuleDefinition(itRules->second);
			cout << ";" << endl;
		}
		//======================================================
	}

	bool Language::_parseComment() throw(LanguageException)
	{
		//EBNF => comment ::= '(*' . ... . '*)';

		if (_parseString("(*"))
		{
			while (!_parseString("*)"))
			{
				_languageFile.get();

				if (!_languageFile.good())
				{
					throw LanguageException(_parameters.getLanguage(), _currentLine, tr("syntaxe incorrecte, symbole \"*)\" absent."));
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
		//EBNF => unary-expression ::= ( rule-identifier | terminal-symbol );

		//variables
		LSRuleIdentifier ruleIdentifier;
		LSTerminalSymbol terminalSymbol;
		LSUnaryExpression unaryExpression;
		LSRegularExpression regularExpression;

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
		else if (_parseRegularExpression())
		{
			//on récupert la dernière expression régulière
			if (_regularExpressionStack.size() >= 1)
			{
				regularExpression = _regularExpressionStack.top();
				_regularExpressionStack.pop();
			}
			else
			{
				//traitement des erreurs
				throw LanguageException(_parameters.getLanguage(), tr("euh ..."));
			}

			//ajout à la pile d'expression unaire
			unaryExpression.type = LSUnaryExpressionType::LSUET_REGULAR_EXPRESSION;
			unaryExpression.regularExpression = regularExpression;
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

	bool Language::_parseRegularExpression() throw(LanguageException)
	{
		//EBNF => regular-expression ::= "{" . ... . "}";

		//variable
		LSRegularExpression regularExpression;
		string regularExpressionStr = "";

		//on parse l'expréssion régulière
		if (_parseCharacter('{'))
		{
			while (!_parseCharacter('}'))
			{
				regularExpressionStr += _languageFile.get();

				//traitement des erreurs
				if (!_languageFile.good())
				{
					throw LanguageException(_parameters.getLanguage(), _currentLine, tr("syntaxe incorrecte, symbole \"}\" absent."));
				}
			}

			//vérification que l'expression régulière n'est pas vide
			if (regularExpressionStr.empty())
			{
				throw LanguageException(_parameters.getLanguage(), _currentLine, tr("syntaxe incorrecte, il n'y a pas d'expression entre les symboles \"{\" et \"}\"."));
			}

			//ajout à la pile de l'expression régulière
			regularExpression.assign(regularExpressionStr);
			_regularExpressionStack.push(regularExpression);

			return true;
		}
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

