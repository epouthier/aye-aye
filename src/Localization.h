
#ifndef _AYEAYE_LOCALIZATION_H
#define _AYEAYE_LOCALIZATION_H

    #include <iostream>
    #include <string>

    #include <boost/regex.hpp>
    #include <libintl.h>

    using namespace std;

    namespace ayeaye
    {
        class Localization
        {
        public:
            static string translate(const string &msgId);

            template<typename... Params>
            string translate(const string &msgId, Params... parameters);
        };

        #define tr(msgId) Localization::translate(msgId)
    }

#endif

