#pragma once

template <typename T>
class CSingleton
{
public:
	CSingleton() {}
	virtual ~CSingleton() {}

	static void CreateClass()
	{
		if ( !m_pMgr )
			m_pMgr = new T;
	}

	static void DestroyClass()
	{
		if ( m_pMgr )
		{
			delete m_pMgr;

			m_pMgr = NULL;
		}
	}

	static T* GetMgr()
	{
		return m_pMgr;
	}

private:
	static T* m_pMgr;
};

template<typename T> T* CSingleton<T>::m_pMgr = NULL;