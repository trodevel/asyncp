/*

interface Async Proxy.

Copyright (C) 2014 Sergey Kolevatov

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.

*/

// $Id: i_async_proxy.h 542 2014-05-21 17:21:57Z serge $

#ifndef ASYNCP_I_ASYNC_PROXY_H
#define ASYNCP_I_ASYNC_PROXY_H

#include <boost/shared_ptr.hpp>     // shared_pt

#include "../utils/types.h"         // uint32

#include "namespace_asyncp.h"       // NAMESPACE_ASYNCP_START

NAMESPACE_ASYNCP_START

class IEvent
{
public:
    virtual ~IEvent() {}

    virtual void invoke()   = 0;
};

typedef boost::shared_ptr< IEvent > IEventPtr;

class IAsyncProxy
{
public:
    virtual ~IAsyncProxy() {};

    virtual bool add_event( IEventPtr event )       = 0;
    virtual bool remove_event( IEventPtr event )    = 0;
};

NAMESPACE_ASYNCP_END

#endif  // ASYNCP_I_ASYNC_PROXY_H
