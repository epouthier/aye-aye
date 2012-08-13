
#include "Localization.h"

namespace ayeaye
{
    string Localization::translate(string msgId)
    {
        return gettext(msgId.c_str());
    }
}

