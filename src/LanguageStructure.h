
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

	#include <boost/regex.hpp>

	using namespace std;

	namespace ayeaye
	{
		enum LSLogicalSymbol
		{
			LSLS_NO_LOGICAL_SYMBOL,
			LSLS_AND,
			LSLS_OR
		};

		enum LSRepetitionSymbol
		{
			LSRS_NO_REPETITION_SYMBOL,
			LSRS_ZERO_TO_N,
			LSRS_ONE_TO_N,
			LSRS_ZERO_OR_ONE
		};

        typedef string LSRuleIdentifier;
		typedef string LSTerminalSymbol;
		typedef boost::regex LSRegularExpression;

		enum LSUnaryExpressionType
		{
			LSUET_RULE_IDENTIFIER,
			LSUET_TERMINAL_SYMBOL,
			LSUET_REGULAR_EXPRESSION
		};

		struct LSUnaryExpression
		{
			LSUnaryExpressionType type;
			LSRuleIdentifier ruleIdentifier;
			LSTerminalSymbol terminalSymbol;
			LSRegularExpression regularExpression;
		};

        enum LSSubRuleDefinitionType
        {
            LSSRDT_UNARY_EXPRESSION,
            LSSRDT_GROUP_EXPRESSION,
            LSSRDT_OPTIONAL_EXPRESSION
        };

		struct LSSubRuleDefinition;

		typedef list<LSSubRuleDefinition> LSRuleDefinition;

        struct LSSubRuleDefinition
        {
            LSSubRuleDefinitionType type;
            LSUnaryExpression unaryExpression;
			LSRuleDefinition ruleDefinition;
            LSRepetitionSymbol repetionSymbol;
			LSLogicalSymbol logicalSymbol;
        };

        typedef map<LSRuleIdentifier, LSRuleDefinition> LSRules;
	}

#endif

