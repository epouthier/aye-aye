
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

	void Language::_parseLanguage() throw(LanguageException)
	{
		//language ::= (unnecessary-character . (comment | rule))*;

		while (_languageFile.good())
		{
			_parseUnnecessaryCharacters();

			if (!_parseComment())
			{
				_parseRule();
			}
		}
	}

	bool Language::_parseComment() throw(LanguageException)
	{
		//comment ::= '(*' . ... . '*)';

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
		//rule ::= rule-identifier . unnecessary-character . '::=' . unnecessary-character . rule-definition . unnecessary-character . ';';

		if (_parseRuleIdentifier())
		{
			_parseUnnecessaryCharacters();

			if (_parseString("::="))
			{
				_parseUnnecessaryCharacters();

				if (_parseRuleDefinition())
				{
					_parseUnnecessaryCharacters();

					if (_parseString(";"))
					{
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
		//rule-identifier ::= regex([a-z]+(-[a-z]+)*);

		string ruleIdentifier = "";
		char c;

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

		if (ruleIdentifier.empty())
		{
			return false;
		}

		//cout << ruleIdentifier << endl; //debug

		return _parseRegex(ruleIdentifier, "[a-z]+(-[a-z]+)*");
	}

	bool Language::_parseRuleDefinition() throw(LanguageException)
	{
		//rule-definition ::= ( optional-expression | group-expression | unary-expression ) . [ unnecessary-character . logical-symbol . rule-definition ];

        if (_parseOptionalExpression() ||
            _parseGroupExpression() ||
            _parseUnaryExpression())
        {
            _parseUnnecessaryCharacters();

            if (_parseLogicalSymbol())
            {
                _parseUnnecessaryCharacters();

                if (_parseRuleDefinition())
                {
                    return true;
                }
                else
                {
                    throw LanguageException(_parameters.getLanguage(), _currentLine, tr("syntaxe incorrecte, sous-règle non défini après un symbole logique (\"|\" ou \".\")."));
                }
            }

            return true;
        }

		return false;
	}

    bool Language::_parseOptionalExpression() throw(LanguageException)
	{
		//optional-expression ::= '[' . unnecessary-character . rule-definition . unnecessary-character . ']' . [ unnecessary-character . repetition-symbol ];

		if (_parseCharacter('['))
		{
			_parseUnnecessaryCharacters();

			if (_parseRuleDefinition())
			{
				_parseUnnecessaryCharacters();

				if (_parseCharacter(']'))
				{
                    _parseUnnecessaryCharacters();

                    if (_parseRepetitionSymbol())
                    {
                    }

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
		//group-expression ::= '(' . unnecessary-character . rule-definition . unnecessary-character . ')' . [ unnecessary-character . repetition-symbol ];

		if (_parseCharacter('('))
		{
			_parseUnnecessaryCharacters();

			if (_parseRuleDefinition())
			{
				_parseUnnecessaryCharacters();

				if (_parseCharacter(')'))
				{
                    _parseUnnecessaryCharacters();

                    if (_parseRepetitionSymbol())
                    {
                    }

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
		//unary-expression ::= ( rule-identifier | terminal-symbol ) . [ unnecessary-character . repetition-symbol ];

		if (_parseRuleIdentifier() ||
			_parseTerminalSymbol())
        {
            _parseUnnecessaryCharacters();

            if (_parseRepetitionSymbol())
            {
            }

			return true;
        }

		return false;
	}

    bool Language::_parseLogicalSymbol() throw(LanguageException)
    {
		//logical-symbol ::= '.' | '|';

        if (_parseCharacter('.') ||
            _parseCharacter('|'))
		{
            return true;
		}

        return false;
    }

    bool Language::_parseRepetitionSymbol() throw(LanguageException)
    {
		//repetition-symbol ::= '*' | '+' | '?';

        if (_parseCharacter('*') ||
            _parseCharacter('+') ||
            _parseCharacter('?'))
		{
            return true;
		}

        return false;
    }

    bool Language::_parseTerminalSymbol() throw(LanguageException)
	{
		//terminal-symbol ::= "'" . ... . "'" | '"' . ... . '"';

		string terminalSymbol = "";

		if (_parseCharacter('\''))
		{
			while (!_parseCharacter('\''))
			{
				terminalSymbol += _languageFile.get();

				if (!_languageFile.good())
				{
					throw LanguageException(_parameters.getLanguage(), _currentLine, tr("syntaxe incorrecte, symbole \"\'\" absent."));
				}
			}

            //cout << "\"" << terminalSymbol << "\"" << endl; //debug

			return true;
		}
		else if (_parseCharacter('"'))
		{
			while (!_parseCharacter('"'))
			{
				terminalSymbol += _languageFile.get();

				if (!_languageFile.good())
				{
					throw LanguageException(_parameters.getLanguage(), _currentLine, tr("syntaxe incorrecte, symbole \'\"\' absent."));
				}
			}

            //cout << "\"" << terminalSymbol << "\"" << endl; //debug

			return true;
		}

		return false;
	}

	void Language::_parseUnnecessaryCharacters() throw(LanguageException)
	{
		//unnecessary-character ::= ( ' ' | '\t' | '\n' )*;

		while (_languageFile.good())
		{
			if ((!_parseCharacter(' ')) &&
				(!_parseCharacter('\t')))
			{
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

