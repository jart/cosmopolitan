/*
 * QuickJS Javascript Calculator
 *
 * Copyright (c) 2017-2020 Fabrice Bellard
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
"use strict";
"use math";

var Integer, Float, Fraction, Complex, Mod, Polynomial, PolyMod, RationalFunction, Series, Matrix;

(function(global) {
    global.Integer = global.BigInt;
    global.Float = global.BigFloat;
    global.algebraicMode = true;

    /* add non enumerable properties */
    function add_props(obj, props) {
        var i, val, prop, tab, desc;
        tab = Reflect.ownKeys(props);
        for(i = 0; i < tab.length; i++) {
            prop = tab[i];
            desc = Object.getOwnPropertyDescriptor(props, prop);
            desc.enumerable = false;
            if ("value" in desc) {
                if (typeof desc.value !== "function") {
                    desc.writable = false;
                    desc.configurable = false;
                }
            } else {
                /* getter/setter */
                desc.configurable = false;
            }
            Object.defineProperty(obj, prop, desc);
        }
    }

    /* same as proto[Symbol.operatorSet] = Operators.create(..op_list)
       but allow shortcuts: left: [], right: [] or both
    */
    function operators_set(proto, ...op_list)
    {
        var new_op_list, i, a, j, b, k, obj, tab;
        var fields = [ "left", "right" ];
        new_op_list = [];
        for(i = 0; i < op_list.length; i++) {
            a = op_list[i];
            if (a.left || a.right) {
                tab = [ a.left, a.right ];
                delete a.left;
                delete a.right;
                for(k = 0; k < 2; k++) {
                    obj = tab[k];
                    if (obj) {
                        if (!Array.isArray(obj)) {
                            obj = [ obj ];
                        }
                        for(j = 0; j < obj.length; j++) {
                            b = {};
                            Object.assign(b, a);
                            b[fields[k]] = obj[j];
                            new_op_list.push(b);
                        }
                    }
                }
            } else {
                new_op_list.push(a);
            }
        }
        proto[Symbol.operatorSet] =
            Operators.create.call(null, ...new_op_list);
    }

    /* Integer */

    function generic_pow(a, b) {
        var r, is_neg, i;
        if (!Integer.isInteger(b)) {
            return exp(log(a) * b);
        }
        if (Array.isArray(a) && !(a instanceof Polynomial ||
                                  a instanceof Series)) {
            r = idn(Matrix.check_square(a));
        } else {
            r = 1;
        }
        if (b == 0)
            return r;
        is_neg = false;
        if (b < 0) {
            is_neg = true;
            b = -b;
        }
        r = a;
        for(i = Integer.floorLog2(b) - 1; i >= 0; i--) {
            r *= r;
            if ((b >> i) & 1)
                r *= a;
        }
        if (is_neg) {
            if (typeof r.inverse != "function")
                throw "negative powers are not supported for this type";
            r = r.inverse();
        }
        return r;
    }

    var small_primes = [ 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181, 191, 193, 197, 199, 211, 223, 227, 229, 233, 239, 241, 251, 257, 263, 269, 271, 277, 281, 283, 293, 307, 311, 313, 317, 331, 337, 347, 349, 353, 359, 367, 373, 379, 383, 389, 397, 401, 409, 419, 421, 431, 433, 439, 443, 449, 457, 461, 463, 467, 479, 487, 491, 499 ];

    function miller_rabin_test(n, t) {
        var d, r, s, i, j, a;
        d = n - 1;
        s = 0;
        while ((d & 1) == 0) {
            d >>= 1;
            s++;
        }
        if (small_primes.length < t)
            t = small_primes.length;
        loop: for(j = 0; j < t; j++) {
            a = small_primes[j];
            r = Integer.pmod(a, d, n);
            if (r == 1 || r == (n - 1))
                continue;
            for(i = 1; i < s; i++) {
                r = (r * r) % n;
                if (r == 1)
                    return false;
                if (r == (n - 1))
                    continue loop;
            }
            return false; /* n is composite */
        }
        return true; /* n is probably prime with probability (1-0.5^t) */
    }

    function fact_rec(a, b) {  /* assumes a <= b */
        var i, r;
        if ((b - a) <= 5) {
            r = a;
            for(i = a + 1; i <= b; i++)
                r *= i;
            return r;
        } else {
            /* to avoid a quadratic running time it is better to
               multiply numbers of similar size */
            i = (a + b) >> 1;
            return fact_rec(a, i) * fact_rec(i + 1, b);
        }
    }

    /* math mode specific quirk to overload the integer division and power */
    Operators.updateBigIntOperators(
        {
            "/"(a, b) {
                if (algebraicMode) {
                    return Fraction.toFraction(a, b);
                } else {
                    return Float(a) / Float(b);
                }
            },
            "**"(a, b) {
                if (algebraicMode) {
                    return generic_pow(a, b);
                } else {
                    return Float(a) ** Float(b);
                }
            }
        });

    add_props(Integer, {
        isInteger(a) {
            /* integers are represented either as bigint or as number */
            return typeof a === "bigint" ||
                (typeof a === "number" && Number.isSafeInteger(a));
        },
        gcd(a, b) {
            var r;
            while (b != 0) {
                r = a % b;
                a = b;
                b = r;
            }
            return a;
        },
        fact(n) {
            return n <= 0 ? 1 : fact_rec(1, n);
        },
        /* binomial coefficient */
        comb(n, k) {
            if (k < 0 || k > n)
                return 0;
            if (k > n - k)
                k = n - k;
            if (k == 0)
                return 1;
            return Integer.tdiv(fact_rec(n - k + 1, n), fact_rec(1, k));
        },
        /* inverse of x modulo y */
        invmod(x, y) {
            var q, u, v, a, c, t;
            u = x;
            v = y;
            c = 1;
            a = 0;
            while (u != 0) {
                t = Integer.fdivrem(v, u);
                q = t[0];
                v = u;
                u = t[1];
                t = c;
                c = a - q * c;
                a = t;
            }
            /* v = gcd(x, y) */
            if (v != 1)
                throw RangeError("not invertible");
            return a % y;
        },
        /* return a ^ b modulo m */
        pmod(a, b, m) {
            var r;
            if (b == 0)
                return 1;
            if (b < 0) {
                a = Integer.invmod(a, m);
                b = -b;
            }
            r = 1;
            for(;;) {
                if (b & 1) {
                    r = (r * a) % m;
                }
                b >>= 1;
                if (b == 0)
                    break;
                a = (a * a) % m;
            }
            return r;
        },

        /* return true if n is prime (or probably prime with
           probability 1-0.5^t) */
        isPrime(n, t) {
            var i, d, n1;
            if (!Integer.isInteger(n))
                throw TypeError("invalid type");
            if (n <= 1)
                return false;
            n1 = small_primes.length;
            /* XXX: need Integer.sqrt() */
            for(i = 0; i < n1; i++) {
                d = small_primes[i];
                if (d == n)
                    return true;
                if (d > n)
                    return false;
                if ((n % d) == 0)
                    return false;
            }
            if (n < d * d)
                return true;
            if (typeof t == "undefined")
                t = 64;
            return miller_rabin_test(n, t);
        },
        nextPrime(n) {
            if (!Integer.isInteger(n))
                throw TypeError("invalid type");
            if (n < 1)
                n = 1;
            for(;;) {
                n++;
                if (Integer.isPrime(n))
                    return n;
            }
        },
        factor(n) {
            var r, d;
            if (!Integer.isInteger(n))
                throw TypeError("invalid type");
            r = [];
            if (abs(n) <= 1) {
                r.push(n);
                return r;
            }
            if (n < 0) {
                r.push(-1);
                n = -n;
            }

            while ((n % 2) == 0) {
                n >>= 1;
                r.push(2);
            }

            d = 3;
            while (n != 1) {
                if (Integer.isPrime(n)) {
                    r.push(n);
                    break;
                }
                /* we are sure there is at least one divisor, so one test */
                for(;;) {
                    if ((n % d) == 0)
                        break;
                    d += 2;
                }
                for(;;) {
                    r.push(d);
                    n = Integer.tdiv(n, d);
                    if ((n % d) != 0)
                        break;
                }
            }
            return r;
        },
    });

    add_props(Integer.prototype, {
        inverse() {
            return 1 / this;
        },
        norm2() {
            return this * this;
        },
        abs() {
            var v = this;
            if (v < 0)
                v = -v;
            return v;
        },
        conj() {
            return this;
        },
        arg() {
            if (this >= 0)
                return 0;
            else
                return Float.PI;
        },
        exp() {
            if (this == 0)
                return 1;
            else
                return Float.exp(this);
        },
        log() {
            if (this == 1)
                return 0;
            else
                return Float(this).log();
        },
    });

    /* Fraction */

    Fraction = function Fraction(a, b)
    {
        var d, r, obj;

        if (new.target)
            throw TypeError("not a constructor");
        if (a instanceof Fraction)
            return a;
        if (!Integer.isInteger(a))
            throw TypeError("integer expected");
        if (typeof b === "undefined") {
            b = 1;
        } else {
            if (!Integer.isInteger(b))
                throw TypeError("integer expected");
            if (b == 0)
                throw RangeError("division by zero");
            d = Integer.gcd(a, b);
            if (d != 1) {
                a = Integer.tdiv(a, d);
                b = Integer.tdiv(b, d);
            }

            /* the fractions are normalized with den > 0 */
            if (b < 0) {
                a = -a;
                b = -b;
            }
        }
        obj = Object.create(Fraction.prototype);
        obj.num = a;
        obj.den = b;
        return obj;
    }

    function fraction_add(a, b) {
        a = Fraction(a);
        b = Fraction(b);
        return Fraction.toFraction(a.num * b.den + a.den * b.num, a.den * b.den);
    }
    function fraction_sub(a, b) {
        a = Fraction(a);
        b = Fraction(b);
        return Fraction.toFraction(a.num * b.den - a.den * b.num, a.den * b.den);
    }
    function fraction_mul(a, b) {
        a = Fraction(a);
        b = Fraction(b);
        return Fraction.toFraction(a.num * b.num, a.den * b.den);
    }
    function fraction_div(a, b) {
        a = Fraction(a);
        b = Fraction(b);
        return Fraction.toFraction(a.num * b.den, a.den * b.num);
    }
    function fraction_mod(a, b) {
        var a1 = Fraction(a);
        var b1 = Fraction(b);
        return a - Integer.ediv(a1.num * b1.den, a1.den * b1.num) * b;
    }
    function fraction_eq(a, b) {
        a = Fraction(a);
        b = Fraction(b);
        /* we assume the fractions are normalized */
        return (a.num == b.num && a.den == b.den);
    }
    function fraction_lt(a, b) {
        a = Fraction(a);
        b = Fraction(b);
        return (a.num * b.den < b.num * a.den);
    }

    /* operators are needed for fractions */
    function float_add(a, b) {
        return Float(a) + Float(b);
    }
    function float_sub(a, b) {
        return Float(a) - Float(b);
    }
    function float_mul(a, b) {
        return Float(a) * Float(b);
    }
    function float_div(a, b) {
        return Float(a) / Float(b);
    }
    function float_mod(a, b) {
        return Float(a) % Float(b);
    }
    function float_pow(a, b) {
        return Float(a) ** Float(b);
    }
    function float_eq(a, b) {
        /* XXX: may be better to use infinite precision for the comparison */
        return Float(a) === Float(b);
    }
    function float_lt(a, b) {
        a = Float(a);
        b = Float(b);
        /* XXX: may be better to use infinite precision for the comparison */
        if (Float.isNaN(a) || Float.isNaN(b))
            return undefined;
        else
            return a < b;
    }

    operators_set(Fraction.prototype,
        {
            "+": fraction_add,
            "-": fraction_sub,
            "*": fraction_mul,
            "/": fraction_div,
            "%": fraction_mod,
            "**": generic_pow,
            "==": fraction_eq,
            "<": fraction_lt,
            "pos"(a) {
                return a;
            },
            "neg"(a) {
                return Fraction(-a.num, a.den);
            },
        },
        {
            left: [Number, BigInt],
            right: [Number, BigInt],
            "+": fraction_add,
            "-": fraction_sub,
            "*": fraction_mul,
            "/": fraction_div,
            "%": fraction_mod,
            "**": generic_pow,
            "==": fraction_eq,
            "<": fraction_lt,
        },
        {
            left: Float,
            right: Float,
            "+": float_add,
            "-": float_sub,
            "*": float_mul,
            "/": float_div,
            "%": float_mod,
            "**": float_pow,
            "==": float_eq,
            "<": float_lt,
        });

    add_props(Fraction, {
        /* (internal use) simplify 'a' to an integer when possible */
        toFraction(a, b) {
            var r = Fraction(a, b);
            if (algebraicMode && r.den == 1)
                return r.num;
            else
                return r;
        },
    });

    add_props(Fraction.prototype, {
        [Symbol.toPrimitive](hint) {
            if (hint === "string") {
                return this.toString();
            } else {
                return Float(this.num) / this.den;
            }
        },
        inverse() {
            return Fraction(this.den, this.num);
        },
        toString() {
            return this.num + "/" + this.den;
        },
        norm2() {
            return this * this;
        },
        abs() {
            if (this.num < 0)
                return -this;
            else
                return this;
        },
        conj() {
            return this;
        },
        arg() {
            if (this.num >= 0)
                return 0;
            else
                return Float.PI;
        },
        exp() {
            return Float.exp(Float(this));
        },
        log() {
            return Float(this).log();
        },
    });

    /* Number (Float64) */

    add_props(Number.prototype, {
        inverse() {
            return 1 / this;
        },
        norm2() {
            return this * this;
        },
        abs() {
            return Math.abs(this);
        },
        conj() {
            return this;
        },
        arg() {
            if (this >= 0)
                return 0;
            else
                return Float.PI;
        },
        exp() {
            return Float.exp(this);
        },
        log() {
            if (this < 0) {
                return Complex(this).log();
            } else {
                return Float.log(this);
            }
        },
    });

    /* Float */

    var const_tab = [];

    /* we cache the constants for small precisions */
    function get_const(n) {
        var t, c, p;
        t = const_tab[n];
        p = BigFloatEnv.prec;
        if (t && t.prec == p) {
            return t.val;
        } else {
            switch(n) {
            case 0: c = Float.exp(1); break;
            case 1: c = Float.log(10); break;
//            case 2: c = Float.log(2); break;
            case 3: c = 1/Float.log(2); break;
            case 4: c = 1/Float.log(10); break;
//            case 5: c = Float.atan(1) * 4; break;
            case 6: c = Float.sqrt(0.5); break;
            case 7: c = Float.sqrt(2); break;
            }
            if (p <= 1024) {
                const_tab[n] = { prec: p, val: c };
            }
            return c;
        }
    }

    add_props(Float, {
        isFloat(a) {
            return typeof a === "number" || typeof a === "bigfloat";
        },
        bestappr(u, b) {
            var num1, num0, den1, den0, u, num, den, n;

            if (typeof b === "undefined")
                throw TypeError("second argument expected");
            num1 = 1;
            num0 = 0;
            den1 = 0;
            den0 = 1;
            for(;;) {
                n = Integer(Float.floor(u));
                num = n * num1 + num0;
                den = n * den1 + den0;
                if (den > b)
                    break;
                u = 1.0 / (u - n);
                num0 = num1;
                num1 = num;
                den0 = den1;
                den1 = den;
            }
            return Fraction(num1, den1);
        },
        /* similar constants as Math.x */
        get E() { return get_const(0); },
        get LN10() { return get_const(1); },
//        get LN2() { return get_const(2); },
        get LOG2E() { return get_const(3); },
        get LOG10E() { return get_const(4); },
//        get PI() { return get_const(5); },
        get SQRT1_2() { return get_const(6); },
        get SQRT2() { return get_const(7); },
    });

    add_props(Float.prototype, {
        inverse() {
            return 1.0 / this;
        },
        norm2() {
            return this * this;
        },
        abs() {
            return Float.abs(this);
        },
        conj() {
            return this;
        },
        arg() {
            if (this >= 0)
                return 0;
            else
                return Float.PI;
        },
        exp() {
            return Float.exp(this);
        },
        log() {
            if (this < 0) {
                return Complex(this).log();
            } else {
                return Float.log(this);
            }
        },
    });

    /* Complex */

    Complex = function Complex(re, im)
    {
        var obj;
        if (new.target)
            throw TypeError("not a constructor");
        if (re instanceof Complex)
            return re;
        if (typeof im === "undefined") {
            im = 0;
        }
        obj = Object.create(Complex.prototype);
        obj.re = re;
        obj.im = im;
        return obj;
    }


    function complex_add(a, b) {
        a = Complex(a);
        b = Complex(b);
        return Complex.toComplex(a.re + b.re, a.im + b.im);
    }
    function complex_sub(a, b) {
        a = Complex(a);
        b = Complex(b);
        return Complex.toComplex(a.re - b.re, a.im - b.im);
    }
    function complex_mul(a, b) {
        a = Complex(a);
        b = Complex(b);
        return Complex.toComplex(a.re * b.re - a.im * b.im,
                                 a.re * b.im + a.im * b.re);
    }
    function complex_div(a, b) {
        a = Complex(a);
        b = Complex(b);
        return a * b.inverse();
    }
    function complex_eq(a, b) {
        a = Complex(a);
        b = Complex(b);
        return a.re == b.re && a.im == b.im;
    }

    operators_set(Complex.prototype,
        {
            "+": complex_add,
            "-": complex_sub,
            "*": complex_mul,
            "/": complex_div,
            "**": generic_pow,
            "==": complex_eq,
            "pos"(a) {
                return a;
            },
            "neg"(a) {
                return Complex(-a.re, -a.im);
            }
        },
        {
            left: [Number, BigInt, Float, Fraction],
            right: [Number, BigInt, Float, Fraction],
            "+": complex_add,
            "-": complex_sub,
            "*": complex_mul,
            "/": complex_div,
            "**": generic_pow,
            "==": complex_eq,
        });

    add_props(Complex, {
        /* simplify to real number when possible */
        toComplex(re, im) {
            if (algebraicMode && im == 0)
                return re;
            else
                return Complex(re, im);
        },
    });

    add_props(Complex.prototype, {
        inverse() {
            var c = this.norm2();
            return Complex(this.re / c, -this.im / c);
        },
        toString() {
            var v, s = "", a = this;
            if (a.re != 0)
                s += a.re.toString();
            if (a.im == 1) {
                if (s != "")
                    s += "+";
                s += "I";
            } else if (a.im == -1) {
                s += "-I";
            } else {
                v = a.im.toString();
                if (v[0] != "-" && s != "")
                    s += "+";
                s += v + "*I";
            }
            return s;
        },
        norm2() {
            return this.re * this.re + this.im * this.im;
        },
        abs() {
            return Float.sqrt(norm2(this));
        },
        conj() {
            return Complex(this.re, -this.im);
        },
        arg() {
            return Float.atan2(this.im, this.re);
        },
        exp() {
            var arg = this.im, r = this.re.exp();
            return Complex(r * cos(arg), r * sin(arg));
        },
        log() {
            return Complex(abs(this).log(), atan2(this.im, this.re));
        },
    });

    /* Mod */

    Mod = function Mod(a, m) {
        var obj, t;
        if (new.target)
            throw TypeError("not a constructor");
        obj = Object.create(Mod.prototype);
        if (Integer.isInteger(m)) {
            if (m <= 0)
                throw RangeError("the modulo cannot be <= 0");
            if (Integer.isInteger(a)) {
                a %= m;
            } else if (a instanceof Fraction) {
                return Mod(a.num, m) / a.den;
            } else {
                throw TypeError("invalid types");
            }
        } else {
            throw TypeError("invalid types");
        }
        obj.res = a;
        obj.mod = m;
        return obj;
    };

    function mod_add(a, b) {
        if (!(a instanceof Mod)) {
            return Mod(a + b.res, b.mod);
        } else if (!(b instanceof Mod)) {
            return Mod(a.res + b, a.mod);
        } else {
            if (a.mod != b.mod)
                throw TypeError("different modulo for binary operator");
            return Mod(a.res + b.res, a.mod);
        }
    }
    function mod_sub(a, b) {
        if (!(a instanceof Mod)) {
            return Mod(a - b.res, b.mod);
        } else if (!(b instanceof Mod)) {
            return Mod(a.res - b, a.mod);
        } else {
            if (a.mod != b.mod)
                throw TypeError("different modulo for binary operator");
            return Mod(a.res - b.res, a.mod);
        }
    }
    function mod_mul(a, b) {
        if (!(a instanceof Mod)) {
            return Mod(a * b.res, b.mod);
        } else if (!(b instanceof Mod)) {
            return Mod(a.res * b, a.mod);
        } else {
            if (a.mod != b.mod)
                throw TypeError("different modulo for binary operator");
            return Mod(a.res * b.res, a.mod);
        }
    }
    function mod_div(a, b) {
        if (!(b instanceof Mod))
            b = Mod(b, a.mod);
        return mod_mul(a, b.inverse());
    }
    function mod_eq(a, b) {
        return (a.mod == b.mod && a.res == b.res);
    }

    operators_set(Mod.prototype,
        {
            "+": mod_add,
            "-": mod_sub,
            "*": mod_mul,
            "/": mod_div,
            "**": generic_pow,
            "==": mod_eq,
            "pos"(a) {
                return a;
            },
            "neg"(a) {
                return Mod(-a.res, a.mod);
            }
        },
        {
            left: [Number, BigInt, Float, Fraction],
            right: [Number, BigInt, Float, Fraction],
            "+": mod_add,
            "-": mod_sub,
            "*": mod_mul,
            "/": mod_div,
            "**": generic_pow,
        });

    add_props(Mod.prototype, {
        inverse() {
            var a = this, m = a.mod;
            if (Integer.isInteger(m)) {
                return Mod(Integer.invmod(a.res, m), m);
            } else {
                throw TypeError("unsupported type");
            }
        },
        toString() {
            return "Mod(" + this.res + "," + this.mod + ")";
        },
    });

    /* Polynomial */

    function polynomial_is_scalar(a)
    {
        if (typeof a === "number" ||
            typeof a === "bigint" ||
            typeof a === "bigfloat")
            return true;
        if (a instanceof Fraction ||
            a instanceof Complex ||
            a instanceof Mod)
            return true;
        return false;
    }

    Polynomial = function Polynomial(a)
    {
        if (new.target)
            throw TypeError("not a constructor");
        if (a instanceof Polynomial) {
            return a;
        } else if (Array.isArray(a)) {
            if (a.length == 0)
                a = [ 0 ];
            Object.setPrototypeOf(a, Polynomial.prototype);
            return a.trim();
        } else if (polynomial_is_scalar(a)) {
            a = [a];
            Object.setPrototypeOf(a, Polynomial.prototype);
            return a;
        } else {
            throw TypeError("invalid type");
        }
    }

    function number_need_paren(c)
    {
        return !(Integer.isInteger(c) ||
                 Float.isFloat(c) ||
                 c instanceof Fraction ||
                 (c instanceof Complex && c.re == 0));
    }

    /* string for c*X^i */
    function monomial_toString(c, i)
    {
        var str1;
        if (i == 0) {
            str1 = c.toString();
        } else {
            if (c == 1) {
                str1 = "";
            } else if (c == -1) {
                str1 = "-";
            } else {
                if (number_need_paren(c)) {
                    str1 = "(" + c + ")";
                } else {
                    str1 = String(c);
                }
                str1 += "*";
            }
            str1 += "X";
            if (i != 1) {
                str1 += "^" + i;
            }
        }
        return str1;
    }

    /* find one complex root of 'p' starting from z at precision eps using
       at most max_it iterations. Return null if could not find root. */
    function poly_root_laguerre1(p, z, max_it)
    {
        var p1, p2, i, z0, z1, z2, d, t0, t1, d1, d2, e, el, zl;

        d = p.deg();
        if (d == 1) {
            /* monomial case */
            return -p[0] / p[1];
        }
        /* trivial zero */
        if (p[0] == 0)
            return 0.0;

        p1 = p.deriv();
        p2 = p1.deriv();
        el = 0.0;
        zl = 0.0;
        for(i = 0; i < max_it; i++) {
            z0 = p.apply(z);
            if (z0 == 0)
                return z; /* simple exit case */

            /* Ward stopping criteria */
            e = abs(z - zl);
//            print("e", i, e);
            if (i >= 2 && e >= el) {
                if (abs(zl) < 1e-4) {
                    if (e < 1e-7)
                        return zl;
                } else {
                    if (e < abs(zl) * 1e-3)
                        return zl;
                }
            }
            el = e;
            zl = z;

            z1 = p1.apply(z);
            z2 = p2.apply(z);
            t0 = (d - 1) * z1;
            t0 = t0 * t0;
            t1 = d * (d - 1) * z0 * z2;
            t0 = sqrt(t0 - t1);
            d1 = z1 + t0;
            d2 = z1 - t0;
            if (norm2(d2) > norm2(d1))
                d1 = d2;
            if (d1 == 0)
                return null;
            z = z - d * z0 / d1;
        }
        return null;
    }

    function poly_roots(p)
    {
        var d, i, roots, j, z, eps;
        var start_points = [ 0.1, -1.4, 1.7 ];

        if (!(p instanceof Polynomial))
            throw TypeError("polynomial expected");
        d = p.deg();
        if (d <= 0)
            return [];
        eps = 2.0 ^ (-BigFloatEnv.prec);
        roots = [];
        for(i = 0; i < d; i++) {
            /* XXX: should select another start point if error */
            for(j = 0; j < 3; j++) {
                z = poly_root_laguerre1(p, start_points[j], 100);
                if (z !== null)
                    break;
            }
            if (j == 3)
                throw RangeError("error in root finding algorithm");
            roots[i] = z;
            p = Polynomial.divrem(p, X - z)[0];
        }
        return roots;
    }

    add_props(Polynomial.prototype, {
        trim() {
            var a = this, i;
            i = a.length;
            while (i > 1 && a[i - 1] == 0)
                i--;
            a.length = i;
            return a;
        },
        conj() {
            var r, i, n, a;
            a = this;
            n = a.length;
            r = [];
            for(i = 0; i < n; i++)
                r[i] = a[i].conj();
            return Polynomial(r);
        },
        inverse() {
            return RationalFunction(Polynomial([1]), this);
        },
        toString() {
            var i, str, str1, c, a = this;
            if (a.length == 1) {
                return a[0].toString();
            }
            str="";
            for(i = a.length - 1; i >= 0; i--) {
                c = a[i];
                if (c == 0 ||
                    (c instanceof Mod) && c.res == 0)
                    continue;
                str1 = monomial_toString(c, i);
                if (str1[0] != "-") {
                    if (str != "")
                        str += "+";
                }
                str += str1;
            }
            return str;
        },
        deg() {
            if (this.length == 1 && this[0] == 0)
                return -Infinity;
            else
                return this.length - 1;
        },
        apply(b) {
            var i, n, r, a = this;
            n = a.length - 1;
            r = a[n];
            while (n > 0) {
                n--;
                r = r * b + a[n];
            }
            return r;
        },
        deriv() {
            var a = this, n, r, i;
            n = a.length;
            if (n == 1) {
                return Polynomial(0);
            } else {
                r = [];
                for(i = 1; i < n; i++) {
                    r[i - 1] = i * a[i];
                }
                return Polynomial(r);
            }
        },
        integ() {
            var a = this, n, r, i;
            n = a.length;
            r = [0];
            for(i = 0; i < n; i++) {
                r[i + 1] = a[i] / (i + 1);
            }
            return Polynomial(r);
        },
        norm2() {
            var a = this, n, r, i;
            n = a.length;
            r = 0;
            for(i = 0; i < n; i++) {
                r += a[i].norm2();
            }
            return r;
        },
    });


    function polynomial_add(a, b) {
        var tmp, r, i, n1, n2;
        a = Polynomial(a);
        b = Polynomial(b);
        if (a.length < b.length) {
            tmp = a;
            a = b;
            b = tmp;
        }
        n1 = b.length;
        n2 = a.length;
        r = [];
        for(i = 0; i < n1; i++)
            r[i] = a[i] + b[i];
        for(i = n1; i < n2; i++)
            r[i] = a[i];
        return Polynomial(r);
    }
    function polynomial_sub(a, b) {
        return polynomial_add(a, -b);
    }
    function polynomial_mul(a, b) {
        var i, j, n1, n2, n, r;
        a = Polynomial(a);
        b = Polynomial(b);
        n1 = a.length;
        n2 = b.length;
        n = n1 + n2 - 1;
        r = [];
        for(i = 0; i < n; i++)
            r[i] = 0;
        for(i = 0; i < n1; i++) {
            for(j = 0; j < n2; j++) {
                r[i + j] += a[i] * b[j];
            }
        }
        return Polynomial(r);
    }
    function polynomial_div_scalar(a, b) {
        return a * (1 / b);
    }
    function polynomial_div(a, b)
    {
        return RationalFunction(Polynomial(a),
                                Polynomial(b));
    }
    function polynomial_mod(a, b) {
        return Polynomial.divrem(a, b)[1];
    }
    function polynomial_eq(a, b) {
        var n, i;
        n = a.length;
        if (n != b.length)
            return false;
        for(i = 0; i < n; i++) {
            if (a[i] != b[i])
                return false;
        }
        return true;
    }

    operators_set(Polynomial.prototype,
        {
            "+": polynomial_add,
            "-": polynomial_sub,
            "*": polynomial_mul,
            "/": polynomial_div,
            "**": generic_pow,
            "==": polynomial_eq,
            "pos"(a) {
                return a;
            },
            "neg"(a) {
                var r, i, n, a;
                n = a.length;
                r = [];
                for(i = 0; i < n; i++)
                r[i] = -a[i];
                return Polynomial(r);
            },
        },
        {
            left: [Number, BigInt, Float, Fraction, Complex, Mod],
            "+": polynomial_add,
            "-": polynomial_sub,
            "*": polynomial_mul,
            "/": polynomial_div,
            "**": generic_pow, /* XXX: only for integer */
        },
        {
            right: [Number, BigInt, Float, Fraction, Complex, Mod],
            "+": polynomial_add,
            "-": polynomial_sub,
            "*": polynomial_mul,
            "/": polynomial_div_scalar,
            "**": generic_pow, /* XXX: only for integer */
        });

    add_props(Polynomial, {
        divrem(a, b) {
            var n1, n2, i, j, q, r, n, c;
            if (b.deg() < 0)
                throw RangeError("division by zero");
            n1 = a.length;
            n2 = b.length;
            if (n1 < n2)
                return [Polynomial([0]), a];
            r = Array.prototype.dup.call(a);
            q = [];
            n2--;
            n = n1 - n2;
            for(i = 0; i < n; i++)
                q[i] = 0;
            for(i = n - 1; i >= 0; i--) {
                c = r[i + n2];
                if (c != 0) {
                    c = c / b[n2];
                    r[i + n2] = 0;
                    for(j = 0; j < n2; j++) {
                        r[i + j] -= b[j] * c;
                    }
                    q[i] = c;
                }
            }
            return [Polynomial(q), Polynomial(r)];
        },
        gcd(a, b) {
            var t;
            while (b.deg() >= 0) {
                t = Polynomial.divrem(a, b);
                a = b;
                b = t[1];
            }
            /* convert to monic form */
            return a / a[a.length - 1];
        },
        invmod(x, y) {
            var q, u, v, a, c, t;
            u = x;
            v = y;
            c = Polynomial([1]);
            a = Polynomial([0]);
            while (u.deg() >= 0) {
                t = Polynomial.divrem(v, u);
                q = t[0];
                v = u;
                u = t[1];
                t = c;
                c = a - q * c;
                a = t;
            }
            /* v = gcd(x, y) */
            if (v.deg() > 0)
                throw RangeError("not invertible");
            return Polynomial.divrem(a, y)[1];
        },
        roots(p) {
            return poly_roots(p);
        }
    });

    /* Polynomial Modulo Q */

    PolyMod = function PolyMod(a, m) {
        var obj, t;
        if (new.target)
            throw TypeError("not a constructor");
        obj = Object.create(PolyMod.prototype);
        if (m instanceof Polynomial) {
            if (m.deg() <= 0)
                throw RangeError("the modulo cannot have a degree <= 0");
            if (a instanceof RationalFunction) {
                return PolyMod(a.num, m) / a.den;
            } else {
                a = Polynomial(a);
                t = Polynomial.divrem(a, m);
                a = t[1];
            }
        } else {
            throw TypeError("invalid types");
        }
        obj.res = a;
        obj.mod = m;
        return obj;
    };

    function polymod_add(a, b) {
        if (!(a instanceof PolyMod)) {
            return PolyMod(a + b.res, b.mod);
        } else if (!(b instanceof PolyMod)) {
            return PolyMod(a.res + b, a.mod);
        } else {
            if (a.mod != b.mod)
                throw TypeError("different modulo for binary operator");
            return PolyMod(a.res + b.res, a.mod);
        }
    }
    function polymod_sub(a, b) {
        return polymod_add(a, -b);
    }
    function polymod_mul(a, b) {
        if (!(a instanceof PolyMod)) {
            return PolyMod(a * b.res, b.mod);
        } else if (!(b instanceof PolyMod)) {
            return PolyMod(a.res * b, a.mod);
        } else {
            if (a.mod != b.mod)
                    throw TypeError("different modulo for binary operator");
            return PolyMod(a.res * b.res, a.mod);
        }
    }
    function polymod_div(a, b) {
        if (!(b instanceof PolyMod))
            b = PolyMod(b, a.mod);
        return polymod_mul(a, b.inverse());
    }
    function polymod_eq(a, b) {
        return (a.mod == b.mod && a.res == b.res);
    }

    operators_set(PolyMod.prototype,
        {
            "+": polymod_add,
            "-": polymod_sub,
            "*": polymod_mul,
            "/": polymod_div,
            "**": generic_pow,
            "==": polymod_eq,
            "pos"(a) {
                return a;
            },
            "neg"(a) {
                return PolyMod(-a.res, a.mod);
            },
        },
        {
            left: [Number, BigInt, Float, Fraction, Complex, Mod, Polynomial],
            right: [Number, BigInt, Float, Fraction, Complex, Mod, Polynomial],
            "+": polymod_add,
            "-": polymod_sub,
            "*": polymod_mul,
            "/": polymod_div,
            "**": generic_pow, /* XXX: only for integer */
        });

    add_props(PolyMod.prototype, {
        inverse() {
            var a = this, m = a.mod;
            if (m instanceof Polynomial) {
                return PolyMod(Polynomial.invmod(a.res, m), m);
            } else {
                throw TypeError("unsupported type");
            }
        },
        toString() {
            return "PolyMod(" + this.res + "," + this.mod + ")";
        },
    });

    /* Rational function */

    RationalFunction = function RationalFunction(a, b)
    {
        var t, r, d, obj;
        if (new.target)
            throw TypeError("not a constructor");
        if (!(a instanceof Polynomial) ||
            !(b instanceof Polynomial))
            throw TypeError("polynomial expected");
        t = Polynomial.divrem(a, b);
        r = t[1];
        if (r.deg() < 0)
            return t[0]; /* no need for a fraction */
        d = Polynomial.gcd(b, r);
        if (d.deg() > 0) {
            a = Polynomial.divrem(a, d)[0];
            b = Polynomial.divrem(b, d)[0];
        }
        obj = Object.create(RationalFunction.prototype);
        obj.num = a;
        obj.den = b;
        return obj;
    }

    add_props(RationalFunction.prototype, {
        inverse() {
            return RationalFunction(this.den, this.num);
        },
        conj() {
            return RationalFunction(this.num.conj(), this.den.conj());
        },
        toString() {
            var str;
            if (this.num.deg() <= 0 &&
                !number_need_paren(this.num[0]))
                str = this.num.toString();
            else
                str = "(" + this.num.toString() + ")";
            str += "/(" + this.den.toString() + ")"
            return str;
        },
        apply(b) {
            return this.num.apply(b) / this.den.apply(b);
        },
        deriv() {
            var n = this.num, d = this.den;
            return RationalFunction(n.deriv() * d - n * d.deriv(), d * d);
        },
    });

    function ratfunc_add(a, b) {
        a = RationalFunction.toRationalFunction(a);
        b = RationalFunction.toRationalFunction(b);
        return RationalFunction(a.num * b.den + a.den * b.num, a.den * b.den);
    }
    function ratfunc_sub(a, b) {
        a = RationalFunction.toRationalFunction(a);
        b = RationalFunction.toRationalFunction(b);
        return RationalFunction(a.num * b.den - a.den * b.num, a.den * b.den);
    }
    function ratfunc_mul(a, b) {
        a = RationalFunction.toRationalFunction(a);
        b = RationalFunction.toRationalFunction(b);
        return RationalFunction(a.num * b.num, a.den * b.den);
    }
    function ratfunc_div(a, b) {
        a = RationalFunction.toRationalFunction(a);
        b = RationalFunction.toRationalFunction(b);
        return RationalFunction(a.num * b.den, a.den * b.num);
    }
    function ratfunc_eq(a, b) {
        a = RationalFunction.toRationalFunction(a);
        b = RationalFunction.toRationalFunction(b);
        /* we assume the fractions are normalized */
        return (a.num == b.num && a.den == b.den);
    }

    operators_set(RationalFunction.prototype,
        {
            "+": ratfunc_add,
            "-": ratfunc_sub,
            "*": ratfunc_mul,
            "/": ratfunc_div,
            "**": generic_pow,
            "==": ratfunc_eq,
            "pos"(a) {
                return a;
            },
            "neg"(a) {
                return RationalFunction(-this.num, this.den);
            },
        },
        {
            left: [Number, BigInt, Float, Fraction, Complex, Mod, Polynomial],
            right: [Number, BigInt, Float, Fraction, Complex, Mod, Polynomial],
            "+": ratfunc_add,
            "-": ratfunc_sub,
            "*": ratfunc_mul,
            "/": ratfunc_div,
            "**": generic_pow, /* should only be used with integers */
        });

    add_props(RationalFunction, {
        /* This function always return a RationalFunction object even
           if it could simplified to a polynomial, so it is not
           equivalent to RationalFunction(a) */
        toRationalFunction(a) {
            var obj;
            if (a instanceof RationalFunction) {
                return a;
            } else {
                obj = Object.create(RationalFunction.prototype);
                obj.num = Polynomial(a);
                obj.den = Polynomial(1);
                return obj;
            }
        },
    });

    /* Power series */

    /* 'a' is an array */
    function get_emin(a) {
        var i, n;
        n = a.length;
        for(i = 0; i < n; i++) {
            if (a[i] != 0)
                return i;
        }
        return n;
    };

    function series_is_scalar_or_polynomial(a)
    {
        return polynomial_is_scalar(a) ||
            (a instanceof Polynomial);
    }

    /* n is the maximum number of terms if 'a' is not a serie */
    Series = function Series(a, n) {
        var emin, r, i;

        if (a instanceof Series) {
            return a;
        } else if (series_is_scalar_or_polynomial(a)) {
            if (n <= 0) {
                /* XXX: should still use the polynomial degree */
                return Series.zero(0, 0);
            } else {
                a = Polynomial(a);
                emin = get_emin(a);
                r = Series.zero(n, emin);
                n = Math.min(a.length - emin, n);
                for(i = 0; i < n; i++)
                    r[i] = a[i + emin];
                return r;
            }
        } else if (a instanceof RationalFunction) {
            return Series(a.num, n) / a.den;
        } else {
            throw TypeError("invalid type");
        }
    };

    function series_add(v1, v2) {
        var tmp, d, emin, n, r, i, j, v2_emin, c1, c2;
        if (!(v1 instanceof Series)) {
            tmp = v1;
            v1 = v2;
            v2 = tmp;
        }
        d = v1.emin + v1.length;
        if (series_is_scalar_or_polynomial(v2)) {
            v2 = Polynomial(v2);
            if (d <= 0)
                return v1;
            v2_emin = 0;
        } else if (v2 instanceof RationalFunction) {
            /* compute the emin of the rational fonction */
            i = get_emin(v2.num) - get_emin(v2.den);
            if (d <= i)
                return v1;
            /* compute the serie with the required terms */
            v2 = Series(v2, d - i);
            v2_emin = v2.emin;
        } else {
            v2_emin = v2.emin;
            d = Math.min(d, v2_emin + v2.length);
        }
        emin = Math.min(v1.emin, v2_emin);
        n = d - emin;
        r = Series.zero(n, emin);
        /* XXX: slow */
        for(i = emin; i < d; i++) {
            j = i - v1.emin;
            if (j >= 0 && j < v1.length)
                c1 = v1[j];
            else
                c1 = 0;
            j = i - v2_emin;
            if (j >= 0 && j < v2.length)
                c2 = v2[j];
            else
                c2 = 0;
            r[i - emin] = c1 + c2;
        }
        return r.trim();
    }
    function series_sub(a, b) {
        return series_add(a, -b);
    }
    function series_mul(v1, v2) {
        var n, i, j, r, n, emin, n1, n2, k;
        if (!(v1 instanceof Series))
            v1 = Series(v1, v2.length);
        else if (!(v2 instanceof Series))
            v2 = Series(v2, v1.length);
        emin = v1.emin + v2.emin;
        n = Math.min(v1.length, v2.length);
        n1 = v1.length;
        n2 = v2.length;
        r = Series.zero(n, emin);
        for(i = 0; i < n1; i++) {
            k = Math.min(n2, n - i);
            for(j = 0; j < k; j++) {
                r[i + j] += v1[i] * v2[j];
            }
        }
        return r.trim();
    }
    function series_div(v1, v2) {
        if (!(v2 instanceof Series))
            v2 = Series(v2, v1.length);
        return series_mul(v1, v2.inverse());
    }
    function series_pow(a, b) {
        if (Integer.isInteger(b)) {
            return generic_pow(a, b);
        } else {
            if (!(a instanceof Series))
                a = Series(a, b.length);
            return exp(log(a) * b);
        }
    }
    function series_eq(a, b) {
        var n, i;
        if (a.emin != b.emin)
            return false;
        n = a.length;
        if (n != b.length)
            return false;
        for(i = 0; i < n; i++) {
            if (a[i] != b[i])
                return false;
        }
        return true;
    }

    operators_set(Series.prototype,
        {
            "+": series_add,
            "-": series_sub,
            "*": series_mul,
            "/": series_div,
            "**": series_pow,
            "==": series_eq,
            "pos"(a) {
                return a;
            },
            "neg"(a) {
                var obj, n, i;
                n = a.length;
                obj = Series.zero(a.length, a.emin);
                for(i = 0; i < n; i++) {
                    obj[i] = -a[i];
                }
                return obj;
            },
        },
        {
            left: [Number, BigInt, Float, Fraction, Complex, Mod, Polynomial],
            right: [Number, BigInt, Float, Fraction, Complex, Mod, Polynomial],
            "+": series_add,
            "-": series_sub,
            "*": series_mul,
            "/": series_div,
            "**": series_pow,
        });

    add_props(Series.prototype, {
        conj() {
            var obj, n, i;
            n = this.length;
            obj = Series.zero(this.length, this.emin);
            for(i = 0; i < n; i++) {
                obj[i] = this[i].conj();
            }
            return obj;
        },
        inverse() {
            var r, n, i, j, sum, v1 = this;
            n = v1.length;
            if (n == 0)
                throw RangeError("division by zero");
            r = Series.zero(n, -v1.emin);
            r[0] = 1 / v1[0];
            for(i = 1; i < n; i++) {
                sum = 0;
                for(j = 1; j <= i; j++) {
                    sum += v1[j] * r[i - j];
                }
                r[i] = -sum * r[0];
            }
            return r;
        },
        /* remove leading zero terms */
        trim() {
            var i, j, n, r, v1 = this;
            n = v1.length;
            i = 0;
            while (i < n && v1[i] == 0)
                i++;
            if (i == 0)
                return v1;
            for(j = i; j < n; j++)
                v1[j - i] = v1[j];
            v1.length = n - i;
            v1.__proto__.emin += i;
            return v1;
        },
        toString() {
            var i, j, str, str1, c, a = this, emin, n;
            str="";
            emin = this.emin;
            n = this.length;
            for(j = 0; j < n; j++) {
                i = j + emin;
                c = a[j];
                if (c != 0) {
                    str1 = monomial_toString(c, i);
                    if (str1[0] != "-") {
                        if (str != "")
                            str += "+";
                    }
                    str += str1;
                }
            }
            if (str != "")
                str += "+";
            str += "O(" + monomial_toString(1, n + emin) + ")";
            return str;
        },
        apply(b) {
            var i, n, r, a = this;
            n = a.length;
            if (n == 0)
                return 0;
            r = a[--n];
            while (n > 0) {
                n--;
                r = r * b + a[n];
            }
            if (a.emin != 0)
                r *= b ^ a.emin;
            return r;
        },
        deriv() {
            var a = this, n = a.length, emin = a.emin, r, i, j;
            if (n == 0 && emin == 0) {
                return Series.zero(0, 0);
            } else {
                r = Series.zero(n, emin - 1);
                for(i = 0; i < n; i++) {
                    j = emin + i;
                    if (j == 0)
                        r[i] = 0;
                    else
                        r[i] = j * a[i];
                }
                return r.trim();
            }
        },
        integ() {
            var a = this, n = a.length, emin = a.emin, i, j, r;
            r = Series.zero(n, emin + 1);
            for(i = 0; i < n; i++) {
                j = emin + i;
                if (j == -1) {
                    if (a[i] != 0)
                        throw RangeError("cannot represent integ(1/X)");
                } else {
                    r[i] = a[i] / (j + 1);
                }
            }
            return r.trim();
        },
        exp() {
            var c, i, r, n, a = this;
            if (a.emin < 0)
                throw RangeError("negative exponent in exp");
            n = a.emin + a.length;
            if (a.emin > 0 || a[0] == 0) {
                c = 1;
            } else {
                c = global.exp(a[0]);
                a -= a[0];
            }
            r = Series.zero(n, 0);
            for(i = 0; i < n; i++) {
                r[i] = c / fact(i);
            }
            return r.apply(a);
        },
        log() {
            var a = this, r;
            if (a.emin != 0)
                throw RangeError("log argument must have a non zero constant term");
            r = integ(deriv(a) / a);
            /* add the constant term */
            r += global.log(a[0]);
            return r;
        },
    });

    add_props(Series, {
        /* new series of length n and first exponent emin */
        zero(n, emin) {
            var r, i, obj;

            r = [];
            for(i = 0; i < n; i++)
                r[i] = 0;
            /* we return an array and store emin in its prototype */
            obj = Object.create(Series.prototype);
            obj.emin = emin;
            Object.setPrototypeOf(r, obj);
            return r;
        },
        O(a) {
            function ErrorO() {
                return TypeError("invalid O() argument");
            }
            var n;
            if (series_is_scalar_or_polynomial(a)) {
                a = Polynomial(a);
                n = a.deg();
                if (n < 0)
                    throw ErrorO();
            } else if (a instanceof RationalFunction) {
                if (a.num.deg() != 0)
                    throw ErrorO();
                n = a.den.deg();
                if (n < 0)
                    throw ErrorO();
                n = -n;
            } else
                throw ErrorO();
            return Series.zero(0, n);
        },
    });

    /* Array (Matrix) */

    Matrix = function Matrix(h, w) {
        var i, j, r, rl;
        if (typeof w === "undefined")
            w = h;
        r = [];
        for(i = 0; i < h; i++) {
            rl = [];
            for(j = 0; j < w; j++)
                rl[j] = 0;
            r[i] = rl;
        }
        return r;
    };

    add_props(Matrix, {
        idn(n) {
            var r, i;
            r = Matrix(n, n);
            for(i = 0; i < n; i++)
                r[i][i] = 1;
            return r;
        },
        diag(a) {
            var r, i, n;
            n = a.length;
            r = Matrix(n, n);
            for(i = 0; i < n; i++)
                r[i][i] = a[i];
            return r;
        },
        hilbert(n) {
            var i, j, r;
            r = Matrix(n);
            for(i = 0; i < n; i++) {
                for(j = 0; j < n; j++) {
                    r[i][j] = 1 / (1 + i + j);
                }
            }
            return r;
        },
        trans(a) {
            var h, w, r, i, j;
            if (!Array.isArray(a))
                throw TypeError("matrix expected");
            h = a.length;
            if (!Array.isArray(a[0])) {
                w = 1;
                r = Matrix(w, h);
                for(i = 0; i < h; i++) {
                    r[0][i] = a[i];
                }
            } else {
                w = a[0].length;
                r = Matrix(w, h);
                for(i = 0; i < h; i++) {
                    for(j = 0; j < w; j++) {
                        r[j][i] = a[i][j];
                    }
                }
            }
            return r;
        },
        check_square(a) {
            var a, n;
            if (!Array.isArray(a))
                throw TypeError("array expected");
            n = a.length;
            if (!Array.isArray(a[0]) || n != a[0].length)
                throw TypeError("square matrix expected");
            return n;
        },
        trace(a) {
            var n, r, i;
            n = Matrix.check_square(a);
            r = a[0][0];
            for(i = 1; i < n; i++) {
                r += a[i][i];
            }
            return r;
        },
        charpoly(a) {
            var n, p, c, i, j, coef;
            n = Matrix.check_square(a);
            p = [];
            for(i = 0; i < n + 1; i++)
                p[i] = 0;
            p[n] = 1;
            c = Matrix.idn(n);
            for(i = 0; i < n; i++) {
                c = c * a;
                coef = -trace(c) / (i + 1);
                p[n - i - 1] = coef;
                for(j = 0; j < n; j++)
                    c[j][j] += coef;
            }
            return Polynomial(p);
        },
        eigenvals(a) {
            return Polynomial.roots(Matrix.charpoly(a));
        },
        det(a) {
            var n, i, j, k, s, src, v, c;

            n = Matrix.check_square(a);
            s = 1;
            src = a.dup();
            for(i=0;i<n;i++) {
                for(j = i; j < n; j++) {
                    if (src[j][i] != 0)
                        break;
                }
                if (j == n)
                    return 0;
                if (j != i) {
                    for(k = 0;k < n; k++) {
                        v = src[j][k];
                        src[j][k] = src[i][k];
                        src[i][k] = v;
                    }
                    s = -s;
                }
                c = src[i][i].inverse();
                for(j = i + 1; j < n; j++) {
                    v = c * src[j][i];
                    for(k = 0;k < n; k++) {
                        src[j][k] -= src[i][k] * v;
                    }
                }
            }
            c = s;
            for(i=0;i<n;i++)
                c *= src[i][i];
            return c;
        },
        inverse(a) {
            var n, dst, src, i, j, k, n2, r, c, v;
            n = Matrix.check_square(a);
            src = a.dup();
            dst = Matrix.idn(n);
            for(i=0;i<n;i++) {
                for(j = i; j < n; j++) {
                    if (src[j][i] != 0)
                        break;
                }
                if (j == n)
                    throw RangeError("matrix is not invertible");
                if (j != i) {
                    /* swap lines in src and dst */
                    v = src[j];
                    src[j] = src[i];
                    src[i] = v;
                    v = dst[j];
                    dst[j] = dst[i];
                    dst[i] = v;
                }

                c = src[i][i].inverse();
                for(k = 0; k < n; k++) {
                    src[i][k] *= c;
                    dst[i][k] *= c;
                }

                for(j = 0; j < n; j++) {
                    if (j != i) {
                        c = src[j][i];
                        for(k = i; k < n; k++) {
                            src[j][k] -= src[i][k] * c;
                        }
                        for(k = 0; k < n; k++) {
                            dst[j][k] -= dst[i][k] * c;
                        }
                    }
                }
            }
            return dst;
        },
        rank(a) {
            var src, i, j, k, w, h, l, c;

            if (!Array.isArray(a) ||
                !Array.isArray(a[0]))
                throw TypeError("matrix expected");
            h = a.length;
            w = a[0].length;
            src = a.dup();
            l = 0;
            for(i=0;i<w;i++) {
                for(j = l; j < h; j++) {
                    if (src[j][i] != 0)
                        break;
                }
                if (j == h)
                    continue;
                if (j != l) {
                    /* swap lines */
                    for(k = 0; k < w; k++) {
                        v = src[j][k];
                        src[j][k] = src[l][k];
                        src[l][k] = v;
                    }
                }

                c = src[l][i].inverse();
                for(k = 0; k < w; k++) {
                    src[l][k] *= c;
                }

                for(j = l + 1; j < h; j++) {
                    c = src[j][i];
                    for(k = i; k < w; k++) {
                        src[j][k] -= src[l][k] * c;
                    }
                }
                l++;
            }
            return l;
        },
        ker(a) {
            var src, i, j, k, w, h, l, m, r, im_cols, ker_dim, c;

            if (!Array.isArray(a) ||
                !Array.isArray(a[0]))
                throw TypeError("matrix expected");
            h = a.length;
            w = a[0].length;
            src = a.dup();
            im_cols = [];
            l = 0;
            for(i=0;i<w;i++) {
                im_cols[i] = false;
                for(j = l; j < h; j++) {
                    if (src[j][i] != 0)
                        break;
                }
                if (j == h)
                    continue;
                im_cols[i] = true;
                if (j != l) {
                    /* swap lines */
                    for(k = 0; k < w; k++) {
                        v = src[j][k];
                        src[j][k] = src[l][k];
                        src[l][k] = v;
                    }
                }

                c = src[l][i].inverse();
                for(k = 0; k < w; k++) {
                    src[l][k] *= c;
                }

                for(j = 0; j < h; j++) {
                    if (j != l) {
                        c = src[j][i];
                        for(k = i; k < w; k++) {
                            src[j][k] -= src[l][k] * c;
                        }
                    }
                }
                l++;
                //        log_str("m=" + cval_toString(v1) + "\n");
            }
            //    log_str("im cols="+im_cols+"\n");

            /* build the kernel vectors */
            ker_dim = w - l;
            r = Matrix(w, ker_dim);
            k = 0;
            for(i = 0; i < w; i++) {
                if (!im_cols[i]) {
                    /* select this column from the matrix */
                    l = 0;
                    m = 0;
                    for(j = 0; j < w; j++) {
                        if (im_cols[j]) {
                            r[j][k] = -src[m][i];
                            m++;
                        } else {
                            if (l == k) {
                                r[j][k] = 1;
                            } else {
                                r[j][k] = 0;
                            }
                            l++;
                        }
                    }
                    k++;
                }
            }
            return r;
        },
        dp(a, b) {
            var i, n, r;
            n = a.length;
            if (n != b.length)
                throw TypeError("incompatible array length");
            /* XXX: could do complex product */
            r = 0;
            for(i = 0; i < n; i++) {
                r += a[i] * b[i];
            }
            return r;
        },
        /* cross product */
        cp(v1, v2) {
            var r;
            if (v1.length != 3 || v2.length != 3)
                throw TypeError("vectors must have 3 elements");
            r = [];
            r[0] = v1[1] * v2[2] - v1[2] * v2[1];
            r[1] = v1[2] * v2[0] - v1[0] * v2[2];
            r[2] = v1[0] * v2[1] - v1[1] * v2[0];
            return r;
        },
    });

    function array_add(a, b) {
        var r, i, n;
        n = a.length;
        if (n != b.length)
            throw TypeError("incompatible array size");
        r = [];
        for(i = 0; i < n; i++)
            r[i] = a[i] + b[i];
        return r;
    }
    function array_sub(a, b) {
        var r, i, n;
        n = a.length;
        if (n != b.length)
            throw TypeError("incompatible array size");
        r = [];
        for(i = 0; i < n; i++)
            r[i] = a[i] - b[i];
        return r;
    }
    function array_scalar_mul(a, b) {
        var r, i, n;
        n = a.length;
        r = [];
        for(i = 0; i < n; i++)
            r[i] = a[i] * b;
        return r;
    }
    function array_mul(a, b) {
        var h, w, l, i, j, k, r, rl, sum, a_mat, b_mat;
        h = a.length;
        a_mat = Array.isArray(a[0]);
        if (a_mat) {
            l = a[0].length;
        } else {
            l = 1;
        }
        if (l != b.length)
            throw RangeError("incompatible matrix size");
        b_mat = Array.isArray(b[0]);
        if (b_mat)
            w = b[0].length;
        else
            w = 1;
        r = [];
        if (a_mat && b_mat) {
            for(i = 0; i < h; i++) {
                rl = [];
                for(j = 0; j < w; j++) {
                    sum = 0;
                    for(k = 0; k < l; k++) {
                        sum += a[i][k] * b[k][j];
                    }
                    rl[j] = sum;
                }
                r[i] = rl;
            }
        } else if (a_mat && !b_mat) {
            for(i = 0; i < h; i++) {
                sum = 0;
                for(k = 0; k < l; k++) {
                    sum += a[i][k] * b[k];
                }
                r[i] = sum;
            }
        } else if (!a_mat && b_mat) {
            for(i = 0; i < h; i++) {
                rl = [];
                for(j = 0; j < w; j++) {
                    rl[j] = a[i] * b[0][j];
                }
                r[i] = rl;
            }
        } else {
            for(i = 0; i < h; i++) {
                r[i] = a[i] * b[0];
            }
        }
        return r;
    }
    function array_div(a, b) {
        return array_mul(a, b.inverse());
    }
    function array_element_wise_inverse(a) {
        var r, i, n;
        n = a.length;
        r = [];
        for(i = 0; i < n; i++)
            r[i] = a[i].inverse();
        return r;
    }
    function array_eq(a, b) {
        var n, i;
        n = a.length;
        if (n != b.length)
            return false;
        for(i = 0; i < n; i++) {
            if (a[i] != b[i])
                return false;
        }
        return true;
    }

    operators_set(Array.prototype,
        {
            "+": array_add,
            "-": array_sub,
            "*": array_mul,
            "/": array_div,
            "==": array_eq,
            "pos"(a) {
                return a;
            },
            "neg"(a) {
                var i, n, r;
                n = a.length;
                r = [];
                for(i = 0; i < n; i++)
                    r[i] = -a[i];
                return r;
            }
        },
        {
            right: [Number, BigInt, Float, Fraction, Complex, Mod,
                    Polynomial, PolyMod, RationalFunction, Series],
            "*": array_scalar_mul,
            "/"(a, b) { return a * b.inverse(); },
            "**": generic_pow, /* XXX: only for integer */
        },
        {
            left: [Number, BigInt, Float, Fraction, Complex, Mod,
                   Polynomial, PolyMod, RationalFunction, Series],
            "*"(a, b) { return array_scalar_mul(b, a); },
            "/"(a, b) { return a * array_element_wise_inverse(b); },
        });

    add_props(Array.prototype, {
        conj() {
            var i, n, r;
            n = this.length;
            r = [];
            for(i = 0; i < n; i++)
                r[i] = this[i].conj();
            return r;
        },
        dup() {
            var r, i, n, el, a = this;
            r = [];
            n = a.length;
            for(i = 0; i < n; i++) {
                el = a[i];
                if (Array.isArray(el))
                    el = el.dup();
                r[i] = el;
            }
            return r;
        },
        inverse() {
            return Matrix.inverse(this);
        },
        norm2: Polynomial.prototype.norm2,
    });

})(this);

/* global definitions */
var I = Complex(0, 1);
var X = Polynomial([0, 1]);
var O = Series.O;

Object.defineProperty(this, "PI", { get: function () { return Float.PI } });

/* put frequently used functions in the global context */
var gcd = Integer.gcd;
var fact = Integer.fact;
var comb = Integer.comb;
var pmod = Integer.pmod;
var invmod = Integer.invmod;
var factor = Integer.factor;
var isprime = Integer.isPrime;
var nextprime = Integer.nextPrime;

function deriv(a)
{
    return a.deriv();
}

function integ(a)
{
    return a.integ();
}

function norm2(a)
{
    return a.norm2();
}

function abs(a)
{
    return a.abs();
}

function conj(a)
{
    return a.conj();
}

function arg(a)
{
    return a.arg();
}

function inverse(a)
{
    return a.inverse();
}

function trunc(a)
{
    if (Integer.isInteger(a)) {
        return a;
    } else if (a instanceof Fraction) {
        return Integer.tdiv(a.num, a.den);
    } else if (a instanceof Polynomial) {
        return a;
    } else if (a instanceof RationalFunction) {
        return Polynomial.divrem(a.num, a.den)[0];
    } else {
        return Float.ceil(a);
    }
}

function floor(a)
{
    if (Integer.isInteger(a)) {
        return a;
    } else if (a instanceof Fraction) {
        return Integer.fdiv(a.num, a.den);
    } else {
        return Float.floor(a);
    }
}

function ceil(a)
{
    if (Integer.isInteger(a)) {
        return a;
    } else if (a instanceof Fraction) {
        return Integer.cdiv(a.num, a.den);
    } else {
        return Float.ceil(a);
    }
}

function sqrt(a)
{
    var t, u, re, im;
    if (a instanceof Series) {
        return a ^ (1/2);
    } else if (a instanceof Complex) {
        t = abs(a);
        u = a.re;
        re = sqrt((t + u) / 2);
        im = sqrt((t - u) / 2);
        if (a.im < 0)
            im = -im;
        return Complex.toComplex(re, im);
    } else {
        a = Float(a);
        if (a < 0) {
            return Complex(0, Float.sqrt(-a));
        } else {
            return Float.sqrt(a);
        }
    }
}

function exp(a)
{
    return a.exp();
}

function log(a)
{
    return a.log();
}

function log2(a)
{
    return log(a) * Float.LOG2E;
}

function log10(a)
{
    return log(a) * Float.LOG10E;
}

function todb(a)
{
    return log10(a) * 10;
}

function fromdb(a)
{
    return 10 ^ (a / 10);
}

function sin(a)
{
    var t;
    if (a instanceof Complex || a instanceof Series) {
        t = exp(a * I);
        return (t - 1/t) / (2 * I);
    } else {
        return Float.sin(Float(a));
    }
}

function cos(a)
{
    var t;
    if (a instanceof Complex || a instanceof Series) {
        t = exp(a * I);
        return (t + 1/t) / 2;
    } else {
        return Float.cos(Float(a));
    }
}

function tan(a)
{
    if (a instanceof Complex || a instanceof Series) {
        return sin(a) / cos(a);
    } else {
        return Float.tan(Float(a));
    }
}

function asin(a)
{
    return Float.asin(Float(a));
}

function acos(a)
{
    return Float.acos(Float(a));
}

function atan(a)
{
    return Float.atan(Float(a));
}

function atan2(a, b)
{
    return Float.atan2(Float(a), Float(b));
}

function sinc(a)
{
    if (a == 0) {
        return 1;
    } else {
        a *= Float.PI;
        return sin(a) / a;
    }
}

function todeg(a)
{
    return a * 180 / Float.PI;
}

function fromdeg(a)
{
    return a * Float.PI / 180;
}

function sinh(a)
{
    var e = Float.exp(Float(a));
    return (e - 1/e) * 0.5;
}

function cosh(a)
{
    var e = Float.exp(Float(a));
    return (e + 1/e) * 0.5;
}

function tanh(a)
{
    var e = Float.exp(Float(a) * 2);
    return (e - 1) / (e + 1);
}

function asinh(a)
{
    var x = Float(a);
    return log(sqrt(x * x + 1) + x);
}

function acosh(a)
{
    var x = Float(a);
    return log(sqrt(x * x - 1) + x);
}

function atanh(a)
{
    var x = Float(a);
    return 0.5 * log((1 + x) / (1 - x));
}

function sigmoid(x)
{
    x = Float(x);
    return 1 / (1 + exp(-x));
}

function lerp(a, b, t)
{
    return a + (b - a) * t;
}

var idn = Matrix.idn;
var diag = Matrix.diag;
var trans = Matrix.trans;
var trace = Matrix.trace;
var charpoly = Matrix.charpoly;
var eigenvals = Matrix.eigenvals;
var det = Matrix.det;
var rank = Matrix.rank;
var ker = Matrix.ker;
var cp = Matrix.cp;
var dp = Matrix.dp;

var polroots = Polynomial.roots;
var bestappr = Float.bestappr;
