
#ifndef _AYEAYE_LANGUAGE_STRUCTURE_H
#define _AYEAYE_LANGUAGE_STRUCTURE_H

	#include <iostream>
	#include <string>

	using namespace std;

	namespace ayeaye
	{
		typedef string LSRuleIdentifier;
		typedef string LSTerminalSymbol;

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

		enum LSUnaryExpressionType
		{
			LSUET_RULE_IDENTIFIER,
			LSUET_TERMINAL_SYMBOL
		};

		struct LSUnaryExpression
		{
			LSUnaryExpressionType type;
			union
			{
				LSRuleIdentifier ruleIdentifier;
				LSTerminalSymbol terminalSymbol;
				LSRepetitionSymbol repetitionSymbol;
			};
		};
	}

#endif

