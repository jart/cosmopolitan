/*
 * Javascript Micro benchmark
 *
 * Copyright (c) 2017-2019 Fabrice Bellard
 * Copyright (c) 2017-2019 Charlie Gordon
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
import * as std from "std";

function pad(str, n) {
    str += "";
    while (str.length < n)
        str += " ";
    return str;
}

function pad_left(str, n) {
    str += "";
    while (str.length < n)
        str = " " + str;
    return str;
}

function pad_center(str, n) {
    str += "";
    while (str.length < n) {
        if ((n - str.length) & 1)
            str = str + " ";
        else
            str = " " + str;
    }
    return str;
}

function toPrec(n, prec) {
    var i, s;
    for (i = 0; i < prec; i++)
        n *= 10;
    s = "" + Math.round(n);
    for (i = s.length - prec; i <= 0; i++)
        s = "0" + s;
    if (prec > 0)
        s = s.substring(0, i) + "." + s.substring(i);
    return s;
}
                
var ref_data;
var log_data;

var heads  = [ "TEST", "N", "TIME (ns)", "REF (ns)", "SCORE (%)" ];
var widths = [    22,   10,          9,     9,       9 ];
var precs  = [     0,   0,           2,     2,       2 ];
var total  = [     0,   0,           0,     0,       0 ];
var total_score = 0;
var total_scale = 0;

if (typeof console == "undefined") {
    var console = { log: print };
}

function log_line() {
    var i, n, s, a;
    s = "";
    for (i = 0, n = arguments.length; i < n; i++) {
        if (i > 0)
            s += " ";
        a = arguments[i];
        if (typeof a == "number") {
            total[i] += a;
            a = toPrec(a, precs[i]);
            s += pad_left(a, widths[i]);
        } else {
            s += pad_left(a, widths[i]);
        }
    }
    console.log(s);
}

var clocks_per_sec = 1000000;
var max_iterations = 100;
var clock_threshold = 2000;  /* favoring short measuring spans */
var min_n_argument = 1;
var get_clock;

if (typeof globalThis.__date_clock != "function") {
    console.log("using fallback millisecond clock");
    clocks_per_sec = 1000;
    max_iterations = 10;
    clock_threshold = 100;
    get_clock = Date.now;
} else {
    get_clock = globalThis.__date_clock;
}

function log_one(text, n, ti) {
    var ref;

    if (ref_data)
        ref = ref_data[text];
    else
        ref = null;

    ti = Math.round(ti * 100) / 100;
    log_data[text] = ti;
    if (typeof ref === "number") {
        log_line(text, n, ti, ref, ti * 100 / ref);
        total_score += ti * 100 / ref;
        total_scale += 100;
    } else {
        log_line(text, n, ti);
        total_score += 100;
        total_scale += 100;
    }
}

function bench(f, text)
{
    var i, j, n, t, t1, ti, nb_its, ref, ti_n, ti_n1, min_ti;

    nb_its = n = 1;
    if (f.bench) {
        ti_n = f(text);
    } else {
        ti_n = 1000000000;
        min_ti = clock_threshold / 10;
        for(i = 0; i < 30; i++) {
            ti = 1000000000;
            for (j = 0; j < max_iterations; j++) {
                t = get_clock();
                while ((t1 = get_clock()) == t)
                    continue;
                nb_its = f(n);
                if (nb_its < 0)
                    return; // test failure
                t1 = get_clock() - t1;
                if (ti > t1)
                    ti = t1;
            }
            if (ti >= min_ti) {
                ti_n1 = ti / nb_its;
                if (ti_n > ti_n1)
                    ti_n = ti_n1;
            }
            if (ti >= clock_threshold && n >= min_n_argument)
                break;

            n = n * [ 2, 2.5, 2 ][i % 3];
        }
        // to use only the best timing from the last loop, uncomment below
        //ti_n = ti / nb_its;
    }
    /* nano seconds per iteration */
    log_one(text, n, ti_n * 1e9 / clocks_per_sec);
}

var global_res; /* to be sure the code is not optimized */

function empty_loop(n) {
    var j;
    for(j = 0; j < n; j++) {
    }
    return n;
}

function date_now(n) {
    var j;
    for(j = 0; j < n; j++) {
        Date.now();
    }
    return n;
}

function prop_read(n)
{
    var obj, sum, j;
    obj = {a: 1, b: 2, c:3, d:4 };
    sum = 0;
    for(j = 0; j < n; j++) {
        sum += obj.a;
        sum += obj.b;
        sum += obj.c;
        sum += obj.d;
    }
    global_res = sum;
    return n * 4;
}

function prop_write(n)
{
    var obj, j;
    obj = {a: 1, b: 2, c:3, d:4 };
    for(j = 0; j < n; j++) {
        obj.a = j;
        obj.b = j;
        obj.c = j;
        obj.d = j;
    }
    return n * 4;
}

function prop_create(n)
{
    var obj, j;
    for(j = 0; j < n; j++) {
        obj = new Object();
        obj.a = 1;
        obj.b = 2;
        obj.c = 3;
        obj.d = 4;
    }
    return n * 4;
}

function prop_delete(n)
{
    var obj, j;
    obj = {};
    for(j = 0; j < n; j++) {
        obj[j] = 1;
    }
    for(j = 0; j < n; j++) {
        delete obj[j];
    }
    return n;
}

function array_read(n)
{
    var tab, len, sum, i, j;
    tab = [];
    len = 10;
    for(i = 0; i < len; i++)
        tab[i] = i;
    sum = 0;
    for(j = 0; j < n; j++) {
        sum += tab[0];
        sum += tab[1];
        sum += tab[2];
        sum += tab[3];
        sum += tab[4];
        sum += tab[5];
        sum += tab[6];
        sum += tab[7];
        sum += tab[8];
        sum += tab[9];
    }
    global_res = sum;
    return len * n;
}

function array_write(n)
{
    var tab, len, i, j;
    tab = [];
    len = 10;
    for(i = 0; i < len; i++)
        tab[i] = i;
    for(j = 0; j < n; j++) {
        tab[0] = j;
        tab[1] = j;
        tab[2] = j;
        tab[3] = j;
        tab[4] = j;
        tab[5] = j;
        tab[6] = j;
        tab[7] = j;
        tab[8] = j;
        tab[9] = j;
    }
    return len * n;
}

function array_prop_create(n)
{
    var tab, i, j, len;
    len = 1000;
    for(j = 0; j < n; j++) {
        tab = [];
        for(i = 0; i < len; i++)
            tab[i] = i;
    }
    return len * n;
}

function array_length_decr(n)
{
    var tab, i, j, len;
    len = 1000;
    tab = [];
    for(i = 0; i < len; i++)
        tab[i] = i;
    for(j = 0; j < n; j++) {
        for(i = len - 1; i >= 0; i--)
            tab.length = i;
    }
    return len * n;
}

function array_hole_length_decr(n)
{
    var tab, i, j, len;
    len = 1000;
    tab = [];
    for(i = 0; i < len; i++) {
        if (i != 3)
            tab[i] = i;
    }
    for(j = 0; j < n; j++) {
        for(i = len - 1; i >= 0; i--)
            tab.length = i;
    }
    return len * n;
}

function array_push(n)
{
    var tab, i, j, len;
    len = 500;
    for(j = 0; j < n; j++) {
        tab = [];
        for(i = 0; i < len; i++)
            tab.push(i);
    }
    return len * n;
}

function array_pop(n)
{
    var tab, i, j, len, sum;
    len = 500;
    for(j = 0; j < n; j++) {
        tab = [];
        for(i = 0; i < len; i++)
            tab[i] = i;
        sum = 0;
        for(i = 0; i < len; i++)
            sum += tab.pop();
        global_res = sum;
    }
    return len * n;
}

function typed_array_read(n)
{
    var tab, len, sum, i, j;
    len = 10;
    tab = new Int32Array(len);
    for(i = 0; i < len; i++)
        tab[i] = i;
    sum = 0;
    for(j = 0; j < n; j++) {
        sum += tab[0];
        sum += tab[1];
        sum += tab[2];
        sum += tab[3];
        sum += tab[4];
        sum += tab[5];
        sum += tab[6];
        sum += tab[7];
        sum += tab[8];
        sum += tab[9];
    }
    global_res = sum;
    return len * n;
}

function typed_array_write(n)
{
    var tab, len, i, j;
    len = 10;
    tab = new Int32Array(len);
    for(i = 0; i < len; i++)
        tab[i] = i;
    for(j = 0; j < n; j++) {
        tab[0] = j;
        tab[1] = j;
        tab[2] = j;
        tab[3] = j;
        tab[4] = j;
        tab[5] = j;
        tab[6] = j;
        tab[7] = j;
        tab[8] = j;
        tab[9] = j;
    }
    return len * n;
}

var global_var0;

function global_read(n)
{
    var sum, j;
    global_var0 = 0;
    sum = 0;
    for(j = 0; j < n; j++) {
        sum += global_var0;
        sum += global_var0;
        sum += global_var0;
        sum += global_var0;
    }
    global_res = sum;
    return n * 4;
}

var global_write =
    (1, eval)(`(function global_write(n)
           {
               var j;
               for(j = 0; j < n; j++) {
                   global_var0 = j;
                   global_var0 = j;
                   global_var0 = j;
                   global_var0 = j;
               }
               return n * 4;
           })`);

function global_write_strict(n)
{
    var j;
    for(j = 0; j < n; j++) {
        global_var0 = j;
        global_var0 = j;
        global_var0 = j;
        global_var0 = j;
    }
    return n * 4;
}

function local_destruct(n)
{
    var j, v1, v2, v3, v4;
    var array = [ 1, 2, 3, 4, 5];
    var o = { a:1, b:2, c:3, d:4 };
    var a, b, c, d;
    for(j = 0; j < n; j++) {
        [ v1, v2,, v3, ...v4] = array;
        ({ a, b, c, d } = o);
        ({ a: a, b: b, c: c, d: d } = o);
    }
    return n * 12;
}

var global_v1, global_v2, global_v3, global_v4;
var global_a, global_b, global_c, global_d;

var global_destruct =
    (1, eval)(`(function global_destruct(n)
           {
               var j, v1, v2, v3, v4;
               var array = [ 1, 2, 3, 4, 5 ];
               var o = { a:1, b:2, c:3, d:4 };
               var a, b, c, d;
               for(j = 0; j < n; j++) {
                   [ global_v1, global_v2,, global_v3, ...global_v4] = array;
                   ({ a: global_a, b: global_b, c: global_c, d: global_d } = o);
               }
               return n * 8;
          })`);

function global_destruct_strict(n)
{
    var j, v1, v2, v3, v4;
    var array = [ 1, 2, 3, 4, 5 ];
    var o = { a:1, b:2, c:3, d:4 };
    var a, b, c, d;
    for(j = 0; j < n; j++) {
        [ global_v1, global_v2,, global_v3, ...global_v4] = array;
        ({ a: global_a, b: global_b, c: global_c, d: global_d } = o);
    }
    return n * 8;
}

function func_call(n)
{
    function f(a)
    {
        return 1;
    }

    var j, sum;
    sum = 0;
    for(j = 0; j < n; j++) {
        sum += f(j);
        sum += f(j);
        sum += f(j);
        sum += f(j);
    }
    global_res = sum;
    return n * 4;
}

function closure_var(n)
{
    function f(a)
    {
        sum++;
    }

    var j, sum;
    sum = 0;
    for(j = 0; j < n; j++) {
        f(j);
        f(j);
        f(j);
        f(j);
    }
    global_res = sum;
    return n * 4;
}

function int_arith(n)
{
    var i, j, sum;
    global_res = 0;
    for(j = 0; j < n; j++) {
        sum = 0;
        for(i = 0; i < 1000; i++) {
            sum += i * i;
        }
        global_res += sum;
    }
    return n * 1000;
}

function float_arith(n)
{
    var i, j, sum, a, incr, a0;
    global_res = 0;
    a0 = 0.1;
    incr = 1.1;
    for(j = 0; j < n; j++) {
        sum = 0;
        a = a0;
        for(i = 0; i < 1000; i++) {
            sum += a * a;
            a += incr;
        }
        global_res += sum;
    }
    return n * 1000;
}

function bigfloat_arith(n)
{
    var i, j, sum, a, incr, a0;
    global_res = 0;
    a0 = BigFloat("0.1");
    incr = BigFloat("1.1");
    for(j = 0; j < n; j++) {
        sum = 0;
        a = a0;
        for(i = 0; i < 1000; i++) {
            sum += a * a;
            a += incr;
        }
        global_res += sum;
    }
    return n * 1000;
}

function float256_arith(n)
{
    return BigFloatEnv.setPrec(bigfloat_arith.bind(null, n), 237, 19);
}

function bigint_arith(n, bits)
{
    var i, j, sum, a, incr, a0, sum0;
    sum0 = global_res = BigInt(0);
    a0 = BigInt(1) << BigInt(Math.floor((bits - 10) * 0.5));
    incr = BigInt(1);
    for(j = 0; j < n; j++) {
        sum = sum0;
        a = a0;
        for(i = 0; i < 1000; i++) {
            sum += a * a;
            a += incr;
        }
        global_res += sum;
    }
    return n * 1000;
}

function bigint64_arith(n)
{
    return bigint_arith(n, 64);
}

function bigint256_arith(n)
{
    return bigint_arith(n, 256);
}

function set_collection_add(n)
{
    var s, i, j, len = 100;
    s = new Set();
    for(j = 0; j < n; j++) {
        for(i = 0; i < len; i++) {
            s.add(String(i), i);
        }
        for(i = 0; i < len; i++) {
            if (!s.has(String(i)))
                throw Error("bug in Set");
        }
    }
    return n * len;
}

function array_for(n)
{
    var r, i, j, sum;
    r = [];
    for(i = 0; i < 100; i++)
        r[i] = i;
    for(j = 0; j < n; j++) {
        sum = 0;
        for(i = 0; i < 100; i++) {
            sum += r[i];
        }
        global_res = sum;
    }
    return n * 100;
}

function array_for_in(n)
{
    var r, i, j, sum;
    r = [];
    for(i = 0; i < 100; i++)
        r[i] = i;
    for(j = 0; j < n; j++) {
        sum = 0;
        for(i in r) {
            sum += r[i];
        }
        global_res = sum;
    }
    return n * 100;
}

function array_for_of(n)
{
    var r, i, j, sum;
    r = [];
    for(i = 0; i < 100; i++)
        r[i] = i;
    for(j = 0; j < n; j++) {
        sum = 0;
        for(i of r) {
            sum += i;
        }
        global_res = sum;
    }
    return n * 100;
}

function math_min(n)
{
    var i, j, r;
    r = 0;
    for(j = 0; j < n; j++) {
        for(i = 0; i < 1000; i++)
            r = Math.min(i, 500);
        global_res = r;
    }
    return n * 1000;
}

/* incremental string contruction as local var */
function string_build1(n)
{
    var i, j, r;
    r = "";
    for(j = 0; j < n; j++) {
        for(i = 0; i < 100; i++)
            r += "x";
        global_res = r;
    }
    return n * 100;
}

/* incremental string contruction as arg */
function string_build2(n, r)
{
    var i, j;
    r = "";
    for(j = 0; j < n; j++) {
        for(i = 0; i < 100; i++)
            r += "x";
        global_res = r;
    }
    return n * 100;
}

/* incremental string contruction by prepending */
function string_build3(n, r)
{
    var i, j;
    r = "";
    for(j = 0; j < n; j++) {
        for(i = 0; i < 100; i++)
            r = "x" + r;
        global_res = r;
    }
    return n * 100;
}

/* incremental string contruction with multiple reference */
function string_build4(n)
{
    var i, j, r, s;
    r = "";
    for(j = 0; j < n; j++) {
        for(i = 0; i < 100; i++) {
            s = r;
            r += "x";
        }
        global_res = r;
    }
    return n * 100;
}

/* sort bench */

function sort_bench(text) {
    function random(arr, n, def) {
        for (var i = 0; i < n; i++)
            arr[i] = def[(Math.random() * n) >> 0];
    }
    function random8(arr, n, def) {
        for (var i = 0; i < n; i++)
            arr[i] = def[(Math.random() * 256) >> 0];
    }
    function random1(arr, n, def) {
        for (var i = 0; i < n; i++)
            arr[i] = def[(Math.random() * 2) >> 0];
    }
    function hill(arr, n, def) {
        var mid = n >> 1;
        for (var i = 0; i < mid; i++)
            arr[i] = def[i];
        for (var i = mid; i < n; i++)
            arr[i] = def[n - i];
    }
    function comb(arr, n, def) {
        for (var i = 0; i < n; i++)
            arr[i] = def[(i & 1) * i];
    }
    function crisscross(arr, n, def) {
        for (var i = 0; i < n; i++)
            arr[i] = def[(i & 1) ? n - i : i];
    }
    function zero(arr, n, def) {
        for (var i = 0; i < n; i++)
            arr[i] = def[0];
    }
    function increasing(arr, n, def) {
        for (var i = 0; i < n; i++)
            arr[i] = def[i];
    }
    function decreasing(arr, n, def) {
        for (var i = 0; i < n; i++)
            arr[i] = def[n - 1 - i];
    }
    function alternate(arr, n, def) {
        for (var i = 0; i < n; i++)
            arr[i] = def[i ^ 1];
    }
    function jigsaw(arr, n, def) {
        for (var i = 0; i < n; i++)
            arr[i] = def[i % (n >> 4)];
    }
    function incbutone(arr, n, def) {
        for (var i = 0; i < n; i++)
            arr[i] = def[i];
        if (n > 0)
            arr[n >> 2] = def[n];
    }
    function incbutfirst(arr, n, def) {
        if (n > 0)
            arr[0] = def[n];
        for (var i = 1; i < n; i++)
            arr[i] = def[i];
    }
    function incbutlast(arr, n, def) {
        for (var i = 0; i < n - 1; i++)
            arr[i] = def[i + 1];
        if (n > 0)
            arr[n - 1] = def[0];
    }

    var sort_cases = [ random, random8, random1, jigsaw, hill, comb,
                      crisscross, zero, increasing, decreasing, alternate,
                      incbutone, incbutlast, incbutfirst ];

    var n = sort_bench.array_size || 10000;
    var array_type = sort_bench.array_type || Array;
    var def, arr;
    var i, j, x, y;
    var total = 0;

    var save_total_score = total_score;
    var save_total_scale = total_scale;

    // initialize default sorted array (n + 1 elements)
    def = new array_type(n + 1);
    if (array_type == Array) {
        for (i = 0; i <= n; i++) {
            def[i] = i + "";
        }
    } else {
        for (i = 0; i <= n; i++) {
            def[i] = i;
        }
    }
    def.sort();
    for (var f of sort_cases) {
        var ti = 0, tx = 0;
        for (j = 0; j < 100; j++) {
            arr = new array_type(n);
            f(arr, n, def);
            var t1 = get_clock();
            arr.sort();
            t1 = get_clock() - t1;
            tx += t1;
            if (!ti || ti > t1)
                ti = t1;
            if (tx >= clocks_per_sec)
                break;
        }
        total += ti;

        i = 0;
        x = arr[0];
        if (x !== void 0) {
            for (i = 1; i < n; i++) {
                y = arr[i];
                if (y === void 0)
                    break;
                if (x > y)
                    break;
                x = y;
            }
        }
        while (i < n && arr[i] === void 0)
            i++;
        if (i < n) {
            console.log("sort_bench: out of order error for " + f.name +
                        " at offset " + (i - 1) +
                        ": " + arr[i - 1] + " > " + arr[i]);
        }
        if (sort_bench.verbose)
            log_one("sort_" + f.name, n, ti, n * 100);
    }
    total_score = save_total_score;
    total_scale = save_total_scale;
    return total / n / 1000;
}
sort_bench.bench = true;
sort_bench.verbose = false;

function int_to_string(n)
{
    var s, r, j;
    r = 0;
    for(j = 0; j < n; j++) {
        s = (j + 1).toString();
    }
    return n;
}

function float_to_string(n)
{
    var s, r, j;
    r = 0;
    for(j = 0; j < n; j++) {
        s = (j + 0.1).toString();
    }
    return n;
}

function string_to_int(n)
{
    var s, r, j;
    r = 0;
    s = "12345";
    r = 0;
    for(j = 0; j < n; j++) {
        r += (s | 0);
    }
    global_res = r;
    return n;
}

function string_to_float(n)
{
    var s, r, j;
    r = 0;
    s = "12345.6";
    r = 0;
    for(j = 0; j < n; j++) {
        r -= s;
    }
    global_res = r;
    return n;
}

function load_result(filename)
{
    var f, str, res;
    if (typeof std === "undefined")
        return null;
    f = std.open(filename, "r");
    if (!f)
        return null;
    str = f.readAsString();
    res = JSON.parse(str);
    f.close();
    return res;
}

function save_result(filename, obj)
{
    var f;
    if (typeof std === "undefined")
        return;
    f = std.open(filename, "w");
    f.puts(JSON.stringify(obj, null, 2));
    f.puts("\n");
    f.close();
}

function main(argc, argv, g)
{
    var test_list = [
        empty_loop,
        date_now,
        prop_read,
        prop_write,
        prop_create,
        prop_delete,
        array_read,
        array_write,
        array_prop_create,
        array_length_decr,
        array_hole_length_decr,
        array_push,
        array_pop,
        typed_array_read,
        typed_array_write,
        global_read,
        global_write,
        global_write_strict,
        local_destruct,
        global_destruct,
        global_destruct_strict,
        func_call,
        closure_var,
        int_arith,
        float_arith,
        set_collection_add,
        array_for,
        array_for_in,
        array_for_of,
        math_min,
        string_build1,
        string_build2,
        //string_build3,
        //string_build4,
        sort_bench,
        int_to_string,
        float_to_string,
        string_to_int,
        string_to_float,
    ];
    var tests = [];
    var i, j, n, f, name;
    
    if (typeof BigInt == "function") {
        /* BigInt test */
        test_list.push(bigint64_arith);
        test_list.push(bigint256_arith);
    }
    if (typeof BigFloat == "function") {
        /* BigFloat test */
        test_list.push(float256_arith);
    }
    
    for (i = 1; i < argc;) {
        name = argv[i++];
        if (name == "-a") {
            sort_bench.verbose = true;
            continue;
        }
        if (name == "-t") {
            name = argv[i++];
            sort_bench.array_type = g[name];
            if (typeof sort_bench.array_type != "function") {
                console.log("unknown array type: " + name);
                return 1;
            }
            continue;
        }
        if (name == "-n") {
            sort_bench.array_size = +argv[i++];
            continue;
        }
        for (j = 0; j < test_list.length; j++) {
            f = test_list[j];
            if (name === f.name) {
                tests.push(f);
                break;
            }
        }
        if (j == test_list.length) {
            console.log("unknown benchmark: " + name);
            return 1;
        }
    }
    if (tests.length == 0)
        tests = test_list;

    ref_data = load_result("microbench.txt");
    log_data = {};
    log_line.apply(null, heads);
    n = 0;

    for(i = 0; i < tests.length; i++) {
        f = tests[i];
        bench(f, f.name, ref_data, log_data);
        if (ref_data && ref_data[f.name])
            n++;
    }
    if (ref_data)
        log_line("total", "", total[2], total[3], total_score * 100 / total_scale);
    else
        log_line("total", "", total[2]);
        
    if (tests == test_list)
        save_result("microbench-new.txt", log_data);
}

if (!scriptArgs)
    scriptArgs = [];
main(scriptArgs.length, scriptArgs, this);
