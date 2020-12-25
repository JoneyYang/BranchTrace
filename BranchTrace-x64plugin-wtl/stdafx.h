#pragma once

#define _ATL_USE_CSTRING

#include <atlbase.h>
#include <wtl\atlapp.h>
extern CAppModule _Module;
#include <atlwin.h>
#include <wtl\atlcrack.h>
#include <wtl\atlctrls.h>
#include <wtl\atlmisc.h>
#include <wtl\atlddx.h>


#include "resource.h"


class CEditImpl : public CWindowImpl<CEditImpl, CEdit>
{
    BEGIN_MSG_MAP(CEditImpl)
    END_MSG_MAP()
};

class CComboBoxImpl : public CWindowImpl<CComboBoxImpl, CComboBox>
{
    BEGIN_MSG_MAP(CComboBoxImpl)
    END_MSG_MAP()
};

class CListViewImpl : public CWindowImpl<CListViewImpl, CListViewCtrl>
{
    BEGIN_MSG_MAP(CListViewImpl)
    END_MSG_MAP()
};

class CButtonImpl :public CWindowImpl<CButtonImpl, CButton>
{
    BEGIN_MSG_MAP(CButtonImpl)
    END_MSG_MAP()
};

class CTrackBarCtrlImpl : public CWindowImpl< CTrackBarCtrlImpl, CTrackBarCtrl> {
    BEGIN_MSG_MAP(CTrackBarCtrlImpl)
    END_MSG_MAP()
};
