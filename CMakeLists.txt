
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

#vérification cmake
cmake_minimum_required(VERSION 2.8)
cmake_policy(VERSION 2.8)

#déclaration du projet
project("Aye-Aye" CXX)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})

#options du projet
set(AYEAYE_LANGUAGE_DIRECTORY "/usr/share/aye-aye/languages" CACHE STRING "Language directory")
set(AYEAYE_LANGUAGE_DIRECTORY_DEFINE "-DAYEAYE_LANGUAGE_DIRECTORY=\"${AYEAYE_LANGUAGE_DIRECTORY}\"")

#vérification de boost
#set(Boost_USE_STATIC_LIBS ON)
include(FindBoost)
find_package(
    Boost
    REQUIRED
	filesystem
	program_options
	regex
	system
)

#vérification de gettext
include(FindGettext)
find_package(
    Gettext
    REQUIRED
)

#ajout des définitions
add_definitions(${AYEAYE_LANGUAGE_DIRECTORY_DEFINE})

#ajout des librairies
include_directories(
    ${Boost_INCLUDE_DIRS}
    ${Gettext_INCLUDE_DIRS}
)
link_directories(
    ${Boost_LIBRARY_DIRS}
    ${Gettext_LIBRARY_DIRS}
)

#option du compilateur
set(CMAKE_CXX_FLAGS "--std=c++0x")

#génération de la liste des fichiers sources
file(
	GLOB_RECURSE
	Sources
	src/*
)

#déclaration de l'exécutable
add_executable(
	ayeaye
	${Sources}
)

#édition des liens
target_link_libraries(
	ayeaye
	${Boost_LIBRARIES}
)

