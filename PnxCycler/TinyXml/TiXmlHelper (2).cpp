// TiXmlHelper.cpp: implementation of the CTiXmlHelper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TiXmlHelper.h"

#include "TinyXml.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTiXmlHelper::CTiXmlHelper()
{

}

CTiXmlHelper::~CTiXmlHelper()
{

}

bool CTiXmlHelper::GetXmlData( TiXmlNode* pNode, CString strName, int* pnData )
{
	ASSERT( pNode && pnData );

	TiXmlElement* pElem = pNode->FirstChildElement( strName );

	if ( pElem )
		*pnData = atoi( pElem->GetText() );
	else
		return false;

	return true;
}

bool CTiXmlHelper::GetXmlData( TiXmlNode* pNode, CString strName, char* pchData )
{
	ASSERT( pNode && pchData );

	TiXmlElement* pElem = pNode->FirstChildElement( strName );

	if ( pElem )
	{
		CString str = pElem->GetText();

		if ( str.GetLength() > 0 )
			*pchData = str[ 0 ];	// char 는 GetText() 결과 문자열의 첫번째 자리. 
	}
	else
		return false;

	return true;
}


bool CTiXmlHelper::GetXmlData( TiXmlNode* pNode, CString strName, CString* pstrData )
{
	ASSERT( pNode && pstrData );

	TiXmlElement* pElem = pNode->FirstChildElement( strName );

	if ( pElem )
		*pstrData = pElem->GetText();
	else
		return false;

	return true;
}


bool CTiXmlHelper::GetXmlData( TiXmlNode* pNode, CString strName, double* pdData )
{
	ASSERT( pNode && pdData );

	TiXmlElement* pElem = pNode->FirstChildElement( strName );

	if ( pElem )
		*pdData = atof( pElem->GetText() );
	else
		return false;

	return true;
}

bool CTiXmlHelper::GetXmlData( TiXmlNode* pNode, CString strName, long* plData )
{
	ASSERT( pNode && plData );

	TiXmlElement* pElem = pNode->FirstChildElement( strName );

	if ( pElem )
		*plData = atoi( pElem->GetText() );
	else
		return false;


	return true;
}


bool CTiXmlHelper::GetXmlData( TiXmlNode* pNode, CString strName, LPSYSTEMTIME pData )
{
	ASSERT( pNode && pData );

	TiXmlElement* pElem = pNode->FirstChildElement( strName );

	if ( pElem )
	{
		char szText[ MAX_PATH ] = { 0, };		
		SYSTEMTIME Value = { 0, };

		strcpy_s( szText, strlen( pElem->GetText() ), pElem->GetText() );

		sscanf_s( szText, "%04d-%02d-%02d %02d:%02d:%02d.%03d", &Value.wYear, &Value.wMonth, &Value.wDay, &Value.wHour, &Value.wMinute, &Value.wSecond, &Value.wMilliseconds );
		
		*pData = Value;
	}
	else
		return false;

	return true;
}


//------------------------------------------------------------------------------
// 
bool CTiXmlHelper::AddXmlData( TiXmlNode* pNodeParent, CString strNodeName, CString strValue, bool bCDATA )
{
	if ( !pNodeParent )
		return false;

	//------------------------------------------------------------
	TiXmlElement* pXmlChild = new TiXmlElement( strNodeName );

	pNodeParent->LinkEndChild( pXmlChild );

	TiXmlText* pXmlText = new TiXmlText( strValue );

	pXmlText->SetCDATA( bCDATA );

	pXmlChild->LinkEndChild( pXmlText );
	
	return true;
}

//------------------------------------------------------------------------------
// 
bool CTiXmlHelper::AddXmlData( TiXmlNode* pNodeParent, CString strNodeName, int nValue, bool bCDATA )
{
	if ( !pNodeParent )
		return false;

	//------------------------------------------------------------
	TiXmlElement* pXmlChild = new TiXmlElement( strNodeName );

	pNodeParent->LinkEndChild( pXmlChild );

	CString strValue;

	strValue.Format( "%d", nValue );
	TiXmlText* pXmlText = new TiXmlText( strValue );

	pXmlText->SetCDATA( bCDATA );

	pXmlChild->LinkEndChild( pXmlText );
	
	return true;
}

bool CTiXmlHelper::AddXmlData( TiXmlNode* pNodeParent, CString strNodeName, double dValue, bool bCDATA )
{
	if ( !pNodeParent )
		return false;

	//------------------------------------------------------------
	TiXmlElement* pXmlChild = new TiXmlElement( strNodeName );

	pNodeParent->LinkEndChild( pXmlChild );

	CString strValue;

	strValue.Format( "%.7lf", dValue );
	strValue.TrimRight( "0" );

	TiXmlText* pXmlText = new TiXmlText( strValue );

	pXmlText->SetCDATA( bCDATA );

	pXmlChild->LinkEndChild( pXmlText );

	return true;
}

bool CTiXmlHelper::AddXmlData( TiXmlNode* pNodeParent, CString strNodeName, SYSTEMTIME Value, bool bCDATA )
{
	if ( !pNodeParent )
		return false;

	//------------------------------------------------------------
	TiXmlElement* pXmlChild = new TiXmlElement( strNodeName );

	pNodeParent->LinkEndChild( pXmlChild );

	CString strValue;

	strValue.Format( "%04d-%02d-%02d %02d:%02d:%02d.%03d", Value.wYear, Value.wMonth, Value.wDay, Value.wHour, Value.wMinute, Value.wSecond, Value.wMilliseconds );

	TiXmlText* pXmlText = new TiXmlText( strValue );

	pXmlText->SetCDATA( bCDATA );

	pXmlChild->LinkEndChild( pXmlText );

	return true;
}
