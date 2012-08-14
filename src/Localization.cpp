
#include "Localization.h"

namespace ayeaye
{
    string Localization::translate(const string &msgId)
    {
        return gettext(msgId.c_str());
    }
}

