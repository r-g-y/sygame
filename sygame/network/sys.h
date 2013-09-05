/**
 * email:jjl_2009_hi@163.com
 * Author jijinlong
 * Data 2011.10.14~2012.3.18~~
 * All Rights Resvered
 ***/
/**
 * 该文件将平台相关的内容提供一致的访问接口,包括
 * 线程的使用
 * 网络的使用
 * 时间的使用
 * 随机的使用
 * */

#pragma once
#include "list"
#include "vector"
#include "string"
#include <stdarg.h>
#include "stdio.h"
#include <time.h>
#include "stdlib.h"
#include "errno.h"
#include "set"
#include "port.h"

#define MAX_SELECT_COUNT 1000
#define MAX_EPOLL_COUNT 4000
#define BEGIN_TAG(name) 

namespace serialize{
	struct Stream;
	class Stores;
}
namespace sys{
	/*
	* bytes
	**/
	typedef std::vector<unsigned char> BYTES;
	typedef std::vector<unsigned char>::iterator BYTES_ITER;
	/**
	 * 封装时间值
	 */
	struct stTimeValue{
		time_t second; // 距离1970 年的秒数
		unsigned int msecond; // 毫秒数
		stTimeValue()
		{
			second = 0;
			msecond = 0;
		}
		bool operator < (stTimeValue & value)
		{
			if (second < value.second) return true;
			if (second == value.second)
			{
				if (msecond < value.msecond) return true;
			}
			return false;
		}
		bool operator > (stTimeValue &value)
		{
			if (second > value.second) return true;
			if (second == value.second)
			{
				if (msecond > value.msecond) return true;
			}
			return false;
		}
		bool operator <= (stTimeValue & value)
		{
			if (second <= value.second) return true;
			if (msecond <= value.msecond) return true;
			return false;
		}
		bool operator != (stTimeValue & value)
		{
			if (second != value.second || msecond != value.msecond) return true;
			return false;
		}
		bool operator == (stTimeValue & value)
		{
			if (second == value.second && msecond == value.msecond) return true;
			return false;
		}
		bool operator >= (stTimeValue & value)
		{
			if (second >= value.second) return true;
			if (msecond >= value.msecond) return true;
			return false;
		}
		stTimeValue & operator = (int value)
		{
			second = value / 1000;
			msecond = value % 1000;
			return *this;
		}
		DYNAMIC_API bool parseRecord(serialize::Stream &record);
		DYNAMIC_API serialize::Stream toRecord();
		DYNAMIC_API bool parseRecord(serialize::Stores& sts);
		/**
		 * 增加的毫秒数
		 */
		stTimeValue operator+(unsigned int value)
		{
			stTimeValue result;
			result.second = second + value / 1000;
			result.msecond = msecond + value % 1000;
			result.second += result.msecond  / 1000;
			result.msecond = result.msecond  % 1000;
			return result;
		}
		stTimeValue  operator+(stTimeValue &value)
		{
			stTimeValue result;
			result.second = second + value.second;
			result.msecond = msecond + value.msecond;
			result.second += result.msecond / 1000;
			result.msecond = result.msecond % 1000;
			return result;
		}
		stTimeValue  operator-(stTimeValue &value)
		{
			stTimeValue result;
			result.second = second - value.second;
			result.msecond = msecond - value.msecond;
			result.second += result.second + result.msecond / 1000;
			result.msecond = result.msecond % 1000;
			return result;
		}
		stTimeValue & operator=(const stTimeValue &value)
		{
			second = value.second;
			msecond = value.msecond;
			return *this;
		}
		/**
		 * 秒数
		 */
		/*stTimeValue & operator=(int value)
		{
			second = value;
			return *this;
		}*/
	};
	/*
	 * 定时器
	 */
	struct sTime{
		/**
 		 * 获取当前时间
 		 * \return 毫秒数
 		 * */
		DYNAMIC_API static stTimeValue getNowTime();
		/**
		 * 获取当前秒数
		 * \return 当前秒数
		 **/
		DYNAMIC_API static time_t getNowSec();
		/**
 		 * 使用间隔构造函数
 		 * \param tick 时间间隔
 		 * */
		DYNAMIC_API sTime(int tick)
		{
			startTime = getNowTime();
			this->tick = tick;
		}
		DYNAMIC_API sTime()
		{
			tick = 0;
		}
		
		/**
 		 * 检查当前定时是否超时
 		 * */
		DYNAMIC_API bool checkOver();
		/**
 		 * 等待 time 时间
 		 * \param 等待时间 
 		 ***/
		static void wait(unsigned int time);
		
		int tick; // 定时器时间间隔
		stTimeValue startTime; // 定时开启时间,tick 过后更新
		static long long debugNowTime;
	};
	/**
 	 * 分析时间
 	 * */
	class AynaTime{
	public:
		/**
 		 * 构造时间 
 		 * \param 毫秒
 		 * */
		DYNAMIC_API AynaTime(sys::stTimeValue nowTime)
		{
			ayna(nowTime.second);	
		}
        DYNAMIC_API AynaTime()
        {
            ayna(time(NULL));
        }
		/*
 		 * 分析时间 得到 日期
 		 * \param nowTime 秒数
         * \ 时间都是从1 开始计数
 		 * **/
		DYNAMIC_API void ayna(time_t nowTime);
		unsigned short week; // 星期几 从1开始计数
		unsigned short day; // 天数
		unsigned short year; // 年数 19xx
		unsigned short month; // 月份 从1 开始计数
		unsigned int sec; // 秒数
		unsigned int mday; // 几号
		unsigned int hour; // 几时
		unsigned int min; // 几分
        
        /**
         * 将字符串解析为当前秒数
         **/
        DYNAMIC_API static unsigned int parseTime(const char *content);
		/**
		 * 当前时间转化为日期
		 */
		DYNAMIC_API std::string toString();
	};
	/*
	 * 随机数生成 
	 **/
	struct sRandom{
		/**
 		 * 是否是概率
 		 * \param value 概率值 
 		 * \return 是否 > value % 100
 		 * */
		static bool isPercent(unsigned int value);
		
		/*
 		 * 生成 min max 之间的随机数
 		 * \return 随机数
 		 * **/
		static unsigned int randBetween(unsigned int min,unsigned int max);
	};
	/*
 	* \brief 定时器回调
	**/	
	struct stTimeTickCallback{
		/*
 		 * 定时器回调
 		 * \param delay 当前tick 时间
 		 * **/
		virtual void tick(unsigned int delay){}
	};
	/**
 	 * 线程实现
 	 * */
	class ThreadManager;
	class Thread{
		friend class ThreadManager;
	public:
		/**
 		 * 线程停止
 		 * */
		virtual void stop(){_stop = true;wait();}
		/**
 		 * 线程开启
 		 * */
		bool start(bool wait = true);
		/**
 		 * 等待线程结束
 		 * */
		void wait();
		/**
 		 * 线程运作
 		 * */
		virtual void run(){}
		/**
 		 * 检查线程是否在运行
 		 * */
		bool isActive(){return !_stop;}
		/**
 		 * 线程构造函数
 		 * */
		Thread(){_stop = true;}
		~Thread(){}
	private:
		/**
 		 * 线程运作函数
 		 * */
		static void * threadWork(void*arg);
#if __LINUX__ || __MAC__
		pthread_t id; // 线程di
#else
		void* hThread;
		int id;
#endif
	protected: 
		bool _stop; // 标示是否运行
	};
	/**
	* \brief 临界区，封装了系统临界区，避免了使用系统临界区时候需要手工初始化和销毁临界区对象的操作
	*
	*/
	class Mutex 
	{

	public:
		DYNAMIC_API Mutex() ;

		/**
		* \brief 析构函数，销毁一个互斥体对象
		*
		*/
		DYNAMIC_API ~Mutex();

		/**
		* \brief 加锁一个互斥体
		*
		*/
		DYNAMIC_API inline void lock();

		/**
		* \brief 解锁一个互斥体
		*
		*/
		DYNAMIC_API inline void unlock();
#if __LINUX__ || __MAC__

		pthread_mutex_t _mutex;
#else
		void* m_hMutex;    /**< 系统互斥体 */
#endif
	};
	class ThreadManager{
	public:
		static ThreadManager *threadM;
		DYNAMIC_API static ThreadManager& getMe()
		{
			return *threadM;
		}
		DYNAMIC_API static void setThreadManager(ThreadManager* tm)
		{
			threadM = tm;
		}
		/**
		 * 等待线程结束 并作清除操作
 		 * */
		DYNAMIC_API void waitAll();
		/**
 		 * 停止所有线程
 		 * */
		DYNAMIC_API void stopAll();
		DYNAMIC_API void addThread(Thread * thread);
		DYNAMIC_API ThreadManager(){
		}
	private:
		std::set<Thread*> threads;
		
		Mutex _mutex;
	};
    /**
     * 解析ip 地址
     **/
    class IpUtil{
    public:
        /**
         * 根据唯一名字 获取 IP
         **/
        static std::string getIP(std::string uniqueName);
        /**
         * 根据唯一名字 获取Port
         **/
        static unsigned short getPort(std::string uniqueName);
        /**
         * 使用唯一名称解析出ip.port
         **/
        IpUtil(std::string unqiueName);
        std::string ip; // ip地址
        unsigned short port; // 端口
		static void split(const char * string,std::vector<std::string>& strings,const char* tag=" ")
		{
			char *nexttoken = NULL;
			char buf[1024] = {'\0'};
			strncpy(buf,string,1024);
			char *token = strtok_s(buf,tag,&nexttoken);
			while ( token != NULL)
			{
				strings.push_back(token);
				token = strtok_s(nexttoken,tag,&nexttoken);
			}
		}
    
	};
	DYNAMIC_API extern sys::ThreadManager theThreadManager;
	

	class Socket{
	public:
		Socket();
		~Socket();
		int send(void *buf,int size);
		int recv(void *buf,int size);
		void close();
		void shutdown();
		int connect(const sockaddr *name,int namelen);
#if __LINUX__ || __MAC__
		int getHandle(){return _socket;}
		void setHandle(int _socket)
		{
			if (checkValid())
			{
				close();
			}
			this->_socket = _socket;
		}
#else
		unsigned int getHandle(){return _socket;}
		void setHandle(SOCKET _socket)
		{
			if (checkValid())
			{
				close();
			}
			this->_socket = _socket;
		}
#endif
		void bind(const sockaddr *name,int namelen);
		void listen(int size);
		bool checkValid();
		void setnonblocking();
	private:
#if __LINUX__ || __MAC__
		int _socket;
#else
		unsigned int _socket;
#endif
	};
	
	/**
 	 * 提供一个简单的日志输出,系统
 	 *  只打印到文件内，文件名按天存储
 	 * */
	class Logger{
	public:
		/**
 		 * \param 日志名
 		 * */
		Logger(const char *name);
		/**
 		 * 对象释放
 		 * */
		~Logger();
		/**
 	 	 * 打印出内容
 	 	 * \param pattern 格式化字符串
 	 	 * */
		void trace(const char * pattern,...);
		/**
 		 * 情况内存缓存
 		 * */
		void flush();
	private:
		unsigned int _yday; // 多少天
		std::string _loggerFile; // 日志名字
		unsigned int _bufferSize; // 日志缓存大小
		std::vector<std::string> _contents; // 日志缓存
		FILE * _logFileHandle; // 日志文件句柄
	};
	/**
	 * 打印进入函数
	 */
	class EnterFunction{
	public:
		EnterFunction(std::string funcName,std::string fileName,int line)
		{
			this->funcName = funcName;
			this->fileName = fileName;
			this->line = line;
			printf("\n enter [%s] \n", funcName.c_str());
		}

		~EnterFunction()
		{
			printf("\n leave [%s] \n", funcName.c_str());
		}
		std::string fileName;
		std::string funcName;
		int line;
	};
#define ENTER_FUNCTION\
	sys::EnterFunction function##__LINE__(__FUNCTION__,__FILE__,__LINE__);
	
	/**
	 * 系统中外放到脚本的接口,主要是时间 和 随机
	 */
	class SysLuaLib{
	public:
#ifdef __USE_LUA_EXT
		static void addLibExt(lua_State* L);
#endif
	};
};
