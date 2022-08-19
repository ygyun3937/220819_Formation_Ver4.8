#ifndef __SAFE_QUEUE__H__INCLUDED_
#define __SAFE_QUEUE__H__INCLUDED_

#include "windows.h"
/*
Queue.h - header file for queue class ( FIFO list )
SB Software 1998-2006

  Usage :
  Queue<CString> MyQueue;
  ...
  MyQueue.Push("Mr. ");
  MyQueue.Push("Bill ");
  MyQueue.Push("Gates ");
  ...
  CString sName=MyQueue.Pop()+MyQueue.Pop()+MyQueue.Pop()+" !!!";

  // sName="Mr. Bill Gates  !!!"

*/


template <class Type> class SafeQueue;			// forward declaration
template <class Type> class _CNXSafe_queue_item {
	Type value;
	_CNXSafe_queue_item *next;
	_CNXSafe_queue_item() { next=NULL;};
	_CNXSafe_queue_item(const Type &val) { 
		value=val;
		next=NULL;
	};
	~_CNXSafe_queue_item() {};
	void DeleteQueue() {					// delete all items
		if(next!=NULL) next->DeleteQueue();
		delete this;
	};
	friend class SafeQueue<Type>;				// the one and the only one friend 
											// who can use this class
};

template <class Type> class SafeQueue 
{
	CRITICAL_SECTION lock;
	_CNXSafe_queue_item<Type> *first;				// head pointer
	_CNXSafe_queue_item<Type> *last;				// Tail pointer

public:
	BOOL Push(const Type & item)	// Add element at the end of queue
	{					
		EnterCriticalSection(&lock);
		_CNXSafe_queue_item<Type> *next=new _CNXSafe_queue_item<Type> (item);
		if (first==NULL) 
			last=first=next;
		else {
			last->next=next;
			last=next;
		}
		LeaveCriticalSection(&lock);
		return TRUE;
	};
	Type Pop() {							// Retrieve element from the head of queue, and remove it from queue
		EnterCriticalSection(&lock);
		Type val;
		_CNXSafe_queue_item<Type> *oldFirst=first;
		if(first==NULL)
		{
			LeaveCriticalSection(&lock);
			throw "SB Queue underflow !";
		} else {
			val=first->value;
			first=first->next;
			delete oldFirst;
			LeaveCriticalSection(&lock);
		}
		return val;
	};
	BOOL IsEmpty() {						// check if queue is empty
		EnterCriticalSection(&lock);
		BOOL empty=(first==NULL);
		LeaveCriticalSection(&lock);
		return empty;
	};
	void Reset() {							// reset contents of the queue
		EnterCriticalSection(&lock);
		if(first!=NULL) {
			first->DeleteQueue();
			first=NULL;
		}
		LeaveCriticalSection(&lock);
	};
	SafeQueue() {
		InitializeCriticalSection( &lock);
		first=last=NULL;
	};
	~SafeQueue() {								// destructor - deletes all
		EnterCriticalSection(&lock);
		if(first!=NULL)
			first->DeleteQueue();
		first=NULL;
		LeaveCriticalSection(&lock);
		DeleteCriticalSection(&lock);
	};
};

#endif // __SAFE_QUEUE__H__INCLUDED_
