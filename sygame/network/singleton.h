#pragma once
/**
 * 单例模式
 */
template <class T>
class Singleton{
public:
	/**
	 * 获取单例的引用
	 */
	static T & getMe()
	{
		if (!t) t = new T();
		return *t;
	}
	/**
	 * 获取单例的指针
	 */
	static T* intance()
	{
		return &getMe();
	}
	static T * t;
	void release()
	{
		if (t)
			delete t;
		t = 0;
	}
};
template <class T>
T* Singleton<T>::t = 0;


#define USE_SINGLETON_OBJECT(CLASS)\
	static CLASS & getMe()\
	{\
		static CLASS cls;\
		return cls;\
	}

