/**
 * email:jjl_2009_hi@163.com
 * Author jijinlong
 * Data 2011.10.14~2012.3.18~~
 * All Rights Resvered
 ***/


#include "sString.h"
#include "serialize.h"
namespace sys{
	/*
	 * 
	 **/
	sString::sString(const char*content)
	{
		if (!content)
		{
			contents.push_back('\0');
			hash_code = 0;
			return;
		}
		int len = mstrlen(content);
		if (len == 0)
		{
			contents.push_back('\0');
			hash_code = 0;
			return;
		}
		contents.resize(len);
		bcopy(content,&this->contents[0],contents.size());
		contents.push_back('\0');
		hash_code = HashStr::calc(&contents[0]);
	}
	sString::sString(std::string& content)
	{
		contents.resize(content.size());
		bcopy(content.c_str(),&this->contents[0],contents.size());
		contents.push_back('\0');
		hash_code = HashStr::calc(&contents[0]);
	}
#ifdef _USE_ICONV
	sString sString::toGB2312()
	{
		sString to;
		iconv_t cd;
		if (!contents.size())
		{
			return to;
		}
		to.contents.resize(contents.size());
		char *temp =& contents[0];
		char **pin =&temp;
		char *temp1 = &to.contents[0];
		char **pout=&temp1;
		cd = iconv_open("gb2312","utf-8");
		if(!cd) 
		{
			return to;
		}
		size_t outlen = contents.size();
		size_t inlen = contents.size();
		if (iconv(cd,pin,&inlen,pout,&outlen) == -1) to.contents.resize(1);
		iconv_close(cd);
		to.hash_code = HashStr::calc(&to.contents[0]);

		return to;
	}
#endif
	sString sString::toXmlString()
	{
		sString str;
		for (unsigned int i =0 ; i < contents.size();i++)
		{
			if (contents[i] == '[')
			{
				str.contents.push_back('<');
				continue;
			}
			if (contents[i] == ']')
			{
				str.contents.push_back('>');
				continue;
			}
			if (contents[i] == '\\')
			{
				if (i + 1 < contents.size())
				{
					if (contents[i+1] == '[')
					{
						str.contents.push_back('[');
						i++;
						continue;
					}
					if (contents[i+1] == ']')
					{
						str.contents.push_back(']');
						i++;
						continue;
					}
				}
			}
			str.contents.push_back(contents[i]);
		}
		str.hash_code = HashStr::calc(&str.contents[0]); 
		return str;
	}
	void sString::split(const char *str,std::vector<std::string>& strs)
	{
		std::vector<char> tempStr;
		for (unsigned int i = 0;i < contents.size();i++)
		{
			bool flag = false;
			const char *temp = str;
			while (*temp++ == contents[i]) 
			{
				if (tempStr.size())
				{
					tempStr.push_back('\0');
					std::string temp(tempStr.begin(),tempStr.end());
					strs.push_back(temp);
					tempStr.clear();
				}
				flag = true;
				break;
			}
			if (!flag)
			tempStr.push_back(contents[i]);
		}
		if (tempStr.size())
		{
			tempStr.push_back('\0');
			std::string temp(&tempStr[0]);
			strs.push_back(temp);
		}
	}
	// 
	void sString::trim()
	{
		if (contents.empty()) return;
		unsigned int i = (unsigned int) contents.size() - 1;
		while ( (
				contents[i]=='\0' || 
				contents[i]==' ' ||
				contents[i]=='\n' ||
				contents[i]=='\t'))
		{
			contents.pop_back();
			i --;
		}
		std::vector<char> temp;
		for (std::vector<char>::iterator iter = contents.begin(); iter !=  contents.end(); ++iter)
		{
			if ( *iter=='\0' || 
				 *iter==' ' ||
				 *iter=='\n' ||
				 *iter=='\t')
			{
				continue;
			}
			temp.insert(temp.begin(),iter,contents.end());
			break;
		}
		contents = temp;	
		contents.push_back('\0');
		hash_code = HashStr::calc(&contents[0]);
	}
	unsigned int sString::toBinary(void * buf,unsigned int size)
	{
		size_t realSize =contents.size() > size? size:contents.size();
		bcopy(&contents[0],buf,realSize);
		return (unsigned int)realSize;
	}
	unsigned int sString::size()
	{
		return (unsigned int)contents.size();
	}
	std::string sString::toString()
	{
		if (!contents.size()) return "";
		return std::string(str());
	}
	void sString::printBinary(const char *value,int size)
	{
		for (unsigned int i =0; i < size;i++)
			printf("%x",*value++);
		printf("\n ??ก่????:%u\n",size);
	}
	sString::sString(const char *content,int size)
	{
		if (!content) return;
		contents.resize(mstrnlen(content,size));
		bcopy(content,&this->contents[0],contents.size());
		contents.push_back('\0');
		hash_code = HashStr::calc(&contents[0]);
	}
	sString &sString::operator = (const sString &content)
	{
		contents.resize(content.contents.size());
		bcopy(&content.contents[0],&this->contents[0],contents.size());
		hash_code = HashStr::calc(&contents[0]);
        return *this;
	}
	bool sString::operator == (const sString &content)
	{
		if (hash_code != content.hash_code) return false;
		return cmp(*this,content);
	}
	
	bool sString::operator != (const sString &content)
	{
		if (hash_code != content.hash_code) return true;
		return !cmp(*this,content);
	}
	sString sString::operator +(const sString &content)
	{
		sString str;
		str.contents.resize(contents.size() + content.contents.size() -1);
		bcopy(&contents[0],&str.contents[0],content.contents.size()-1);
		bcopy(&content.contents[0],&str.contents[content.contents.size()-1],content.contents.size());
		return str;
	}
	
	sString& sString::operator += (const sString &content)
	{
		contents.pop_back();
		contents.insert(contents.end(),content.contents.begin(),content.contents.end());
		return *this;
	}
	const char *sString::str()
	{
		if (!contents.size()) return "";
		if (contents[contents.size()-1]!='\0') contents[contents.size()-1] = '\0';
		return &contents[0];
	}
	void sString::append(char c)
	{
		contents.pop_back();
		contents.push_back(c);
		contents.push_back('\0');
		hash_code = HashStr::calc(&contents[0]);
	}
	void sString::append(const char *str)
	{
		contents.pop_back();
		unsigned int len = mstrlen(str);
		contents.insert(contents.end(),str,str+len);
		contents.push_back('\0');
		hash_code = HashStr::calc(&contents[0]);
	}
	void sString::clear()
	{
		contents.clear();
		hash_code = 0;
	}
	bool sString::cmp(const sString & s1,const sString &s2 ) 
	{
		if (s1.contents.size() != s2.contents.size())
		{
			return false;
		}
		for (int i = 0; i < s1.contents.size();i++)
		{
			if (s1.contents[i] != s2.contents[i])
			{
				return false;
			}
		}
		return true;
	}
	int sString::mstrlen(const char *str)
	{
		int i = 0;
		for ( i =0; str && *str != '\0'; ++str,++i){}
		return i;
	}

	int sString::mstrnlen(const char *str,int size)
	{
		int i = 0;
		for ( i =0; str && *str != '\0' && i < size; ++str,++i){}
		return i;
	}
	bool sString::equal(const char *str)
	{
		sys::sString temp(str);
		return (temp == *this);
	}
	int HashStr::calc(const char* str) {
		unsigned int hash = 0;
		int i; 
		for (i=0; *str && i < 256; i++)
		{
			if ((i & 1) == 0)
			{
				hash ^= ((hash << 7) ^ (*str++) ^ (hash >> 3));
			}
			else
			{
				hash ^= (~((hash << 11) ^ (*str++) ^ (hash >> 5)));
			}
		}
 
		return (hash & 0x7FFFFFFF); 	
	}
	Stream sString::toRecord()
	{
		Stores sts;
		sts.addStore(&contents[0],(unsigned int)contents.size(),0);
		return sts.toRecord();
	}	

	bool sString::parseRecord(Stream &record)
	{
		Stores sts;
		sts.parseRecord(record);
		Store store;
		if (record.pickStore(store) && store.equal(0))
		{
			if (store.coreData.size)
			{
				bcopy(&store.content[0],&contents[0],store.coreData.size);
				return true;
			}
		}
		return false;
	}
};
