/*

Async Proxy.

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

// $Revision: 3420 $ $Date:: 2016-02-16 #$ $Author: serge $

#ifndef ASYNCP_ASYNC_PROXY_H
#define ASYNCP_ASYNC_PROXY_H

#include <string>                   // std::string
#include <list>
#include <mutex>                    // std::mutex
#include <condition_variable>       // std::condition_variable
#include <cstdint>                  // uint32_t

#include "i_async_proxy.h"              // IAsyncProxy, IEvent
#include "../threcon/i_controllable.h"  // IControllable

#include "namespace_asyncp.h"       // NAMESPACE_ASYNCP_START

NAMESPACE_ASYNCP_START

class AsyncProxy: public virtual IAsyncProxy, public virtual threcon::IControllable
{
public:
    struct Config
    {
        uint32_t    sleep_time_ms;  // try 1 ms
        std::string name;
    };

public:
    AsyncProxy();
    ~AsyncProxy();

    bool init( const Config & cfg );
    void thread_func();

    // interface IAsyncProxy
    virtual bool add_event( IEventPtr event );
    virtual bool remove_event( IEventPtr event );

    // interface threcon::IControllable
    virtual bool shutdown();

private:

    bool has_events() const;
    void check_queue();
    void wakeup();

private:

    typedef std::list< IEventPtr > EventList;

private:
    mutable std::mutex      mutex_;
    mutable std::mutex      mutex_cond_;
    mutable std::condition_variable    cond_;

    bool                    should_run_;
    Config                  cfg_;

    EventList               events_;
};

NAMESPACE_ASYNCP_END

#endif  // ASYNCP_ASYNC_PROXY_H
