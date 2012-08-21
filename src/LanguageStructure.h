
#ifndef _AYEAYE_LANGUAGE_STRUCTURE_H
#define _AYEAYE_LANGUAGE_STRUCTURE_H

	#include <iostream>
    #include <map>
	#include <string>

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

		enum LSUnaryExpressionType
		{
			LSUET_RULE_IDENTIFIER,
			LSUET_TERMINAL_SYMBOL
		};

		struct LSUnaryExpression
		{
			LSUnaryExpressionType type;
			LSRuleIdentifier ruleIdentifier;
			LSTerminalSymbol terminalSymbol;
			LSRepetitionSymbol repetitionSymbol;
		};

        enum LSSubRuleDefinitionType
        {
            LSSRDT_UNARY_EXPRESSION,
            LSSRDT_GROUP_EXPRESSION,
            LSSRDT_OPTIONAL_EXPRESSION
        };

        struct LSSubRuleDefinition
        {
            LSSubRuleDefinitionType type;
            LSUnaryExpression unaryExpression;
            LSSubRuleDefinition &subRuleDefinition;
            LSRepetitionSymbol repetionSymbol;
        };

        typedef vector<pair<LSLogicalSymbol, LSSubRuleDefinition>> LSRuleDefinition;
        typedef map<LSRuleIdentifier, LSRuleDefinition> LSRule;
	}

#endif

