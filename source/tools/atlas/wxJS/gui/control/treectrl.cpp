#include "precompiled.h"

/*
 * wxJavaScript - treectrl.cpp
 *
 * Copyright (c) 2002-2007 Franky Braem and the wxJavaScript project
 *
 * Project Info: http://www.wxjavascript.net or http://wxjs.sourceforge.net
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 *
 * $Id: treectrl.cpp 598 2007-03-07 20:13:28Z fbraem $
 */
// wxJSTreeCtrl.cpp

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "../../common/main.h"

#include "../event/evthand.h"
#include "../event/jsevent.h"
#include "../event/treeevt.h"

#include "../misc/app.h"
#include "../misc/validate.h"
#include "../misc/point.h"
#include "../misc/size.h"
#include "../misc/rect.h"
#include "../misc/colour.h"
#include "../misc/imagelst.h"
#include "../misc/font.h"

#include "treectrl.h"
#include "treeid.h"
#include "treeitem.h"
#include "treehit.h"
#include "window.h"
#include "textctrl.h"

using namespace wxjs;
using namespace wxjs::gui;

TreeCtrl::TreeCtrl(JSContext *cx, JSObject *obj)
    : wxTreeCtrl()
    , Object(obj, cx)
{
	PushEventHandler(new EventHandler(this));
}

TreeCtrl::~TreeCtrl()
{
    PopEventHandler(true);
}

int TreeCtrl::OnCompareItems(const wxTreeItemId& item1,
                                 const wxTreeItemId& item2)
{
    JSContext *cx = GetContext();
    if (cx == NULL )
        return 0;

    jsval rval;
    jsval argv[] = {
                     TreeItemId::CreateObject(cx, new wxTreeItemId(item1)),
                     TreeItemId::CreateObject(cx, new wxTreeItemId(item2)) 
                   };
    jsval fval;
	if ( GetFunctionProperty(cx, GetObject(), "onCompareItems", &fval) == JS_TRUE )
    {
        if ( JS_CallFunctionValue(cx, GetObject(), fval, 2, argv, &rval) == JS_TRUE )
        {
            int rc;
            if ( FromJS(cx, rval, rc) )
                return rc;
        }
        else
        {
            if ( JS_IsExceptionPending(cx) )
            {
                JS_ReportPendingException(cx);
            }
        }
    }
    return wxTreeCtrl::OnCompareItems(item1, item2);
}

/***
 * <file>control/treectrl</file>
 * <module>gui</module>
 * <class name="wxTreeCtrl" prototype="@wxControl">
 *  A tree control presents information as a hierarchy, with items that may be 
 *  expanded to show further items. Items in a tree control are referenced by 
 *  @wxTreeItemId handles.
 * </class>
 */
WXJS_INIT_CLASS(TreeCtrl, "wxTreeCtrl", 2)

/***
 * <properties>
 *  <property name="count" type="Integer" readonly="Y">
 *   Get the total number of items in the control.
 *  </property>
 *  <property name="firstVisibleItem" type="@wxTreeItemId">
 *   Gets the first visible item.
 *  </property>
 *  <property name="imageList" type="@wxImageList">
 *   Get/Set the normal image list.
 *  </property>
 *  <property name="indent" type="Integer">
 *  Get/Set the indent. Indent is the number of pixels the children are indented relative to
 *  the parents position. Setting indent also redraws the control immediately.
 *  </property>
 *  <property name="onCompareItems" type="Function">
 *   Set this to a function when you want to change the sort order of the items in the
 *   tree control. The function should return a negative, zero or positive value if the first 
 *   item is less than, equal to or greater than the second one. The function gets two
 *   @wxTreeItemId objects as arguments. When not set, the items are sorted
 *   alphabetically.
 *  </property>
 *  <property name="rootItem" type="@wxTreeItemId" readonly="Y">
 *   Gets the root item. 
 *  </property>
 *  <property name="selection" type="@wxTreeItemId" readonly="Y">
 *  Gets the selection. Use @wxTreeCtrl#selections when the tree can have more then 
 *  one selected item (MULTIPLE style).
 *  </property>
 *  <property name="selections" type="Array" readonly="Y">
 *   Gets all selected items (An array with @wxTreeItemId objects).
 *  </property>
 *  <property name="stateImageList" type="@wxImageList">
 *   Get/Set the state image list.
 *  </property>
 * </properties>
 */
WXJS_BEGIN_PROPERTY_MAP(TreeCtrl)
    WXJS_READONLY_PROPERTY(P_COUNT, "count")
    WXJS_PROPERTY(P_INDENT, "indent")
    WXJS_PROPERTY(P_IMAGE_LIST, "imageList")
    WXJS_PROPERTY(P_STATE_IMAGE_LIST, "stateImageList")
    WXJS_READONLY_PROPERTY(P_ROOT_ITEM, "rootItem")
    WXJS_READONLY_PROPERTY(P_SELECTION, "selection")
    WXJS_READONLY_PROPERTY(P_SELECTIONS, "selections")
    WXJS_READONLY_PROPERTY(P_FIRST_VISIBLE, "firstVisibleItem")
    WXJS_READONLY_PROPERTY(P_EDIT_CONTROL, "editControl")
WXJS_END_PROPERTY_MAP()

bool TreeCtrl::GetProperty(wxTreeCtrl *p, JSContext *cx, JSObject *obj, int id, jsval *vp)
{
    switch(id)
    {
    case P_COUNT:
        *vp = ToJS(cx, p->GetCount());
        break;
    case P_INDENT:
        *vp = ToJS(cx, p->GetIndent());
        break;
    case P_IMAGE_LIST:
        {
            Object *imgList = dynamic_cast<Object *>(p->GetImageList());
            *vp =( imgList == NULL ) ? JSVAL_VOID : OBJECT_TO_JSVAL(imgList->GetObject());
            break;
        }
    case P_STATE_IMAGE_LIST:
        {
            Object *imgList = dynamic_cast<Object *>(p->GetStateImageList());
            *vp =( imgList == NULL ) ? JSVAL_VOID : OBJECT_TO_JSVAL(imgList->GetObject());
            break;
        }
    case P_ROOT_ITEM:
        *vp = TreeItemId::CreateObject(cx, new wxTreeItemId(p->GetRootItem()));
        break;
    case P_SELECTION:
        *vp = TreeItemId::CreateObject(cx, new wxTreeItemId(p->GetSelection()));
        break;
    case P_FIRST_VISIBLE:
        *vp = TreeItemId::CreateObject(cx, new wxTreeItemId(p->GetFirstVisibleItem()));
        break;
    case P_EDIT_CONTROL:
        *vp = TextCtrl::CreateObject(cx, p->GetEditControl(), obj);
        break;
    case P_SELECTIONS:
        {
            wxArrayTreeItemIds selections;
			jsint count = p->GetSelections(selections);
			JSObject *objSelections = JS_NewArrayObject(cx, count, NULL);

			*vp = OBJECT_TO_JSVAL(objSelections);
			
			jsint i = 0;
			for (; i < count; i++)
			{
                jsval element = TreeItemId::CreateObject(cx, new wxTreeItemId(selections[i]));
				JS_SetElement(cx, objSelections, i++, &element);
			}
        }
        break;
    }
    return true;
}

bool TreeCtrl::SetProperty(wxTreeCtrl *p, JSContext *cx, JSObject *obj, int id, jsval *vp)
{
    switch(id)
    {
    case P_INDENT:
        {
            int indent;
            if ( FromJS(cx, p->GetIndent(), indent))
                p->SetIndent(indent);
            break;
        }
    case P_IMAGE_LIST:
        {
            wxImageList *imgList = ImageList::GetPrivate(cx, *vp);
			if ( imgList != NULL )
				p->SetImageList(imgList);
			break;
        }
    case P_STATE_IMAGE_LIST:
        {
            wxImageList *imgList = ImageList::GetPrivate(cx, *vp);
			if ( imgList != NULL )
				p->SetStateImageList(imgList);
			break;
        }
    }
    return true;
}

/***
 * <constants>
 *  <type name="Styles">
 *   <constant name="NO_BUTTONS" />
 *   <constant name="HAS_BUTTONS" />
 *   <constant name="TWIST_BUTTONS" />
 *   <constant name="NO_LINES" />
 *   <constant name="LINES_AT_ROOT" />
 *   <constant name="MAC_BUTTONS" />
 *   <constant name="AQUA_BUTTONS" />
 *   <constant name="SINGLE" />
 *   <constant name="MULTIPLE" />
 *   <constant name="EXTENDED" />
 *   <constant name="FULL_ROW_HIGHLIGHT" />
 *   <constant name="EDIT_LABELS" />
 *   <constant name="ROW_LINES" />
 *   <constant name="HIDE_ROOT" />
 *   <constant name="HAS_VARIABLE_ROW_HEIGHT" />
 *   <constant name="DEFAULT_STYLE" />
 *  </type>
 * </constants>
 */
WXJS_BEGIN_CONSTANT_MAP(TreeCtrl)
    WXJS_CONSTANT(wxTR_, NO_BUTTONS)
    WXJS_CONSTANT(wxTR_, HAS_BUTTONS)
    WXJS_CONSTANT(wxTR_, TWIST_BUTTONS)
    WXJS_CONSTANT(wxTR_, NO_LINES)
    WXJS_CONSTANT(wxTR_, LINES_AT_ROOT)
    WXJS_CONSTANT(wxTR_, MAC_BUTTONS)
    WXJS_CONSTANT(wxTR_, AQUA_BUTTONS)
    WXJS_CONSTANT(wxTR_, SINGLE)
    WXJS_CONSTANT(wxTR_, MULTIPLE)
    WXJS_CONSTANT(wxTR_, EXTENDED)
    WXJS_CONSTANT(wxTR_, FULL_ROW_HIGHLIGHT)
    WXJS_CONSTANT(wxTR_, EDIT_LABELS)
    WXJS_CONSTANT(wxTR_, ROW_LINES)
    WXJS_CONSTANT(wxTR_, HIDE_ROOT)
    WXJS_CONSTANT(wxTR_, HAS_VARIABLE_ROW_HEIGHT)
    WXJS_CONSTANT(wxTR_, DEFAULT_STYLE)
WXJS_END_CONSTANT_MAP()

/***
 * <ctor>
 *  <function>
 *   <arg name="Parent" type="@wxWindow">
 *    The parent of wxTreeCtrl.
 *   </arg>
 *   <arg name="Id" type="Integer">
 *    An window identifier. Use -1 when you don't need it.
 *   </arg>
 *   <arg name="Position" type="@wxPoint" default="wxDefaultPosition">
 *    The position of the TreeCtrl control on the given parent.
 *   </arg>
 *   <arg name="Size" type="@wxSize" default="wxDefaultSize">
 *    The size of the TreeCtrl control.
 *   </arg>
 *   <arg name="Style" type="Integer" default="wxTreeCtrl.ICON">
 *    The wxTreeCtrl style.
 *   </arg>
 *   <arg name="Validator" type="@wxValidator" defautl="null">
 *    Validator.
 *   </arg>
 *  </function>
 *  <desc>
 *   Constructs a new wxTreeCtrl object.
 *  </desc>
 * </ctor>
 */
wxTreeCtrl* TreeCtrl::Construct(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, bool constructing)
{
    if ( argc > 6 )
        argc = 6;

    int style = wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT;
    const wxPoint *pt = &wxDefaultPosition;
    const wxSize *size = &wxDefaultSize;
    const wxValidator *val = &wxDefaultValidator;

    switch(argc)
    {
    case 6:
        val = Validator::GetPrivate(cx, argv[5]);
        if ( val == NULL )
            break;
    case 5:
        if ( ! FromJS(cx, argv[4], style) )
            break;
        // Fall through
    case 4:
		size = Size::GetPrivate(cx, argv[3]);
		if ( size == NULL )
			break;
		// Fall through
	case 3:
		pt = Point::GetPrivate(cx, argv[2]);
		if ( pt == NULL )
			break;
		// Fall through
    default:
        
        int id;
        if ( ! FromJS(cx, argv[1], id) )
            break;

        wxWindow *parent = Window::GetPrivate(cx, argv[0]);
		if ( parent == NULL )
			break;

        Object *wxjsParent = dynamic_cast<Object *>(parent);
        JS_SetParent(cx, obj, wxjsParent->GetObject());

	    wxTreeCtrl *p = new TreeCtrl(cx, obj);
	    p->Create(parent, id, *pt, *size, style, *val);

        return p;
    }
	return NULL;
}

void TreeCtrl::Destruct(JSContext *cx, wxTreeCtrl* p)
{
}

WXJS_BEGIN_METHOD_MAP(TreeCtrl)
    WXJS_METHOD("getItem", getItem, 1)
    WXJS_METHOD("getItemText", getItemText, 1)
    WXJS_METHOD("setItemText", setItemText, 2)
    WXJS_METHOD("getItemImage", getItemImage, 2)
    WXJS_METHOD("setItemImage", setItemImage, 3)
    WXJS_METHOD("getItemData", getItemData, 1)
    WXJS_METHOD("setItemData", setItemData, 2)
    WXJS_METHOD("getItemTextColour", getItemTextColour, 1)
    WXJS_METHOD("setItemTextColour", setItemTextColour, 2)
    WXJS_METHOD("getItemBackgroundColour", getItemBackgroundColour, 1)
    WXJS_METHOD("setItemBackgroundColour", setItemBackgroundColour, 2)
    WXJS_METHOD("getItemFont", getItemFont, 1)
    WXJS_METHOD("setItemFont", setItemFont, 2)
    WXJS_METHOD("setItemHasChildren", setItemHasChildren, 1)
    WXJS_METHOD("isBold", isBold, 1)
    WXJS_METHOD("setItemBold", setItemBold, 1)
    WXJS_METHOD("setItemDropHighlight", setItemDropHighlight, 1)
    WXJS_METHOD("isVisible", isVisible, 1)
    WXJS_METHOD("isExpanded", isExpanded, 1)
    WXJS_METHOD("isSelected", isSelected, 1)
    WXJS_METHOD("getChildrenCount", getChildrenCount, 1)
    WXJS_METHOD("getItemParent", getItemParent, 1)
    WXJS_METHOD("getFirstChild", getFirstChild, 2)
    WXJS_METHOD("getNextChild", getNextChild, 2)
    WXJS_METHOD("getPrevSibling", getPrevSibling, 1)
    WXJS_METHOD("getNextSibling", getNextSibling, 1)
    WXJS_METHOD("getPrevVisible", getPrevVisible, 1)
    WXJS_METHOD("getNextVisible", getNextVisible, 1)
    WXJS_METHOD("addRoot", addRoot, 1)
    WXJS_METHOD("appendItem", appendItem, 2)
    WXJS_METHOD("prependItem", prependItem, 2)
    WXJS_METHOD("insertItem", insertItem, 3)
    WXJS_METHOD("deleteItem", deleteItem, 1)
    WXJS_METHOD("deleteChildren", deleteChildren, 1)
    WXJS_METHOD("deleteAllItems", deleteAllItems, 0)
    WXJS_METHOD("expand", expand, 1)
    WXJS_METHOD("collapse", collapse, 1)
    WXJS_METHOD("collapseAndReset", collapseAndReset, 1)
    WXJS_METHOD("toggle", toggle, 1)
    WXJS_METHOD("unselect", unselect, 0)
    WXJS_METHOD("unselectAll", unselectAll, 0)
    WXJS_METHOD("selectItem", selectItem, 1)
    WXJS_METHOD("ensureVisible", ensureVisible, 1)
    WXJS_METHOD("scrollTo", scrollTo, 1)
    WXJS_METHOD("editLabel", editLabel, 1)
    WXJS_METHOD("sortChildren", sortChildren, 1)
WXJS_END_METHOD_MAP()

/***
 * <method name="getItem">
 *  <function returns="@wxTreeItem">
 *   <arg name="Id" type="@wxTreeItemId">
 *    The id of the item
 *   </arg>
 *  </function>
 *  <desc>
 *   Creates a @wxTreeItem from the given @wxTreeItemId.
 *   @wxTreeItem can be used to alter the item without
 *   always passing the id to the tree control. null is returned
 *   when the id is not valid.
 *  </desc>
 * </method>
 */
JSBool TreeCtrl::getItem(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    wxTreeCtrl *p = GetPrivate(cx, obj);
    if ( p == NULL )
        return JS_FALSE;

    wxTreeItemId *id = TreeItemId::GetPrivate(cx, argv[0]);
    if ( id != NULL ) 
    {
        if ( id->IsOk() )
            *rval = TreeItem::CreateObject(cx, new wxTreeItemId(*id));
        else
            *rval = JSVAL_VOID;
        return JS_TRUE;
    }
    return JS_FALSE;
}

/***
 * <method name="getItemText">
 *  <function returns="String">
 *   <arg name="Id" type="@wxTreeItemId">
 *    The id of the item
 *   </arg>
 *  </function>
 *  <desc>
 *   Gets the text of the given item.
 *   See also @wxTreeItem @wxTreeItem#text property
 *  </desc>
 * </method>
 */
JSBool TreeCtrl::getItemText(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    wxTreeCtrl *p = GetPrivate(cx, obj);
    if ( p == NULL )
        return JS_FALSE;

    wxTreeItemId *id = TreeItemId::GetPrivate(cx, argv[0]);
    if ( id != NULL )
    {
        *rval = ToJS(cx, p->GetItemText(*id));
        return JS_TRUE;
    }
    return JS_FALSE;
}

/***
 * <method name="setItemText">
 *  <function>
 *   <arg name="Id" type="@wxTreeItemId">
 *    The id of the item
 *   </arg>
 *   <arg name="Text" type="String">
 *    The new text for the item.
 *   </arg>
 *  </function>
 *  <desc>
 *   Sets the text of the given item.
 *   The example changes the text of the root.
 *   <code class="whjs">
 *    tree.setItemText(tree.rootItem, "root");
 *   </code>
 *    See @wxTreeItem @wxTreeItem#text property
 *  </desc>
 * </method>
 */
JSBool TreeCtrl::setItemText(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    wxTreeCtrl *p = GetPrivate(cx, obj);
    if ( p == NULL )
        return JS_FALSE;

    wxTreeItemId *id = TreeItemId::GetPrivate(cx, argv[0]);
    if ( id != NULL )
    {
        wxString text;
        FromJS(cx, argv[1], text);
        p->SetItemText(*id, text);
        return JS_TRUE;
    }
    return JS_FALSE;
}

/***
 * <method name="getItemImage">
 *  <function returns="Integer">
 *   <arg name="Id" type="@wxTreeItemId">
 *    The id of the item
 *   </arg>
 *   <arg name="Which" type="Integer">
 *    A @wxTreeItemIcon constant.
 *   </arg>
 *  </function>
 *  <desc>
 *   Gets the specified item image.
 *   See @wxTreeItem @wxTreeItem#getImage method
 *  </desc>
 * </method>
 */
JSBool TreeCtrl::getItemImage(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    wxTreeCtrl *p = GetPrivate(cx, obj);
    if ( p == NULL )
        return JS_FALSE;

    wxTreeItemId *id = TreeItemId::GetPrivate(cx, argv[0]);
    int which;

    if (    id != NULL 
         && FromJS(cx, argv[1], which) )
    {
        *rval = ToJS(cx, p->GetItemImage(*id, (wxTreeItemIcon) which));
        return JS_TRUE;
    }
    return JS_FALSE;
}

/***
 * <method name="setItemImage">
 *  <function>
 *   <arg name="Id" type="@wxTreeItemId">
 *    The id of the item
 *   </arg>
 *   <arg name="Image" type="Integer">
 *    The new image for the item.
 *   </arg>
 *   <arg name="Which" type="Integer">
 *    A @wxTreeItemIcon constant.
 *   </arg>
 *  </function>
 *  <desc>
 *   Sets the image for the given item.
 *   See @wxTreeItem @wxTreeItem#setImage method
 *  </desc>
 * </method>
 */
JSBool TreeCtrl::setItemImage(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    wxTreeCtrl *p = GetPrivate(cx, obj);
    if ( p == NULL )
        return JS_FALSE;

    wxTreeItemId *id = TreeItemId::GetPrivate(cx, argv[0]);
    int image;
    int which;
    if ( id != NULL 
         && FromJS(cx, argv[1], image)
         && FromJS(cx, argv[2], which) )
    {
        p->SetItemImage(*id, image, (wxTreeItemIcon) which);
        return JS_TRUE;
    }
    return JS_FALSE;
}

/***
 * <method name="getItemData">
 *  <function returns="Any">
 *   <arg name="Id" type="@wxTreeItemId">
 *    The id of the item
 *   </arg>
 *  </function>
 *  <desc>
 *   Gets the associated object or value of the item. The item data
 *   can be any possible JavaScript type.
 *   See @wxTreeItem @wxTreeItem#data property, @wxTreeCtrl#setItemData
 *  </desc>
 * </method>
 */
JSBool TreeCtrl::getItemData(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    wxTreeCtrl *p = GetPrivate(cx, obj);
    if ( p == NULL )
        return JS_FALSE;

    wxTreeItemId *id = TreeItemId::GetPrivate(cx, argv[0]);
    if ( id != NULL )
    {
        ObjectTreeData *data = (ObjectTreeData*) p->GetItemData(*id);
        *rval = ( data == NULL ) ? JSVAL_VOID : data->GetJSVal();
        return JS_TRUE;
    }
    return JS_FALSE;
}

/***
 * <method name="setItemData">
 *  <function>
 *   <arg name="Id" type="@wxTreeItemId">
 *    The id of the item
 *   </arg>
 *   <arg name="Data" type="Any">
 *    The data to associate with this item.
 *   </arg>
 *  </function>
 *  <desc>
 *   Sets the associated data for the given item. You can use
 *   any type: Integer, Object, Boolean, ...
 *   <br /><br />
 *   The following example shows that you can use an object as item data.
 *   <pre><code class="whjs">
 *    dlg = new wxDialog(null, -1, "Tree example");
 *    dlg.tree = new wxTreeCtrl(dlg, 1);
 *
 *    var root = dlg.tree.addRoot("Root");
 *    var id = dlg.tree.appendItem(root, "Child 1");
 *
 *    dlg.tree.setItemData(id, new Date());
 *    id = dlg.tree.appendItem(root, "Child 2");
 *    dlg.tree.setItemData(id, new Date());
 *    id = dlg.tree.appendItem(root, "Child 3");
 *    dlg.tree.setItemData(id, new Date());
 *   </code></pre>
 *    See @wxTreeItem @wxTreeItem#data property, @wxTreeCtrl#getItemData
 *  </desc>
 * </method>
 */
JSBool TreeCtrl::setItemData(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    wxTreeCtrl *p = GetPrivate(cx, obj);
    if ( p == NULL )
        return JS_FALSE;

    wxTreeItemId *id = TreeItemId::GetPrivate(cx, argv[0]);
    if ( id != NULL )
    {
        ObjectTreeData *data = new ObjectTreeData(cx, argv[1]);
        p->SetItemData(*id, data);
        return JS_TRUE;
    }
    return JS_FALSE;

}

/***
 * <method name="getItemTextColour">
 *  <function returns="@wxColour">
 *   <arg name="Id" type="@wxTreeItemId">
 *    The id of the item
 *   </arg>
 *  </function>
 *  <desc>
 *   Gets the text colour of the item.
 *   See @wxTreeItem @wxTreeItem#textColour property
 *  </desc>
 * </method>
 */
JSBool TreeCtrl::getItemTextColour(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    wxTreeCtrl *p = GetPrivate(cx, obj);
    if ( p == NULL )
        return JS_FALSE;

    wxTreeItemId *id = TreeItemId::GetPrivate(cx, argv[0]);
    if ( id != NULL )
    {
        *rval = Colour::CreateObject(cx, new wxColour(p->GetItemTextColour(*id)));
        return JS_TRUE;
    }
    return JS_FALSE;
}

/***
 * <method name="setItemTextColour">
 *  <function>
 *   <arg name="Id" type="@wxTreeItemId">
 *    The id of the item
 *   </arg>
 *   <arg name="Colour" type="@wxColour">
 *    The new colour.
 *   </arg>
 *  </function>
 *  <desc>
 *   Sets a new text colour for this item.
 *   The example sets red as the text colour of the root item.
 *   <code class="whjs">
 *     tree.setItemTextColour(tree.rootItem, wxRED);
 *   </code>
 *   See @wxTreeItem @wxTreeItem#textColour property
 *  </desc>
 * </method>
 */
JSBool TreeCtrl::setItemTextColour(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    wxTreeCtrl *p = GetPrivate(cx, obj);
    if ( p == NULL )
        return JS_FALSE;

    wxTreeItemId *id = TreeItemId::GetPrivate(cx, argv[0]);
    wxColour *colour = Colour::GetPrivate(cx, argv[1]);
    if ( id != NULL )
    {
        p->SetItemTextColour(*id, *colour);
        return JS_TRUE;
    }
    return JS_FALSE;

}

/***
 * <method name="getItemBackgroundColour">
 *  <function returns="@wxColour">
 *   <arg name="Id" type="@wxTreeItemId">
 *    The id of the item
 *   </arg>
 *  </function>
 *  <desc>
 *   Gets the background colour of the item.
 *   See @wxTreeItem @wxTreeItem#backgroundColour property
 *  </desc>
 * </method>
 */
JSBool TreeCtrl::getItemBackgroundColour(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    wxTreeCtrl *p = GetPrivate(cx, obj);
    if ( p == NULL )
        return JS_FALSE;

    wxTreeItemId *id = TreeItemId::GetPrivate(cx, argv[0]);
    if ( id != NULL )
    {
        *rval = Colour::CreateObject(cx, new wxColour(p->GetItemBackgroundColour(*id)));
        return JS_TRUE;
    }
    return JS_FALSE;
}

/***
 * <method name="setItemBackgroundColour">
 *  <function>
 *   <arg name="Id" type="@wxTreeItemId">
 *    The id of the item
 *   </arg>
 *   <arg name="Colour" type="@wxColour">
 *    The new colour.
 *   </arg>
 *  </function>
 *  <desc>
 *   Sets a new background colour for this item.
 *   The example sets the backgroundcolour of the root item to red.
 *   <code class="whjs">
 *    tree.setItemBackgroundColour(tree.rootItem, wxRED);
 *   </code>
 *   See @wxTreeItem @wxTreeItem#backgroundColour property
 *  </desc>
 * </method>
 */
JSBool TreeCtrl::setItemBackgroundColour(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    wxTreeCtrl *p = GetPrivate(cx, obj);
    if ( p == NULL )
        return JS_FALSE;

    wxTreeItemId *id = TreeItemId::GetPrivate(cx, argv[0]);
    wxColour *colour = Colour::GetPrivate(cx, argv[1]);
    if ( id != NULL )
    {
        p->SetItemBackgroundColour(*id, *colour);
        return JS_TRUE;
    }
    return JS_FALSE;
}

/***
 * <method name="getItemFont">
 *  <function returns="@wxFont"> 
 *   <arg name="Id" type="@wxTreeItemId">
 *    The id of the item
 *   </arg>
 *  </function>
 *  <desc>
 *   Gets the font of the item.
 *   See @wxTreeItem @wxTreeItem#font property
 *  </desc>
 * </method>
 */
JSBool TreeCtrl::getItemFont(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    wxTreeCtrl *p = GetPrivate(cx, obj);
    if ( p == NULL )
        return JS_FALSE;

    wxTreeItemId *id = TreeItemId::GetPrivate(cx, argv[0]);
    if ( id != NULL )
    {
        *rval = Font::CreateObject(cx, new wxFont(p->GetItemFont(*id)));
        return JS_TRUE;
    }
    return JS_FALSE;
}

/***
 * <method name="setItemFont">
 *  <function>
 *   <arg name="Id" type="@wxTreeItemId">
 *    The id of the item
 *   </arg>
 *   <arg name="Font" type="@wxFont">
 *    The new font.
 *   </arg>
 *  </function>
 *  <desc>
 *   Sets a new font for this item.
 *   See @wxTreeItem @wxTreeItem#font property.
 *  </desc>
 * </method>
 */
JSBool TreeCtrl::setItemFont(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    wxTreeCtrl *p = GetPrivate(cx, obj);
    if ( p == NULL )
        return JS_FALSE;

    wxTreeItemId *id = TreeItemId::GetPrivate(cx, argv[0]);
    wxFont *font = Font::GetPrivate(cx, argv[1]);
    if (   id != NULL
        && font != NULL )
    {
        p->SetItemFont(*id, *font);
        return JS_TRUE;
    }
    return JS_FALSE;

}

/***
 * <method name="setItemHasChildren">
 *  <function>
 *   <arg name="Id" type="@wxTreeItemId">
 *    The id of the item
 *   </arg>
 *   <arg name="Children" type="Boolean" default="true" />
 *  </function>
 *  <desc>
 *   Force appearance of the button next to the item. This is useful to allow 
 *   the user to expand the items which don't have any children now, but instead 
 *   adding them only when needed, thus minimizing memory usage and loading time.
 *   See @wxTreeItem @wxTreeItem#hasChildren property
 *  </desc>
 * </method>
 */
JSBool TreeCtrl::setItemHasChildren(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    wxTreeCtrl *p = GetPrivate(cx, obj);
    if ( p == NULL )
        return JS_FALSE;

    bool hasChildren;
    if ( argc == 1 )
        hasChildren = true;
    else if ( ! FromJS(cx, argv[1], hasChildren) )
        return JS_FALSE;

    wxTreeItemId *id = TreeItemId::GetPrivate(cx, argv[0]);
    if ( id != NULL )
    {
        p->SetItemHasChildren(*id, hasChildren);
        return JS_TRUE;
    }
    return JS_FALSE;
}


/***
 * <method name="isBold">
 *  <function returns="Boolean">
 *   <arg name="Id" type="@wxTreeItemId">
 *    The id of the item
 *   </arg>
 *  </function>
 *  <desc>
 *   Returns true when the item text is in bold.
 *   See @wxTreeItem @wxTreeItem#bold property
 *  </desc>
 * </method>
 */
JSBool TreeCtrl::isBold(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    wxTreeCtrl *p = GetPrivate(cx, obj);
    if ( p == NULL )
        return JS_FALSE;

    wxTreeItemId *id = TreeItemId::GetPrivate(cx, argv[0]);
    if ( id != NULL )
    {
        *rval = ToJS(cx, p->IsBold(*id));
        return JS_TRUE;
    }
    return JS_FALSE;
}

/***
 * <method name="setItemBold">
 *  <function>
 *   <arg name="Id" type="@wxTreeItemId">
 *    The id of the item
 *   </arg>
 *   <arg name="Bold" type="Boolean" default="true" />
 *  </function>
 *  <desc>
 *   Makes item appear in bold font if bold parameter is true
 *   or resets it to the normal state.
 *   See @wxTreeItem @wxTreeItem#bold property
 *  </desc>
 * </method>
 */
JSBool TreeCtrl::setItemBold(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    wxTreeCtrl *p = GetPrivate(cx, obj);
    if ( p == NULL )
        return JS_FALSE;

    wxTreeItemId *id = TreeItemId::GetPrivate(cx, argv[0]);
    bool bold;
    if ( argc == 1 )
        bold = true;
    else if ( ! FromJS(cx, argv[1], bold) )
        return JS_FALSE;

    if ( id != NULL )
    {
        p->SetItemBold(*id, bold);
        return JS_TRUE;
    }
    return JS_FALSE;

}

/***
 * <method name="setItemDropHighlight">
 *  <function>
 *   <arg name="Id" type="@wxTreeItemId">
 *    The id of the item
 *   </arg>
 *   <arg name="Highlight" type="Boolean" default="true" />
 *  </function>
 *  <desc>
 *   The item will be shown with a drop highlight or not.
 *  </desc>
 * </method>
 */
JSBool TreeCtrl::setItemDropHighlight(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    wxTreeCtrl *p = GetPrivate(cx, obj);
    if ( p == NULL )
        return JS_FALSE;

    wxTreeItemId *id = TreeItemId::GetPrivate(cx, argv[0]);
    bool highlight;
    if ( argc == 1 )
        highlight = true;
    else if ( ! FromJS(cx, argv[1], highlight) )
        return JS_FALSE;

    if ( id != NULL )
    {
        p->SetItemDropHighlight(*id, highlight);
        return JS_TRUE;
    }
    return JS_FALSE;

}

/***
 * <method name="isVisible">
 *  <function>
 *   <arg name="Id" type="@wxTreeItemId">
 *    The id of the item
 *   </arg>
 *  </function>
 *  <desc>
 *   Returns true when the item is visible.
 *   See @wxTreeItem @wxTreeItem#visible property
 *  </desc>
 * </method>
 */
JSBool TreeCtrl::isVisible(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    wxTreeCtrl *p = GetPrivate(cx, obj);
    if ( p == NULL )
        return JS_FALSE;

    wxTreeItemId *id = TreeItemId::GetPrivate(cx, argv[0]);
    if ( id != NULL )
    {
        *rval = ToJS(cx, p->IsVisible(*id));
        return JS_TRUE;
    }
    return JS_FALSE;
}

/***
 * <method name="isExpanded">
 *  <function returns="Boolean">
 *   <arg name="Id" type="@wxTreeItemId">
 *    The id of the item
 *   </arg>
 *  </function>
 *  <desc>
 *   Returns true when the item is expanded.
 *   See @wxTreeItem @wxTreeItem#expanded property
 *  </desc>
 * </method>
 */
JSBool TreeCtrl::isExpanded(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    wxTreeCtrl *p = TreeCtrl::GetPrivate(cx, obj);
    if ( p == NULL )
        return JS_FALSE;

    wxTreeItemId *id = TreeItemId::GetPrivate(cx, argv[0]);
    if ( id != NULL )
    {
        *rval = ToJS(cx, p->IsExpanded(*id));
        return JS_TRUE;
    }
    return JS_FALSE;
}

/***
 * <method name="isSelected">
 *  <function returns="Boolean">
 *   <arg name="Id" type="@wxTreeItemId">
 *    The id of the item
 *   </arg>
 *  </function>
 *  <desc>
 *   Returns true when the item is selected.
 *   See @wxTreeItem @wxTreeItem#selected property
 *  </desc>
 * </method>
 */
JSBool TreeCtrl::isSelected(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    wxTreeCtrl *p = GetPrivate(cx, obj);
    if ( p == NULL )
        return JS_FALSE;

    wxTreeItemId *id = TreeItemId::GetPrivate(cx, argv[0]);
    if ( id != NULL )
    {
        *rval = ToJS(cx, p->IsSelected(*id));
        return JS_TRUE;
    }
    return JS_FALSE;
}

/***
 * <method name="getChildrenCount">
 *  <function returns="Integer">
 *   <arg name="Id" type="@wxTreeItemId">
 *    The id of the item
 *   </arg>
 *   <arg name="Recurse" type="Boolean" default="true">
 *    If Recurse is true (the default), returns the total number of descendants,
 *    otherwise only one level of children is counted
 *   </arg>
 *  </function>
 *  <desc>
 *   Returns the number of childrens of the item.
 *   See @wxTreeItem @wxTreeItem#bold property
 *  </desc>
 * </method>
 */
JSBool TreeCtrl::getChildrenCount(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    wxTreeCtrl *p = GetPrivate(cx, obj);
    if ( p == NULL )
        return JS_FALSE;

    wxTreeItemId *id = TreeItemId::GetPrivate(cx, argv[0]);
    bool recursive;
    if ( argc == 1 )
        recursive = true;
    else if ( ! FromJS(cx, argv[1], recursive) )
        return JS_FALSE;

    if ( id != NULL )
    {
        *rval = ToJS(cx, p->GetChildrenCount(*id, recursive));
        return JS_TRUE;
    }
    return JS_FALSE;

}

/***
 * <method name="getItemParent">
 *  <function returns="@wxTreeItemId">
 *   <arg name="Id" type="@wxTreeItemId">
 *    The id of the item
 *   </arg>
 *  </function>
 *  <desc>
 *   Returns the parent of the item.
 *   See @wxTreeItem @wxTreeItem#parent property
 *  </desc>
 * </method>
 */
JSBool TreeCtrl::getItemParent(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    wxTreeCtrl *p = GetPrivate(cx, obj);
    if ( p == NULL )
        return JS_FALSE;

    wxTreeItemId *id = TreeItemId::GetPrivate(cx, argv[0]);

    if ( id != NULL )
    {
        *rval = TreeItemId::CreateObject(cx, new wxTreeItemId(p->GetItemParent(*id)));
        return JS_TRUE;
    }
    return JS_FALSE;

}

/***
 * <method name="getFirstChild">
 *  <function returns="@wxTreeItemId">
 *   <arg name="Id" type="@wxTreeItemId">
 *    The id of the item
 *   </arg>
 *   <arg name="Cookie" type="Integer">
 *    For this enumeration function you must pass in a 'cookie' parameter 
 *    which is opaque for the application but is necessary for the library 
 *    to make these functions reentrant (i.e. allow more than one enumeration 
 *    on one and the same object simultaneously). The cookie passed 
 *    to getFirstChild and @wxTreeCtrl_getNextChild should be the same variable.
 *   </arg>
 *  </function>
 *  <desc>
 *    Returns the first child item. An invalid item is returned when there is no child item.
 *    See @wxTreeItem @wxTreeItem#getFirstChild method
 *  </desc>
 * </method>
 */
JSBool TreeCtrl::getFirstChild(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    wxTreeCtrl *p = GetPrivate(cx, obj);
    if ( p == NULL )
        return JS_FALSE;

    wxTreeItemId *id = TreeItemId::GetPrivate(cx, argv[0]);
    long cookie;
    if ( id != NULL 
         && FromJS(cx, argv[1], cookie) )
    {
		wxTreeItemIdValue newCookie = (wxTreeItemIdValue) cookie;
        *rval = TreeItemId::CreateObject(cx, new wxTreeItemId(p->GetFirstChild(*id, newCookie)));
        return JS_TRUE;
    }
    return JS_FALSE;

}

/***
 * <method name="getNextChild">
 *  <function returns="@wxTreeItemId">
 *   <arg name="Id" type="@wxTreeItemId">
 *    The id of the item
 *   </arg>
 *   <arg name="Cookie" type="Integer">
 *    For this enumeration function you must pass in a 'cookie' parameter 
 *    which is opaque for the application but is necessary for the library 
 *    to make these functions reentrant (i.e. allow more than one enumeration 
 *    on one and the same object simultaneously). The cookie passed 
 *    to @wxTreeCtrl#getNextChild and getNextChild should be the same variable.
 *   </arg>
 *  </function>
 *  <desc>
 *   Returns the next child item. An invalid item is returned when there is no child item.
 *   See @wxTreeItem @wxTreeItem#getNextChild method
 *  </desc>
 * </method>
 */
JSBool TreeCtrl::getNextChild(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    wxTreeCtrl *p = GetPrivate(cx, obj);
    if ( p == NULL )
        return JS_FALSE;

    wxTreeItemId *id = TreeItemId::GetPrivate(cx, argv[0]);
    long cookie;
    if ( id != NULL 
         && FromJS(cx, argv[1], cookie) )
    {
		wxTreeItemIdValue newCookie = (wxTreeItemIdValue) cookie;
        *rval = TreeItemId::CreateObject(cx, new wxTreeItemId(p->GetNextChild(*id, newCookie)));
        return JS_TRUE;
    }
    return JS_FALSE;

}

/***
 * <method name="getPrevSibling">
 *  <function returns="@wxTreeItemId">
 *   <arg name="Id" type="@wxTreeItemId">
 *    The id of the item
 *   </arg>
 *  </function>
 *  <desc>
 *   Returns the previous sibling item. An invalid item is returned when there is no child item.
 *   See @wxTreeItem @wxTreeItem#prevSibling property
 *  </desc>
 * </method>
 */
JSBool TreeCtrl::getPrevSibling(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    wxTreeCtrl *p = GetPrivate(cx, obj);
    if ( p == NULL )
        return JS_FALSE;

    wxTreeItemId *id = TreeItemId::GetPrivate(cx, argv[0]);
    if ( id != NULL )
    {
        *rval = TreeItemId::CreateObject(cx, new wxTreeItemId(p->GetPrevSibling(*id)));
        return JS_TRUE;
    }
    return JS_FALSE;
}

/***
 * <method name="getNextSibling">
 *  <function returns="@wxTreeItemId">
 *   <arg name="Id" type="@wxTreeItemId">
 *    The id of the item
 *   </arg>
 *  </function>
 *  <desc>
 *   Returns the next sibling item. An invalid item is returned when there is no child item.
 *   See @wxTreeItem @wxTreeItem#nextSibling property
 *  </desc>
 * </method>
 */
JSBool TreeCtrl::getNextSibling(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    wxTreeCtrl *p = GetPrivate(cx, obj);
    if ( p == NULL )
        return JS_FALSE;

    wxTreeItemId *id = TreeItemId::GetPrivate(cx, argv[0]);
    if ( id != NULL )
    {
        *rval = TreeItemId::CreateObject(cx, new wxTreeItemId(p->GetNextSibling(*id)));
        return JS_TRUE;
    }
    return JS_FALSE;
}

/***
 * <method name="getPrevVisible">
 *  <function returns="@wxTreeItemId">
 *   <arg name="Id" type="@wxTreeItemId">
 *    The id of the item
 *   </arg>
 *  </function>
 *  <desc>
 *   Returns the previous visible item.
 *  </desc>
 * </method>
 */
JSBool TreeCtrl::getPrevVisible(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    wxTreeCtrl *p = GetPrivate(cx, obj);
    if ( p == NULL )
        return JS_FALSE;

    wxTreeItemId *id = TreeItemId::GetPrivate(cx, argv[0]);
    if ( id != NULL )
    {
        *rval = TreeItemId::CreateObject(cx, new wxTreeItemId(p->GetPrevVisible(*id)));
        return JS_TRUE;
    }
    return JS_FALSE;
}

/***
 * <method name="getNextVisible">
 *  <function returns="@wxTreeItemId">
 *   <arg name="Id" type="@wxTreeItemId">
 *    The id of the item
 *   </arg>
 *  </function>
 *  <desc>
 *   Returns the next visible item. The item itself must also be visible.
 *   See @wxTreeItem @wxTreeItem#nextSibling property
 *  </desc>
 * </method>
 */
JSBool TreeCtrl::getNextVisible(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    wxTreeCtrl *p = GetPrivate(cx, obj);
    if ( p == NULL )
        return JS_FALSE;

    wxTreeItemId *id = TreeItemId::GetPrivate(cx, argv[0]);
    if ( id != NULL )
    {
        *rval = TreeItemId::CreateObject(cx, new wxTreeItemId(p->GetNextVisible(*id)));
        return JS_TRUE;
    }
    return JS_FALSE;
}


/***
 * <method name="addRoot">
 *  <function returns="@wxTreeItemId">
 *   <arg name="Text" type="String">
 *    The text for the root item.
 *   </arg>
 *   <arg name="Image" type="Integer" default="-1">
 *    Index of the image for normal items. The default is -1 which means don't use.
 *   </arg>
 *   <arg name="SelectedImage" type="Integer" default="-1">
 *    Index of the image for selected items. The default is -1 which means don't use.
 *   </arg>
 *   <arg name="Data" type="Any" default="null" />
 *  </function>
 *  <desc>
 *   Adds the root node to the tree, returning the id of the new item.
 *   The <i>Image</i> and <i>SelectedImage</i> parameters are an index within the normal image list 
 *   specifying the image to use for unselected and selected items, respectively.
 *   If <i>Image</i> > -1 and <i>SelectedImage</i> is -1, the same image is used for both selected 
 *   and unselected items.
 *   <br /><br />
 *   The following example adds a root item and 3 childs to the root.
 *   <pre><code class="whjs">
 *     var dlg = new wxDialog(null, -1, "Tree example");
 *     dlg.tree = new wxTreeCtrl(dlg, 1);
 *
 *     var root = dlg.tree.addRoot("Root");
 *     dlg.tree.appendItem(root, "Child 1");
 *     dlg.tree.appendItem(root, "Child 2");
 *     dlg.tree.appendItem(root, "Child 3");
 *    </code></pre>
 *  </desc>
 * </method>
 */
JSBool TreeCtrl::addRoot(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    wxTreeCtrl *p = GetPrivate(cx, obj);
    if ( p == NULL )
        return JS_FALSE;

    if (argc > 4 )
        argc = 4;

    ObjectTreeData *data = NULL;
    int selImage = -1;
    int image = -1;

    switch(argc)
    {
    case 4:
        data = new ObjectTreeData(cx, argv[3]);
        // Fall through
    case 3:
        if ( ! FromJS(cx, argv[2], selImage) )
            break;
        // Fall through
    case 2:
        if ( ! FromJS(cx, argv[1], image) )
            break;
        // Fall through
    default:

        wxString text;
        FromJS(cx, argv[0], text);
        *rval = TreeItemId::CreateObject(cx, new wxTreeItemId(p->AddRoot(text, image, selImage, data)));
        return JS_TRUE;
    }
    return JS_FALSE;
}

/***
 * <method name="appendItem">
 *  <function returns="@wxTreeItemId">
 *   <arg name="Parent" type="@wxTreeItemId">
 *    The id of the parent item.
 *   </arg>
 *   <arg name="Text" type="String">
 *    The text for the root item.
 *   </arg>
 *   <arg name="Image" type="Integer" default="-1">
 *    Index of the image for normal items. The default is -1 which means don't use.
 *   </arg>
 *   <arg name="SelectedImage" type="Integer" default="-1">
 *    Index of the image for selected items. The default is -1 which means don't use.
 *   </arg>
 *   <arg name="Data" type="Any" default="null" />
 *  </function>
 *  <desc>
 *   Appends an item to the end of the branch identified by parent, return a new item id.
 *   The <i>Image</i> and <i>SelectedImage</i> parameters are an index within the normal image list 
 *   specifying the image to use for unselected and selected items, respectively.
 *   If <i>Image</i> > -1 and <i>SelectedImage</i> is -1, the same image is used for both selected 
 *   and unselected items.
 *  </desc>
 * </method>
 */
JSBool TreeCtrl::appendItem(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    wxTreeCtrl *p = GetPrivate(cx, obj);
    if ( p == NULL )
        return JS_FALSE;

    if (argc > 5 )
        argc = 5;

    ObjectTreeData *data = NULL;
    int selImage = -1;
    int image = -1;

    switch(argc)
    {
    case 5:
        data = new ObjectTreeData(cx, argv[4]);
        // Fall through
    case 4:
        if ( ! FromJS(cx, argv[3], selImage) )
            break;
        // Fall through
    case 3:
        if ( ! FromJS(cx, argv[2], image) )
            break;
        // Fall through
    default:

        wxString text;
        FromJS(cx, argv[1], text);

        wxTreeItemId *parent = TreeItemId::GetPrivate(cx, argv[0]);
        if ( parent == NULL )
            break;

        wxTreeItemId newId = p->AppendItem(*parent, text, image, selImage, data);
        *rval = TreeItemId::CreateObject(cx, new wxTreeItemId(newId));
        return JS_TRUE;
    }
    return JS_FALSE;
}

/***
 * <method name="prependItem">
 *  <function returns="@wxTreeItemId">
 *   <arg name="Parent" type="@wxTreeItemId">
 *    The id of the parent item.
 *   </arg>
 *   <arg name="Text" type="String">
 *    The text for the root item.
 *   </arg>
 *   <arg name="Image" type="Integer" default="-1">
 *    Index of the image for normal items. The default is -1 which means don't use.
 *   </arg>
 *   <arg name="SelectedImage" type="Integer" default="-1">
 *    Index of the image for selected items. The default is -1 which means don't use.
 *   </arg>
 *   <arg name="Data" type="Any" default="null" />
 *  </function>
 *  <desc>
 *   Inserts a new item as the first child of the parent item, returns a new item id.
 *   The <i>Image</i> and <i>SelectedImage</i> parameters are an index within the normal image list 
 *   specifying the image to use for unselected and selected items, respectively.
 *   If <i>Image</i> > -1 and <i>SelectedImage</i> is -1, the same image is used for both selected 
 *   and unselected items.
 *  </desc>
 * </method>
 */
JSBool TreeCtrl::prependItem(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    wxTreeCtrl *p = GetPrivate(cx, obj);
    if ( p == NULL )
        return JS_FALSE;

    if (argc > 5 )
        argc = 5;

    ObjectTreeData *data = NULL;
    int selImage = -1;
    int image = -1;

    switch(argc)
    {
    case 5:
        data = new ObjectTreeData(cx, argv[4]);
        // Fall through
    case 4:
        if ( ! FromJS(cx, argv[3], selImage) )
            break;
        // Fall through
    case 3:
        if ( ! FromJS(cx, argv[2], image) )
            break;
        // Fall through
    default:

        wxString text;
        FromJS(cx, argv[1], text);

        wxTreeItemId *parent = TreeItemId::GetPrivate(cx, argv[0]);
        if ( parent == NULL )
            break;

        wxTreeItemId newId = p->PrependItem(*parent, text, image, selImage, data);
        *rval = TreeItemId::CreateObject(cx, new wxTreeItemId(newId));
        return JS_TRUE;
    }
    return JS_FALSE;
}

/***
 * <method name="insertItem">
 *  <function returns="@wxTreeItemId">
 *   <arg name="Parent" type="@wxTreeItemId">
 *    The id of the parent item.
 *   </arg>
 *   <arg name="Prev" type="@wxTreeItemId">
 *    The id of the item to use as previous sibling.
 *   </arg>
 *   <arg name="Text" type="String">
 *    The text for the root item.
 *   </arg>
 *   <arg name="Image" type="Integer" default="-1">
 *    Index of the image for normal items. The default is -1 which means don't use.
 *   </arg>
 *   <arg name="SelectedImage" type="Integer" default="-1">
 *    Index of the image for selected items. The default is -1 which means don't use.
 *   </arg>
 *  </function>
 *  <function returns="@wxTreeItemId">
 *   <arg name="Parent" type="@wxTreeItemId">
 *    The id of the parent item.
 *   </arg>
 *   <arg name="Pos" type="Integer">
 *    The position of the item to use as previous sibling.
 *   </arg>
 *   <arg name="Text" type="String">
 *    The text for the root item.
 *   </arg>
 *   <arg name="Image" type="Integer" default="-1">
 *    Index of the image for normal items. The default is -1 which means don't use.
 *   </arg>
 *   <arg name="SelectedImage" type="Integer" default="-1">
 *    Index of the image for selected items. The default is -1 which means don't use.
 *   </arg>
 *  </function>
 *  <desc>
 *   Inserts an item after a given one (Prev) or before one identified by its position (Pos).
 *   The <i>Image</i> and <i>SelectedImage</i> parameters are an index within the normal image list 
 *   specifying the image to use for unselected and selected items, respectively.
 *   If <i>Image</i> > -1 and <i>SelectedImage</i> is -1, the same image is used for both selected 
 *   and unselected items.
 *  </desc>
 * </method>
 */
JSBool TreeCtrl::insertItem(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    wxTreeCtrl *p = GetPrivate(cx, obj);
    if ( p == NULL )
        return JS_FALSE;

    if (argc > 6 )
        argc = 6;

    ObjectTreeData *data = NULL;
    int selImage = -1;
    int image = -1;

    switch(argc)
    {
    case 6:
        data = new ObjectTreeData(cx, argv[5]);
        // Fall through
    case 5:
        if ( ! FromJS(cx, argv[4], selImage) )
            break;
        // Fall through
    case 4:
        if ( ! FromJS(cx, argv[3], image) )
            break;
        // Fall through
    default:

        wxString text;
        FromJS(cx, argv[2], text);

        wxTreeItemId *parent = TreeItemId::GetPrivate(cx, argv[0]);
        if ( parent == NULL )
            break;

        wxTreeItemId newId;
        if ( JSVAL_IS_INT(argv[1]) )
        {
            int pos;
            FromJS(cx, argv[1], pos);
            newId = p->InsertItem(*parent, pos, text, image, selImage, data);
        }
        else
        {
            wxTreeItemId *prevId = TreeItemId::GetPrivate(cx, argv[1]);
            if ( prevId == NULL )
                break;
            newId = p->InsertItem(*parent, prevId, text, image, selImage, data);
        }
        *rval = TreeItemId::CreateObject(cx, new wxTreeItemId(newId));
        return JS_TRUE;
    }
    return JS_FALSE;
}

/***
 * <method name="deleteItem">
 *  <function>
 *   <arg name="Id" type="@wxTreeItemId">
 *    The id of the item
 *   </arg>
 *  </function>
 *  <desc>
 *   Deletes the item.
 *   You receive an event as @wxTreeCtrl#onDeleteItem.
 *  </desc>
 * </method>
 */
JSBool TreeCtrl::deleteItem(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    wxTreeCtrl *p = GetPrivate(cx, obj);
    if ( p == NULL )
        return JS_FALSE;

    wxTreeItemId *id = TreeItemId::GetPrivate(cx, argv[0]);
    if ( id != NULL )
    {
        p->Delete(*id);
        return JS_TRUE;
    }
    return JS_FALSE;
}

/***
 * <method name="deleteChildren">
 *  <function>
 *   <arg name="Id" type="@wxTreeItemId">
 *    The id of the item
 *   </arg>
 *  </function>
 *  <desc> 
 *   Deletes all the children of the item. You don't get an event as @wxTreeCtrl#onDeleteItem
 *   event for the deleted items.
 *  </desc>
 * </method>
 */
JSBool TreeCtrl::deleteChildren(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    wxTreeCtrl *p = GetPrivate(cx, obj);
    if ( p == NULL )
        return JS_FALSE;

    wxTreeItemId *id = TreeItemId::GetPrivate(cx, argv[0]);
    if ( id != NULL )
    {
        p->DeleteChildren(*id);
        return JS_TRUE;
    }
    return JS_FALSE;
}

/***
 * <method name="deleteAllItems">
 *  <function />
 *  <desc>
 *   Deletes all items. You don't get an @wxTreeCtrl#onDeleteItem
 *   event for the deleted items.
 *  </desc>
 * </method>
 */
JSBool TreeCtrl::deleteAllItems(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    wxTreeCtrl *p = GetPrivate(cx, obj);
    if ( p == NULL )
        return JS_FALSE;

    p->DeleteAllItems();
    return JS_TRUE;
}

/***
 * <method name="expand">
 *  <function>
 *   <arg name="Id" type="@wxTreeItemId">
 *    The id of the item
 *   </arg>
 *  </function>
 *  <desc>
 *   Expands the item.
 *   See @wxTreeItem @wxTreeItem#expanded property
 *  </desc>
 * </method>
 */
JSBool TreeCtrl::expand(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    wxTreeCtrl *p = GetPrivate(cx, obj);
    if ( p == NULL )
        return JS_FALSE;

    wxTreeItemId *id = TreeItemId::GetPrivate(cx, argv[0]);
    if ( id != NULL )
    {
        p->Expand(*id);
        return JS_TRUE;
    }
    return JS_FALSE;
}

/***
 * <method name="collapse">
 *  <function>
 *   <arg name="Id" type="@wxTreeItemId">
 *    The id of the item
 *   </arg>
 *  </function>
 *  <desc>
 *   Collapses the item.
 *   See @wxTreeItem @wxTreeItem#expanded property
 *  </desc>
 * </method>
 */
JSBool TreeCtrl::collapse(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    wxTreeCtrl *p = GetPrivate(cx, obj);
    if ( p == NULL )
        return JS_FALSE;

    wxTreeItemId *id = TreeItemId::GetPrivate(cx, argv[0]);
    if ( id != NULL )
    {
        p->Collapse(*id);
        return JS_TRUE;
    }
    return JS_FALSE;
}

/***
 * <method name="collapseAndReset">
 *  <function>
 *   <arg name="Id" type="@wxTreeItemId">
 *    The id of the item
 *   </arg>
 *  </function>
 *  <desc>
 *   Collapses the item and removes the children.
 *  </desc>
 * </method>
 */
JSBool TreeCtrl::collapseAndReset(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    wxTreeCtrl *p = GetPrivate(cx, obj);
    if ( p == NULL )
        return JS_FALSE;

    wxTreeItemId *id = TreeItemId::GetPrivate(cx, argv[0]);
    if ( id != NULL )
    {
        p->CollapseAndReset(*id);
        return JS_TRUE;
    }
    return JS_FALSE;
}

/***
 * <method name="toggle">
 *  <function>
 *   <arg name="Id" type="@wxTreeItemId">
 *    The id of the item
 *   </arg>
 *  </function>
 *  <desc>
 *   Toggles the given item between collapsed and expanded states.
 *   See @wxTreeItem @wxTreeItem#expanded property
 *  </desc>
 * </method>
 */
JSBool TreeCtrl::toggle(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    wxTreeCtrl *p = GetPrivate(cx, obj);
    if ( p == NULL )
        return JS_FALSE;

    wxTreeItemId *id = TreeItemId::GetPrivate(cx, argv[0]);
    if ( id != NULL )
    {
        p->Toggle(*id);
        return JS_TRUE;
    }
    return JS_FALSE;
}

/***
 * <method name="unselect">
 *  <function />
 *  <desc>
 *   Unselects the selected item.
 *   See @wxTreeItem @wxTreeItem#selected property
 *  </desc>
 * </method>
 */
JSBool TreeCtrl::unselect(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    wxTreeCtrl *p = GetPrivate(cx, obj);
    if ( p == NULL )
        return JS_FALSE;

    p->Unselect();
    return JS_TRUE;
}

/***
 * <method name="unselectAll">
 *  <function />
 *  <desc>
 *   Unselects all selected items.
 *  </desc>
 * </method>
 */
JSBool TreeCtrl::unselectAll(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    wxTreeCtrl *p = GetPrivate(cx, obj);
    if ( p == NULL )
        return JS_FALSE;

    p->UnselectAll();
    return JS_TRUE;
}

/***
 * <method name="selectItem">
 *  <function>
 *   <arg name="Id" type="@wxTreeItemId">
 *    The id of the item
 *   </arg>
 *  </function>
 *  <desc>
 *   Selects the given item.
 *   See @wxTreeItem @wxTreeItem#selected property
 *  </desc>
 * </method>
 */
JSBool TreeCtrl::selectItem(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    wxTreeCtrl *p = GetPrivate(cx, obj);
    if ( p == NULL )
        return JS_FALSE;

    wxTreeItemId *id = TreeItemId::GetPrivate(cx, argv[0]);
    if ( id != NULL )
    {
        p->SelectItem(*id);
        return JS_TRUE;
    }
    return JS_FALSE;
}

/***
 * <method name="ensureVisible">
 *  <function>
 *   <arg name="Id" type="@wxTreeItemId">
 *    The id of the item
 *   </arg>
 *  </function>
 *  <desc>
 *   Make sure the item is visible (expanding the parent item and/or
 *   scrolling to this item if necessary)
 *   See @wxTreeItem @wxTreeItem#visible property
 *  </desc>
 * </method>
 */
JSBool TreeCtrl::ensureVisible(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    wxTreeCtrl *p = GetPrivate(cx, obj);
    if ( p == NULL )
        return JS_FALSE;

    wxTreeItemId *id = TreeItemId::GetPrivate(cx, argv[0]);
    if ( id != NULL )
    {
        p->EnsureVisible(*id);
        return JS_TRUE;
    }
    return JS_FALSE;
}

/***
 * <method name="scrollTo">
 *  <function>
 *   <arg name="Id" type="@wxTreeItemId">
 *    The id of the item
 *   </arg>
 *  </function>
 *  <desc>
 *   Scroll to the item (but don't expand its parent)
 *  </desc>
 * </method>
 */
JSBool TreeCtrl::scrollTo(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    wxTreeCtrl *p = GetPrivate(cx, obj);
    if ( p == NULL )
        return JS_FALSE;

    wxTreeItemId *id = TreeItemId::GetPrivate(cx, argv[0]);
    if ( id != NULL )
    {
        p->ScrollTo(*id);
        return JS_TRUE;
    }
    return JS_FALSE;
}

/***
 * <method name="editLabel">
 *  <function>
 *   <arg name="Id" type="@wxTreeItemId">
 *    The id of the item
 *   </arg>
 *  </function>
 *  <desc>
 *   Start editing the label of the item. The item will be selected when
 *   it hadn't been before.
 *  </desc>
 * </method>
 */
JSBool TreeCtrl::editLabel(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    wxTreeCtrl *p = GetPrivate(cx, obj);
    if ( p == NULL )
        return JS_FALSE;

    wxTreeItemId *id = TreeItemId::GetPrivate(cx, argv[0]);
    if ( id != NULL )
    {
        p->EditLabel(*id);
        return JS_TRUE;
    }
    return JS_FALSE;
}

/***
 * <method name="endEditLabel">
 *  <function>
 *   <arg name="Id" type="@wxTreeItemId">
 *    The id of the item
 *   </arg>
 *   <arg name="Discard" type="Boolean" default="false">
 *    Accept (false) or discard(true) the changes to the item label. Default is false.
 *   </arg>
 *  </function>
 *  <desc>
 *   Ends editing the label of the item. Only on Windows
 *  </desc>
 * </method>
 */
JSBool TreeCtrl::endEditLabel(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    wxTreeCtrl *p = GetPrivate(cx, obj);
    if ( p == NULL )
        return JS_FALSE;

	#ifdef __WXMSW__
        wxTreeItemId *id = TreeItemId::GetPrivate(cx, argv[0]);
        bool discard = false;
        if ( id != NULL )
        {
            p->EditLabel(*id);
	        if ( argc > 1 )
	        {
	            if ( ! FromJS(cx, argv[1], discard) )
	                return JS_FALSE;
	        }
        }
	    p->EndEditLabel(*id, discard);
	#endif
	
    return JS_TRUE;
}

/***
 * <method name="sortChildren">
 *  <function>
 *   <arg name="Id" type="@wxTreeItemId">
 *    The id of the item
 *   </arg>
 *  </function>
 *  <desc>
 *   Sorts the children of the given item using the function of @wxTreeCtrl#onCompareItems.
 *   When no function is set, the items are sorted alphabetically.
 *  </desc>
 * </method>
 */
JSBool TreeCtrl::sortChildren(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    wxTreeCtrl *p = GetPrivate(cx, obj);
    if ( p == NULL )
        return JS_FALSE;

    wxTreeItemId *id = TreeItemId::GetPrivate(cx, argv[0]);
    if ( id != NULL )
    {
        p->SortChildren(*id);
        return JS_TRUE;
    }
    return JS_FALSE;
}

/***
 * <method name="hitTest">
 *  <function returns="@wxTreeHitTest">
 *   <arg name="Pos" type="@wxPoint">
 *    The position to test.
 *   </arg>
 *  </function>
 *  <desc>
 *   Determines which item (if any) is at the specified point.
 *  </desc>
 * </method>
 */
JSBool TreeCtrl::hitTest(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    wxTreeCtrl *p = GetPrivate(cx, obj);
    if ( p == NULL )
        return JS_FALSE;

    wxPoint *pt = Point::GetPrivate(cx, argv[0]);
    if ( pt != NULL )
    {
        int flags;
        wxTreeItemId item = p->HitTest(*pt, flags);

        *rval = TreeHitTest::CreateObject(cx, new wxTreeHitTest(item, flags));
        return JS_TRUE;
    }
    
    return JS_FALSE;
}

/***
 * <events>
 * <event name="onBeginDrag">
 *  This event is triggered when the user starts dragging with the left mouse button.
 *  The event can be vetoed using @wxNotifyEvent#veto or @wxNotifyEvent#allowed.
 *  The function receives a @wxTreeEvent as argument.
 * </event>
 * <event name="onBeginRDrag">
 *  This event is triggered when the user starts dragging with the right mouse button.
 *  The event can be vetoed using @wxNotifyEvent#veto or @wxNotifyEvent#allowed.
 *  The function receives a @wxTreeEvent as argument.
 * </event>
 * <event name="onBeginLabelEdit">
 *  This event is triggered when the user starts editing an item.
 *  The event can be vetoed using @wxNotifyEvent#veto or @wxNotifyEvent#allowed.
 *  The function receives a @wxTreeEvent as argument.
 * </event>
 * <event name="onEndLabelEdit">
 *  This event is triggered when the user ends editing the label.
 *  The function receives a @wxTreeEvent as argument.
 *  The event can be vetoed using @wxNotifyEvent#veto or @wxNotifyEvent#allowed.
 * </event>
 * <event name="onDeleteItem">
 *  This event is triggered when an item is deleted.
 *  See @wxTreeCtrl#deleteItem.
 *  The function receives a @wxTreeEvent as argument.
 * </event>
 * <event name="onGetInfo">
 *  This event is triggered when the application needs information.
 *  The function receives a @wxTreeEvent as argument.
 * </event>
 * <event name="onSetInfo">
 *  This event is triggered when information is supplied.
 *  The function receives a @wxTreeEvent as argument.
 * </event>
 * <event name="onItemExpanded">
 *  This event is triggered when an item is expanded.
 *  The function receives a @wxTreeEvent as argument.
 * </event>
 * <event name="onItemExpanding">
 *  This event is triggered when an item is about to expanded.
 *  The event can be vetoed using @wxNotifyEvent#veto or @wxNotifyEvent#allowed.
 *  The function receives a @wxTreeEvent as argument.
 * </event>
 * <event name="onItemCollapsed">
 *  This event is triggered when an item is collapsed.
 *  The function receives a @wxTreeEvent as argument.
 * </event>
 * <event name="onItemCollapsing">
 *  This event is triggered when an item is about to collapse.
 *  The event can be vetoed using @wxNotifyEvent#veto or @wxNotifyEvent#allowed.
 *  The function receives a @wxTreeEvent as argument.
 * </event>
 * <event name="onSelChanged">
 *  This event is triggered when the selection is changed.
 *  The function receives a @wxTreeEvent as argument.
 * </event>
 * <event name="onSelChanging">
 *  This event is triggered when the selection is about to change.
 *  The function receives a @wxTreeEvent as argument.
 * </event>
 * <event name="onKeyDown">
 *  This event is triggered when a key is pressed.
 *  The function receives a @wxTreeEvent as argument.
 * </event>
 * <event name="onItemActivated">
 *  This event is triggered when an item is activated.
 *  The function receives a @wxTreeEvent as argument.
 * </event>
 * <event name="onItemRightClick">
 *  This event is triggered when an item is clicked with the right mousebutton.
 *  The function receives a @wxTreeEvent as argument.
 * </event>
 * <event name="onItemMiddleClick">
 *  This event is triggered when an item is clicked with the middle mousebutton.
 *  The function receives a @wxTreeEvent as argument.
 * </event>
 * <event name="onEndDrag">
 *  This event is triggered when the user releases the mouse button.
 *  The function receives a @wxTreeEvent as argument.
 * </event>
 * </events>
 */
BEGIN_EVENT_TABLE(TreeCtrl, wxTreeCtrl)
    EVT_TREE_BEGIN_DRAG(-1, TreeCtrl::onBeginDrag)
    EVT_TREE_BEGIN_RDRAG(-1, TreeCtrl::onBeginRDrag)
    EVT_TREE_BEGIN_LABEL_EDIT(-1, TreeCtrl::onBeginLabelEdit)
    EVT_TREE_END_LABEL_EDIT(-1, TreeCtrl::onEndLabelEdit)
    EVT_TREE_DELETE_ITEM(-1, TreeCtrl::onDeleteItem)
    EVT_TREE_GET_INFO(-1, TreeCtrl::onGetInfo)
    EVT_TREE_SET_INFO(-1, TreeCtrl::onSetInfo)
    EVT_TREE_ITEM_EXPANDED(-1, TreeCtrl::onItemExpanded)
    EVT_TREE_ITEM_EXPANDING(-1, TreeCtrl::onItemExpanding)
    EVT_TREE_ITEM_COLLAPSED(-1, TreeCtrl::onItemCollapsed)
    EVT_TREE_ITEM_COLLAPSING(-1, TreeCtrl::onItemCollapsing)
    EVT_TREE_SEL_CHANGED(-1, TreeCtrl::onSelChanged)
    EVT_TREE_SEL_CHANGING(-1, TreeCtrl::onSelChanging)
    EVT_TREE_KEY_DOWN(-1, TreeCtrl::onKeyDown)
    EVT_TREE_ITEM_ACTIVATED(-1, TreeCtrl::onItemActivated)
    EVT_TREE_ITEM_RIGHT_CLICK(-1, TreeCtrl::onItemRightClick)
    EVT_TREE_ITEM_MIDDLE_CLICK(-1, TreeCtrl::onItemMiddleClick)
    EVT_TREE_END_DRAG(-1, TreeCtrl::onEndDrag)
END_EVENT_TABLE()

void TreeCtrl::onBeginDrag(wxTreeEvent &event)
{
	PrivTreeEvent::Fire<TreeEvent>(this, event, "onBeginDrag");
}

void TreeCtrl::onBeginRDrag(wxTreeEvent &event)
{
	PrivTreeEvent::Fire<TreeEvent>(this, event, "onBeginRDrag");
}

void TreeCtrl::onBeginLabelEdit(wxTreeEvent &event)
{
	PrivTreeEvent::Fire<TreeEvent>(this, event, "onBeginLabelEdit");
}

void TreeCtrl::onEndLabelEdit(wxTreeEvent &event)
{
	PrivTreeEvent::Fire<TreeEvent>(this, event, "onEndLabelEdit");
}

void TreeCtrl::onDeleteItem(wxTreeEvent &event)
{
	PrivTreeEvent::Fire<TreeEvent>(this, event, "onDeleteItem");
}

void TreeCtrl::onGetInfo(wxTreeEvent &event)
{
	PrivTreeEvent::Fire<TreeEvent>(this, event, "onGetInfo");
}

void TreeCtrl::onSetInfo(wxTreeEvent &event)
{
	PrivTreeEvent::Fire<TreeEvent>(this, event, "onSetInfo");
}

void TreeCtrl::onItemExpanded(wxTreeEvent &event)
{
	PrivTreeEvent::Fire<TreeEvent>(this, event, "onItemExpanded");
}

void TreeCtrl::onItemExpanding(wxTreeEvent &event)
{
	PrivTreeEvent::Fire<TreeEvent>(this, event, "onItemExpanding");
}

void TreeCtrl::onItemCollapsed(wxTreeEvent &event)
{
	PrivTreeEvent::Fire<TreeEvent>(this, event, "onItemCollapsed");
}

void TreeCtrl::onItemCollapsing(wxTreeEvent &event)
{
	PrivTreeEvent::Fire<TreeEvent>(this, event, "onItemCollapsing");
}

void TreeCtrl::onSelChanged(wxTreeEvent &event)
{
	PrivTreeEvent::Fire<TreeEvent>(this, event, "onSelChanged");
}

void TreeCtrl::onSelChanging(wxTreeEvent &event)
{
	PrivTreeEvent::Fire<TreeEvent>(this, event, "onSelChanging");
}

void TreeCtrl::onKeyDown(wxTreeEvent &event)
{
	PrivTreeEvent::Fire<TreeEvent>(this, event, "onKeyDown");
}

void TreeCtrl::onItemActivated(wxTreeEvent &event)
{
	PrivTreeEvent::Fire<TreeEvent>(this, event, "onItemActivated");
}

void TreeCtrl::onItemRightClick(wxTreeEvent &event)
{
	PrivTreeEvent::Fire<TreeEvent>(this, event, "onItemRightClick");
}

void TreeCtrl::onItemMiddleClick(wxTreeEvent &event)
{
	PrivTreeEvent::Fire<TreeEvent>(this, event, "onItemMiddleClick");
}

void TreeCtrl::onEndDrag(wxTreeEvent &event)
{
	PrivTreeEvent::Fire<TreeEvent>(this, event, "onEndDrag");
}
