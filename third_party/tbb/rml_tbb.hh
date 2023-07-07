// clang-format off
/*
    Copyright (c) 2005-2021 Intel Corporation

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

// Header guard and namespace names follow TBB conventions.

#ifndef __TBB_rml_tbb_H
#define __TBB_rml_tbb_H

#include "third_party/tbb/version.hh"
#include "third_party/tbb/rml_base.hh"

namespace tbb {
namespace detail {
namespace r1 {
namespace rml {

//------------------------------------------------------------------------
// Classes instantiated by the server
//------------------------------------------------------------------------

//! Represents a set of oneTBB worker threads provided by the server.
class tbb_server: public ::rml::server {
public:
    //! Inform server of adjustments in the number of workers that the client can profitably use.
    virtual void adjust_job_count_estimate( int delta ) = 0;

#if _WIN32 || _WIN64
    //! Inform server of a oneTBB external thread.
    virtual void register_external_thread( execution_resource_t& v ) = 0;

    //! Inform server that the oneTBB external thread is done with its work.
    virtual void unregister_external_thread( execution_resource_t v ) = 0;
#endif /* _WIN32||_WIN64 */
};

//------------------------------------------------------------------------
// Classes instantiated by the client
//------------------------------------------------------------------------

class tbb_client: public ::rml::client {
public:
    //! Defined by TBB to steal a task and execute it.  
    /** Called by server when it wants an execution context to do some TBB work.
        The method should return when it is okay for the thread to yield indefinitely. */
    virtual void process( job& ) RML_PURE(void)
};

/** Client must ensure that instance is zero-inited, typically by being a file-scope object. */
class tbb_factory: public ::rml::factory {

    //! Pointer to routine that creates an RML server.
    status_type (*my_make_server_routine)( tbb_factory&, tbb_server*&, tbb_client& );

    //! Pointer to routine that calls callback function with server version info.
    void (*my_call_with_server_info_routine)( ::rml::server_info_callback_t cb, void* arg );

public:
    typedef ::rml::versioned_object::version_type version_type;
    typedef tbb_client client_type;
    typedef tbb_server server_type;

    //! Open factory.
    /** Dynamically links against RML library. 
        Returns st_success, st_incompatible, or st_not_found. */
    status_type open();

    //! Factory method to be called by client to create a server object.
    /** Factory must be open. 
        Returns st_success, or st_incompatible . */
    status_type make_server( server_type*&, client_type& );

    //! Close factory
    void close();
};

} // namespace rml
} // namespace r1
} // namespace detail
} // namespace tbb

#endif /*__TBB_rml_tbb_H */
