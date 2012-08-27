
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

