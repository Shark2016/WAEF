
// WAEFDoc.h : CWAEFDoc ��Ľӿ�
//


#pragma once
#include "WAEFSet.h"


class CWAEFDoc : public CDocument
{
protected: // �������л�����
	CWAEFDoc();
	DECLARE_DYNCREATE(CWAEFDoc)

// ����
public:
	CWAEFSet m_WAEFSet;

// ����
public:

// ��д
public:
	virtual BOOL OnNewDocument();
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// ʵ��
public:
	virtual ~CWAEFDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// ����Ϊ����������������������ݵ� Helper ����
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
