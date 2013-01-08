
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

#include "Parameters.h"

namespace ayeaye
{
    Parameters::Parameters(int argc, char **argv) throw(ParametersException) :
        _sourceDirectory(""),
        _languageDirectory(AYEAYE_LANGUAGE_DIRECTORY)
    {
        //variables
        const option longOptions[] = {{"help", no_argument, nullptr, 'h'},
                                      {"version", no_argument, nullptr, 'v'},
                                      {"source-directory", required_argument, nullptr, 'S'},
                                      {"language-directory", required_argument, nullptr, 'L'},
                                      {nullptr, 0, nullptr, 0}};

        const char *longOptionsStr = "hvS: L: ";

        int longOptionsIndex = 0, opt = 0;

        //aucune option
        if (argc <= 1)
        {
            _showHelp();
            exit(0);
        }

        //analyse des options
        while (true)
        {
            opt = getopt_long(argc, argv, longOptionsStr, longOptions, &longOptionsIndex);

            if (opt == (-1))
            {
                break;
            }

            switch (opt)
            {
                case 'h': //--help ou -h
                    _showHelp();
                    exit(0);
                break;
                case 'v': //--version ou -v
                    _showVersion();
                    exit(0);
                break;
                case 'S': //--source-directory=<directory> ou -S<directory>
                    _sourceDirectory = optarg;
                break;
                case 'L': //--language-directory=<directory> ou -L<directory>
                    _languageDirectory = optarg;
                break;
                case 0:
                break;
                default:
                    exit(1);
                break;
            }
        }

        //analyse des sources
        while (optind < argc)
        {
            _sources.push_back(argv[optind]);
            optind++;
        }
    }

    void Parameters::_showHelp()
    {
        cout << tr("Utilisation : ayeaye [OPTIONS] SOURCES") << endl
             << tr("Options :") << endl
             << " -h, --help                                        " << tr("Affiche l'aide") << endl
             << " -v, --version                                     " << tr("Affiche le numéro de version") << endl
             << " -S<directory>, --source-directory=<directory>     " << tr("Définie le répertoire des sources") << endl
             << " -L<directory>, --language-directory=<directory>   " << tr("Définie le répertoire des langages") << endl;
    }

    void Parameters::_showVersion()
    {
        cout << "Aye-Aye version " << AYEAYE_VERSION << endl
             << "Copyright (C) 2013 Emmanuel Pouthier" << endl
             << endl
             << "Aye-Aye is free software: you can redistribute it and/or modify" << endl
             << "it under the terms of the GNU General Public License as published by" << endl
             << "the Free Software Foundation, either version 3 of the License, or" << endl
             << "(at your option) any later version." << endl
             << endl
             << "Aye-Aye is distributed in the hope that it will be useful," << endl
             << "but WITHOUT ANY WARRANTY; without even the implied warranty of" << endl
             << "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the" << endl
             << "GNU General Public License for more details." << endl
             << endl
             << "You should have received a copy of the GNU General Public License" << endl
             << "along with this program.  If not, see <http://www.gnu.org/licenses/>." << endl;
    }
}

