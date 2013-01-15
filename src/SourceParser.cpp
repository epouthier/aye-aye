
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

#include "SourceParser.h"

namespace ayeaye
{
    SourceParser::SourceParser() :
        _sourceIdentifier(""),
        _sourceBuffer(nullptr),
        _sourceLanguage(nullptr),
        _separatorParsing(false)
    {
    }

    void SourceParser::parseSource(const string &sourceIdentifier, FileBuffer *sourceBuffer, Language *sourceLanguage) throw(SourceException)
    {
        //initialisation des variables
        _sourceIdentifier = sourceIdentifier;
        _sourceBuffer = sourceBuffer;
        _sourceLanguage = sourceLanguage;
        _separatorParsing = false;

        //parsage
        _parseSource();
    }

    //debug
    //===================================================================================
    void printRootNode(SourceNode *rootNode, unsigned int heightNode)
    {
        for (unsigned int i = heightNode; i > 0; i--) { cout << " "; }
        cout << rootNode->getRuleIdentifier();
        if (! rootNode->getValue().empty()) { cout << " = \"" << rootNode->getValue() << "\""; }
        cout << endl;

        SourceNodeChildren::iterator itrChild;
        for (itrChild = rootNode->getChildren().begin(); itrChild != rootNode->getChildren().end(); itrChild++)
        {
            printRootNode(*itrChild, heightNode + 1);
        }
    }
    //===================================================================================

    void SourceParser::_parseSource() throw(SourceException)
    {
        //variable
        SourceNode* rootNode = nullptr;

        //initialisation du noeud principale
        rootNode = new SourceNode(_sourceIdentifier);

        /*//initialisation du buffer
        _sourceBuffer->reset();

        //suppression des commentaires
        */

        //reinitialisation du buffer
        _sourceBuffer->reset();

        //parsage de la règle principale
        if (!_parseRule(rootNode, _sourceLanguage->getLanguageIdentifier()))
        {
            //traitement des erreurs
            throw SourceException(_sourceIdentifier, _sourceBuffer->getCurrentLine(), tr("syntaxe incorrecte, la source ne correspond pas au langage défini."));
        }

        //debug
        //======================================================
        printRootNode(rootNode, 0);
        //======================================================

        //destruction du noeud principale
        delete rootNode;
    }

    bool SourceParser::_parseRule(SourceNode *rootNode, const LSRuleIdentifier &ruleIdentifier) throw(SourceException)
    {
        //variable
        SourceNode *ruleNode = nullptr;
        SourceNodeValue ruleValue = "";

        //initialisation du noeud
        ruleNode = new SourceNode(ruleIdentifier);

        if (_parseRuleDefinition(ruleNode, ruleValue, _sourceLanguage->getLanguageStructure()[ruleIdentifier].ruleDefinition))
        {
            //ajout de la valeur au noeud
            if (_sourceLanguage->getLanguageStructure()[ruleIdentifier].ruleParameters.isValue)
            {
                ruleNode->attachValue(ruleValue);
            }

            //ajout du noeud au noeud parent
            rootNode->attachChild(ruleNode);

            return true;
        }

        //destruction du noeud
        delete ruleNode;

        return false;
    }

    bool SourceParser::_parseRuleDefinition(SourceNode *ruleNode, SourceNodeValue &ruleValue, const LSRuleDefinition &ruleDefinition) throw(SourceException)
    {
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

    bool SourceParser::_parseExpressionList(SourceNode *ruleNode, SourceNodeValue &ruleValue, const LSExpressionList &expressionList) throw(SourceException)
    {
        //variables
        LSExpressionList::const_iterator itExpression;
        bool result, joker = false;
        pair<unsigned long, unsigned int> saveState;

        //sauvegarde du buffer
        saveState = _sourceBuffer->saveState();

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
                //restoration du buffer
                _sourceBuffer->restoreState(saveState);

                return false;
            }
        }

        return true;
    }

    bool SourceParser::_parseExpression(SourceNode *ruleNode, SourceNodeValue &ruleValue, const LSExpression &expression, bool withSeparator) throw(SourceException)
    {
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

    bool SourceParser::_parseUnaryExpression(SourceNode *ruleNode, SourceNodeValue &ruleValue, const LSUnaryExpression &unaryExpression, bool withSeparator) throw(SourceException)
    {
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

    bool SourceParser::_parseJokerSymbol(SourceNodeValue &ruleValue) throw(SourceException)
    {
        //variable
        char c;

        //traitement des erreurs
        if (!_sourceBuffer->hasData())
        {
            throw SourceException(_sourceIdentifier, _sourceBuffer->getCurrentLine(), tr("syntaxe incorrecte, fin de fichier inattendu."));
        }

        //parse joker symbol
        c = _sourceBuffer->nextData();

        //construction de la valeur
        ruleValue += c;

        return true;
    }

    bool SourceParser::_parseIntervalSymbol(SourceNodeValue &ruleValue, const LSIntervalSymbol &intervalSymbol) throw(SourceException)
    {
        //variable
        char c;

        //traitement des erreurs
        if (!_sourceBuffer->hasData())
        {
            throw SourceException(_sourceIdentifier, _sourceBuffer->getCurrentLine(), tr("syntaxe incorrecte, fin de fichier inattendu."));
        }

        //parse interval symbol
        c = _sourceBuffer->nextData();

        if (!((c >= intervalSymbol.first) && (c <= intervalSymbol.second)))
        {
            _sourceBuffer->decrementIndex();
            return false;
        }

        //construction de la valeur
        ruleValue += c;

        return true;
    }

    bool SourceParser::_parseTerminalSymbol(SourceNodeValue &ruleValue, const LSTerminalSymbol &terminalSymbol) throw(SourceException)
    {
        //parse terminal symbol
        for (unsigned int i = 0; i < terminalSymbol.size(); i++)
        {
            if (!_parseCharacter(terminalSymbol[i]))
            {
                for (unsigned int j = 0; j < i; j++)
                {
                    _sourceBuffer->decrementIndex();
                }

                return false;
            }
        }

        //construction de la valeur
        ruleValue += terminalSymbol;

        return true;
    }

    bool SourceParser::_parseSeparator(SourceNode *ruleNode, SourceNodeValue &ruleValue) throw(SourceException)
    {
        //variable
        bool result = false;

        //lock separator parsing
        _separatorParsing = true;

        //traitement du separator custom
        if (_sourceLanguage->getLanguageStructure().find("separator") != _sourceLanguage->getLanguageStructure().end())
        {
            result = _parseRuleDefinition(ruleNode, ruleValue, _sourceLanguage->getLanguageStructure()["separator"].ruleDefinition);
        }
        else
        {
            //traitement du separator default
            result = (_parseCharacter(' ') || _parseCharacter('\t') || _parseCharacter('\n'));
        }

        //unlock separator parsing
        _separatorParsing = false;

        return result;
    }

    bool SourceParser::_parseCharacter(const char c) throw(SourceException)
    {
        //parse character
        if (!_sourceBuffer->hasData())
        {
            throw SourceException(_sourceIdentifier, _sourceBuffer->getCurrentLine(), tr("syntaxe incorrecte, fin de fichier inattendu."));
        }

        if (_sourceBuffer->nextData() != c)
        {
            _sourceBuffer->decrementIndex();
            return false;
        }

        return true;
    }
}

