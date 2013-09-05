#include "sys.h"
#include "package.h"
#include "zlib.h"

/**
 * 将报文打包
 * \param tag 报文tag
 * \param cmd 报文内容
 * \param size 报文内容大小
 * \return packsize 报文的大小
 */
int stPackContent::encode(stPackTag& ptag,void *cmd,int size)
{
	unsigned char tag = ptag.tag;
	int offset = 0;
	stPackTag packTag;
	packTag.tag = tag;
	offset += feedContent(&packTag,sizeof(packTag));
	// 先加hash
	if (tag & stPackTag::HASH_PACK)
	{
		stPack_hash hash;
		hash.hash_code = calcHash((char*) cmd,size);
		offset += feedContent(&hash,sizeof(hash));
	}
	std::vector<char> buf;
	// 再加密
	if (tag & stPackTag::DES_PACK) 
	{
		des(cmd,size,buf);
	}
	// 再压缩
	if (tag & stPackTag::ZLIB_PACK)
	{
		// 添加zip 的头
		stPack_zip ziphead;
		ziphead.sourceSize = size;
		offset += feedContent(&ziphead,sizeof(ziphead));
		zip(cmd,size,buf);
	}
	if (buf.size() == 0) return -1;

	// 处理报文头
	if (buf.size() < 255 )
	{
		content[0] |= stPackTag::MINI_PACK;
		stPackHead_min min;
		min.size = buf.size();
		offset += feedContent(&min,sizeof(min));
	}
	else if (buf.size() >= MAX_PACK_SIZE && (tag & stPackTag::MAX_PACK))
	{
		stPackHead_max max;
		max.size = buf.size();
		offset += feedContent(&max,sizeof(max));
	}
	else if (buf.size() >= MAX_PACK_SIZE )
	{
		content[0] |= stPackTag::SPLIT_PACK;
		// 处理报文分割
		offset += splitContent(&buf[0],buf.size());
		return offset;
	}
	else if (buf.size() <= MAX_PACK_SIZE)
	{
		content[0] |= stPackTag::NORMAL_PACK;
		stPackHead_none none;
		none.size = buf.size();
		offset += feedContent(&none,sizeof(none));
	}
	
	offset += feedContent(&buf[0],buf.size());
	return offset;
}
/**
 * 解析buffer 中的内容到pack
 * \param buffer 缓存首地址
 * \param startPos 开始解析的地址
 * \param allSize 缓存的大小
 * \return 当前处理到的位置 -1 表述出错
 */
int stPackContent::decode(char *buffer,int startPos,unsigned int allsize)
{
	// 完成解包 解压缩 解密 报文重组
	int offset = startPos;
	unsigned int needPackSize = sizeof(stPackTag);
	unsigned int startPackPos = 0;
	offset = eatContent(buffer,startPos,offset,needPackSize,allsize);

	if (content.size() < needPackSize)
	{
		return offset;
	}
	//已经获取到tag了 处理几种不同的报文
	stPackTag * tag =(stPackTag*) &content[startPackPos];
	startPackPos += sizeof(stPackTag);
	// 如果存在hash校验 
	stPack_hash *hash = NULL;
	if ((tag->tag & stPackTag::HASH_PACK) == 1)
	{
		//stPack_hash 就要接受hash
		needPackSize += sizeof(stPack_hash);

		offset = eatContent(buffer,startPos,offset,needPackSize,allsize);// 看必要吃缓存

		if (content.size() < needPackSize)
		{
			return offset;
		}
		hash = (stPack_hash*)&content[startPackPos];
		startPackPos += sizeof(stPack_hash);
	}
	stPack_zip *zip = NULL;
	if ((tag->tag & stPackTag::ZLIB_PACK) == 1) // 如果是zlib 报文 那么空出int 来存放实际数据的大小
	{
		needPackSize += sizeof(stPack_zip);
		offset = eatContent(buffer,startPos,offset,needPackSize,allsize);
		if (content.size() < needPackSize)
		{
			return offset;
		}
		zip = (stPack_zip*) & content[startPackPos];
		startPackPos += sizeof(stPack_zip);
	}
	// 除了分割报文外其他的报文内容长度都一样
	unsigned int contentSize = 0;
	if (tag->tag & stPackTag::MINI_PACK)
	{
		needPackSize += sizeof(stPackHead_min);
		offset = eatContent(buffer,startPos,offset,needPackSize,allsize); // 看必要吃缓存

		if (content.size() < needPackSize)
		{
			return offset;
		}
		stPackHead_min *none = (stPackHead_min*) &content[startPackPos];
		// 获取到pack的头
		if (none->size >= MAX_PACK_SIZE)
		{
			setBad();
			return offset;
		}
		needPackSize +=  none->size;
		// 获取到pack的头
		if (none->size >= MAX_PACK_SIZE)
		{
			setBad();
			return offset;
		}
		contentSize = none->size;
		needPackSize +=  none->size;
		startPackPos += sizeof(stPackHead_min);
	}
	else if (tag->tag & stPackTag::NORMAL_PACK)
	{
		needPackSize += sizeof(stPackHead_none);
		
		offset = eatContent(buffer,startPos,offset,needPackSize,allsize); // 看必要吃缓存

		if (content.size() < needPackSize)
		{
			return offset;
		}
		stPackHead_none *none = (stPackHead_none*) &content[startPackPos];
		// 获取到pack的头
		if (none->size >= MAX_PACK_SIZE)
		{
			setBad();
			return offset;
		}
		needPackSize +=  none->size;
		startPackPos += sizeof(stPackHead_none);
		contentSize = none->size;
	}
	else if (tag->tag & stPackTag::MAX_PACK)
	{
		needPackSize += sizeof(stPackHead_max);
		
		offset = eatContent(buffer,startPos,offset,needPackSize,allsize); // 看必要吃缓存

		if (content.size() < needPackSize)
		{
			return offset;
		}
		stPackHead_max *none = (stPackHead_max*) &content[startPackPos];
		// 获取到pack的头
		if (none->size >= MAX_PACK_SIZE)
		{
			setBad();
			return offset;
		}
		needPackSize +=  none->size;
		startPackPos += sizeof(stPackHead_max);
		contentSize = none->size;
	}
	else if ((tag->tag & stPackTag::SPLIT_PACK) == 1)
	{
		needPackSize += sizeof(stPackHead_split);
		
		offset = eatContent(buffer,startPos,offset,needPackSize,allsize); // 看必要吃缓存

		if (content.size() < needPackSize)
		{
			return offset;
		}
		stPackHead_split *none = (stPackHead_split*) &content[startPackPos];
		// 获取到pack的头
		if (none->size >= MAX_PACK_SIZE)
		{
			setBad();
			return offset;
		}
		needPackSize +=  none->size;
		// 获取到pack的头
		if (none->size >= MAX_PACK_SIZE)
		{
			setBad();
			return offset;
		}
		needPackSize +=  none->size;
		startPackPos += sizeof(stPackHead_split);

		offset = eatContent(buffer,startPos,offset,needPackSize,allsize);// 看必要吃缓存
		if (content.size() < needPackSize)
		{
			return offset;
		}
		
		if (none->split_id >= MAX_PACK_SIZE || none->split_id >= (none->all_size / MAX_PACK_SIZE + 1))
		{
			setBad();
			return offset;
		}
		if (none->split_id >= splitpacks.size()) splitpacks.resize(none->split_id + 1);
		stPackContent pack;
		pack.content = this->content;
		pack.contentStartPos = startPackPos;
		splitpacks[none->split_id] = pack;
		setOk();
		contentStartPos = startPackPos; // 记录逻辑层数据的起始位置
		if (splitpacks.size() == none->all_size / MAX_PACK_SIZE + 1)
		{
			// 开始合并报文
			mergePackets();
			contentSize = none->all_size;
		}
		else
		{
			return offset;
		}
	}
	if (!((tag->tag & stPackTag::SPLIT_PACK)))
	{
		offset = eatContent(buffer,startPos,offset,needPackSize,allsize);// 看必要吃缓存
		if (content.size() < needPackSize)
		{
			return offset;
		}
	}
	if (tag->tag & stPackTag::HASH_PACK)
	{
		// 先进行验证
		if (hash)
		{
			if (hash->hash_code != calcHash(&content[startPackPos],contentSize))
			{
				setBad();
				return offset;
			}
		}
		else
		{
			setBad();
			return offset;
		}
	}
	// 逻辑层数据在 content[startPackPos];
	if (tag->tag& stPackTag::ZLIB_PACK)
	{
		// 先处理解压
		if (!zip)
		{
			setBad();
			return offset;
		}
		if (!unZip(startPackPos,contentSize,zip->sourceSize)) // 就知道改解压的数据块
		{
			setBad();
			return offset;
		}
	}
	if (tag->tag & stPackTag::DES_PACK)
	{
		// 处理解密
		unDes(startPackPos,contentSize); // 该解密的数据块
	}
	setOk();
	contentStartPos = startPackPos;
	return offset;
}
/**
 * \param offset 当前的缓存偏移
 * \param buffer 当前的缓存
 * \param needPackSize 当前的所需要的Content的长度
 * \param allsize 当前缓存的大小
 * \return 新的偏移
 */
int stPackContent::eatContent(char *buffer,int startPos,int offset,unsigned int needPackSize,unsigned int allsize)
{
	if (content.size() < needPackSize)
	{
		startPos = offset;
		offset = needPackSize - content.size();
		if (offset < allsize - startPos)
			offset = allsize - startPos;
		content.insert(content.end(),buffer+startPos,buffer + startPos + offset);
		offset += startPos;
	}
	return offset;
}
/**
 * 清除解析
 */
void stPackContent::reset()
{
	packageState = PACK_NULL;
	content.clear();
}
/**
*当前报文是ok的
 * \return 报文ok true false 报文还待接受数据
 */
bool stPackContent::isOk()
{
	return (PACK_OK == packageState);
}
/**
 * 当前报文是bad 的
  * \return true报文 坏了
 */
bool stPackContent::isBad()
{
	return (PACK_BAD == packageState);
}
/**
 * 设置是好报文
 */
void stPackContent::setOk()
{
	packageState = PACK_OK;
}
/**
 * 设置是坏报文
 */
void stPackContent::setBad()
{
	packageState = PACK_BAD;
}
/**
 * des 解密
 * \param startPos,当前缓存的起始地址
 * \param size 大小
 */
void stPackContent::unDes(unsigned int startPos,unsigned int size)
{
	CEncrypt enc;
	enc.setEncMethod(CEncrypt::ENCDEC_DES);
	enc.set_key_des(&key);
	enc.encdec(&content[startPos],size,true);
}
/**
 * zip 解压
 * \param startPos 当前缓存的起始地址
 * \param size 大小
 * \param sourceSize 源大小
 */
bool stPackContent::unZip(unsigned int startPos,unsigned int size,unsigned int sourceSize)
{
	std::vector<char> buf;
	buf.resize(sourceSize);
	uLongf destLen = sourceSize;
	int  zOk = uncompress((Bytef*)&buf[0], &destLen,(Bytef*) &content[startPos], size);
	if (zOk == Z_OK && destLen == sourceSize)
	{
		content.resize(startPos + destLen);
		bcopy(&buf[0],&content[startPos],destLen);
		return true;
	}
	else
	{
		return false;
	}
}
/**
 * des 加密
 * \param buffer 待加密的buffer
 * \param bufferSize buffer的大小
 * \param out 输出
 */
void stPackContent::des(void *buffer,unsigned int bufferSize,std::vector<char> &out)
{
	CEncrypt enc;
	out.resize(bufferSize);
	enc.setEncMethod(CEncrypt::ENCDEC_DES);
	enc.set_key_des(&key);
	enc.encdec(&out[0],out.size(),true);
}
/**
 * 设置连接的密钥
 */
void stPackContent::setKey(const_ZES_cblock &key)
{
	bcopy(&key[0],&this->key[0],sizeof(const_ZES_cblock));
}
/**
 * zip 压缩
 * \param buffer 待压缩的buffer
 * \param bufferSize buffer 的大小
 * \param out 输出
 */
void stPackContent::zip(void *buffer,unsigned int bufferSize,std::vector<char> &out)
{
	out.resize((bufferSize + 12)*1.5);
	uLongf destLen = out.size();
	int zRes = compress((Bytef *)&out[0], &destLen,(Bytef *)buffer,bufferSize);
	if (zRes == Z_OK)
		out.resize(destLen);
}
/**
 * 计算hash 值
 * \param buffer 缓存开始
 * \param bufferSize 缓存大小
 * \return hash 值
 */
unsigned int stPackContent::calcHash(char *buffer,unsigned int bufferSize)
{
	unsigned int hash = 0;
	int i; 
	for (i=0; *buffer && i < bufferSize; i++)
	{
		if ((i & 1) == 0)
		{
			hash ^= ((hash << 7) ^ (*buffer++) ^ (hash >> 3));
		}
		else
		{
			hash ^= (~((hash << 11) ^ (*buffer++) ^ (hash >> 5)));
		}
	}

	return (hash & 0x7FFFFFFF); 
}
/**
 * 填充报文
 * \param content 填充内容的地址
 * \param 填充内容的大小
 * \return 获取的当前的content的大小
 */
int stPackContent::feedContent(void *content,unsigned int size)
{
	this->content.insert(this->content.end(),(char*)content,(char*)content+size);
	return this->content.size();
}
/**
 * 填充报文
 * \param pack 报文
 * \param content 填充内容的地址
 * \param 填充内容的大小
 * \return 获取的当前的content的大小
 */
int stPackContent::feedContent(stPackContent& pack,void *content,unsigned int size)
{
	pack.content.insert(pack.content.end(),(char*)content,(char*)content+size);
	return pack.content.size();
}
/**
 * 处理报文分割
 * \param buffer带分割内容的起始地址
 * \param bufferSize 内容长度
 * \return 报文的大小
 */
int stPackContent::splitContent(void *buffer,unsigned int bufferSize)
{
	if (bufferSize < MAX_PACK_SIZE) return -1;
	// 当前content 有hash + tag 了 无头
	stPackHead_split head;
	head.all_size = bufferSize;
	unsigned int packCount = bufferSize / MAX_PACK_SIZE;
	int offset = 0;
	unsigned int index = 0;
	for ( index = 0; index < packCount;index++)
	{
		head.size = MAX_PACK_SIZE;
		head.split_id = index;
		stPackContent buf;
		feedContent(buf,&content[0],content.size());
		feedContent(buf,&head,sizeof(head));
		feedContent(buf,((char*)buffer)+offset,head.size);
		splitpacks.push_back(buf);
		offset += head.size;
	}
	head.size = bufferSize % MAX_PACK_SIZE;
	if (head.size)
	{
		head.size = MAX_PACK_SIZE;
		head.split_id = index;
		stPackContent buf;
		feedContent(buf,&content[0],content.size());
		feedContent(buf,&head,sizeof(head));
		feedContent(buf,((char*)buffer)+offset,head.size);
		splitpacks.push_back(buf); // 得到子块数
		offset += head.size;
	}
	content.clear();
	return offset;
}

/**
 * 合并报文
 */
void stPackContent::mergePackets()
{
	unsigned int offset = contentStartPos;
	for(int index = 0;index < splitpacks.size();index++)
	{
		stPackContent &pack = splitpacks[index];
		if (pack.content.size() > pack.contentStartPos)
		{
			if (offset + pack.content.size() - pack.contentStartPos < content.size())
			{
				content.resize(offset + pack.content.size() - pack.contentStartPos);
			}
			bcopy(&pack.content[pack.contentStartPos],&content[offset],pack.content.size() - pack.contentStartPos);
		}
	}
}

/**
 * 检查是否存在子块
 * \return true 存在子块 false 不存在子块
 */
bool stPackContent::hadChildPack()
{
	return splitpacks.size();
}

/**
 * 获取逻辑层的首指针
 */
void *stPackContent::getContentPointer()
{
	if (contentStartPos >= content.size()) return NULL;
	return &content[contentStartPos];
}
/**
 * 获取内容的大小
 */
unsigned int stPackContent::getContentSize()
{
	return content.size() - contentStartPos;
}