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
PATH_EXEC="${0%/clean.sh}";

#paramètres du script
if [ "$1" = "--all" ]
then
    ALL=1;
elif [ "$1" = "--help" ]
then
    echo "Utilisation: ./clean.sh [OPTION]";
    echo "supprime tous les fichiers temporaires"
    echo "OPTION:";
    echo -e "  --all\t\tsupprime aussi les fichiers construits";
    exit 0;
else
    ALL=0;
fi

#script
if [ $ALL -eq 1 ]
then
    rm -rf $PATH_EXEC/../build
    rm -f $PATH_EXEC/../ayeaye
fi

rm -f $PATH_EXEC/../*~
rm -f $PATH_EXEC/../docs/*~
rm -f $PATH_EXEC/../share/*~
rm -f $PATH_EXEC/../share/*/*~
rm -f $PATH_EXEC/../share/*/*/*~
rm -f $PATH_EXEC/../src/*~
rm -f $PATH_EXEC/../test/*~
rm -f $PATH_EXEC/../tools/*~

