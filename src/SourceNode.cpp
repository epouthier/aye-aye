
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

#include "SourceNode.h"

namespace ayeaye
{
    SourceNode::SourceNode(LSRuleIdentifier ruleIdentifier) :
        _ruleIdentifier(ruleIdentifier),
        _value("")
    {
    }

    SourceNode::~SourceNode()
    {
        //variable
        SourceNodeChildren::iterator itrChild;

        //destruction des noeuds enfants
        for (itrChild = _children.begin(); itrChild != _children.end(); itrChild++)
        {
            delete (*itrChild);
        }
    }

    void SourceNode::attachValue(const SourceNodeValue &value)
    {
        _value = value;
    }

    void SourceNode::attachChild(SourceNode *child)
    {
        _children.push_back(child);
    }
}
