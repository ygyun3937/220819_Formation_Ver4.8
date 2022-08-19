#pragma once


template < typename T >
class TSingleton
{
protected:
    TSingleton()
    {

    }
    virtual ~TSingleton()
    {

    }

public:
    static T* GetInstance()
    {
        if (m_pInstance == NULL)
            m_pInstance = new T;
        return m_pInstance;
    };

    static void DestroyInstance()
    {
        if (m_pInstance)
        {
            delete m_pInstance;
            m_pInstance = NULL;
        }
    };

private:
    static T* m_pInstance;
};
template <typename T> T* TSingleton<T>::m_pInstance = 0;


class CWorkLogger/* : public TSingleton<CWorkLogger>*/
{
public:
	CWorkLogger();
	~CWorkLogger();

private:
    CRegKey _regKey;

public:
    int Initialize(int ch);
    CString GetStringValue(LPCSTR valueName);
    int GetIntValue(LPCSTR valueName);

    void WriteStringValue(LPCSTR valueName, LPCSTR value);
    void WriteIntValue(LPCSTR valueName, int value);
};
