
#include "Language.h"

namespace ayeaye
{
	Language::Language(Parameters &parameters) throw(LanguageException) :
		_parameters(parameters)
	{
		//variables
		path languageFilePath;


		//vérification que le langage existe
		if (_parameters.getLanguage() == "")
			throw LanguageException("Le langage des fichiers sources n'est pas spécifié");

		if (_parameters.getLanguageDirectory() == "")
			languageFilePath = AYEAYE_LANGUAGE_DIRECTORY;
		else
			languageFilePath = _parameters.getLanguageDirectory();

        languageFilePath /= _parameters.getLanguage() + ".aye";

		if (!exists(languageFilePath))
            throw LanguageException("Le langage \"" + _parameters.getLanguage() + "\" n'existe pas dans le répertoire des langages");


		//chargement du langage
		_languageFile.open(languageFilePath.c_str(), ios::in);

		if (_languageFile.is_open())
		{
			_parseLanguage();
			_languageFile.close();
		}
		else
		{
			throw LanguageException("Parser", "Le fichier du langage \"" + _parameters.getLanguage() + "\" est invalide");
		}
	}

	void Language::_parseLanguage() throw(LanguageException)
	{
		while (_languageFile.good())
		{
			_parseUnnecessaryCharacters();

			if (!_parseComment())
				_parseRule();
		}
	}

	bool Language::_parseComment() throw(LanguageException)
	{
		if (_parseString("(*"))
		{
			while (!_parseString("*)"))
			{
				_languageFile.get();
			}

			return true;
		}

		return false;
	}

	bool Language::_parseRule() throw(LanguageException)
	{
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
				}
			}
		}

		return false;
	}

	bool Language::_parseRuleIdentifier() throw(LanguageException)
	{
		string ruleIdentifier = "";
		char c;

		while (true)
		{
			if (_languageFile.eof())
				throw LanguageException("Parser", "La syntaxe du fichier du language \"" + _parameters.getLanguage() + "\" est invalide");

			ruleIdentifier += _languageFile.get();

			if (!_parseRegex(ruleIdentifier, "[a-z-]+"))
			{
				ruleIdentifier = ruleIdentifier.substr(0, ruleIdentifier.size() - 1);
				_languageFile.unget();
				break;
			}
		}

		if (ruleIdentifier.empty())
			return false;

		cout << ruleIdentifier << endl;

		return (_parseRegex(ruleIdentifier, "[a-z]+(-[a-z]+)*"));
	}

	bool Language::_parseRuleDefinition() throw(LanguageException)
	{
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
                    return false;
                }
            }

            return true;
        }

		return false;
	}

    bool Language::_parseOptionalExpression() throw(LanguageException)
	{
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
			}
		}

		return false;
	}

    bool Language::_parseGroupExpression() throw(LanguageException)
	{
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
			}
		}

		return false;
	}

    bool Language::_parseUnaryExpression() throw(LanguageException)
	{
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
        if (_parseCharacter('.') ||
            _parseCharacter('|'))
            return true;

        return false;
    }

    bool Language::_parseRepetitionSymbol() throw(LanguageException)
    {
        if (_parseCharacter('*') ||
            _parseCharacter('+') ||
            _parseCharacter('?'))
            return true;

        return false;
    }

    bool Language::_parseTerminalSymbol() throw(LanguageException)
	{
		string terminalSymbol = "";

		if (_parseCharacter('\''))
		{
			while (!_parseCharacter('\''))
			{
				terminalSymbol += _languageFile.get();
			}

            cout << "\"" << terminalSymbol << "\"" << endl;

			return true;
		}
		else if (_parseCharacter('"'))
		{
			while (!_parseCharacter('"'))
			{
				terminalSymbol += _languageFile.get();
			}

            cout << "\"" << terminalSymbol << "\"" << endl;

			return true;
		}

		return false;
	}

	void Language::_parseUnnecessaryCharacters() throw(LanguageException)
	{
		while (_languageFile.good())
		{
			if ((!_parseCharacter(' ')) &&
				(!_parseCharacter('\t')) &&
				(!_parseCharacter('\n')) &&
				(!_parseCharacter('\r')))
				break;
		}
	}

	bool Language::_parseRegex(string str, string rstr)
	{
		boost::regex r(rstr);

		return (regex_match(str, r));
	}

	bool Language::_parseString(string str) throw(LanguageException)
	{
		for (unsigned int i = 0; i < str.size(); i++)
		{
			if (!_parseCharacter(str[i]))
			{
				for (unsigned int j = 0; j < i; j++)
					_languageFile.unget();

				return false;
			}
		}

		return true;
	}

	bool Language::_parseCharacter(char c) throw(LanguageException)
	{
		if (_languageFile.eof())
			throw LanguageException("Parser", "La syntaxe du fichier du language \"" + _parameters.getLanguage() + "\" est invalide");

		if (_languageFile.get() != c)
		{            
			_languageFile.unget();
			return false;
		}

        /*char ctmp = _languageFile.get();
        cout << ctmp << endl;
        if (ctmp != c)
        {
            _languageFile.unget();
            return false;
        }*/

		return true;
	}
}

