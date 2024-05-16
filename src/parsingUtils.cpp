#include "../include/Server.hpp"

std::string skipWhiteSpace(std::string line)
{
	size_t i = 0;
	while (i < line.length() && (line[i] == ' ' || line[i] == '\t'))
		i++;
	return (line.substr(i));
}

bool containsOnlyNumbers(const std::string& str)
{
    if (str[str.length() - 1] == ';')
    {
        for (size_t i = 0; i < str.length() - 1; i++)
        {
            if (!std::isdigit(str[i]))
                return false;
        }
    }
    else
    {
        for (size_t i = 0; i < str.length(); i++)
        {
            if (!std::isdigit(str[i]))
                return false;
        }
    }
    return true;
}

std::string firstWord(std::string line)
{
    size_t i = 0;
    size_t e = 0;
    while (i < line.length() && (line[i] == ' ' || line[i] == '\t'))
        i++;
    e = i;
    while (i < line.length() && line[i] != ' ' && line[i] != '\t')
        i++;
    i--;
    return (line.substr(e, i));
}

int skipFirstWord(std::string line)
{
	size_t i = 0;
	while (i < line.length() && (line[i] == ' ' || line[i] == '\t'))
		i++;
	while (i < line.length() && (line[i] != ' ' && line[i] != '\t'))
		i++;
	while (i < line.length() && (line[i] == ' ' || line[i] == '\t'))
		i++;
	return (i);
}

int howManyServerName(std::string line)
{
	int count = 0;
	size_t i = 0;
	while (i < line.length())
	{
		while (line[i] == ' ' || line[i] == '\t')
            i++;
		if (line[i])	
            count++;
        while (line[i] != ' ' && line[i] != '\t' && line[i] != ';')
		    i++;
        if (line[i] == ';')
            break;
	}
	return (count);
}