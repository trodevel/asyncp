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

// $Revision: 10614 $ $Date:: 2019-03-10 #$ $Author: serge $

#include "async_proxy.h"                // self

#include <algorithm>                    // std::find

#include "../utils/utils_assert.h"            // ASSERT
#include "../utils/mutex_helper.h"      // MUTEX_SCOPE_LOCK
#include "../utils/dummy_logger.h"      // dummy_log

#define MODULENAME      "AsyncProxy"

NAMESPACE_ASYNCP_START

AsyncProxy::AsyncProxy():
    should_run_( true )
{
}

AsyncProxy::~AsyncProxy()
{
}

bool AsyncProxy::init( const Config & cfg )
{
    MUTEX_SCOPE_LOCK( mutex_ );

    dummy_log_info( MODULENAME, "init() - %s", cfg.name.c_str() );

    cfg_    = cfg;

    return true;
}

void AsyncProxy::thread_func()
{
    dummy_log_info( MODULENAME, "thread_func: started" );

    while( true )
    {
        {
            std::unique_lock<std::mutex> lock( mutex_cond_ );
            cond_.wait( lock );
        }

        {
            MUTEX_SCOPE_LOCK( mutex_ );

            if( should_run_ == false )
                break;

            if( has_events() )
            {
                check_queue();
            }
        }
    }

    dummy_log_info( MODULENAME, "thread_func: ended" );
}

bool AsyncProxy::has_events() const
{
    // private: no MUTEX lock
    if( events_.empty() )
        return false;

    return true;
}

void AsyncProxy::check_queue()
{
    // private: no MUTEX lock

    if( events_.empty() )
        return;

    IEventPtr ev = events_.front();

    events_.pop_front();

    ev->invoke();
}

void AsyncProxy::wakeup()
{
    // PRIVATE:

    cond_.notify_one();     // wake-up the thread
}

bool AsyncProxy::add_event( IEventPtr event )
{
    MUTEX_SCOPE_LOCK( mutex_ );

    if( std::find( events_.begin(), events_.end(), event ) != events_.end() )
    {
        dummy_log_error( MODULENAME, "add_event: event %p already exists", event.get() );

        return false;
    }

    events_.push_back( event );

    dummy_log_debug( MODULENAME, "add_event: added event %p", event.get() );

    wakeup();

    return true;
}

bool AsyncProxy::remove_event( IEventPtr event )
{
    MUTEX_SCOPE_LOCK( mutex_ );

    EventList::iterator it = std::find( events_.begin(), events_.end(), event );

    if( it == events_.end() )
    {
        dummy_log_error( MODULENAME, "remove_event: cannot remove - event %p not found", event.get() );

        return false;
    }

    events_.erase( it );

    dummy_log_debug( MODULENAME, "remove_event: removed event %p", event.get() );

    return true;
}

bool AsyncProxy::shutdown()
{
    MUTEX_SCOPE_LOCK( mutex_ );

    should_run_ = false;

    wakeup();

    return true;
}

NAMESPACE_ASYNCP_END
