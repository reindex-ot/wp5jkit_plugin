#pragma once

#import "msxml.dll" named_guids

class CMsXmlp
{
public:
	CMsXmlp(void);
	~CMsXmlp(void);

	BOOL Load(LPCTSTR xmlFile);
	BOOL GetNode(LPCTSTR node,CString &val);
	BOOL Save(LPCTSTR xmlFile);
	BOOL SetNode(LPCTSTR node,LPCTSTR val);

private:
	MSXML::IXMLDOMDocumentPtr m_pDoc;
	MSXML::IXMLDOMElementPtr m_pRoot;
};
