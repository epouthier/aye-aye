
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

template<typename... Params>
string ayeaye::Localization::translate(const string &msgId, const Params&... parameters)
{
	//variables
	size_t pos = 0;
	ostringstream oss;
	string pattern = "";

	//les paramètres
	int size = sizeof...(parameters);
	string array[sizeof...(parameters)] = {parameters...};

	//traduction du message
	string msgStr = gettext(msgId.c_str());

	//ajout des paramètres
	for (int i = 0; i < size; i++)
	{
		//création du pattern
		oss.str("");
		oss << i;
		pattern = "%" + oss.str();

		//remplacement du pattern par le paramètre
		pos = msgStr.find(pattern);
		while (pos != string::npos)
		{
			msgStr.erase(pos, pattern.size());
			msgStr.insert(pos, array[i]);

			pos = msgStr.find(pattern);
		}
	}

	return msgStr;
}

