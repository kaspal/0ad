/*
 * wxJavaScript - textval.h
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
 * $Id: textval.h 598 2007-03-07 20:13:28Z fbraem $
 */
#ifndef _WXJSTextValidator_H
#define _WXJSTextValidator_H

/////////////////////////////////////////////////////////////////////////////
// Name:        .h
// Purpose:     TextValidator ports wxTextValidator to JavaScript
// Author:      Franky Braem
// Modified by:
// Created:     27-12-2002
// Copyright:   (c) 2001-2002 Franky Braem
// Licence:     LGPL
/////////////////////////////////////////////////////////////////////////////


namespace wxjs
{
    namespace gui
    {
        class TextValidator : public wxTextValidator
                                , public ApiWrapper<TextValidator, wxTextValidator>
        {
        public:

            TextValidator(long style = wxFILTER_NONE,
                          const wxString &value = wxEmptyString);

            /**
             * Callback for retrieving properties of wxTextValidator
             */
            static bool GetProperty(wxTextValidator *p, JSContext *cx, JSObject *obj, int id, jsval *vp);

            /**
             * Callback for setting properties
             */
            static bool SetProperty(wxTextValidator *p, JSContext *cx, JSObject *obj, int id, jsval *vp);

            /**
             * Callback for when a wxTextValidator object is created
             */
            static wxTextValidator* Construct(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, bool constructing);

            WXJS_DECLARE_PROPERTY_MAP()

            /**
             * Property Ids.
             */
            enum
            {
                P_EXCLUDE_LIST
                , P_INCLUDE_LIST
                , P_STYLE
                , P_VALUE
            };

        private:
            wxString m_value;
        };
    }; // namespace gui
}; // namespace wxjs

#endif //_WXJSTextValidator_H
