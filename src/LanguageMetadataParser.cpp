
/*
 * Copyright (C) 2013 Emmanuel Pouthier
 *
 * This file is part of Aye-Aye.
 *
 * Aye-Aye is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Aye-Aye is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "LanguageMetadataParser.h"

namespace ayeaye
{
    LanguageMetadataParser::LanguageMetadataParser() :
        _languageIdentifier(""),
        _languageMetadataBuffer(nullptr)
    {
    }

    LMRules LanguageMetadataParser::parseLanguageMetadata(const string &languageIdentifier, FileBuffer *languageMetadataBuffer) throw(LanguageException)
    {
        //initialisation des variables
        _languageIdentifier = languageIdentifier;
        _languageMetadataBuffer = languageMetadataBuffer;

        //initialisation et réinitialisation des piles
        while (! _ruleIdentifierStack.empty())
        {
            _ruleIdentifierStack.pop();
        }

        while (! _ruleDefinitionStack.empty())
        {
            _ruleDefinitionStack.pop();
        }

        while (! _expressionStack.empty())
        {
            _expressionStack.pop();
        }

        _rules.clear();

        //parsage des meta-données
        _parseLanguageMetadata();
        _checkLanguageMetadata();

        //retourne les meta-données du language
        return _rules;
    }

    void LanguageMetadataParser::_checkLanguageMetadata() throw(LanguageException)
    {
        //variable
        LMRules::const_iterator itRules;

        //vérification que la règle extension est définie
        if (_rules.find("extension") == _rules.end())
        {
            //traitement des erreurs
            throw LanguageException(_languageIdentifier, tr("la règle \"extension\" n'est pas définie."));
        }
    }

    void LanguageMetadataParser::_parseLanguageMetadata() throw(LanguageException)
    {
        //on parse les règles
        _languageMetadataBuffer->reset();

        while (_languageMetadataBuffer->hasData())
        {
            _parseUnnecessaryCharacters();

            if (_languageMetadataBuffer->hasData())
            {
                if (! _parseComment())
                {
                    _parseRule();
                }
            }
        }

        //debug
        //======================================================
        /*LMRules::iterator itRules;
        LMRuleDefinition::iterator itExpression;

        for (itRules = _rules.begin(); itRules != _rules.end(); itRules++)
        {
            cout << itRules->first << " ::= ";
            for (itExpression = itRules->second.begin(); itExpression != itRules->second.end(); itExpression++)
            {
                if (itExpression == itRules->second.begin())
                {
                    cout << "\"" << (*itExpression) << "\"";
                }
                else
                {
                    cout << ", \"" << (*itExpression) << "\"";
                }
            }
            cout << ";" << endl;
        }*/
        //======================================================
    }

    bool LanguageMetadataParser::_parseComment() throw(LanguageException)
    {
        //comment ::= '(*' .* '*)';

        //parse comment
        if (_parseString("(*"))
        {
            while (!_parseString("*)"))
            {
                if (!_languageMetadataBuffer->hasData())
                {
                    throw LanguageException(_languageIdentifier, tr("syntaxe incorrecte, symbole \"*)\" absent."));
                }

                _languageMetadataBuffer->nextData();
            }

            return true;
        }

        return false;
    }

    bool LanguageMetadataParser::_parseRule() throw(LanguageException)
    {
        //rule ::= rule-identifier unnecessary-character '::=' unnecessary-character rule-definition unnecessary-character ';';

        //variables
        LMRuleIdentifier ruleIdentifier;
        LMRuleDefinition ruleDefinition;

        if (_parseRuleIdentifier())
        {
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
                            throw LanguageException(_languageIdentifier, tr("euh ..."));
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
                            throw LanguageException(_languageIdentifier, tr("euh ..."));
                        }

                        //ajout de la règle dans le tableau des règles
                        _rules[ruleIdentifier] = ruleDefinition;

                        return true;
                    }
                    else
                    {
                        throw LanguageException(_languageIdentifier, _languageMetadataBuffer->getCurrentLine() - 1, tr("syntaxe incorrecte, symbole \";\" absent."));
                    }
                }
                else
                {
                    throw LanguageException(_languageIdentifier, _languageMetadataBuffer->getCurrentLine(), tr("syntaxe incorrecte, règle non défini après le symbole \"::=\"."));
                }
            }
            else
            {
                throw LanguageException(_languageIdentifier, _languageMetadataBuffer->getCurrentLine(), tr("syntaxe incorrecte, symbole \"::=\" absent."));
            }
        }

        return false;
    }

    bool LanguageMetadataParser::_parseRuleIdentifier() throw(LanguageException)
    {
        //rule-identifier ::= regex([a-z]+(-[a-z]+)*);

        //variables
        LMRuleIdentifier ruleIdentifier = "";
        char c;

        //on parse l'identifiant de la règle
        while (_languageMetadataBuffer->hasData())
        {
            ruleIdentifier += _languageMetadataBuffer->nextData();

            if (!_parseRegex(ruleIdentifier, "[a-z-]+"))
            {
                ruleIdentifier = ruleIdentifier.substr(0, ruleIdentifier.size() - 1);
                _languageMetadataBuffer->decrementIndex();
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
            throw LanguageException(_languageIdentifier, _languageMetadataBuffer->getCurrentLine(), tr("syntaxe incorrecte, \"%0\", la convention de nommage des identifiants des règles est un ensemble de mots (composé de une ou plusieurs lettres minuscules de a à z) séparé par des traits d'unions, ex: \"rule-identifier\".", ruleIdentifier));
        }

        //ajout à la pile d'indentifiants de règles
        _ruleIdentifierStack.push(ruleIdentifier);

        return true;
    }

    bool LanguageMetadataParser::_parseRuleDefinition() throw(LanguageException)
    {
        //rule-definition ::= expression ( unnecessary-character expression )*;

        //variables
        LMRuleDefinition ruleDefinition;
        LMExpression expression;

        //parse rule definition
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
                throw LanguageException(_languageIdentifier, tr("euh ..."));
            }

            //ajout de l'expression à la définition de la règle
            ruleDefinition.push_back(expression);
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
                throw LanguageException(_languageIdentifier, tr("euh ..."));
            }

            //ajout de l'expression à la définition de la règle
            ruleDefinition.push_back(expression);

            _parseUnnecessaryCharacters();
        }

        //ajout à la pile de définition de règle
        _ruleDefinitionStack.push(ruleDefinition);

        return true;
    }

    bool LanguageMetadataParser::_parseExpression() throw(LanguageException)
    {
        //expression ::= "'" .* "'" | '"' .* '"';

        //variable
        LMExpression expression = "";

        //on parse l'expression
        if (_parseCharacter('\'')) // "'" .* "'"
        {
            while (!_parseCharacter('\''))
            {
                if (!_languageMetadataBuffer->hasData())
                {
                    throw LanguageException(_languageIdentifier, tr("syntaxe incorrecte, symbole \"'\" absent."));
                }

                expression += _languageMetadataBuffer->nextData();
            }

            //vérification que l'expression n'est pas vide
            if (expression.empty())
            {
                throw LanguageException(_languageIdentifier, _languageMetadataBuffer->getCurrentLine(), tr("syntaxe incorrecte, il n'y a pas d'expression entre les symboles \"'\"."));
            }

            //ajout à la pile d'expression
            _expressionStack.push(expression);

            return true;
        }
        else if (_parseCharacter('"')) // '"' .* '"'
        {
            while (!_parseCharacter('"'))
            {
                if (!_languageMetadataBuffer->hasData())
                {
                    throw LanguageException(_languageIdentifier, tr("syntaxe incorrecte, symbole '\"' absent."));
                }

                expression += _languageMetadataBuffer->nextData();
            }

            //vérification que l'expression n'est pas vide
            if (expression.empty())
            {
                throw LanguageException(_languageIdentifier, _languageMetadataBuffer->getCurrentLine(), tr("syntaxe incorrecte, il n'y a pas d'expression entre les symboles '\"'."));
            }

            //ajout à la pile d'expression
            _expressionStack.push(expression);

            return true;
        }

        return false;
    }

    void LanguageMetadataParser::_parseUnnecessaryCharacters() throw(LanguageException)
    {
        //unnecessary-character ::= ( ' ' | '\t' | '\n' )*;

        while (_languageMetadataBuffer->hasData() && (_parseCharacter(' ') || _parseCharacter('\t') || _parseCharacter('\n')));
    }

    bool LanguageMetadataParser::_parseRegex(const string &str, const string &rstr)
    {
        boost::regex r(rstr);

        return (regex_match(str, r));
    }

    bool LanguageMetadataParser::_parseString(const string &str) throw(LanguageException)
    {
        for (unsigned int i = 0; i < str.size(); i++)
        {
            if (!_parseCharacter(str[i]))
            {
                for (unsigned int j = 0; j < i; j++)
                {
                    _languageMetadataBuffer->decrementIndex();
                }

                return false;
            }
        }

        return true;
    }

    bool LanguageMetadataParser::_parseCharacter(const char c) throw(LanguageException)
    {
        //parse character
        if (! _languageMetadataBuffer->hasData())
        {
            throw LanguageException(_languageIdentifier, _languageMetadataBuffer->getCurrentLine(), tr("syntaxe incorrecte, fin de fichier inattendu."));
        }

        if (_languageMetadataBuffer->nextData() != c)
        {
            _languageMetadataBuffer->decrementIndex();
            return false;
        }

        return true;
    }
}

