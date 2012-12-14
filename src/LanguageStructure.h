
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

#ifndef _AYEAYE_LANGUAGE_STRUCTURE_H
#define _AYEAYE_LANGUAGE_STRUCTURE_H

	#include <iostream>
	#include <list>
    #include <map>
	#include <string>

	using namespace std;

	namespace ayeaye
	{
        //Repetition symbol
		enum LSRepetitionSymbol
		{
			LSRS_NO_REPETITION_SYMBOL,
			LSRS_ZERO_TO_N,
			LSRS_ONE_TO_N
		};


        //Rule indentifier
        typedef string LSRuleIdentifier;


        //Terminal symbol
		typedef string LSTerminalSymbol;


        //Interval Symbol
        typedef int LSCharacterCode;
        typedef pair<LSCharacterCode, LSCharacterCode> LSIntervalSymbol;
        typedef map<string, LSIntervalSymbol> LSPresetIntervalArray;

        class LSPresetInterval
        {
        private:
            /* Attributs */
            static LSPresetIntervalArray _presetIntervalArray;

        public:
            /* Getters */
            static LSPresetIntervalArray& getPresetIntervalArray() {return _presetIntervalArray;}
        };


        //Unary expression
		enum LSUnaryExpressionType
		{
			LSUET_RULE_IDENTIFIER,
			LSUET_TERMINAL_SYMBOL,
            LSUET_INTERVAL_SYMBOL,
            LSUET_JOKER_SYMBOL
		};

		struct LSUnaryExpression
		{
			LSUnaryExpressionType type;
			LSRuleIdentifier ruleIdentifier;
			LSTerminalSymbol terminalSymbol;
            LSIntervalSymbol intervalSymbol;
		};


        //Rule definition and expression
        enum LSExpressionType
        {
            LSET_UNARY_EXPRESSION,
            LSET_GROUP_EXPRESSION,
            LSET_OPTIONAL_EXPRESSION
        };

        struct LSExpression;

        typedef list<LSExpression> LSExpressionList;
        typedef list<LSExpressionList> LSRuleDefinition;

        struct LSExpression
        {
            LSExpressionType type;
            LSUnaryExpression unaryExpression;
            LSRuleDefinition ruleDefinition;
            LSRepetitionSymbol repetitionSymbol;
        };


        //Rule parameters
        struct LSRuleParameters
        {
            bool isValue = false;
        };


        //Rule
        struct LSRule
        {
            LSRuleParameters ruleParameters;
            LSRuleDefinition ruleDefinition;
        };

        typedef map<LSRuleIdentifier, LSRule> LSRules;
	}

#endif

