#pragma once
#include "des.h"
#include <vector>
#pragma pack(1)
/**
 * 当前的报文包
 */
struct stPackTag{
	unsigned char tag; // 报文标识
	enum{
		NULL_PACK = 0, // 普通非加工报文
		DES_PACK = 1 << 0, // 加密后的报文
		ZLIB_PACK = 1 << 1, // 当前报文是压缩报文
		SPLIT_PACK = 1 << 2, // 当前是分割的报文
		HASH_PACK = 1 << 3, // 加了hash校验
		MINI_PACK = 1 << 4, // 报文字节数小于 256 的 >= 256 不将使用该报文
		MAX_PACK = 1 << 5,// 极大报文
		NORMAL_PACK = 1 << 6, // 普通报文
	};
};
/*
 *可变头 hash 当是验证报文时 hash 头存在
 */
struct stPack_hash{
	unsigned int hash_code;
};
/*
 *可变头 zip 当时zip 报文时 zip 头存在
 */
struct stPack_zip{
	unsigned int sourceSize;
};
struct stPackHead_min{
	unsigned char size; // 迷你报文大小,不使用zip 
};
struct stPackHead_none{
	unsigned short size; // 普通报文 长度必然小于 2^16
};
struct stPackHead_max{
	unsigned int size; // 系统允许的最大单个报文 希望是群组内使用
};
struct stPackHead_split{
	unsigned short size; // 子块的实际数据大小
	unsigned int all_size; // 块的总大小
	unsigned short split_id;// 当前的子块号
};
#pragma pack()
/**
 * 获取断流数据
 **/
struct stPackContent{
	stPackContent(){
		packageState = PACK_NULL;
	};
	std::vector<char> content; // 当前待接受的子块
	unsigned int contentStartPos; // 逻辑层数据起始位置
	std::vector<stPackContent> splitpacks; // 大包下的子块
	/**
	 *  解析buffer 中的内容到pack
	 * \param buffer 缓存首地址
	 * \param startPos 开始解析的地址
	 * \param allSize 缓存的大小
	 * \return 当前处理到的位置
	 */
	int decode(char *buffer,int startPos,unsigned int allsize);
	/**
	 * 清除解析
	 */
	void reset();
	/**
	 * 将报文打包
	 * \param tag 报文tag
	 * \param cmd 报文内容
	 * \param size 报文内容大小
	 * \return packsize 报文的大小
	 */
	int encode(stPackTag& tag,void *cmd,int size);
	/**
	 * 当前报文是ok的
	 * \return 报文ok true false 报文还待接受数据
	 */
	bool isOk();
	/**
	 * 当前报文是bad 的
	  * \return true报文 坏了
	 */
	bool isBad();

	/**
	 * 检查是否存在子块
	 * \return true 存在子块 false 不存在子块
	 */
	bool hadChildPack();
	/**
	 * 获取逻辑层的首指针
	 */
	void *getContentPointer();
	/**
	 * 获取内容的大小
	 */
	unsigned int getContentSize();
	/**
	 * 设置连接的密钥
	 */
	void setKey(const_ZES_cblock &key);
private:
	enum{
		PACK_NULL = 0, // 未标识报文
		PACK_OK = 1 << 1, // 好报文
		PACK_BAD =1 << 2 , // 坏报文
	};
	enum{
		MAX_PACK_SIZE = 0xffffffff - 1,
	};
	int packageState; // 指示pack 已经接受好了
	const_ZES_cblock key; //报文的key值
	/**
	 * 设置是好报文
	 */
	void setOk();
	/**
	 * 设置是坏报文
	 */
	void setBad();
	/**
	 * \param offset 当前的缓存偏移
	 * \param buffer 当前的缓存
	 * \param needPackSize 当前的所需要的Content的长度
	 * \param allsize 当前缓存的大小
	 * \out offset 新的偏移
	 */
	int eatContent(char *buffer,int startPos,int offset,unsigned int needPackSize,unsigned int allsize);
	/**
	 * 填充报文
	 * \param content 填充内容的地址
	 * \param 填充内容的大小
	 * \return 获取的当前的content的大小
	 */
	int feedContent(void *content,unsigned int size);
	/**
	 * 填充报文
	 * \param pack 报文
	 * \param content 填充内容的地址
	 * \param 填充内容的大小
	 * \return 获取的当前的content的大小
	 */
	int feedContent(stPackContent& pack,void *content,unsigned int size);
	/**
	 * 计算hash 值
	 * \param buffer 缓存开始
	 * \param bufferSize 缓存大小
	 * \return hash 值
	 */
	unsigned int calcHash(char *buffer,unsigned int bufferSize);
	/**
	 * des 解密
	 * \param startPos,当前缓存的起始地址
	 */
	void unDes(unsigned int startPos,unsigned int size);
	/**
	 * zip 解压
	 * \param startPos 当前缓存的起始地址
	 */
	bool unZip(unsigned int startPos,unsigned int size,unsigned int sourceSize);
	/**
	 * des 加密
	 * \param buffer 待加密的buffer
	 * \param bufferSize buffer的大小
	 * \param out 输出
	 */
	void des(void *buffer,unsigned int bufferSize,std::vector<char> &out);
	/**
	 * zip 压缩
	 * \param buffer 待压缩的buffer
	 * \param bufferSize buffer 的大小
	 * \param out 输出
	 */
	void zip(void *buffer,unsigned int bufferSize,std::vector<char> &out);
	/**
	 * 处理报文分割
	 * \param buffer带分割内容的起始地址
	 * \param bufferSize 内容长度
	 * \return 报文的大小
	 */
	int splitContent(void *buffer,unsigned int bufferSize);

	/**
	 * 合并报文
	 */
	void mergePackets();
};