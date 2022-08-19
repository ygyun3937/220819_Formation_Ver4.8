// TiXmlHelper.h: interface for the CTiXmlHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIXMLHELPER_H__15650D45_3A76_42A3_917A_07C4EC07352E__INCLUDED_)
#define AFX_TIXMLHELPER_H__15650D45_3A76_42A3_917A_07C4EC07352E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class TiXmlNode;

class CTiXmlHelper  
{
private:

	CTiXmlHelper();
	virtual ~CTiXmlHelper();

public:

	// LoadXml, SaveXml  에서 사용하기 위한 Heler 함수들
	static bool GetXmlData( TiXmlNode* pNode, CString strName, int* pnData );
	static bool GetXmlData( TiXmlNode* pNode, CString strName, char* pchData );
	static bool GetXmlData( TiXmlNode* pNode, CString strName, CString* pstrData );
	static bool GetXmlData( TiXmlNode* pNode, CString strName, double* pnData );
	static bool GetXmlData( TiXmlNode* pNode, CString strName, long* pnData );
	static bool GetXmlData( TiXmlNode* pNode, CString strName, LPSYSTEMTIME pnData );

	static bool AddXmlData( TiXmlNode* pNodeParent, CString strNodeName, CString strValue, bool bCDATA );
	static bool AddXmlData( TiXmlNode* pNodeParent, CString strNodeName, int nValue, bool bCDATA );
	static bool AddXmlData( TiXmlNode* pNodeParent, CString strNodeName, double dValue, bool bCDATA );
	static bool AddXmlData( TiXmlNode* pNodeParent, CString strNodeName, SYSTEMTIME Value, bool bCDATA );
};

#endif // !defined(AFX_TIXMLHELPER_H__15650D45_3A76_42A3_917A_07C4EC07352E__INCLUDED_)
