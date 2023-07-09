#include "StdAfx.h"
#include "MsXmlp.h"

#include <shlwapi.h> // SHCreateStreamOnFile()
#import "msxml.dll" named_guids

CMsXmlp::CMsXmlp(void)
{
	m_pRoot = NULL;
	m_pDoc = NULL;
}

CMsXmlp::~CMsXmlp(void)
{
}

BOOL CMsXmlp::Load(LPCTSTR xmlFile)
{
	try
	{
		if(!m_pDoc)
		{
			m_pDoc.CreateInstance(MSXML::CLSID_DOMDocument);
			m_pDoc->put_async(VARIANT_FALSE);
		}
		
		VARIANT_BOOL hr = m_pDoc->load(_variant_t((wchar_t *)xmlFile));
		if(hr != VARIANT_TRUE)
		{
			return FALSE;
		}

		m_pDoc->get_documentElement(&m_pRoot);
	}
	catch(_com_error &e)
	{
		e;
		TRACE(_T("xml exeption: %s\r\n"),e.Description());

		return FALSE;
	}

	return TRUE;
}

static BOOL _GetEntryValue(MSXML::IXMLDOMNode *entry,CString &name,CString &val)
{
	HRESULT hr;
	MSXML::IXMLDOMNamedNodeMap *attrib;
	hr = entry->get_attributes(&attrib);
	
	MSXML::IXMLDOMNode *namedItem;
	
	long len;
	len = attrib->Getlength();

	BOOL bHit = FALSE;
	for(long i=0; i<len; i++)
	{
		attrib->get_item(i,&namedItem);
		
#ifdef _DEBUG
		BSTR nodeName;
		hr = namedItem->get_nodeName(&nodeName);
#endif
		
		BSTR bstrName;
		namedItem->get_nodeName(&bstrName);

		VARIANT bstrTitle; 
		hr = namedItem->get_nodeValue(&bstrTitle);

		if(_tcscmp(bstrName,_T("name")) == 0)
		{
			name = bstrTitle;
			bHit = TRUE;
		}
		else if(_tcscmp(bstrName,_T("value")) == 0)
		{
			val = bstrTitle;
		}
	}

	return bHit;
}

static BOOL _GetSectionName(MSXML::IXMLDOMNode *childSection,CString &val)
{
	HRESULT hr;
	MSXML::IXMLDOMNamedNodeMap *attrib;
	hr = childSection->get_attributes(&attrib);
	
	MSXML::IXMLDOMNode *namedItem;
	
	long len;
	len = attrib->Getlength();

	for(long i=0; i<len; i++)
	{
		attrib->get_item(i,&namedItem);
		
#ifdef _DEBUG
		BSTR nodeName;
		hr = namedItem->get_nodeName(&nodeName);
#endif

		BSTR bstrName;
		namedItem->get_nodeName(&bstrName);

		VARIANT bstrTitle; 
		hr = namedItem->get_nodeValue(&bstrTitle);

		if(_tcscmp(bstrName,_T("name")) == 0)
		{
			val = bstrTitle;

			return TRUE;
		}
	}

	return FALSE;
}

static BOOL _WalkSection(MSXML::IXMLDOMNodePtr pNodeSection,LPCTSTR node,CString &val)
{
	HRESULT hr;
	CString strNode = node;
	CString strNodeNext = node;
	int idx = strNode.Find(_T("\\"));
	if(idx != -1)
	{
		strNode.Delete(idx,strNode.GetLength()-idx);
		strNodeNext.Delete(0,strNode.GetLength()+1);
	}
	else
	{
		strNodeNext = _T("");
	}

	MSXML::IXMLDOMNode *childSection;
	hr = pNodeSection->get_firstChild(&childSection);
	while(childSection)
	{
		BSTR nodeName;
		hr = childSection->get_nodeName(&nodeName);

//		CString n,v;
//		_GetEntryValue(childSection,n,v);
		if(_tcscmp(nodeName,_T("section")) == 0)
		{
			if(strNode[0] == _T('>'))
			{
				CString _sectionName;
				if(_GetSectionName(childSection,_sectionName) &&
					(_tcscmp(&(((LPCTSTR )strNode)[1]),_sectionName) == 0))
				{
					if(_WalkSection(childSection,strNodeNext,val))
					{
						return TRUE;
					}
				}
			}
		}
		else if(_tcscmp(nodeName,_T("entry")) == 0)
		{
			if(strNode[0] == _T('<'))
			{
				CString _name;
				CString _val;
				if(_GetEntryValue(childSection,_name,_val))
				{
					if(_name.Compare(&(((LPCTSTR )strNode)[1])) == 0)
					{
						val = _val;

						return TRUE;
					}
				}
			}
		}

		childSection->get_nextSibling(&childSection);
	}

	return FALSE;
}


static BOOL _GetString(MSXML::IXMLDOMNodePtr pNodePtr,LPCTSTR node,CString &val)
{
	HRESULT hr;
	CString strNode = node;
	CString strNodeNext = node;
	int idx = strNode.Find(_T("\\"));
	if(idx != -1)
	{
		strNode.Delete(idx,strNode.GetLength()-idx);
		strNodeNext.Delete(0,strNode.GetLength()+1);
	}
	else
	{
		strNodeNext = _T("");
	}

	if(strNode[0] == _T('>'))
	{
		strNode.Delete(0,1);

		BSTR nodeName;
		hr = pNodePtr->get_nodeName(&nodeName);
		
		MSXML::IXMLDOMNode *childSection;

		hr = pNodePtr->get_firstChild(&childSection);
		while(childSection)
		{
#ifdef _DEBUG
			BSTR nodeName;
			hr = childSection->get_nodeName(&nodeName);
#endif

			CString strNameValue;
			if(_GetSectionName(childSection,strNameValue))
			{
				if(strNameValue.Compare(strNode) == 0)
				{
					if(_WalkSection(childSection,strNodeNext,val))
					{
						return TRUE;
					}
				}
			}
		
			pNodePtr->get_nextSibling(&childSection);
		}

		return FALSE;
	}

	MSXML::IXMLDOMNodeList *childList;
	hr = pNodePtr->get_childNodes(&childList);
	if(!SUCCEEDED(hr))
	{
		return FALSE;
	}

	long lLen;
	hr = childList->get_length(&lLen);
	if(!SUCCEEDED(hr))
	{
		return FALSE;
	}
	
	for(long i=0; i<lLen; i++)
	{
		MSXML::IXMLDOMNodePtr nodeAttrItem = childList->Getitem(i);

		MSXML::DOMNodeType nodeType;
		hr = nodeAttrItem->get_nodeType(&nodeType);

		if(nodeType != NODE_ELEMENT)
		{
			continue;
		}

		BSTR nodeName;
		hr = nodeAttrItem->get_nodeName(&nodeName);
		if(strNode.Compare(nodeName) == 0)
		{
			if(_GetString(nodeAttrItem,strNodeNext,val))
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CMsXmlp::GetNode(LPCTSTR node,CString &val)
{
	if(!m_pRoot)
	{
		return FALSE;
	}

	try
	{
		return _GetString(m_pDoc->documentElement,node,val);
	}
	catch(_com_error &e)
	{
		e;
		TRACE(_T("xml exeption: %s\r\n"),e.Description());

		return FALSE;
	}
}

BOOL CMsXmlp::Save(LPCTSTR xmlFile)
{
	if(!m_pRoot)
	{
		return FALSE;
	}
	
	try
	{
#if 0
		IStreamPtr stream;
		SHCreateStreamOnFile(xmlFile,STGM_READWRITE|STGM_SHARE_DENY_WRITE|STGM_CREATE,&stream);

		MSXML::IMXWriterPtr wp;
		wp.CreateInstance("MSXML.MXXMLWriter");
		wp->put_version(L"1.0");
		wp->put_encoding(L"Shift_JIS");
		wp->put_indent(VARIANT_TRUE);
		wp->put_output(_variant_t((IUnknown *)(IUnknownPtr )stream));

		MSXML::ISAXXMLReaderPtr rdr;
		rdr.CreateInstance("MSXML.SAXXMLReader");
		rdr->putContentHandler((MSXML::ISAXContentHandlerPtr)wrt);
		rdr->parse(_variant_t((IUnknown *)(IUnknownPtr )m_pDoc));
#else
		HRESULT hr = m_pDoc->save(_variant_t((wchar_t *)xmlFile));
		if(!SUCCEEDED(hr))
		{
			return FALSE;
		}
#endif
	}
	catch(_com_error &e)
	{
		e;
		TRACE(_T("xml exeption: %s\r\n"),e.Description());

		return FALSE;
	}

	return TRUE;
}

static BOOL _WalkSection_W(MSXML::IXMLDOMDocumentPtr pDoc,MSXML::IXMLDOMNodePtr pNodeSection,LPCTSTR node,LPCTSTR val)
{
	HRESULT hr;
	CString strNode = node;
	CString strNodeNext = node;
	int idx = strNode.Find(_T("\\"));
	if(idx != -1)
	{
		strNode.Delete(idx,strNode.GetLength()-idx);
		strNodeNext.Delete(0,strNode.GetLength()+1);
	}
	else
	{
		strNodeNext = _T("");
	}

	MSXML::IXMLDOMNode *childSection;
	hr = pNodeSection->get_firstChild(&childSection);
	while(childSection)
	{
		BSTR nodeName;
		hr = childSection->get_nodeName(&nodeName);

		if(_tcscmp(nodeName,_T("section")) == 0)
		{
			if(strNode[0] == _T('>'))
			{
				CString _sectionName;
				if(_GetSectionName(childSection,_sectionName) &&
					(_tcscmp(&(((LPCTSTR )strNode)[1]),_sectionName) == 0))
				{
					if(_WalkSection_W(pDoc,childSection,strNodeNext,val))
					{
						return TRUE;
					}
				}
			}
		}
		else if(_tcscmp(nodeName,_T("entry")) == 0)
		{
			if(strNode[0] == _T('<'))
			{
				CString _name;
				CString _val;
				if(_GetEntryValue(childSection,_name,_val))
				{
					if(_name.Compare(&(((LPCTSTR )strNode)[1])) == 0)
					{
//						pNodeSection->removeChild(childSection);
						
						// name val‚ð’Ç‰Á
						MSXML::IXMLDOMAttributePtr pName = pDoc->createAttribute(_T("name"));
						pName->text = (LPCTSTR )_name;
						MSXML::IXMLDOMAttributePtr pValue = pDoc->createAttribute(_T("value"));
						pValue->text = val;

						childSection->attributes->setNamedItem(pName);
						childSection->attributes->setNamedItem(pValue);

						return TRUE;
					}
				}
			}
		}

		childSection->get_nextSibling(&childSection);
	}

	LPCTSTR _name = &(((LPCTSTR )strNode)[1]);
	if(strNode[0] == _T('>'))
	{
		// ŠY“–‚·‚ésection‚ª–³‚¯‚ê‚Îì‚é
		MSXML::IXMLDOMNodePtr pSection = pDoc->createNode(MSXML::NODE_ELEMENT,_T("section"),_T(""));
		pNodeSection->appendChild(pSection);
		
		// name val‚ð’Ç‰Á
		MSXML::IXMLDOMAttributePtr pName = pDoc->createAttribute(_T("name"));
		pName->text = (LPCTSTR )_name;

		return _WalkSection_W(pDoc,pSection,strNodeNext,val);
	}
	else if(strNode[0] == _T('<'))
	{
		// ŠY“–‚·‚éentry‚ª–³‚¯‚ê‚Îì‚é
		MSXML::IXMLDOMNodePtr pEntry = pDoc->createNode(MSXML::NODE_ELEMENT,_T("entry"),_T(""));
		pNodeSection->appendChild(pEntry);

		// name val‚ð’Ç‰Á
		MSXML::IXMLDOMAttributePtr pName = pDoc->createAttribute(_T("name"));
		pName->text = (LPCTSTR )_name;
		MSXML::IXMLDOMAttributePtr pValue = pDoc->createAttribute(_T("value"));
		pValue->text = val;

		pEntry->attributes->setNamedItem(pName);
		pEntry->attributes->setNamedItem(pValue);

		return TRUE;
	}

	return FALSE;
}

static BOOL _SetString(MSXML::IXMLDOMDocumentPtr pDoc,MSXML::IXMLDOMNodePtr pNodePtr,LPCTSTR node,LPCTSTR val)
{
	HRESULT hr;
	CString strNode = node;
	CString strNodeNext = node;
	int idx = strNode.Find(_T("\\"));
	if(idx != -1)
	{
		strNode.Delete(idx,strNode.GetLength()-idx);
		strNodeNext.Delete(0,strNode.GetLength()+1);
	}
	else
	{
		strNodeNext = _T("");
	}

	if(strNode[0] == _T('>'))
	{
		strNode.Delete(0,1);

#ifdef _DEBUG
		BSTR nodeName;
		hr = pNodePtr->get_nodeName(&nodeName);
#endif
		
		MSXML::IXMLDOMNode *childSection;

		hr = pNodePtr->get_firstChild(&childSection);
		while(childSection)
		{
#ifdef _DEBUG
			BSTR nodeName;
			hr = childSection->get_nodeName(&nodeName);
#endif

			CString strNameValue;
			if(_GetSectionName(childSection,strNameValue))
			{
				if(strNameValue.Compare(strNode) == 0)
				{
					if(_WalkSection_W(pDoc,childSection,strNodeNext,val))
					{
						return TRUE;
					}
				}
			}
		
			pNodePtr->get_nextSibling(&childSection);
		}

		return FALSE;
	}

	MSXML::IXMLDOMNodeList *childList;
	hr = pNodePtr->get_childNodes(&childList);
	if(!SUCCEEDED(hr))
	{
		return FALSE;
	}

	long lLen;
	hr = childList->get_length(&lLen);
	if(!SUCCEEDED(hr))
	{
		return FALSE;
	}
	
	for(long i=0; i<lLen; i++)
	{
		MSXML::IXMLDOMNodePtr nodeAttrItem = childList->Getitem(i);

		MSXML::DOMNodeType nodeType;
		hr = nodeAttrItem->get_nodeType(&nodeType);

		if(nodeType != NODE_ELEMENT)
		{
			continue;
		}

		BSTR nodeName;
		hr = nodeAttrItem->get_nodeName(&nodeName);
		if(strNode.Compare(nodeName) == 0)
		{
			if(_SetString(pDoc,nodeAttrItem,strNodeNext,val))
			{
				return TRUE;
			}
		}
	}
	
	return FALSE;
}

BOOL CMsXmlp::SetNode(LPCTSTR node,LPCTSTR val)
{
	if(!m_pRoot)
	{
		return FALSE;
	}

	try
	{
		return _SetString(m_pDoc,m_pDoc->documentElement,node,val);
	}
	catch(_com_error &e)
	{
		e;
		TRACE(_T("xml exeption: %s\r\n"),e.Description());

		return FALSE;
	}
}
