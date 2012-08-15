
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

