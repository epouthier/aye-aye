
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

#include "SourceStructure.h"

namespace ayeaye
{
    SSNode::SSNode(LSRuleIdentifier ruleIdentifier) :
        _ruleIdentifier(ruleIdentifier),
        _value("")
    {
    }

    SSNode::~SSNode()
    {
        //variable
        list<SSNode*>::iterator itrNode;

        //destruction des noeuds enfants
        for (itrNode = _childNodes.begin(); itrNode != _childNodes.end(); itrNode++)
        {
            delete (*itrNode);
        }
    }

    void SSNode::attachChildNode(SSNode *childNode)
    {
        _childNodes.push_back(childNode);
    }

    void SSNode::attachValue(const SSValue &value)
    {
        _value = value;
    }
}

