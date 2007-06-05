/*
 * wxJavaScript - stsizer.h
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
 * $Id: stsizer.h 598 2007-03-07 20:13:28Z fbraem $
 */
/**
 * (c) 2001-2002 Franky Braem (S.A.W.)
 *
 * This file is part of wxJS. wxJS ports wxWindows to JavaScript
 * 
 */

#ifndef _wxJSStaticBoxSizer_H
#define _wxJSStaticBoxSizer_H

namespace wxjs
{
    class Object;

    namespace gui
    {
        class StaticBoxSizer : public wxStaticBoxSizer
                                 , public ApiWrapper<StaticBoxSizer, StaticBoxSizer>
                                 , public Object
						         , public AttachedSizer
        {
        public:
            /**
             * Constructor
             */
            StaticBoxSizer(JSContext *cx, JSObject *obj, wxStaticBox *box, int orient);

            /**
             * Destructor
             */
            virtual ~StaticBoxSizer()
            {
                if ( IsAttached() )
                {
                    JS_SetPrivate(GetContext(), GetObject(), NULL);
                }
            }

            static StaticBoxSizer* Construct(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, bool constructing);
            static void Destruct(JSContext *cx, StaticBoxSizer *p);

	        static bool GetProperty(wxStaticBoxSizer *p, JSContext *cx, JSObject *obj, int id, jsval *vp);
        	
            WXJS_DECLARE_PROPERTY_MAP()

	        enum
	        {
		        P_STATIC_BOX
	        };
        };
    }; // namespace gui
}; // namespace wxjs

#endif //_wxJSStaticBoxSizer_H

