
#ifndef _AYEAYE_LOCALIZATION_H
#define _AYEAYE_LOCALIZATION_H

    #include <iostream>
	#include <sstream>
    #include <string>

    #include <libintl.h>

    using namespace std;

    namespace ayeaye
    {
        class Localization
        {
        public:
            static string translate(const string &msgId);

			template<typename... Params>
            static string translate(const string &msgId, Params... parameters);
        };

        #define tr Localization::translate

		#include "Localization.hpp"
    }

#endif

