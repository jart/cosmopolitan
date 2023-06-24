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

#include "third_party/tbb/dynamic_link.h"
#include "third_party/tbb/environment.h"

#include "third_party/tbb/detail/_template_helpers.h"
#include "third_party/tbb/detail/_utils.h"

/*
    This file is used by both TBB and OpenMP RTL. Do not use __TBB_ASSERT() macro
    and runtime_warning() function because they are not available in OpenMP. Use
    __TBB_ASSERT_EX and DYNAMIC_LINK_WARNING instead.
*/

#include "third_party/libcxx/cstdarg"          // va_list etc.
#include "third_party/libcxx/cstring"          // strrchr
#if _WIN32
    #include "libc/mem/mem.h"

    // Unify system calls
    #define dlopen( name, flags )   LoadLibrary( name )
    #define dlsym( handle, name )   GetProcAddress( handle, name )
    #define dlclose( handle )       ( ! FreeLibrary( handle ) )
    #define dlerror()               GetLastError()
#ifndef PATH_MAX
    #define PATH_MAX                MAX_PATH
#endif
#else /* _WIN32 */
    #include "libc/runtime/dlfcn.h"
    #include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/runtime/pathconf.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/sysconf.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/ok.h"
#include "libc/time/time.h"
#include "third_party/getopt/getopt.h"
#include "third_party/musl/crypt.h"
#include "third_party/musl/lockf.h"

    #include "third_party/libcxx/climits"
    #include "third_party/libcxx/cstdlib"
#endif /* _WIN32 */

#if __TBB_WEAK_SYMBOLS_PRESENT && !__TBB_DYNAMIC_LOAD_ENABLED
    //TODO: use function attribute for weak symbols instead of the pragma.
    #pragma weak dlopen
    #pragma weak dlsym
    #pragma weak dlclose
#endif /* __TBB_WEAK_SYMBOLS_PRESENT && !__TBB_DYNAMIC_LOAD_ENABLED */


#define __USE_STATIC_DL_INIT    ( !__ANDROID__ )


/*
dynamic_link is a common interface for searching for required symbols in an
executable and dynamic libraries.

dynamic_link provides certain guarantees:
  1. Either all or none of the requested symbols are resolved. Moreover, if
  symbols are not resolved, the dynamic_link_descriptor table is not modified;
  2. All returned symbols have secured lifetime: this means that none of them
  can be invalidated until dynamic_unlink is called;
  3. Any loaded library is loaded only via the full path. The full path is that
  from which the runtime itself was loaded. (This is done to avoid security
  issues caused by loading libraries from insecure paths).

dynamic_link searches for the requested symbols in three stages, stopping as
soon as all of the symbols have been resolved.

  1. Search the global scope:
    a. On Windows: dynamic_link tries to obtain the handle of the requested
    library and if it succeeds it resolves the symbols via that handle.
    b. On Linux: dynamic_link tries to search for the symbols in the global
    scope via the main program handle. If the symbols are present in the global
    scope their lifetime is not guaranteed (since dynamic_link does not know
    anything about the library from which they are exported). Therefore it
    tries to "pin" the symbols by obtaining the library name and reopening it.
    dlopen may fail to reopen the library in two cases:
       i. The symbols are exported from the executable. Currently dynamic _link
      cannot handle this situation, so it will not find these symbols in this
      step.
      ii. The necessary library has been unloaded and cannot be reloaded. It
      seems there is nothing that can be done in this case. No symbols are
      returned.

  2. Dynamic load: an attempt is made to load the requested library via the
  full path.
    The full path used is that from which the runtime itself was loaded. If the
    library can be loaded, then an attempt is made to resolve the requested
    symbols in the newly loaded library.
    If the symbols are not found the library is unloaded.

  3. Weak symbols: if weak symbols are available they are returned.
*/

namespace tbb {
namespace detail {
namespace r1 {

#if __TBB_WEAK_SYMBOLS_PRESENT || __TBB_DYNAMIC_LOAD_ENABLED

#if !defined(DYNAMIC_LINK_WARNING) && !__TBB_WIN8UI_SUPPORT && __TBB_DYNAMIC_LOAD_ENABLED
    // Report runtime errors and continue.
    #define DYNAMIC_LINK_WARNING dynamic_link_warning
    static void dynamic_link_warning( dynamic_link_error_t code, ... ) {
        suppress_unused_warning(code);
    } // library_warning
#endif /* !defined(DYNAMIC_LINK_WARNING) && !__TBB_WIN8UI_SUPPORT && __TBB_DYNAMIC_LOAD_ENABLED */

    static bool resolve_symbols( dynamic_link_handle module, const dynamic_link_descriptor descriptors[], std::size_t required )
    {
        if ( !module )
            return false;

        #if !__TBB_DYNAMIC_LOAD_ENABLED /* only __TBB_WEAK_SYMBOLS_PRESENT is defined */
            if ( !dlsym ) return false;
        #endif /* !__TBB_DYNAMIC_LOAD_ENABLED */

        const std::size_t n_desc=20; // Usually we don't have more than 20 descriptors per library
        __TBB_ASSERT_EX( required <= n_desc, "Too many descriptors is required" );
        if ( required > n_desc ) return false;
        pointer_to_handler h[n_desc];

        for ( std::size_t k = 0; k < required; ++k ) {
            dynamic_link_descriptor const & desc = descriptors[k];
            pointer_to_handler addr = (pointer_to_handler)dlsym( module, desc.name );
            if ( !addr ) {
                return false;
            }
            h[k] = addr;
        }

        // Commit the entry points.
        // Cannot use memset here, because the writes must be atomic.
        for( std::size_t k = 0; k < required; ++k )
            *descriptors[k].handler = h[k];
        return true;
    }

#if __TBB_WIN8UI_SUPPORT
    bool dynamic_link( const char*  library, const dynamic_link_descriptor descriptors[], std::size_t required, dynamic_link_handle*, int flags ) {
        dynamic_link_handle tmp_handle = nullptr;
        TCHAR wlibrary[256];
        if ( MultiByteToWideChar(CP_UTF8, 0, library, -1, wlibrary, 255) == 0 ) return false;
        if ( flags & DYNAMIC_LINK_LOAD )
            tmp_handle = LoadPackagedLibrary( wlibrary, 0 );
        if (tmp_handle != nullptr){
            return resolve_symbols(tmp_handle, descriptors, required);
        }else{
            return false;
        }
    }
    void dynamic_unlink( dynamic_link_handle ) {}
    void dynamic_unlink_all() {}
#else
#if __TBB_DYNAMIC_LOAD_ENABLED
/*
    There is a security issue on Windows: LoadLibrary() may load and execute malicious code.
    See http://www.microsoft.com/technet/security/advisory/2269637.mspx for details.
    To avoid the issue, we have to pass full path (not just library name) to LoadLibrary. This
    function constructs full path to the specified library (it is assumed the library located
    side-by-side with the tbb.dll.

    The function constructs absolute path for given relative path. Important: Base directory is not
    current one, it is the directory tbb.dll loaded from.

    Example:
        Let us assume "tbb.dll" is located in "c:\program files\common\intel\" directory, e.g.
        absolute path of the library is "c:\program files\common\intel\tbb.dll". Absolute path for
        "tbbmalloc.dll" would be "c:\program files\common\intel\tbbmalloc.dll". Absolute path for
        "malloc\tbbmalloc.dll" would be "c:\program files\common\intel\malloc\tbbmalloc.dll".
*/

    // Struct handle_storage is used by dynamic_link routine to store handles of
    // all loaded or pinned dynamic libraries. When TBB is shut down, it calls
    // dynamic_unlink_all() that unloads modules referenced by handle_storage.
    // This struct should not have any constructors since it may be used before
    // the constructor is called.
    #define MAX_LOADED_MODULES 8 // The number of maximum possible modules which can be loaded

    using atomic_incrementer = std::atomic<std::size_t>;

    static struct handles_t {
        atomic_incrementer my_size;
        dynamic_link_handle my_handles[MAX_LOADED_MODULES];

        void add(const dynamic_link_handle &handle) {
            const std::size_t ind = my_size++;
            __TBB_ASSERT_EX( ind < MAX_LOADED_MODULES, "Too many modules are loaded" );
            my_handles[ind] = handle;
        }

        void free() {
            const std::size_t size = my_size;
            for (std::size_t i=0; i<size; ++i)
                dynamic_unlink( my_handles[i] );
        }
    } handles;

    static std::once_flag init_dl_data_state;

    static struct ap_data_t {
        char _path[PATH_MAX+1];
        std::size_t _len;
    } ap_data;

    static void init_ap_data() {
    #if _WIN32
        // Get handle of our DLL first.
        HMODULE handle;
        BOOL brc = GetModuleHandleEx(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            (LPCSTR)( & dynamic_link ), // any function inside the library can be used for the address
            & handle
            );
        if ( !brc ) { // Error occurred.
            int err = GetLastError();
            DYNAMIC_LINK_WARNING( dl_sys_fail, "GetModuleHandleEx", err );
            return;
        }
        // Now get path to our DLL.
        DWORD drc = GetModuleFileName( handle, ap_data._path, static_cast< DWORD >( PATH_MAX ) );
        if ( drc == 0 ) { // Error occurred.
            int err = GetLastError();
            DYNAMIC_LINK_WARNING( dl_sys_fail, "GetModuleFileName", err );
            return;
        }
        if ( drc >= PATH_MAX ) { // Buffer too short.
            DYNAMIC_LINK_WARNING( dl_buff_too_small );
            return;
        }
        // Find the position of the last backslash.
        char *backslash = std::strrchr( ap_data._path, '\\' );

        if ( !backslash ) {    // Backslash not found.
            __TBB_ASSERT_EX( backslash != nullptr, "Unbelievable.");
            return;
        }
        __TBB_ASSERT_EX( backslash >= ap_data._path, "Unbelievable.");
        ap_data._len = (std::size_t)(backslash - ap_data._path) + 1;
        *(backslash+1) = 0;
    #else
        // Get the library path
        Dl_info dlinfo;
        int res = dladdr( (void*)&dynamic_link, &dlinfo ); // any function inside the library can be used for the address
        if ( !res ) {
            char const * err = dlerror();
            DYNAMIC_LINK_WARNING( dl_sys_fail, "dladdr", err );
            return;
        } else {
            __TBB_ASSERT_EX( dlinfo.dli_fname!=nullptr, "Unbelievable." );
        }

        char const *slash = std::strrchr( dlinfo.dli_fname, '/' );
        std::size_t fname_len=0;
        if ( slash ) {
            __TBB_ASSERT_EX( slash >= dlinfo.dli_fname, "Unbelievable.");
            fname_len = (std::size_t)(slash - dlinfo.dli_fname) + 1;
        }

        std::size_t rc;
        if ( dlinfo.dli_fname[0]=='/' ) {
            // The library path is absolute
            rc = 0;
            ap_data._len = 0;
        } else {
            // The library path is relative so get the current working directory
            if ( !getcwd( ap_data._path, sizeof(ap_data._path)/sizeof(ap_data._path[0]) ) ) {
                DYNAMIC_LINK_WARNING( dl_buff_too_small );
                return;
            }
            ap_data._len = std::strlen( ap_data._path );
            ap_data._path[ap_data._len++]='/';
            rc = ap_data._len;
        }

        if ( fname_len>0 ) {
            ap_data._len += fname_len;
            if ( ap_data._len>PATH_MAX ) {
                DYNAMIC_LINK_WARNING( dl_buff_too_small );
                ap_data._len=0;
                return;
            }
            std::strncpy( ap_data._path+rc, dlinfo.dli_fname, fname_len );
            ap_data._path[ap_data._len]=0;
        }
    #endif /* _WIN32 */
    }

    static void init_dl_data() {
        init_ap_data();
    }

    /*
        The function constructs absolute path for given relative path. Important: Base directory is not
        current one, it is the directory libtbb.so loaded from.

        Arguments:
        in  name -- Name of a file (may be with relative path; it must not be an absolute one).
        out path -- Buffer to save result (absolute path) to.
        in  len  -- Size of buffer.
        ret      -- 0         -- Error occurred.
                    > len     -- Buffer too short, required size returned.
                    otherwise -- Ok, number of characters (incl. terminating null) written to buffer.
    */
    static std::size_t abs_path( char const * name, char * path, std::size_t len ) {
        if ( ap_data._len == 0 )
            return 0;

        std::size_t name_len = std::strlen( name );
        std::size_t full_len = name_len+ap_data._len;
        if ( full_len < len ) {
            __TBB_ASSERT( ap_data._path[ap_data._len] == 0, nullptr);
            __TBB_ASSERT( std::strlen(ap_data._path) == ap_data._len, nullptr);
            std::strncpy( path, ap_data._path, ap_data._len + 1 );
            __TBB_ASSERT( path[ap_data._len] == 0, nullptr);
            std::strncat( path, name, len - ap_data._len );
            __TBB_ASSERT( std::strlen(path) == full_len, nullptr);
        }
        return full_len+1; // +1 for null character
    }
#endif  // __TBB_DYNAMIC_LOAD_ENABLED
    void init_dynamic_link_data() {
    #if __TBB_DYNAMIC_LOAD_ENABLED
        std::call_once( init_dl_data_state, init_dl_data );
    #endif
    }

    #if __USE_STATIC_DL_INIT
    // ap_data structure is initialized with current directory on Linux.
    // So it should be initialized as soon as possible since the current directory may be changed.
    // static_init_ap_data object provides this initialization during library loading.
    static struct static_init_dl_data_t {
        static_init_dl_data_t() {
            init_dynamic_link_data();
        }
    } static_init_dl_data;
    #endif

    #if __TBB_WEAK_SYMBOLS_PRESENT
    static bool weak_symbol_link( const dynamic_link_descriptor descriptors[], std::size_t required )
    {
        // Check if the required entries are present in what was loaded into our process.
        for ( std::size_t k = 0; k < required; ++k )
            if ( !descriptors[k].ptr )
                return false;
        // Commit the entry points.
        for ( std::size_t k = 0; k < required; ++k )
            *descriptors[k].handler = (pointer_to_handler) descriptors[k].ptr;
        return true;
    }
    #else
    static bool weak_symbol_link( const dynamic_link_descriptor[], std::size_t ) {
        return false;
    }
    #endif /* __TBB_WEAK_SYMBOLS_PRESENT */

    void dynamic_unlink( dynamic_link_handle handle ) {
    #if !__TBB_DYNAMIC_LOAD_ENABLED /* only __TBB_WEAK_SYMBOLS_PRESENT is defined */
        if ( !dlclose ) return;
    #endif
        if ( handle ) {
            dlclose( handle );
        }
    }

    void dynamic_unlink_all() {
    #if __TBB_DYNAMIC_LOAD_ENABLED
        handles.free();
    #endif
    }

    static dynamic_link_handle global_symbols_link( const char* library, const dynamic_link_descriptor descriptors[], std::size_t required ) {
        dynamic_link_handle library_handle{};
#if _WIN32
        auto res = GetModuleHandleEx(0, library, &library_handle);
        __TBB_ASSERT_EX((res && library_handle) || (!res && !library_handle), nullptr);
#else /* _WIN32 */
    #if !__TBB_DYNAMIC_LOAD_ENABLED /* only __TBB_WEAK_SYMBOLS_PRESENT is defined */
        if ( !dlopen ) return 0;
    #endif /* !__TBB_DYNAMIC_LOAD_ENABLED */
        // RTLD_GLOBAL - to guarantee that old TBB will find the loaded library
        // RTLD_NOLOAD - not to load the library without the full path
        library_handle = dlopen(library, RTLD_LAZY | RTLD_GLOBAL | RTLD_NOLOAD);
#endif /* _WIN32 */
        if (library_handle) {
            if (!resolve_symbols(library_handle, descriptors, required)) {
                dynamic_unlink(library_handle);
                library_handle = nullptr;
            }
        }
        return library_handle;
    }

    static void save_library_handle( dynamic_link_handle src, dynamic_link_handle *dst ) {
        __TBB_ASSERT_EX( src, "The library handle to store must be non-zero" );
        if ( dst )
            *dst = src;
    #if __TBB_DYNAMIC_LOAD_ENABLED
        else
            handles.add( src );
    #endif /* __TBB_DYNAMIC_LOAD_ENABLED */
    }

#if !_WIN32
    int loading_flags(bool local_binding) {
        int flags = RTLD_NOW;
        if (local_binding) {
            flags = flags | RTLD_LOCAL;
#if (__linux__ && __GLIBC__) && !__TBB_USE_SANITIZERS
            if( !GetBoolEnvironmentVariable("TBB_ENABLE_SANITIZERS") ) {
                flags = flags | RTLD_DEEPBIND;
            }
#endif
        } else {
            flags = flags | RTLD_GLOBAL;
        }
        return flags;
    }
#endif

    dynamic_link_handle dynamic_load( const char* library, const dynamic_link_descriptor descriptors[], std::size_t required, bool local_binding ) {
        ::tbb::detail::suppress_unused_warning( library, descriptors, required, local_binding );
#if __TBB_DYNAMIC_LOAD_ENABLED
        std::size_t const len = PATH_MAX + 1;
        char path[ len ];
        std::size_t rc = abs_path( library, path, len );
        if ( 0 < rc && rc <= len ) {
#if _WIN32
            // Prevent Windows from displaying silly message boxes if it fails to load library
            // (e.g. because of MS runtime problems - one of those crazy manifest related ones)
            UINT prev_mode = SetErrorMode (SEM_FAILCRITICALERRORS);
#endif /* _WIN32 */
            // The second argument (loading_flags) is ignored on Windows
            dynamic_link_handle library_handle = dlopen( path, loading_flags(local_binding) );
#if _WIN32
            SetErrorMode (prev_mode);
#endif /* _WIN32 */
            if( library_handle ) {
                if( !resolve_symbols( library_handle, descriptors, required ) ) {
                    // The loaded library does not contain all the expected entry points
                    dynamic_unlink( library_handle );
                    library_handle = nullptr;
                }
            } else
                DYNAMIC_LINK_WARNING( dl_lib_not_found, path, dlerror() );
            return library_handle;
        } else if ( rc>len )
                DYNAMIC_LINK_WARNING( dl_buff_too_small );
                // rc == 0 means failing of init_ap_data so the warning has already been issued.

#endif /* __TBB_DYNAMIC_LOAD_ENABLED */
            return nullptr;
    }

    bool dynamic_link( const char* library, const dynamic_link_descriptor descriptors[], std::size_t required, dynamic_link_handle *handle, int flags ) {
        init_dynamic_link_data();

        // TODO: May global_symbols_link find weak symbols?
        dynamic_link_handle library_handle = ( flags & DYNAMIC_LINK_GLOBAL ) ? global_symbols_link( library, descriptors, required ) : nullptr;

#if defined(_MSC_VER) && _MSC_VER <= 1900
#pragma warning (push)
// MSVC 2015 warning: 'int': forcing value to bool 'true' or 'false'
#pragma warning (disable: 4800)
#endif
        if ( !library_handle && ( flags & DYNAMIC_LINK_LOAD ) )
            library_handle = dynamic_load( library, descriptors, required, flags & DYNAMIC_LINK_LOCAL );

#if defined(_MSC_VER) && _MSC_VER <= 1900
#pragma warning (pop)
#endif
        if ( !library_handle && ( flags & DYNAMIC_LINK_WEAK ) )
            return weak_symbol_link( descriptors, required );

        if ( library_handle ) {
            save_library_handle( library_handle, handle );
            return true;
        }
        return false;
    }

#endif /*__TBB_WIN8UI_SUPPORT*/
#else /* __TBB_WEAK_SYMBOLS_PRESENT || __TBB_DYNAMIC_LOAD_ENABLED */
    bool dynamic_link( const char*, const dynamic_link_descriptor*, std::size_t, dynamic_link_handle *handle, int ) {
        if ( handle )
            *handle=0;
        return false;
    }
    void dynamic_unlink( dynamic_link_handle ) {}
    void dynamic_unlink_all() {}
#endif /* __TBB_WEAK_SYMBOLS_PRESENT || __TBB_DYNAMIC_LOAD_ENABLED */

} // namespace r1
} // namespace detail
} // namespace tbb
