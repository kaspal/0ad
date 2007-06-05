#include "precompiled.h"

/*
 * wxJavaScript - mostream.cpp
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
 * $Id: mostream.cpp 598 2007-03-07 20:13:28Z fbraem $
 */
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "../common/main.h"

#include "stream.h"
#include "mostream.h"

#include "../ext/wxjs_ext.h"
#include "../ext/membuf.h"

using namespace wxjs;
using namespace wxjs::io;

MemoryOutputStream::MemoryOutputStream(  JSContext *cx 
											   , JSObject *obj
											   , char *data
											   , size_t len) : wxMemoryOutputStream(data, len)
                                                             , Object(obj, cx)
                                                             , m_data(data)
{
    if ( m_data == NULL )
        GetOutputStreamBuffer()->SetBufferIO(1024);
}

MemoryOutputStream::~MemoryOutputStream()
{
    delete[] m_data;
}

/***
 * <file>mostream</file>
 * <module>io</module>
 * <class name="wxMemoryOutputStream" prototype="@wxOutputStream" version="0.8.2">
 *  wxMemoryOutputStream collects its output in a buffer which can be converted to a String.
 *  See also @wxMemoryInputStream.
 *  An example:
 *  <pre><code class="whjs">
 *   var mos = new wxMemoryOutputStream();
 *   mos.write("This is a test");
 *  </code></pre>
 * </class>
 */
WXJS_INIT_CLASS(MemoryOutputStream, "wxMemoryOutputStream", 0)

/***
 * <ctor>
 *  <function>
 *   <arg name="Length" type="Integer" default="1024">
 *    The length of the buffer used in memory.
 *   </arg>
 *  </function>
 *  <desc>
 *   Constructs a new wxMemoryOutputStream object. When no length is specified, a buffer is 
 *   created with size 1024. If necessary it will grow.
 *  </desc>
 * </ctor>
 */
Stream* MemoryOutputStream::Construct(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, bool constructing)
{
    if ( argc == 0 )
    {	
        return new Stream(new MemoryOutputStream(cx, obj, NULL, 0));
    }

    int length;
    if ( FromJS(cx, argv[0], length) )
    {
        char *dataPtr = new char[length];
        return new Stream(new MemoryOutputStream(cx, obj, dataPtr, length));
    }

    return NULL;
}

/***
 * <properties>
 *  <property name="data" type="@wxMemoryBuffer" readonly="Y">
 *   Gets the buffer.
 *  </property>
 * </properties>
 */
WXJS_BEGIN_PROPERTY_MAP(MemoryOutputStream)
    WXJS_READONLY_PROPERTY(P_DATA, "data")
WXJS_END_PROPERTY_MAP()

bool MemoryOutputStream::GetProperty(Stream *p, JSContext *cx, JSObject *obj, int id, jsval *vp)
{
    MemoryOutputStream *mstream = (MemoryOutputStream *) p->GetStream();

    switch(id)
    {
    case P_DATA:
        {
            int size = mstream->GetOutputStreamBuffer()->GetIntPosition();
            char* buffer = new char[size];
            mstream->CopyTo(buffer, size);

            *vp = OBJECT_TO_JSVAL(wxjs::ext::CreateMemoryBuffer(cx, buffer, size));
			delete[] buffer;
            break;
        }
    }
    return true;
}
