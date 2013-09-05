/**
 * email:ji.jin.dragon@gmail.com
 * Author jijinlong
 * Data 2011.10.14~~~
 * All Rights Resvered
 ***/
#include "stdio.h"
#include "sstream"
#include "map"
#include "string"
#include "vector"
enum STATE{
	START,
	KEY,
	WAITEQUAL,
	WAITVALUE,
	VALUE,
};
class Prop{
public:
	Prop(const char *filename)
	{
		FILE *hF = fopen(filename,"r");
		if (hF)
		{
			while (!feof(hF))
			{
				char line[1025]={'\0'};
				char *res = fgets(line,1024,hF);
				//int res = fread(line,1,1024,hF);
				if (res)
				{
					parseLine(line,1024);
				}
			}
		}
		else
		{
			printf("cant find %s",filename);
		}
	}
	Prop()
	{
		
	}
	std::string getValue(std::string key)
	{
		std::vector<std::string> values = keyvalues[key];
		if (values.empty()) return "";
		return keyvalues[key][0];
	}
	template<class T>
	bool getValue(const char * key,T &value)
	{
		std::vector<std::string> values = keyvalues[key];
		if (values.empty()) return false;
		value = atoi( keyvalues[key][0].c_str());;
		return true;
	}
	bool getValue(const char *key,float &value)
	{
		std::vector<std::string> values = keyvalues[key];
		if (values.empty()) return false;
		value = atof( keyvalues[key][0].c_str());;
		return true;
	}
	std::vector<std::string> & getValues(std::string key)
	{
		return keyvalues[key];
	}
	void parse(std::string line)
	{
		parse(line.c_str(),line.size());
	}
	void parse(const char  * line,int size)
	{
		int i = 0;
		int startx = 0;
		bool null = true;
		while (*(line+i) && i < size)
		{
			const char * ptr = (line + i);
			if (*ptr == ' ' && null)
			{
				i++;
				continue;
			}
			null = false;
			if (*ptr == ';' || *ptr == ' ')
			{
				parseLine(line + startx, i - startx);
				startx = i+1;
				null = true;
			}
			i++;
		}
		if (startx < size && size > i)
			parseLine(line + startx,size - i);
	}
	void clear()
	{
		keyvalues.clear();
	}
private:
	void parseLine(const char *line,int size)
	{
		std::stringstream key;
		std::stringstream value;
		state = START;
		for (int i = 0;i < size && line[i];i++)
		{
			if (state == START)
			{
				if (checkSplit(line[i]))
				{
					state = START;
					if (line[i] == '#')
					{
						return;
					}
				}
				else
				{
					key<<line[i];
					state = KEY;
				}
				continue;
			}
			if (state == KEY)
			{
				if (checkSplit(line[i]))
				{
					// get key
					state = WAITEQUAL;
					if (line[i] == '=')
					{
						state = WAITVALUE;
					}
					continue;
				}
				key<<line[i];
				state = KEY;
				continue;
			}
			if (state == WAITEQUAL)
			{
				if (line[i] == '=')
				{
					state = WAITVALUE;
				}
				continue;
			}
			if (state == WAITVALUE)
			{
				if (checkSplit(line[i]))
				{
					continue;
				}
				else
				{
					value<<line[i];
					state = VALUE;
				}
				continue;
			}
			if (state == VALUE)
			{
				if (checkSplit(line[i]))
				{
					// get value
					std::string keys = key.str();
					std::string values = value.str();
					keyvalues[keys].push_back( values);
			//		printf("get key:%s value:%s",key.str().c_str(),value.str().c_str());
					return;
				}
				else
				{
					value<<line[i];
					state = VALUE;
				}
			}
		}
		std::string keys = key.str();
		std::string values = value.str();
		keyvalues[keys].push_back( values);
	}
	bool checkSplit(char c)
	{
		if (c == ' ' || c=='\n' ||c=='\t' || c == '='|| c=='#')
			return true;
		return false;
	}
	
	STATE state; // 1 ±íÊ¾startline
	std::map<std::string,std::vector<std::string> > keyvalues;
};