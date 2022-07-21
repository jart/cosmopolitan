"use strict";

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

function assertThrows(err, func)
{
    var ex;
    ex = false;
    try {
        func();
    } catch(e) {
        ex = true;
        assert(e instanceof err);
    }
    assert(ex, true, "exception expected");
}

// load more elaborate version of assert if available
try { __loadScript("test_assert.js"); } catch(e) {}

/*----------------*/

function bigint_pow(a, n)
{
    var r, i;
    r = 1n;
    for(i = 0n; i < n; i++)
        r *= a;
    return r;
}

/* a must be < b */
function test_less(a, b)
{
    assert(a < b);
    assert(!(b < a));
    assert(a <= b);
    assert(!(b <= a));
    assert(b > a);
    assert(!(a > b));
    assert(b >= a);
    assert(!(a >= b));
    assert(a != b);
    assert(!(a == b));
}

/* a must be numerically equal to b */
function test_eq(a, b)
{
    assert(a == b);
    assert(b == a);
    assert(!(a != b));
    assert(!(b != a));
    assert(a <= b);
    assert(b <= a);
    assert(!(a < b));
    assert(a >= b);
    assert(b >= a);
    assert(!(a > b));
}

function test_bigint1()
{
    var a, r;

    test_less(2n, 3n);
    test_eq(3n, 3n);

    test_less(2, 3n);
    test_eq(3, 3n);

    test_less(2.1, 3n);
    test_eq(Math.sqrt(4), 2n);

    a = bigint_pow(3n, 100n);
    assert((a - 1n) != a);
    assert(a == 515377520732011331036461129765621272702107522001n);
    assert(a == 0x5a4653ca673768565b41f775d6947d55cf3813d1n);

    r = 1n << 31n;
    assert(r, 2147483648n, "1 << 31n === 2147483648n");

    r = 1n << 32n;
    assert(r, 4294967296n, "1 << 32n === 4294967296n");
}

function test_bigint2()
{
    assert(BigInt(""), 0n);
    assert(BigInt("  123"), 123n);
    assert(BigInt("  123   "), 123n);
    assertThrows(SyntaxError, () => { BigInt("+") } );
    assertThrows(SyntaxError, () => { BigInt("-") } );
    assertThrows(SyntaxError, () => { BigInt("\x00a") } );
    assertThrows(SyntaxError, () => { BigInt("  123  r") } );
}

function test_divrem(div1, a, b, q)
{
    var div, divrem, t;
    div = BigInt[div1];
    divrem = BigInt[div1 + "rem"];
    assert(div(a, b) == q);
    t = divrem(a, b);
    assert(t[0] == q);
    assert(a == b * q + t[1]);
}

function test_idiv1(div, a, b, r)
{
    test_divrem(div, a, b, r[0]);
    test_divrem(div, -a, b, r[1]);
    test_divrem(div, a, -b, r[2]);
    test_divrem(div, -a, -b, r[3]);
}

/* QuickJS BigInt extensions */
function test_bigint_ext()
{
    var r;
    assert(BigInt.floorLog2(0n) === -1n);
    assert(BigInt.floorLog2(7n) === 2n);

    assert(BigInt.sqrt(0xffffffc000000000000000n) === 17592185913343n);
    r = BigInt.sqrtrem(0xffffffc000000000000000n);
    assert(r[0] === 17592185913343n);
    assert(r[1] === 35167191957503n);

    test_idiv1("tdiv", 3n, 2n, [1n, -1n, -1n, 1n]);
    test_idiv1("fdiv", 3n, 2n, [1n, -2n, -2n, 1n]);
    test_idiv1("cdiv", 3n, 2n, [2n, -1n, -1n, 2n]);
    test_idiv1("ediv", 3n, 2n, [1n, -2n, -1n, 2n]);
}

function test_bigfloat()
{
    var e, a, b, sqrt2;

    assert(typeof 1n === "bigint");
    assert(typeof 1l === "bigfloat");
    assert(1 == 1.0l);
    assert(1 !== 1.0l);

    test_less(2l, 3l);
    test_eq(3l, 3l);

    test_less(2, 3l);
    test_eq(3, 3l);

    test_less(2.1, 3l);
    test_eq(Math.sqrt(9), 3l);

    test_less(2n, 3l);
    test_eq(3n, 3l);

    e = new BigFloatEnv(128);
    assert(e.prec == 128);
    a = BigFloat.sqrt(2l, e);
    assert(a === BigFloat.parseFloat("0x1.6a09e667f3bcc908b2fb1366ea957d3e", 0, e));
    assert(e.inexact === true);
    assert(BigFloat.fpRound(a) == 0x1.6a09e667f3bcc908b2fb1366ea95l);

    b = BigFloatEnv.setPrec(BigFloat.sqrt.bind(null, 2), 128);
    assert(a === b);

    assert(BigFloat.isNaN(BigFloat(NaN)));
    assert(BigFloat.isFinite(1l));
    assert(!BigFloat.isFinite(1l/0l));

    assert(BigFloat.abs(-3l) === 3l);
    assert(BigFloat.sign(-3l) === -1l);

    assert(BigFloat.exp(0.2l) === 1.2214027581601698339210719946396742l);
    assert(BigFloat.log(3l) === 1.0986122886681096913952452369225256l);
    assert(BigFloat.pow(2.1l, 1.6l) === 3.277561666451861947162828744873745l);

    assert(BigFloat.sin(-1l) === -0.841470984807896506652502321630299l);
    assert(BigFloat.cos(1l) === 0.5403023058681397174009366074429766l);
    assert(BigFloat.tan(0.1l) === 0.10033467208545054505808004578111154l);

    assert(BigFloat.asin(0.3l) === 0.30469265401539750797200296122752915l);
    assert(BigFloat.acos(0.4l) === 1.1592794807274085998465837940224159l);
    assert(BigFloat.atan(0.7l) === 0.610725964389208616543758876490236l);
    assert(BigFloat.atan2(7.1l, -5.1l) === 2.1937053809751415549388104628759813l);

    assert(BigFloat.floor(2.5l) === 2l);
    assert(BigFloat.ceil(2.5l) === 3l);
    assert(BigFloat.trunc(-2.5l) === -2l);
    assert(BigFloat.round(2.5l) === 3l);

    assert(BigFloat.fmod(3l,2l) === 1l);
    assert(BigFloat.remainder(3l,2l) === -1l);

    /* string conversion */
    assert((1234.125l).toString(), "1234.125");
    assert((1234.125l).toFixed(2), "1234.13");
    assert((1234.125l).toFixed(2, "down"), "1234.12");
    assert((1234.125l).toExponential(), "1.234125e+3");
    assert((1234.125l).toExponential(5), "1.23413e+3");
    assert((1234.125l).toExponential(5, BigFloatEnv.RNDZ), "1.23412e+3");
    assert((1234.125l).toPrecision(6), "1234.13");
    assert((1234.125l).toPrecision(6, BigFloatEnv.RNDZ), "1234.12");

    /* string conversion with binary base */
    assert((0x123.438l).toString(16), "123.438");
    assert((0x323.438l).toString(16), "323.438");
    assert((0x723.438l).toString(16), "723.438");
    assert((0xf23.438l).toString(16), "f23.438");
    assert((0x123.438l).toFixed(2, BigFloatEnv.RNDNA, 16), "123.44");
    assert((0x323.438l).toFixed(2, BigFloatEnv.RNDNA, 16), "323.44");
    assert((0x723.438l).toFixed(2, BigFloatEnv.RNDNA, 16), "723.44");
    assert((0xf23.438l).toFixed(2, BigFloatEnv.RNDNA, 16), "f23.44");
    assert((0x0.0000438l).toFixed(6, BigFloatEnv.RNDNA, 16), "0.000044");
    assert((0x1230000000l).toFixed(1, BigFloatEnv.RNDNA, 16), "1230000000.0");
    assert((0x123.438l).toPrecision(5, BigFloatEnv.RNDNA, 16), "123.44");
    assert((0x123.438l).toPrecision(5, BigFloatEnv.RNDZ, 16), "123.43");
    assert((0x323.438l).toPrecision(5, BigFloatEnv.RNDNA, 16), "323.44");
    assert((0x723.438l).toPrecision(5, BigFloatEnv.RNDNA, 16), "723.44");
    assert((-0xf23.438l).toPrecision(5, BigFloatEnv.RNDD, 16), "-f23.44");
    assert((0x123.438l).toExponential(4, BigFloatEnv.RNDNA, 16), "1.2344p+8");
}

function test_bigdecimal()
{
    assert(1m === 1m);
    assert(1m !== 2m);
    test_less(1m, 2m);
    test_eq(2m, 2m);

    test_less(1, 2m);
    test_eq(2, 2m);

    test_less(1.1, 2m);
    test_eq(Math.sqrt(4), 2m);

    test_less(2n, 3m);
    test_eq(3n, 3m);

    assert(BigDecimal("1234.1") === 1234.1m);
    assert(BigDecimal("    1234.1") === 1234.1m);
    assert(BigDecimal("    1234.1  ") === 1234.1m);

    assert(BigDecimal(0.1) === 0.1m);
    assert(BigDecimal(123) === 123m);
    assert(BigDecimal(true) === 1m);

    assert(123m + 1m === 124m);
    assert(123m - 1m === 122m);

    assert(3.2m * 3m === 9.6m);
    assert(10m / 2m === 5m);
    assertThrows(RangeError, () => { 10m / 3m } );

    assert(10m % 3m === 1m);
    assert(-10m % 3m === -1m);

    assert(1234.5m ** 3m === 1881365963.625m);
    assertThrows(RangeError, () => { 2m ** 3.1m } );
    assertThrows(RangeError, () => { 2m ** -3m } );

    assert(BigDecimal.sqrt(2m,
                       { roundingMode: "half-even",
                         maximumSignificantDigits: 4 }) === 1.414m);
    assert(BigDecimal.sqrt(101m,
                       { roundingMode: "half-even",
                         maximumFractionDigits: 3 }) === 10.050m);
    assert(BigDecimal.sqrt(0.002m,
                       { roundingMode: "half-even",
                         maximumFractionDigits: 3 }) === 0.045m);

    assert(BigDecimal.round(3.14159m,
                       { roundingMode: "half-even",
                         maximumFractionDigits: 3 }) === 3.142m);

    assert(BigDecimal.add(3.14159m, 0.31212m,
                          { roundingMode: "half-even",
                            maximumFractionDigits: 2 }) === 3.45m);
    assert(BigDecimal.sub(3.14159m, 0.31212m,
                          { roundingMode: "down",
                            maximumFractionDigits: 2 }) === 2.82m);
    assert(BigDecimal.mul(3.14159m, 0.31212m,
                          { roundingMode: "half-even",
                            maximumFractionDigits: 3 }) === 0.981m);
    assert(BigDecimal.mod(3.14159m, 0.31211m,
                          { roundingMode: "half-even",
                            maximumFractionDigits: 4 }) === 0.0205m);
    assert(BigDecimal.div(20m, 3m,
                       { roundingMode: "half-even",
                         maximumSignificantDigits: 3 }) === 6.67m);
    assert(BigDecimal.div(20m, 3m,
                       { roundingMode: "half-even",
                         maximumFractionDigits: 50 }) ===
           6.66666666666666666666666666666666666666666666666667m);

    /* string conversion */
    assert((1234.125m).toString(), "1234.125");
    assert((1234.125m).toFixed(2), "1234.13");
    assert((1234.125m).toFixed(2, "down"), "1234.12");
    assert((1234.125m).toExponential(), "1.234125e+3");
    assert((1234.125m).toExponential(5), "1.23413e+3");
    assert((1234.125m).toExponential(5, "down"), "1.23412e+3");
    assert((1234.125m).toPrecision(6), "1234.13");
    assert((1234.125m).toPrecision(6, "down"), "1234.12");
    assert((-1234.125m).toPrecision(6, "floor"), "-1234.13");
}

test_bigint1();
test_bigint2();
test_bigint_ext();
test_bigfloat();
test_bigdecimal();
