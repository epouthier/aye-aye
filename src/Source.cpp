
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

			_sourceFile.open(_sourceFilePath.c_str(), ios::in);

			if (_sourceFile.is_open())
			{
				_sourceFile.close();
			}
			else
			{
				throw SourceException(tr("Le fichier source \"%0\" est invalide.", _sourceFilePath.native()));
			}
		}
    }

	void Source::_parseSource() throw(SourceException)
	{
	}

	bool Source::_parseRegularExpression(LSRegularExpression regularExpression) throw(SourceException)
	{
		return false;
	}

	bool Source::_parseTerminalSymbol(LSTerminalSymbol terminalSymbol) throw(SourceException)
	{
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

