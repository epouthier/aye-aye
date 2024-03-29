
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

#ifndef _AYEAYE_SOURCE_NODE_H
#define _AYEAYE_SOURCE_NODE_H

    #include <iostream>
    #include <iterator>
    #include <list>
    #include <string>

    #include "LanguageStructure.h"

    using namespace std;

    namespace ayeaye
    {
        class SourceNode;

        typedef string SourceNodeValue;
        typedef list<SourceNode*> SourceNodeChildren;

        class SourceNode
        {
        private:
            /* Attributs */
            LSRuleIdentifier _ruleIdentifier = "";
            SourceNodeValue _value = "";
            SourceNodeChildren _children;

        public:
            /* Constructeur et destructeur */
            SourceNode(LSRuleIdentifier ruleIdentifier);
            ~SourceNode();

            /* Getters */
            LSRuleIdentifier &getRuleIdentifier() {return _ruleIdentifier;}
            SourceNodeValue &getValue() {return _value;}
            SourceNodeChildren &getChildren() {return _children;}

            /* Méthodes */
            void attachValue(const SourceNodeValue &value);
            void attachChild(SourceNode *child);
        };
    }

#endif

