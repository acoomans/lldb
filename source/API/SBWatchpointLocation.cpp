//===-- SBWatchpointLocation.cpp --------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "lldb/API/SBWatchpointLocation.h"
#include "lldb/API/SBDefines.h"
#include "lldb/API/SBAddress.h"
#include "lldb/API/SBDebugger.h"
#include "lldb/API/SBStream.h"

#include "lldb/lldb-types.h"
#include "lldb/lldb-defines.h"
#include "lldb/Breakpoint/WatchpointLocation.h"
#include "lldb/Breakpoint/WatchpointLocationList.h"
#include "lldb/Core/Log.h"
#include "lldb/Core/Stream.h"
#include "lldb/Core/StreamFile.h"
#include "lldb/Target/Target.h"

using namespace lldb;
using namespace lldb_private;


SBWatchpointLocation::SBWatchpointLocation () :
    m_opaque_sp ()
{
}

SBWatchpointLocation::SBWatchpointLocation (const lldb::WatchpointLocationSP &watch_loc_sp) :
    m_opaque_sp (watch_loc_sp)
{
    LogSP log(lldb_private::GetLogIfAllCategoriesSet (LIBLLDB_LOG_API));

    if (log)
    {
        SBStream sstr;
        GetDescription (sstr, lldb::eDescriptionLevelBrief);
        log->Printf ("SBWatchpointLocation::SBWatchpointLocation (const lldb::WatchpointLocationsSP &watch_loc_sp"
                     "=%p)  => this.sp = %p (%s)", watch_loc_sp.get(), m_opaque_sp.get(), sstr.GetData());
    }
}

SBWatchpointLocation::SBWatchpointLocation(const SBWatchpointLocation &rhs) :
    m_opaque_sp (rhs.m_opaque_sp)
{
}

const SBWatchpointLocation &
SBWatchpointLocation::operator = (const SBWatchpointLocation &rhs)
{
    if (this != &rhs)
        m_opaque_sp = rhs.m_opaque_sp;
    return *this;
}


SBWatchpointLocation::~SBWatchpointLocation ()
{
}

watch_id_t
SBWatchpointLocation::GetID () const
{
    LogSP log(lldb_private::GetLogIfAllCategoriesSet (LIBLLDB_LOG_API));

    watch_id_t watch_id = LLDB_INVALID_WATCH_ID;
    if (m_opaque_sp)
        watch_id = m_opaque_sp->GetID();

    if (log)
    {
        if (watch_id == LLDB_INVALID_WATCH_ID)
            log->Printf ("SBWatchpointLocation(%p)::GetID () => LLDB_INVALID_WATCH_ID", m_opaque_sp.get());
        else
            log->Printf ("SBWatchpointLocation(%p)::GetID () => %u", m_opaque_sp.get(), watch_id);
    }

    return watch_id;
}

bool
SBWatchpointLocation::IsValid() const
{
    return m_opaque_sp.get() != NULL;
}

int32_t
SBWatchpointLocation::GetHardwareIndex () const
{
    int32_t hw_index = -1;

    if (m_opaque_sp)
    {
        Mutex::Locker api_locker (m_opaque_sp->GetTarget().GetAPIMutex());
        hw_index = m_opaque_sp->GetHardwareIndex();
    }

    return hw_index;
}

addr_t
SBWatchpointLocation::GetWatchAddress () const
{
    addr_t ret_addr = LLDB_INVALID_ADDRESS;

    if (m_opaque_sp)
    {
        Mutex::Locker api_locker (m_opaque_sp->GetTarget().GetAPIMutex());
        ret_addr = m_opaque_sp->GetLoadAddress();
    }

    return ret_addr;
}

size_t
SBWatchpointLocation::GetWatchSize () const
{
    size_t watch_size = 0;

    if (m_opaque_sp)
    {
        Mutex::Locker api_locker (m_opaque_sp->GetTarget().GetAPIMutex());
        watch_size = m_opaque_sp->GetByteSize();
    }

    return watch_size;
}

void
SBWatchpointLocation::SetEnabled (bool enabled)
{
    if (m_opaque_sp)
    {
        Mutex::Locker api_locker (m_opaque_sp->GetTarget().GetAPIMutex());
        m_opaque_sp->GetTarget().DisableWatchpointLocationByID(m_opaque_sp->GetID());
    }
}

bool
SBWatchpointLocation::IsEnabled ()
{
    if (m_opaque_sp)
    {
        Mutex::Locker api_locker (m_opaque_sp->GetTarget().GetAPIMutex());
        return m_opaque_sp->IsEnabled();
    }
    else
        return false;
}

uint32_t
SBWatchpointLocation::GetHitCount () const
{
    uint32_t count = 0;
    if (m_opaque_sp)
    {
        Mutex::Locker api_locker (m_opaque_sp->GetTarget().GetAPIMutex());
        count = m_opaque_sp->GetHitCount();
    }

    LogSP log(lldb_private::GetLogIfAllCategoriesSet (LIBLLDB_LOG_API));
    if (log)
        log->Printf ("SBWatchpointLocation(%p)::GetHitCount () => %u", m_opaque_sp.get(), count);

    return count;
}

uint32_t
SBWatchpointLocation::GetIgnoreCount ()
{
    if (m_opaque_sp)
    {
        Mutex::Locker api_locker (m_opaque_sp->GetTarget().GetAPIMutex());
        return m_opaque_sp->GetIgnoreCount();
    }
    else
        return 0;
}

void
SBWatchpointLocation::SetIgnoreCount (uint32_t n)
{
    if (m_opaque_sp)
    {
        Mutex::Locker api_locker (m_opaque_sp->GetTarget().GetAPIMutex());
        m_opaque_sp->SetIgnoreCount (n);
    }
}

bool
SBWatchpointLocation::GetDescription (SBStream &description, DescriptionLevel level)
{
    if (m_opaque_sp)
    {
        Mutex::Locker api_locker (m_opaque_sp->GetTarget().GetAPIMutex());
        description.ref();
        m_opaque_sp->GetDescription (description.get(), level);
        description.get()->EOL();
    }
    else
        description.Printf ("No value");

    return true;
}

lldb_private::WatchpointLocation *
SBWatchpointLocation::operator->() const
{
    return m_opaque_sp.get();
}

lldb_private::WatchpointLocation *
SBWatchpointLocation::get() const
{
    return m_opaque_sp.get();
}

lldb::WatchpointLocationSP &
SBWatchpointLocation::operator *()
{
    return m_opaque_sp;
}

const lldb::WatchpointLocationSP &
SBWatchpointLocation::operator *() const
{
    return m_opaque_sp;
}

