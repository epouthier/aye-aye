#!/bin/bash

#
# Copyright (C) 2012 Emmanuel Pouthier
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
#

#update_pot
function update_pot {
    sources="$1";
    target="$2";

    touch $target;

    PACKAGE="AYE-AYE";
    VERSION="1.0";
    POT_CREATION_DATE=`date +%F\ %R%z`;
    CHARSET="UTF-8";

    echo "#" > $target;
    echo "# Copyright (C) 2012 Emmanuel Pouthier" >> $target;
    echo "#" >> $target;
    echo "# This program is free software: you can redistribute it and/or modify" >> $target;
    echo "# it under the terms of the GNU General Public License as published by" >> $target;
    echo "# the Free Software Foundation, either version 3 of the License, or" >> $target;
    echo "# (at your option) any later version." >> $target;
    echo "#" >> $target;
    echo "# This program is distributed in the hope that it will be useful," >> $target;
    echo "# but WITHOUT ANY WARRANTY; without even the implied warranty of" >> $target;
    echo "# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the" >> $target;
    echo "# GNU General Public License for more details." >> $target;
    echo "#" >> $target;
    echo "# You should have received a copy of the GNU General Public License" >> $target;
    echo "# along with this program. If not, see <http://www.gnu.org/licenses/>." >> $target;
    echo "#" >> $target;
    echo "#, fuzzy" >> $target;
    echo "msgid \"\"" >> $target;
    echo "msgstr \"\"" >> $target;
    echo "\"Project-Id-Version: $PACKAGE $VERSION\n\"" >> $target;
    echo "\"Report-Msgid-Bugs-To: \n\"" >> $target;
    echo "\"POT-Creation-Date: $POT_CREATION_DATE\n\"" >> $target;
    echo "\"PO-Revision-Date: YEAR-MO-DA HO:MI+ZONE\n\"" >> $target;
    echo "\"Last-Translator: FULL NAME <EMAIL@ADDRESS>\n\"" >> $target;
    echo "\"Language-Team: LANGUAGE <EMAIL@ADDRESS>\n\"" >> $target;
    echo "\"Language: \n\"" >> $target;
    echo "\"MIME-Version: 1.0\n\"" >> $target;
    echo "\"Content-Type: text/plain; charset=$CHARSET\n\"" >> $target;
    echo "\"Content-Transfer-Encoding: 8bit\n\"" >> $target;
    echo "" >> $target;

    nbMsg=`grep -Eno "tr\(\".+\".*\)" $sources | wc -l`;
    declare -A msgs;
    for (( i = $nbMsg ; i > 0 ; i-- ))
    do
        msg=`grep -Eno "tr\(\".+\".*\)" $sources | tail -n $i | head -n 1`;

        file=`echo $msg | cut -d":" -f1`;
        line=`echo $msg | cut -d":" -f2`;
        msgid="";

        extract=0;
        for (( j = 0 ; j < ${#msg} ; j++ ))
        do
            char=${msg:$j:1};

            if [ "$char" = "\"" ]
            then
                if [ "$lastchar" != "\\" ]
                then
                    if [ $extract -eq 1 ]
                    then
                        break;
                    fi
                fi
            fi

            if [ $extract -eq 1 ]
            then
                msgid="$msgid$char";
            fi

            if [ "$char" = "\"" ]
            then
                if [ $extract -eq 0 ]
                then
                    extract=1;
                fi
            fi

            lastchar="$char";
        done
        
        msgs[$msgid]="${msgs[$msgid]} $file:$line";
    done

    for msgid in "${!msgs[@]}";
    do
        echo "#:${msgs["$msgid"]}" >> $target;
        echo "msgid \"$msgid\"" >> $target;
        echo "msgstr \"\"" >> $target;
        echo "" >> $target;
    done
}

#ayeaye.pot
update_pot "../src/*.cpp ../src/*.hpp ../src/*.h" "../lang/ayeaye.pot";

