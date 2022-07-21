import * as bjson from "./bjson.so";

function assert(actual, expected, message) {
    if (arguments.length == 1)
        expected = true;

    if (actual === expected)
        return;

    if (actual !== null && expected !== null
    &&  typeof actual == 'object' && typeof expected == 'object'
    &&  actual.toString() === expected.toString())
        return;

    throw Error("assertion failed: got |" + actual + "|" +
                ", expected |" + expected + "|" +
                (message ? " (" + message + ")" : ""));
}

function toHex(a)
{
    var i, s = "", tab, v;
    tab = new Uint8Array(a);
    for(i = 0; i < tab.length; i++) {
        v = tab[i].toString(16);
        if (v.length < 2)
            v = "0" + v;
        if (i !== 0)
            s += " ";
        s += v;
    }
    return s;
}

function isArrayLike(a)
{
    return Array.isArray(a) ||
        (a instanceof Uint8ClampedArray) ||
        (a instanceof Uint8Array) ||
        (a instanceof Uint16Array) ||
        (a instanceof Uint32Array) ||
        (a instanceof Int8Array) ||
        (a instanceof Int16Array) ||
        (a instanceof Int32Array) ||
        (a instanceof Float32Array) ||
        (a instanceof Float64Array);
}

function toStr(a)
{
    var s, i, props, prop;

    switch(typeof(a)) {
    case "object":
        if (a === null)
            return "null";
        if (a instanceof Date) {
            s = "Date(" + toStr(a.valueOf()) + ")";
        } else if (a instanceof Number) {
            s = "Number(" + toStr(a.valueOf()) + ")";
        } else if (a instanceof String) {
            s = "String(" + toStr(a.valueOf()) + ")";
        } else if (a instanceof Boolean) {
            s = "Boolean(" + toStr(a.valueOf()) + ")";
        } else if (isArrayLike(a)) {
            s = "[";
            for(i = 0; i < a.length; i++) {
                if (i != 0)
                    s += ",";
                s += toStr(a[i]);
            }
            s += "]";
        } else {
            props = Object.keys(a);
            s = "{";
            for(i = 0; i < props.length; i++) {
                if (i != 0)
                    s += ",";
                prop = props[i];
                s += prop + ":" + toStr(a[prop]);
            }
            s += "}";
        }
        return s;
    case "undefined":
        return "undefined";
    case "string":
        return a.__quote();
    case "number":
    case "bigfloat":
        if (a == 0 && 1 / a < 0)
            return "-0";
        else
            return a.toString();
        break;
    default:
        return a.toString();
    }
}

function bjson_test(a)
{
    var buf, r, a_str, r_str;
    a_str = toStr(a);
    buf = bjson.write(a);
    if (0) {
        print(a_str, "->", toHex(buf));
    }
    r = bjson.read(buf, 0, buf.byteLength);
    r_str = toStr(r);
    if (a_str != r_str) {
        print(a_str);
        print(r_str);
        assert(false);
    }
}

/* test multiple references to an object including circular
   references */
function bjson_test_reference()
{
    var array, buf, i, n, array_buffer;
    n = 16;
    array = [];
    for(i = 0; i < n; i++)
        array[i] = {};
    array_buffer = new ArrayBuffer(n);
    for(i = 0; i < n; i++) {
        array[i].next = array[(i + 1) % n];
        array[i].idx = i;
        array[i].typed_array = new Uint8Array(array_buffer, i, 1);
    }
    buf = bjson.write(array, true);

    array = bjson.read(buf, 0, buf.byteLength, true);

    /* check the result */
    for(i = 0; i < n; i++) {
        assert(array[i].next, array[(i + 1) % n]);
        assert(array[i].idx, i);
        assert(array[i].typed_array.buffer, array_buffer);
        assert(array[i].typed_array.length, 1);
        assert(array[i].typed_array.byteOffset, i);
    }
}

function bjson_test_all()
{
    var obj;

    bjson_test({x:1, y:2, if:3});
    bjson_test([1, 2, 3]);
    bjson_test([1.0, "aa", true, false, undefined, null, NaN, -Infinity, -0.0]);
    if (typeof BigInt !== "undefined") {
        bjson_test([BigInt("1"), -BigInt("0x123456789"),
               BigInt("0x123456789abcdef123456789abcdef")]);
    }
    if (typeof BigFloat !== "undefined") {
        BigFloatEnv.setPrec(function () {
            bjson_test([BigFloat("0.1"), BigFloat("-1e30"), BigFloat("0"),
                   BigFloat("-0"), BigFloat("Infinity"), BigFloat("-Infinity"),
                   0.0 / BigFloat("0"), BigFloat.MAX_VALUE,
                   BigFloat.MIN_VALUE]);
        }, 113, 15);
    }
    if (typeof BigDecimal !== "undefined") {
        bjson_test([BigDecimal("0"),
                    BigDecimal("0.8"), BigDecimal("123321312321321e100"),
                    BigDecimal("-1233213123213214332333223332e100"),
                    BigDecimal("1.233e-1000")]);
    }

    bjson_test([new Date(1234), new String("abc"), new Number(-12.1), new Boolean(true)]);

    bjson_test(new Int32Array([123123, 222111, -32222]));
    bjson_test(new Float64Array([123123, 222111.5]));

    /* tested with a circular reference */
    obj = {};
    obj.x = obj;
    try {
        bjson.write(obj);
        assert(false);
    } catch(e) {
        assert(e instanceof TypeError);
    }

    bjson_test_reference();
}

bjson_test_all();
