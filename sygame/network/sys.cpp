/**
 * email:ji.jin.dragon@gmail.com
 * email:jjl_2009_hi@163.com
 * Author jijinlong
 * Data 2011.10.14~2012.3.18~~
 * All Rights Resvered
 ***/

#include "sys.h"
#include "time.h"
#include "sstream"
#include "serialize.h"
using namespace serialize;
namespace sys{
	bool stTimeValue::parseRecord(Stream &record)
	{
		return false;
	}
	Stream stTimeValue::toRecord()
	{
		Stream ss;
		return ss;
	}
	bool stTimeValue::parseRecord(Stores& sts)
	{
		return false;
	}
#ifdef __WINDOWS__
	int gettimeofday(struct timeval *tp, void *tzp)
	{
		time_t clock;
		struct tm tm;
		SYSTEMTIME wtm;

		GetLocalTime(&wtm);
		tm.tm_year     = wtm.wYear - 1900;
		tm.tm_mon     = wtm.wMonth - 1;
		tm.tm_mday     = wtm.wDay;
		tm.tm_hour     = wtm.wHour;
		tm.tm_min     = wtm.wMinute;
		tm.tm_sec     = wtm.wSecond;
		tm. tm_isdst    = -1;
		clock = mktime(&tm);
		tp->tv_sec = clock;
		tp->tv_usec = wtm.wMilliseconds * 1000;

		return (0);
	}
#endif

	long long sTime::debugNowTime = 0;
	/*
	 *获取当前时间 单位为毫秒 只处理非脚本时间
	 */
	stTimeValue sTime::getNowTime(){ // ms
//		if (debugNowTime) return debugNowTime;
#ifdef _USE_CLOCK_
		struct timespec tv;
		clock_gettime(CLOCK_REALTIME,&tv);
		long long retval = 0;
		retval += tv.tv_sec *1000;
		retval += tv.tv_nsec / 1000000L;
		return retval;
#else
		struct timeval tv;
		gettimeofday(&tv,NULL);
		stTimeValue value;
		value.second = tv.tv_sec;
		value.msecond = tv.tv_usec / 1000L;
		return value;
#endif
	}
    /**
     * 获取秒数
     * \return 秒数
     */
	time_t sTime::getNowSec()
	{
		time_t timep;
		struct tm *p;
		time(&timep);
		p=localtime(&timep);
		timep = mktime(p);
		return timep; // 返回本地时间
	}
    /**
     * 检查当前设定的定时是否超时
     */
	bool sTime::checkOver()
	{
		if ( 0 == tick) return false;
		stTimeValue nowTime = getNowTime();
		if (startTime + tick < nowTime)
		{
			startTime = nowTime;
			return true;
		}
		return false;
	}
    /**
     * 等待time 微妙 0.001 毫秒
     */
	void sTime::wait(unsigned int time)
	{
#if __LINUX__ || __MAC__
		::usleep(time);
#else
		Sleep(time);
#endif
	}
	
    /**
     * 解析时间 获取天 年 日 月 秒 周天
     */
	void AynaTime::ayna(time_t nowTime)
	{
		struct tm * t = localtime((time_t*) &nowTime);
		if (t)
		{
			week = t->tm_wday;//? t->tm_wday:7;
			sec = t->tm_sec;
			day = t->tm_yday;
			year = t->tm_year + 1900;
			month = t->tm_mon + 1;
			mday = t->tm_mday;
			hour = t->tm_hour;
			min = t->tm_min;
		}
	}
	unsigned int AynaTime::parseTime(const char *content)
    {
        struct tm t;
		bzero(&t,sizeof(t));
        time_t time;
        sscanf(content, "%d-%d-%d %d:%d:%d",&t.tm_year,&t.tm_mon,&t.tm_mday,&t.tm_hour,&t.tm_min,&t.tm_sec);
        t.tm_year-=1900;
        t.tm_mon-=1;
        time=mktime(&t);      //转换
        return time;
    }
	/**
	 * 当前时间转化为日期
	 */
	std::string AynaTime::toString()
	{
		char content[1024];
		time_t nowTime = time(NULL);
		struct tm t;
#ifdef __WINDOWS__
		localtime_s(&t,&nowTime);
#else
		localtime_r(&nowTime,&t);
#endif
		sprintf(content, "%d-%d-%d %d:%d:%d",t.tm_year + 1900,t.tm_mon + 1,t.tm_mday,t.tm_hour,t.tm_min,t.tm_sec);
		return std::string(content);
	}
	/*
	 * 随机数生成 
	 **/
	bool sRandom::isPercent(unsigned int value)
	{
		DWORD temp = randBetween(0,100);
		if (temp < value) return true;
		return false;
	}
    /**
     * 获取随机数 在 min max 之间 可以取到 min max
     */
	unsigned int sRandom::randBetween(unsigned int min,unsigned int max)
	{
		static sTime tick = sTime(3);
		static unsigned int rand_seed = (int)time(NULL);
		if (tick.checkOver())
		{
			rand_seed = (unsigned int) time(NULL);
		}
		if (min == max) return min;
		else if (min > max)
#ifdef __LINUX__
			return max + (DWORD)(((double) min - (double)  max + 1.0) * rand_r(&rand_seed) / (RAND_MAX+1.0));	
#else	
			return max + (DWORD)(((double) min - (double)  max + 1.0) * rand() / (RAND_MAX+1.0));
#endif	
		else
#ifdef __LINUX__
			return min + (DWORD)(((double) max - (double)  min + 1.0) * rand_r(&rand_seed) / (RAND_MAX+1.0));	
#else		
			return max + (DWORD)(((double) min - (double)  max + 1.0) * rand() / (RAND_MAX+1.0));
#endif
	}
	sys::ThreadManager theThreadManager;
    /**
     * 开启线程
     **/
	bool Thread::start(bool wait)
	{
		_stop = true;
#if __LINUX__ || __MAC__
		int ret = pthread_create(&id,NULL,&Thread::threadWork,this);
		if (ret == -1) 
		{
			printf("thread create error!\n");
			return false;
		}
#else
		hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)threadWork,(LPVOID)this,0,(LPDWORD)&id);
#endif
	//	printf("thread start!\n");
		_stop = false;
		if (wait)
			theThreadManager.addThread(this);
		return true;
	}
    /**
     * 线程等待
     **/
	void Thread::wait()
	{
#if __LINUX__ || __MAC__ 
		pthread_join(id,NULL);
#else
		if (hThread)
			WaitForSingleObject(hThread,-1);
#endif
	}
    /**
     * 线程工作者
     **/
	void * Thread::threadWork(void *arg)
	{
		Thread *t = (Thread*)arg;
		if (!t)
			return 0;
		t->_stop = false;
		t->run();
		t->_stop = true;
		//printf("pthread exit\n");
#if __LINUX__ || __MAC__
		pthread_exit(NULL);
#else
	 	CloseHandle(t->hThread);
		t->hThread = NULL;
#endif
		return 0;
	}
    /**
     * 等待线程结束
     **/
	void ThreadManager::waitAll()
	{
		_mutex.lock();
		//printf("thread size():%lu\n",threads.size());
		for (std::set<Thread*>::iterator iter = threads.begin();
			iter != threads.end();++iter)
		{
			if (*iter/* && (*iter)->_stop != true*/)
			{
		//		printf("[thread]wait...\n");
				(*iter)->wait();	
			}
		}	
		threads.clear();
		_mutex.unlock();
	}
	Mutex::Mutex() 
	{
#if __LINUX__ || __MAC__ 
	//	pthread_mutexattr_t attr;
	//	::pthread_mutexattr_init(&attr);
	//	::pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_FAST_NP);
		::pthread_mutex_init(&_mutex,NULL);
#else
		 m_hMutex = CreateMutex(NULL,FALSE,NULL);
#endif	
	}

	/**
	* \brief 析构函数，销毁一个互斥体对象
	*
	*/
	Mutex::~Mutex()
	{
#if __LINUX__ || __MAC__
		::pthread_mutex_destroy(&_mutex);
#else
		CloseHandle(m_hMutex);	
#endif	
	}

	/**
	* \brief 加锁一个互斥体
	*
	*/
	void Mutex::lock()
	{
#if __LINUX__ || __MAC__
//		::pthread_mutex_lock(&_mutex);
#else
		WaitForSingleObject(m_hMutex,10000);
#endif
	}

	/**
	* \brief 解锁一个互斥体
	*
	*/
	 void Mutex::unlock()
	{
#if __LINUX__ || __MAC__ 
//		::pthread_mutex_unlock(&_mutex);
#else
		ReleaseMutex(m_hMutex);
#endif
	}
    /**
     * 停止所有线程
     **/
	void ThreadManager::stopAll()
	{
		_mutex.lock();
		for (std::set<Thread*>::iterator iter = threads.begin();
			iter != threads.end(); ++iter)
		{
			if (*iter)
				(*iter)->stop();	
		}	
		_mutex.unlock();
	}
    /**
     * 线程管理 增加线程
     * \param thread 线程
     **/
	void ThreadManager::addThread(Thread *thread)
	{
		_mutex.lock();
		threads.insert(thread);
		_mutex.unlock();
	}
	ThreadManager *ThreadManager::threadM = NULL;
    /**
     * 获取地址
     * \param unqiueName 地址名字
     * \return 地址
     **/
    std::string IpUtil::getIP(std::string uniqueName)
    {
		std::vector<std::string> strs;
        split(uniqueName.c_str(),strs,":");
        if (strs.size() == 2)
        {
            return strs[0];
        }
        return "";
    }
    /**
     * 获取port
     * \param uniqueName 唯一地址
     * \return port
     **/
    WORD IpUtil::getPort(std::string uniqueName)
    {
        std::vector<std::string> strs;
        split(uniqueName.c_str(),strs,":");
        if (strs.size() == 2)
        {
            return atoi(strs[1].c_str());
        }
        return 0;
    }
    /**
     * 构造函数
     **/
    IpUtil::IpUtil(std::string uniqueName)
    {
        std::vector<std::string> strs;
        split(uniqueName.c_str(),strs,":");
        if (strs.size() == 2)
        {
            ip = strs[0];
            port = atoi(strs[1].c_str());
        }
    }
	Socket::Socket()
	{
#ifdef __LINUX__
		
#else
		//if ( WSAStartup(MAKEWORD(2,2), &Ws) != 0 ) 
		{  
		} 
#endif
		_socket = -1;
		//_socket = ::socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

	}
	Socket::~Socket()
	{
		close();
	}
	int Socket::send(void *buf,int size)
	{
#if __LINUX__
		return TEMP_FAILURE_RETRY(::send(_socket,buf,size,0));
#else
		return ::send(_socket,(char*)buf,size,0);	
#endif

	}
	void Socket::shutdown()
	{
		if (_socket!=-1)
#if __LINUX__ || __MAC__
			::shutdown(_socket, SHUT_RDWR);
#else
			::shutdown(_socket,SD_BOTH);
#endif
	}
	void Socket::close()
	{
#if __LINUX__ || __MAC__
		if (_socket != -1)
		::close(_socket);
#else
		if (_socket!=-1)
			::closesocket(_socket);
		//WSACleanup();
#endif
		_socket = -1;
	}
	int Socket::recv(void *buf,int size)
	{	
#if __LINUX__
		return TEMP_FAILURE_RETRY(::recv(_socket,buf,size,0));
#else
		return ::recv(_socket,(char*)buf,size,0);	
#endif		
	}
	void Socket::bind(const sockaddr *name,int namelen)
	{
		::bind(_socket,name,namelen);
	}
	void Socket::listen(int size)
	{
		::listen(_socket,size);
	}
	bool Socket::checkValid()
	{
		if (-1 == _socket) return false;
		return true;
	}
	int Socket::connect(const sockaddr *name,int namelen)
	{
		return ::connect(_socket,name,namelen);
	}
	void Socket::setnonblocking()
	{
	#if __LINUX__ || __MAC__
		int opts;
		opts=fcntl(_socket,F_GETFL);
		if(opts<0)
		{
			return;
		}
		opts = opts|O_NONBLOCK;
		if(fcntl(_socket,F_SETFL,opts)<0)
		{
			return;
		} 
	#else
		unsigned long ul = 1;
		ioctlsocket(_socket, FIONBIO, (unsigned long*)&ul);
	#endif
	}
	/**
 	 * 打印内容
 	 * */	
	void Logger::trace(const char*pattern,...)
	{
		AynaTime nowTime;
		if (_yday != nowTime.day)
		{
			if (NULL != _logFileHandle)
			{
				fclose(_logFileHandle);
			}
			char fileName[256] = {'\0'};
			_yday = nowTime.day;
			sprintf(fileName,
				"%s%04d%02d%02d.log",_loggerFile.c_str(),
				nowTime.year,
				nowTime.month,
				nowTime.mday);
			_logFileHandle = fopen(fileName,"at");
		}	
		if (!_loggerFile.empty())
		{
			// 构建日志内容
			char buffer[40] = {'\0'};
			char buffer1[20]= {'\0'};
			char buffer2[4026] = {'\0'};
			va_list vp;
			sprintf(buffer,"[%s] ",_loggerFile.c_str());
			sprintf(buffer1,"%04d/%02d/%02d/%02d/%02d/%02d ",
				nowTime.year,
				nowTime.month,
				nowTime.mday,
				nowTime.hour,
				nowTime.min,
				nowTime.sec
			);
			va_start(vp,pattern);
			vsprintf(buffer2,pattern,vp);
			va_end(vp);
			std::stringstream ss;
			ss<<buffer<<buffer1<<buffer2<<"\r\n";	
			_contents.push_back(ss.str());
			if (_contents.size() >= _bufferSize)
			{
				flush();
			}
		}
	}
	void Logger::flush()
	{
		if (!_logFileHandle) return;
		for (std::vector<std::string>::iterator iter = _contents.begin(); iter != _contents.end(); ++iter)
		{
			fwrite(iter->c_str(),iter->size(),1,_logFileHandle);	
		}
	}
	Logger::Logger(const char *name)
	{
		_loggerFile = name;
		_bufferSize = 0;
		_logFileHandle = NULL;
	}

	Logger::~Logger()
	{
		if (_logFileHandle)
		{
			fclose(_logFileHandle);
		}
		_logFileHandle = NULL;
	}
#ifdef __USE_LUA_EXT
	void SysLuaLib::addLibExt(lua_State* L)
	{
		if (!L) return;
		lua_tinker::class_add<stTimeValue>(L, "stTimeValue");
		lua_tinker::class_mem<stTimeValue>(L,"second",&stTimeValue::second);
		lua_tinker::class_mem<stTimeValue>(L,"msecond",&stTimeValue::msecond);

		lua_tinker::class_add<sRandom>(L, "sRandom");
		lua_tinker::class_def<sRandom>(L,"isPercent",&sRandom::isPercent);
		lua_tinker::class_def<sRandom>(L,"randBetween",&sRandom::randBetween);
	}
#endif
};
