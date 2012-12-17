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
PATH_EXEC="${0%/copyright.sh}";

#paramètres du script
if [ "$1" = "--ayeaye-comment" ]
then
    COMMENT_TYPE=2; #0 => no comment, 1 => line comment, 2 => block comment
    COMMENT_START="(*";
    COMMENT_END="*)";
elif [ "$1" = "--c-comment" ]
then
    COMMENT_TYPE=2;
    COMMENT_START="/*";
    COMMENT_END="*/";
elif [ "$1" = "--shell-comment" ]
then
    COMMENT_TYPE=1;
    COMMENT_START="#";
elif [ "$1" = "--help" ]
then
    echo "Utilisation: ./copyright.sh [COMMENT_TYPE]";
    echo "écrit sur la sortie standard l'avis du copyright"
    echo "COMMENT_TYPE:";
    echo -e "  --ayeaye-comment\t\tl'avis est dans un block commentaire de type ayeaye";
    echo -e "  --c-comment\t\t\tl'avis est dans un block commentaire de type c";
    echo -e "  --shell-comment\t\tl'avis est dans des lignes de commentaires de type shell";
    exit 0;
else
    COMMENT_TYPE=0;
fi

#script
if [ $COMMENT_TYPE -eq 2 ]
then
    echo "$COMMENT_START";
    sed "s/^/ /" "$PATH_EXEC/../COPYRIGHT"
    echo "$COMMENT_END";
elif [ $COMMENT_TYPE -eq 1 ]
then
    echo "$COMMENT_START";
    sed "s/^/$COMMENT_START /" "$PATH_EXEC/../COPYRIGHT"
    echo "$COMMENT_START";
else
    cat "$PATH_EXEC/../COPYRIGHT";
fi

