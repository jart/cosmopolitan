// clang-format off
/*
    Copyright (c) 2005-2023 Intel Corporation

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

#include "third_party/tbb/detail/_assert.h"

#include "third_party/tbb/rml_tbb.h"
#include "third_party/tbb/dynamic_link.h"

namespace tbb {
namespace detail {
namespace r1 {
namespace rml {

#define MAKE_SERVER(x) DLD(__TBB_make_rml_server,x)
#define GET_INFO(x) DLD(__TBB_call_with_my_server_info,x)
#define SERVER tbb_server
#define CLIENT tbb_client
#define FACTORY tbb_factory

#if __TBB_WEAK_SYMBOLS_PRESENT
    #pragma weak __TBB_make_rml_server
    #pragma weak __TBB_call_with_my_server_info
    extern "C" {
        ::rml::factory::status_type __TBB_make_rml_server( rml::tbb_factory& f, rml::tbb_server*& server, rml::tbb_client& client );
        void __TBB_call_with_my_server_info( ::rml::server_info_callback_t cb, void* arg );
    }
#endif /* __TBB_WEAK_SYMBOLS_PRESENT */

#if TBB_USE_DEBUG
#define DEBUG_SUFFIX "_debug"
#else
#define DEBUG_SUFFIX
#endif /* TBB_USE_DEBUG */

// RML_SERVER_NAME is the name of the RML server library.
#if _WIN32 || _WIN64
#define RML_SERVER_NAME "irml" DEBUG_SUFFIX ".dll"
#elif __APPLE__
#define RML_SERVER_NAME "libirml" DEBUG_SUFFIX ".1.dylib"
#elif __FreeBSD__ || __NetBSD__ || __OpenBSD__ || __sun || _AIX
#define RML_SERVER_NAME "libirml" DEBUG_SUFFIX ".so"
#elif __unix__
#define RML_SERVER_NAME "libirml" DEBUG_SUFFIX ".so.1"
#else
#error Unknown OS
#endif

const ::rml::versioned_object::version_type CLIENT_VERSION = 2;

#if __TBB_WEAK_SYMBOLS_PRESENT
    #pragma weak __RML_open_factory
    #pragma weak __RML_close_factory
    extern "C" {
        ::rml::factory::status_type __RML_open_factory ( ::rml::factory&, ::rml::versioned_object::version_type&, ::rml::versioned_object::version_type );
        void __RML_close_factory( ::rml::factory& f );
    }
#endif /* __TBB_WEAK_SYMBOLS_PRESENT */

::rml::factory::status_type FACTORY::open() {
    // Failure of following assertion indicates that factory is already open, or not zero-inited.
    __TBB_ASSERT_EX( !library_handle, nullptr);
    status_type (*open_factory_routine)( factory&, version_type&, version_type );
    dynamic_link_descriptor server_link_table[4] = {
        DLD(__RML_open_factory,open_factory_routine),
        MAKE_SERVER(my_make_server_routine),
        DLD(__RML_close_factory,my_wait_to_close_routine),
        GET_INFO(my_call_with_server_info_routine),
    };
    status_type result;
    if ( dynamic_link( RML_SERVER_NAME, server_link_table, 4, &library_handle ) ) {
        version_type server_version;
        result = (*open_factory_routine)( *this, server_version, CLIENT_VERSION );
        // server_version can be checked here for incompatibility if necessary.
    } else {
        library_handle = nullptr;
        result = st_not_found;
    }
    return result;
}

void FACTORY::close() {
    if ( library_handle )
        (*my_wait_to_close_routine)(*this);
    if ( (size_t)library_handle>FACTORY::c_dont_unload ) {
        dynamic_unlink(library_handle);
        library_handle = nullptr;
    }
}

::rml::factory::status_type FACTORY::make_server( SERVER*& s, CLIENT& c) {
    // Failure of following assertion means that factory was not successfully opened.
    __TBB_ASSERT_EX( my_make_server_routine, nullptr);
    return (*my_make_server_routine)(*this,s,c);
}

} // namespace rml
} // namespace r1
} // namespace detail
} // namespace tbb
