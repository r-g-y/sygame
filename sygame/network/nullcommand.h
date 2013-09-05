#pragma once
#pragma pack(1)
/**
 * 空消息
 **/
struct stNullCmd{
	unsigned char msgType; // 消息头
	unsigned char type; // 消息类型1
	unsigned short param; // 消息类型2
	stNullCmd(){
		msgType = 0x80; // 最高位设置为1
		type = 0;
		param = 0;
	};
};
#pragma pack()

