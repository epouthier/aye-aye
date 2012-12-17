#!/bin/bash

#
# Copyright (C) 2013 Emmanuel Pouthier
#
# This file is part of Aye-Aye.
#
# Aye-Aye is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Aye-Aye is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

#path_exec
PATH_EXEC="${0%/pot.sh}";

#paramètres du script
if [ "$1" = "--help" ]
then
    echo -e "Utilisation: ./pot.sh \"SOURCES\" TARGET";
    echo "construit un fichier template pot à partir des sources"
    exit 0;
else
    SOURCES="$1"
    if [ -z "$SOURCES" ]
    then
        SOURCES="$PATH_EXEC/../src/*"
    fi

    TARGET="$2";
    if [ -z "$TARGET" ]
    then
        TARGET="$PATH_EXEC/../build/ayeaye.pot"
        mkdir -p $PATH_EXEC/../build;
    fi
fi

#script
PACKAGE="AYE-AYE";
VERSION="1.0";
POT_CREATION_DATE=`date +%F\ %R%z`;
CHARSET="UTF-8";

touch $TARGET;
if [ $? -gt 0 ]
then
    exit 1;
fi
$PATH_EXEC/copyright.sh --shell-comment > $TARGET;
echo "#, fuzzy" >> $TARGET;
echo "msgid \"\"" >> $TARGET;
echo "msgstr \"\"" >> $TARGET;
echo "\"Project-Id-Version: $PACKAGE $VERSION\n\"" >> $TARGET;
echo "\"Report-Msgid-Bugs-To: \n\"" >> $TARGET;
echo "\"POT-Creation-Date: $POT_CREATION_DATE\n\"" >> $TARGET;
echo "\"PO-Revision-Date: YEAR-MO-DA HO:MI+ZONE\n\"" >> $TARGET;
echo "\"Last-Translator: FULL NAME <EMAIL@ADDRESS>\n\"" >> $TARGET;
echo "\"Language-Team: LANGUAGE <EMAIL@ADDRESS>\n\"" >> $TARGET;
echo "\"Language: \n\"" >> $TARGET;
echo "\"MIME-Version: 1.0\n\"" >> $TARGET;
echo "\"Content-Type: text/plain; charset=$CHARSET\n\"" >> $TARGET;
echo "\"Content-Transfer-Encoding: 8bit\n\"" >> $TARGET;
echo "" >> $TARGET;

nbMsg=`grep -Eno "tr\(\".+\".*\)" $SOURCES | wc -l`;
declare -A msgs;
for (( i = $nbMsg ; i > 0 ; i-- ))
do
    msg=`grep -Eno "tr\(\".+\".*\)" $SOURCES | tail -n $i | head -n 1`;

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
    echo "#:${msgs["$msgid"]}" >> $TARGET;
    echo "msgid \"$msgid\"" >> $TARGET;
    echo "msgstr \"\"" >> $TARGET;
    echo "" >> $TARGET;
done

