
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

#ajout des définitions
add_definitions(${AYEAYE_LANGUAGE_DIRECTORY_DEFINE})

#ajout des librairies
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

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

