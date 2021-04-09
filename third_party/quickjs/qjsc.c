/*
 * QuickJS command line compiler
 * 
 * Copyright (c) 2018-2021 Fabrice Bellard
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <inttypes.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#if !defined(_WIN32)
#include <sys/wait.h>
#endif

#include "cutils.h"
#include "quickjs-libc.h"

typedef struct {
    char *name;
    char *short_name;
    int flags;
} namelist_entry_t;

typedef struct namelist_t {
    namelist_entry_t *array;
    int count;
    int size;
} namelist_t;

typedef struct {
    const char *option_name;
    const char *init_name;
} FeatureEntry;

static namelist_t cname_list;
static namelist_t cmodule_list;
static namelist_t init_module_list;
static uint64_t feature_bitmap;
static FILE *outfile;
static BOOL byte_swap;
static BOOL dynamic_export;
static const char *c_ident_prefix = "qjsc_";

#define FE_ALL (-1)

static const FeatureEntry feature_list[] = {
    { "date", "Date" },
    { "eval", "Eval" },
    { "string-normalize", "StringNormalize" },
    { "regexp", "RegExp" },
    { "json", "JSON" },
    { "proxy", "Proxy" },
    { "map", "MapSet" },
    { "typedarray", "TypedArrays" },
    { "promise", "Promise" },
#define FE_MODULE_LOADER 9
    { "module-loader", NULL },
#ifdef CONFIG_BIGNUM
    { "bigint", "BigInt" },
#endif
};

void namelist_add(namelist_t *lp, const char *name, const char *short_name,
                  int flags)
{
    namelist_entry_t *e;
    if (lp->count == lp->size) {
        size_t newsize = lp->size + (lp->size >> 1) + 4;
        namelist_entry_t *a =
            realloc(lp->array, sizeof(lp->array[0]) * newsize);
        /* XXX: check for realloc failure */
        lp->array = a;
        lp->size = newsize;
    }
    e =  &lp->array[lp->count++];
    e->name = strdup(name);
    if (short_name)
        e->short_name = strdup(short_name);
    else
        e->short_name = NULL;
    e->flags = flags;
}

void namelist_free(namelist_t *lp)
{
    while (lp->count > 0) {
        namelist_entry_t *e = &lp->array[--lp->count];
        free(e->name);
        free(e->short_name);
    }
    free(lp->array);
    lp->array = NULL;
    lp->size = 0;
}

namelist_entry_t *namelist_find(namelist_t *lp, const char *name)
{
    int i;
    for(i = 0; i < lp->count; i++) {
        namelist_entry_t *e = &lp->array[i];
        if (!strcmp(e->name, name))
            return e;
    }
    return NULL;
}

static void get_c_name(char *buf, size_t buf_size, const char *file)
{
    const char *p, *r;
    size_t len, i;
    int c;
    char *q;
    
    p = strrchr(file, '/');
    if (!p)
        p = file;
    else
        p++;
    r = strrchr(p, '.');
    if (!r)
        len = strlen(p);
    else
        len = r - p;
    pstrcpy(buf, buf_size, c_ident_prefix);
    q = buf + strlen(buf);
    for(i = 0; i < len; i++) {
        c = p[i];
        if (!((c >= '0' && c <= '9') ||
              (c >= 'A' && c <= 'Z') ||
              (c >= 'a' && c <= 'z'))) {
            c = '_';
        }
        if ((q - buf) < buf_size - 1)
            *q++ = c;
    }
    *q = '\0';
}

static void dump_hex(FILE *f, const uint8_t *buf, size_t len)
{
    size_t i, col;
    col = 0;
    for(i = 0; i < len; i++) {
        fprintf(f, " 0x%02x,", buf[i]);
        if (++col == 8) {
            fprintf(f, "\n");
            col = 0;
        }
    }
    if (col != 0)
        fprintf(f, "\n");
}

static void output_object_code(JSContext *ctx,
                               FILE *fo, JSValueConst obj, const char *c_name,
                               BOOL load_only)
{
    uint8_t *out_buf;
    size_t out_buf_len;
    int flags;
    flags = JS_WRITE_OBJ_BYTECODE;
    if (byte_swap)
        flags |= JS_WRITE_OBJ_BSWAP;
    out_buf = JS_WriteObject(ctx, &out_buf_len, obj, flags);
    if (!out_buf) {
        js_std_dump_error(ctx);
        exit(1);
    }

    namelist_add(&cname_list, c_name, NULL, load_only);
    
    fprintf(fo, "const uint32_t %s_size = %u;\n\n", 
            c_name, (unsigned int)out_buf_len);
    fprintf(fo, "const uint8_t %s[%u] = {\n",
            c_name, (unsigned int)out_buf_len);
    dump_hex(fo, out_buf, out_buf_len);
    fprintf(fo, "};\n\n");

    js_free(ctx, out_buf);
}

static int js_module_dummy_init(JSContext *ctx, JSModuleDef *m)
{
    /* should never be called when compiling JS code */
    abort();
}

static void find_unique_cname(char *cname, size_t cname_size)
{
    char cname1[1024];
    int suffix_num;
    size_t len, max_len;
    assert(cname_size >= 32);
    /* find a C name not matching an existing module C name by
       adding a numeric suffix */
    len = strlen(cname);
    max_len = cname_size - 16;
    if (len > max_len)
        cname[max_len] = '\0';
    suffix_num = 1;
    for(;;) {
        snprintf(cname1, sizeof(cname1), "%s_%d", cname, suffix_num);
        if (!namelist_find(&cname_list, cname1))
            break;
        suffix_num++;
    }
    pstrcpy(cname, cname_size, cname1);
}

JSModuleDef *jsc_module_loader(JSContext *ctx,
                              const char *module_name, void *opaque)
{
    JSModuleDef *m;
    namelist_entry_t *e;

    /* check if it is a declared C or system module */
    e = namelist_find(&cmodule_list, module_name);
    if (e) {
        /* add in the static init module list */
        namelist_add(&init_module_list, e->name, e->short_name, 0);
        /* create a dummy module */
        m = JS_NewCModule(ctx, module_name, js_module_dummy_init);
    } else if (has_suffix(module_name, ".so")) {
        fprintf(stderr, "Warning: binary module '%s' will be dynamically loaded\n", module_name);
        /* create a dummy module */
        m = JS_NewCModule(ctx, module_name, js_module_dummy_init);
        /* the resulting executable will export its symbols for the
           dynamic library */
        dynamic_export = TRUE;
    } else {
        size_t buf_len;
        uint8_t *buf;
        JSValue func_val;
        char cname[1024];
        
        buf = js_load_file(ctx, &buf_len, module_name);
        if (!buf) {
            JS_ThrowReferenceError(ctx, "could not load module filename '%s'",
                                   module_name);
            return NULL;
        }
        
        /* compile the module */
        func_val = JS_Eval(ctx, (char *)buf, buf_len, module_name,
                           JS_EVAL_TYPE_MODULE | JS_EVAL_FLAG_COMPILE_ONLY);
        js_free(ctx, buf);
        if (JS_IsException(func_val))
            return NULL;
        get_c_name(cname, sizeof(cname), module_name);
        if (namelist_find(&cname_list, cname)) {
            find_unique_cname(cname, sizeof(cname));
        }
        output_object_code(ctx, outfile, func_val, cname, TRUE);
        
        /* the module is already referenced, so we must free it */
        m = JS_VALUE_GET_PTR(func_val);
        JS_FreeValue(ctx, func_val);
    }
    return m;
}

static void compile_file(JSContext *ctx, FILE *fo,
                         const char *filename,
                         const char *c_name1,
                         int module)
{
    uint8_t *buf;
    char c_name[1024];
    int eval_flags;
    JSValue obj;
    size_t buf_len;
    
    buf = js_load_file(ctx, &buf_len, filename);
    if (!buf) {
        fprintf(stderr, "Could not load '%s'\n", filename);
        exit(1);
    }
    eval_flags = JS_EVAL_FLAG_COMPILE_ONLY;
    if (module < 0) {
        module = (has_suffix(filename, ".mjs") ||
                  JS_DetectModule((const char *)buf, buf_len));
    }
    if (module)
        eval_flags |= JS_EVAL_TYPE_MODULE;
    else
        eval_flags |= JS_EVAL_TYPE_GLOBAL;
    obj = JS_Eval(ctx, (const char *)buf, buf_len, filename, eval_flags);
    if (JS_IsException(obj)) {
        js_std_dump_error(ctx);
        exit(1);
    }
    js_free(ctx, buf);
    if (c_name1) {
        pstrcpy(c_name, sizeof(c_name), c_name1);
    } else {
        get_c_name(c_name, sizeof(c_name), filename);
    }
    output_object_code(ctx, fo, obj, c_name, FALSE);
    JS_FreeValue(ctx, obj);
}

static const char main_c_template1[] =
    "int main(int argc, char **argv)\n"
    "{\n"
    "  JSRuntime *rt;\n"
    "  JSContext *ctx;\n"
    "  rt = JS_NewRuntime();\n"
    "  js_std_set_worker_new_context_func(JS_NewCustomContext);\n"
    "  js_std_init_handlers(rt);\n"
    ;

static const char main_c_template2[] =
    "  js_std_loop(ctx);\n"
    "  JS_FreeContext(ctx);\n"
    "  JS_FreeRuntime(rt);\n"
    "  return 0;\n"
    "}\n";

#define PROG_NAME "qjsc"

void help(void)
{
    printf("QuickJS Compiler version " CONFIG_VERSION "\n"
           "usage: " PROG_NAME " [options] [files]\n"
           "\n"
           "options are:\n"
           "-c          only output bytecode in a C file\n"
           "-e          output main() and bytecode in a C file (default = executable output)\n"
           "-o output   set the output filename\n"
           "-N cname    set the C name of the generated data\n"
           "-m          compile as Javascript module (default=autodetect)\n"
           "-D module_name         compile a dynamically loaded module or worker\n"
           "-M module_name[,cname] add initialization code for an external C module\n"
           "-x          byte swapped output\n"
           "-p prefix   set the prefix of the generated C names\n"
           "-S n        set the maximum stack size to 'n' bytes (default=%d)\n",
           JS_DEFAULT_STACK_SIZE);
#ifdef CONFIG_LTO
    {
        int i;
        printf("-flto       use link time optimization\n");
        printf("-fbignum    enable bignum extensions\n");
        printf("-fno-[");
        for(i = 0; i < countof(feature_list); i++) {
            if (i != 0)
                printf("|");
            printf("%s", feature_list[i].option_name);
        }
        printf("]\n"
               "            disable selected language features (smaller code size)\n");
    }
#endif
    exit(1);
}

#if defined(CONFIG_CC) && !defined(_WIN32)

int exec_cmd(char **argv)
{
    int pid, status, ret;

    pid = fork();
    if (pid == 0) {
        execvp(argv[0], argv);
        exit(1);
    } 

    for(;;) {
        ret = waitpid(pid, &status, 0);
        if (ret == pid && WIFEXITED(status))
            break;
    }
    return WEXITSTATUS(status);
}

static int output_executable(const char *out_filename, const char *cfilename,
                             BOOL use_lto, BOOL verbose, const char *exename)
{
    const char *argv[64];
    const char **arg, *bn_suffix, *lto_suffix;
    char libjsname[1024];
    char exe_dir[1024], inc_dir[1024], lib_dir[1024], buf[1024], *p;
    int ret;
    
    /* get the directory of the executable */
    pstrcpy(exe_dir, sizeof(exe_dir), exename);
    p = strrchr(exe_dir, '/');
    if (p) {
        *p = '\0';
    } else {
        pstrcpy(exe_dir, sizeof(exe_dir), ".");
    }

    /* if 'quickjs.h' is present at the same path as the executable, we
       use it as include and lib directory */
    snprintf(buf, sizeof(buf), "%s/quickjs.h", exe_dir);
    if (access(buf, R_OK) == 0) {
        pstrcpy(inc_dir, sizeof(inc_dir), exe_dir);
        pstrcpy(lib_dir, sizeof(lib_dir), exe_dir);
    } else {
        snprintf(inc_dir, sizeof(inc_dir), "%s/include/quickjs", CONFIG_PREFIX);
        snprintf(lib_dir, sizeof(lib_dir), "%s/lib/quickjs", CONFIG_PREFIX);
    }
    
    lto_suffix = "";
    bn_suffix = "";
    
    arg = argv;
    *arg++ = CONFIG_CC;
    *arg++ = "-O2";
#ifdef CONFIG_LTO
    if (use_lto) {
        *arg++ = "-flto";
        lto_suffix = ".lto";
    }
#endif
    /* XXX: use the executable path to find the includes files and
       libraries */
    *arg++ = "-D";
    *arg++ = "_GNU_SOURCE";
    *arg++ = "-I";
    *arg++ = inc_dir;
    *arg++ = "-o";
    *arg++ = out_filename;
    if (dynamic_export)
        *arg++ = "-rdynamic";
    *arg++ = cfilename;
    snprintf(libjsname, sizeof(libjsname), "%s/libquickjs%s%s.a",
             lib_dir, bn_suffix, lto_suffix);
    *arg++ = libjsname;
    *arg++ = "-lm";
    *arg++ = "-ldl";
    *arg++ = "-lpthread";
    *arg = NULL;
    
    if (verbose) {
        for(arg = argv; *arg != NULL; arg++)
            printf("%s ", *arg);
        printf("\n");
    }
    
    ret = exec_cmd((char **)argv);
    unlink(cfilename);
    return ret;
}
#else
static int output_executable(const char *out_filename, const char *cfilename,
                             BOOL use_lto, BOOL verbose, const char *exename)
{
    fprintf(stderr, "Executable output is not supported for this target\n");
    exit(1);
    return 0;
}
#endif


typedef enum {
    OUTPUT_C,
    OUTPUT_C_MAIN,
    OUTPUT_EXECUTABLE,
} OutputTypeEnum;

int main(int argc, char **argv)
{
    int c, i, verbose;
    const char *out_filename, *cname;
    char cfilename[1024];
    FILE *fo;
    JSRuntime *rt;
    JSContext *ctx;
    BOOL use_lto;
    int module;
    OutputTypeEnum output_type;
    size_t stack_size;
#ifdef CONFIG_BIGNUM
    BOOL bignum_ext = FALSE;
#endif
    namelist_t dynamic_module_list;
    
    out_filename = NULL;
    output_type = OUTPUT_EXECUTABLE;
    cname = NULL;
    feature_bitmap = FE_ALL;
    module = -1;
    byte_swap = FALSE;
    verbose = 0;
    use_lto = FALSE;
    stack_size = 0;
    memset(&dynamic_module_list, 0, sizeof(dynamic_module_list));
    
    /* add system modules */
    namelist_add(&cmodule_list, "std", "std", 0);
    namelist_add(&cmodule_list, "os", "os", 0);

    for(;;) {
        c = getopt(argc, argv, "ho:cN:f:mxevM:p:S:D:");
        if (c == -1)
            break;
        switch(c) {
        case 'h':
            help();
        case 'o':
            out_filename = optarg;
            break;
        case 'c':
            output_type = OUTPUT_C;
            break;
        case 'e':
            output_type = OUTPUT_C_MAIN;
            break;
        case 'N':
            cname = optarg;
            break;
        case 'f':
            {
                const char *p;
                p = optarg;
                if (!strcmp(optarg, "lto")) {
                    use_lto = TRUE;
                } else if (strstart(p, "no-", &p)) {
                    use_lto = TRUE;
                    for(i = 0; i < countof(feature_list); i++) {
                        if (!strcmp(p, feature_list[i].option_name)) {
                            feature_bitmap &= ~((uint64_t)1 << i);
                            break;
                        }
                    }
                    if (i == countof(feature_list))
                        goto bad_feature;
                } else
#ifdef CONFIG_BIGNUM
                if (!strcmp(optarg, "bignum")) {
                    bignum_ext = TRUE;
                } else
#endif
                {
                bad_feature:
                    fprintf(stderr, "unsupported feature: %s\n", optarg);
                    exit(1);
                }
            }
            break;
        case 'm':
            module = 1;
            break;
        case 'M':
            {
                char *p;
                char path[1024];
                char cname[1024];
                pstrcpy(path, sizeof(path), optarg);
                p = strchr(path, ',');
                if (p) {
                    *p = '\0';
                    pstrcpy(cname, sizeof(cname), p + 1);
                } else {
                    get_c_name(cname, sizeof(cname), path);
                }
                namelist_add(&cmodule_list, path, cname, 0);
            }
            break;
        case 'D':
            namelist_add(&dynamic_module_list, optarg, NULL, 0);
            break;
        case 'x':
            byte_swap = TRUE;
            break;
        case 'v':
            verbose++;
            break;
        case 'p':
            c_ident_prefix = optarg;
            break;
        case 'S':
            stack_size = (size_t)strtod(optarg, NULL);
            break;
        default:
            break;
        }
    }

    if (optind >= argc)
        help();

    if (!out_filename) {
        if (output_type == OUTPUT_EXECUTABLE) {
            out_filename = "a.out";
        } else {
            out_filename = "out.c";
        }
    }

    if (output_type == OUTPUT_EXECUTABLE) {
#if defined(_WIN32) || defined(__ANDROID__)
        /* XXX: find a /tmp directory ? */
        snprintf(cfilename, sizeof(cfilename), "out%d.c", getpid());
#else
        snprintf(cfilename, sizeof(cfilename), "/tmp/out%d.c", getpid());
#endif
    } else {
        pstrcpy(cfilename, sizeof(cfilename), out_filename);
    }
    
    fo = fopen(cfilename, "w");
    if (!fo) {
        perror(cfilename);
        exit(1);
    }
    outfile = fo;
    
    rt = JS_NewRuntime();
    ctx = JS_NewContext(rt);
#ifdef CONFIG_BIGNUM
    if (bignum_ext) {
        JS_AddIntrinsicBigFloat(ctx);
        JS_AddIntrinsicBigDecimal(ctx);
        JS_AddIntrinsicOperators(ctx);
        JS_EnableBignumExt(ctx, TRUE);
    }
#endif
    
    /* loader for ES6 modules */
    JS_SetModuleLoaderFunc(rt, NULL, jsc_module_loader, NULL);

    fprintf(fo, "/* File generated automatically by the QuickJS compiler. */\n"
            "\n"
            );
    
    if (output_type != OUTPUT_C) {
        fprintf(fo, "#include \"quickjs-libc.h\"\n"
                "\n"
                );
    } else {
        fprintf(fo, "#include <inttypes.h>\n"
                "\n"
                );
    }

    for(i = optind; i < argc; i++) {
        const char *filename = argv[i];
        compile_file(ctx, fo, filename, cname, module);
        cname = NULL;
    }

    for(i = 0; i < dynamic_module_list.count; i++) {
        if (!jsc_module_loader(ctx, dynamic_module_list.array[i].name, NULL)) {
            fprintf(stderr, "Could not load dynamic module '%s'\n",
                    dynamic_module_list.array[i].name);
            exit(1);
        }
    }
    
    if (output_type != OUTPUT_C) {
        fprintf(fo,
                "static JSContext *JS_NewCustomContext(JSRuntime *rt)\n"
                "{\n"
                "  JSContext *ctx = JS_NewContextRaw(rt);\n"
                "  if (!ctx)\n"
                "    return NULL;\n");
        /* add the basic objects */
        fprintf(fo, "  JS_AddIntrinsicBaseObjects(ctx);\n");
        for(i = 0; i < countof(feature_list); i++) {
            if ((feature_bitmap & ((uint64_t)1 << i)) &&
                feature_list[i].init_name) {
                fprintf(fo, "  JS_AddIntrinsic%s(ctx);\n",
                        feature_list[i].init_name);
            }
        }
#ifdef CONFIG_BIGNUM
        if (bignum_ext) {
            fprintf(fo,
                    "  JS_AddIntrinsicBigFloat(ctx);\n"
                    "  JS_AddIntrinsicBigDecimal(ctx);\n"
                    "  JS_AddIntrinsicOperators(ctx);\n"
                    "  JS_EnableBignumExt(ctx, 1);\n");
        }
#endif
        /* add the precompiled modules (XXX: could modify the module
           loader instead) */
        for(i = 0; i < init_module_list.count; i++) {
            namelist_entry_t *e = &init_module_list.array[i];
            /* initialize the static C modules */
            
            fprintf(fo,
                    "  {\n"
                    "    extern JSModuleDef *js_init_module_%s(JSContext *ctx, const char *name);\n"
                    "    js_init_module_%s(ctx, \"%s\");\n"
                    "  }\n",
                    e->short_name, e->short_name, e->name);
        }
        for(i = 0; i < cname_list.count; i++) {
            namelist_entry_t *e = &cname_list.array[i];
            if (e->flags) {
                fprintf(fo, "  js_std_eval_binary(ctx, %s, %s_size, 1);\n",
                        e->name, e->name);
            }
        }
        fprintf(fo,
                "  return ctx;\n"
                "}\n\n");
        
        fputs(main_c_template1, fo);

        if (stack_size != 0) {
            fprintf(fo, "  JS_SetMaxStackSize(rt, %u);\n",
                    (unsigned int)stack_size);
        }
        
        /* add the module loader if necessary */
        if (feature_bitmap & (1 << FE_MODULE_LOADER)) {
            fprintf(fo, "  JS_SetModuleLoaderFunc(rt, NULL, js_module_loader, NULL);\n");
        }
        
        fprintf(fo,
                "  ctx = JS_NewCustomContext(rt);\n"
                "  js_std_add_helpers(ctx, argc, argv);\n");

        for(i = 0; i < cname_list.count; i++) {
            namelist_entry_t *e = &cname_list.array[i];
            if (!e->flags) {
                fprintf(fo, "  js_std_eval_binary(ctx, %s, %s_size, 0);\n",
                        e->name, e->name);
            }
        }
        fputs(main_c_template2, fo);
    }
    
    JS_FreeContext(ctx);
    JS_FreeRuntime(rt);

    fclose(fo);

    if (output_type == OUTPUT_EXECUTABLE) {
        return output_executable(out_filename, cfilename, use_lto, verbose,
                                 argv[0]);
    }
    namelist_free(&cname_list);
    namelist_free(&cmodule_list);
    namelist_free(&init_module_list);
    return 0;
}
