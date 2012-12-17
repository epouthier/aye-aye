
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



#========== CMAKE ===================================================================================

#version de cmake
cmake_minimum_required(VERSION 2.8)
cmake_policy(VERSION 2.8)

#====================================================================================================





#========== PROJET ==================================================================================

#déclaration du projet
project("Aye-Aye")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})

#options du projet
set(AYEAYE_LANGUAGE_DIRECTORY "/usr/share/aye-aye/languages" CACHE STRING "Language directory")
set(AYEAYE_LANGUAGE_DIRECTORY_DEFINE "-DAYEAYE_LANGUAGE_DIRECTORY=\"${AYEAYE_LANGUAGE_DIRECTORY}\"")

#====================================================================================================





#========== LIBRAIRIES ==============================================================================

#Boost
set(Boost_USE_STATIC_LIBS ON)
include(FindBoost)
find_package(
    Boost
    1.49
    COMPONENTS filesystem program_options regex system
    REQUIRED
)

#liblzma
include(FindLibLZMA)
find_package(
    LibLZMA
    5.1
    REQUIRED
)

#gettext
include(FindGettext)
find_package(
    Gettext
    0.18
    REQUIRED
)

#====================================================================================================





#========== OPTIONS DU COMPILATEUR ==================================================================

#-D
add_definitions(${AYEAYE_LANGUAGE_DIRECTORY_DEFINE})

#-I et -L
include_directories(
    ${Boost_INCLUDE_DIRS}
    ${LibLZMA_INCLUDE_DIRS}
    ${Gettext_INCLUDE_DIRS}
)

link_directories(
    ${Boost_LIBRARY_DIRS}
    ${LibLZMA_INCLUDE_DIRS}
    ${Gettext_LIBRARY_DIRS}
)

#autres options
set(CMAKE_CXX_FLAGS "--std=c++0x")

#====================================================================================================





#========== FICHIERS ================================================================================

#les sources
file(
	GLOB_RECURSE
	Sources
	src/*
)

#les outils
add_custom_target(pot)
add_custom_command(
    SOURCE pot
    TARGET pot
    COMMAND ${CMAKE_SOURCE_DIR}/tools/pot.sh
)

#====================================================================================================





#========== CONSTRUCTION ============================================================================

#compilation
add_executable(
	ayeaye
	${Sources}
)

#édition des liens
target_link_libraries(
	ayeaye
	${Boost_LIBRARIES}
    ${LibLZMA_LIBRARIES}
    ${Gettext_LIBRARIES}
)

#====================================================================================================

