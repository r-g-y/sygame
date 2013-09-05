#pragma once
#include "memorymanager.h"
#include "memorynode.h"
#include <new>
template <class T>
class Ref{
public:
	DYNAMIC_API static Ref get()
	{
		Ref<T> object;
		object.alloc();
		return object;
	}
	DYNAMIC_API Ref(stRefMemory ref)
	{
		this->ref.pointer  = ref.pointer;
		this->ref.refQuickId = ref.refQuickId;
	}
	DYNAMIC_API T* pointer()
	{
		if (-1 != ref.refQuickId)
		{
			MemoryNode * node = theMemoryManager.findNode(ref);
			if (node)
			{
				return reinterpret_cast<T*>(&node->content[0]);
			}
			return NULL;
		}
		return NULL;
		/*
		MemoryNode *node =  theMemoryManager.alloc(sizeof(T));
		ref.createTime = node->createTime;
		ref.refQuickId = node->refQuickId;
		return new (&node->content[0]) T;
		*/
	}
	DYNAMIC_API T* alloc()
	{
		if (ref.refQuickId != -1)
		{
			return NULL;
		}
		MemoryNode *node =  theMemoryManager.alloc(sizeof(T));
		ref.pointer = node->pointer;
		ref.refQuickId = node->refQuickId;
		printf("当前分配了:%u\n",theMemoryManager.blockSize());
		return new (&node->content[0]) T;
	}
	DYNAMIC_API T* operator->()
	{
		MemoryNode * node = theMemoryManager.findNode(ref);
		if (node)
		{
			return reinterpret_cast<T*>(&node->content[0]);
		}
		return NULL;
	}
	DYNAMIC_API void dealloc()
	{
		theMemoryManager.dealloc(ref);
	}
	DYNAMIC_API Ref & operator=(stRefMemory& ref)
	{
		this->ref.pointer  = ref.pointer;
		this->ref.refQuickId = ref.refQuickId;
		return *this;
	}
	DYNAMIC_API stRefMemory& getRef()
	{
		return ref;
	}
private:
	stRefMemory ref;
	Ref()
	{
		//alloc();
	}
};

template <class T>
class LocalRef{
public:
	DYNAMIC_API static LocalRef get()
	{
		LocalRef<T> object;
		object.alloc();
		return object;
	}
	DYNAMIC_API LocalRef(stRefMemory ref)
	{
		this->ref.pointer  = ref.pointer;
		this->ref.refQuickId = ref.refQuickId;
	}
	DYNAMIC_API T* pointer()
	{
		if (-1 != ref.refQuickId)
		{
			MemoryNode * node = manager.findNode(ref);
			if (node)
			{
				return reinterpret_cast<T*>(&node->content[0]);
			}
			return NULL;
		}
		return NULL;
		/*
		MemoryNode *node =  manager.alloc(sizeof(T));
		
		if (!node) return NULL;
		ref.createTime = node->createTime;
		ref.refQuickId = node->refQuickId;
		return new (&node->content[0]) T;
		*/
	}
	DYNAMIC_API T* alloc()
	{
		if (ref.refQuickId != -1)
		{
			return NULL;
		}
		MemoryNode *node =  manager.alloc(sizeof(T));
		if (!node) return NULL;
		printf("当前分配了:%u\n",manager.blockSize());
		ref.pointer = node->pointer;
		ref.refQuickId = node->refQuickId;
		return new (&node->content[0]) T;
	}

	DYNAMIC_API T* operator->()
	{
		MemoryNode * node = manager.findNode(ref);
		if (node)
		{
			return reinterpret_cast<T*>(&node->content[0]);
		}
		return NULL;
	}
	DYNAMIC_API void dealloc()
	{
		manager.dealloc(ref);
	}
	DYNAMIC_API LocalRef & operator=(stRefMemory& ref)
	{
		this->ref.pointer  = ref.pointer;
		this->ref.refQuickId = ref.refQuickId;
		return *this;
	}
private:
	LocalRef()
	{
		//alloc();
	}
	stRefMemory ref;
	static MemoryManager manager;
};
template <class T>
MemoryManager LocalRef<T>::manager(true);
