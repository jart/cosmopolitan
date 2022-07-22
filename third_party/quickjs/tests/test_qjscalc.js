"use math";
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

function pow(a, n)
{
    var r, i;
    r = 1;
    for(i = 0; i < n; i++)
        r *= a;
    return r;
}

function test_integer()
{
    var a, r;
    a = pow(3, 100);
    assert((a - 1) != a);
    assert(a == 515377520732011331036461129765621272702107522001);
    assert(a == 0x5a4653ca673768565b41f775d6947d55cf3813d1);
    assert(Integer.isInteger(1) === true);
    assert(Integer.isInteger(1.0) === false);

    assert(Integer.floorLog2(0) === -1);
    assert(Integer.floorLog2(7) === 2);

    r = 1 << 31;
    assert(r, 2147483648, "1 << 31 === 2147483648");
    
    r = 1 << 32;
    assert(r, 4294967296, "1 << 32 === 4294967296");
    
    r = (1 << 31) < 0;
    assert(r, false, "(1 << 31) < 0 === false");

    assert(typeof 1 === "number");
    assert(typeof 9007199254740991 === "number");
    assert(typeof 9007199254740992 === "bigint");
}

function test_float()
{
    assert(typeof 1.0 === "bigfloat");
    assert(1 == 1.0);
    assert(1 !== 1.0);
}

/* jscalc tests */

function test_modulo()
{
    var i, p, a, b;

    /* Euclidian modulo operator */
    assert((-3) % 2 == 1);
    assert(3 % (-2) == 1);

    p = 101;
    for(i = 1; i < p; i++) {
        a = Integer.invmod(i, p);
        assert(a >= 0 && a < p);
        assert((i * a) % p == 1);
    }

    assert(Integer.isPrime(2^107-1));
    assert(!Integer.isPrime((2^107-1) * (2^89-1)));
    a = Integer.factor((2^89-1)*2^3*11*13^2*1009);
    assert(a == [ 2,2,2,11,13,13,1009,618970019642690137449562111 ]);
}

function test_fraction()
{
    assert((1/3 + 1).toString(), "4/3")
    assert((2/3)^30, 1073741824/205891132094649);
    assert(1/3 < 2/3);
    assert(1/3 < 1);
    assert(1/3 == 1.0/3);
    assert(1.0/3 < 2/3);
}

function test_mod()
{
    var a, b, p;
    
    a = Mod(3, 101);
    b = Mod(-1, 101);
    assert((a + b) == Mod(2, 101));
    assert(a ^ 100 == Mod(1, 101));

    p = 2 ^ 607 - 1; /* mersenne prime */
    a = Mod(3, p) ^ (p - 1);
    assert(a == Mod(1, p));
}

function test_polynomial()
{
    var a, b, q, r, t, i;
    a = (1 + X) ^ 4;
    assert(a == X^4+4*X^3+6*X^2+4*X+1);
    
    r = (1 + X);
    q = (1+X+X^2);
    b = (1 - X^2);
    a = q * b + r;
    t = Polynomial.divrem(a, b);
    assert(t[0] == q);
    assert(t[1] == r);

    a = 1 + 2*X + 3*X^2;
    assert(a.apply(0.1) == 1.23);

    a = 1-2*X^2+2*X^3;
    assert(deriv(a) == (6*X^2-4*X));
    assert(deriv(integ(a)) == a);

    a = (X-1)*(X-2)*(X-3)*(X-4)*(X-0.1);
    r = polroots(a);
    for(i = 0; i < r.length; i++) {
        b = abs(a.apply(r[i]));
        assert(b <= 1e-13);
    }
}

function test_poly_mod()
{
    var a, p;

    /* modulo using polynomials */
    p = X^2 + X + 1;
    a = PolyMod(3+X, p) ^ 10;
    assert(a == PolyMod(-3725*X-18357, p));

    a = PolyMod(1/X, 1+X^2);
    assert(a == PolyMod(-X, X^2+1));
}

function test_rfunc()
{
    var a;
    a = (X+1)/((X+1)*(X-1));
    assert(a == 1/(X-1));
    a = (X + 2) / (X - 2);
    assert(a.apply(1/3) == -7/5);

    assert(deriv((X^2-X+1)/(X-1)) == (X^2-2*X)/(X^2-2*X+1));
}

function test_series()
{
    var a, b;
    a = 1+X+O(X^5);
    b = a.inverse();
    assert(b == 1-X+X^2-X^3+X^4+O(X^5));
    assert(deriv(b) == -1+2*X-3*X^2+4*X^3+O(X^4));
    assert(deriv(integ(b)) == b);

    a = Series(1/(1-X), 5);
    assert(a == 1+X+X^2+X^3+X^4+O(X^5));
    b = a.apply(0.1);
    assert(b == 1.1111);

    assert(exp(3*X^2+O(X^10)) == 1+3*X^2+9/2*X^4+9/2*X^6+27/8*X^8+O(X^10));
    assert(sin(X+O(X^6)) == X-1/6*X^3+1/120*X^5+O(X^6));
    assert(cos(X+O(X^6)) == 1-1/2*X^2+1/24*X^4+O(X^6));
    assert(tan(X+O(X^8)) == X+1/3*X^3+2/15*X^5+17/315*X^7+O(X^8));
        assert((1+X+O(X^6))^(2+X) == 1+2*X+2*X^2+3/2*X^3+5/6*X^4+5/12*X^5+O(X^6));
}

function test_matrix()
{
    var a, b, r;
    a = [[1, 2],[3, 4]];
    b = [3, 4];
    r = a * b;
    assert(r == [11, 25]);
    r = (a^-1) * 2;
    assert(r == [[-4, 2],[3, -1]]);

    assert(norm2([1,2,3]) == 14);

    assert(diag([1,2,3]) == [ [ 1, 0, 0 ], [ 0, 2, 0 ], [ 0, 0, 3 ] ]);
    assert(trans(a) == [ [ 1, 3 ], [ 2, 4 ] ]);
    assert(trans([1,2,3]) == [[1,2,3]]);
    assert(trace(a) == 5);

    assert(charpoly(Matrix.hilbert(4)) == X^4-176/105*X^3+3341/12600*X^2-41/23625*X+1/6048000);
    assert(det(Matrix.hilbert(4)) == 1/6048000);

    a = [[1,2,1],[-2,-3,1],[3,5,0]];
    assert(rank(a) == 2);
    assert(ker(a) == [ [ 5 ], [ -3 ], [ 1 ] ]);

    assert(dp([1, 2, 3], [3, -4, -7]) === -26);
    assert(cp([1, 2, 3], [3, -4, -7]) == [ -2, 16, -10 ]);
}

function assert_eq(a, ref)
{
    assert(abs(a / ref - 1.0) <= 1e-15);
}

function test_trig()
{
    assert_eq(sin(1/2), 0.479425538604203);
    assert_eq(sin(2+3*I), 9.154499146911428-4.168906959966565*I);
    assert_eq(cos(2+3*I), -4.189625690968807-9.109227893755337*I);
    assert_eq((2+0.5*I)^(1.1-0.5*I), 2.494363021357619-0.23076804554558092*I);
    assert_eq(sqrt(2*I), 1 + I);
}

test_integer();
test_float();

test_modulo();
test_fraction();
test_mod();
test_polynomial();
test_poly_mod();
test_rfunc();
test_series();
test_matrix();
test_trig();
