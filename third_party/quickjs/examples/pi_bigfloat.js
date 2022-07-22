/*
 * PI computation in Javascript using the QuickJS bigfloat type
 * (binary floating point)
 */
"use strict";

/* compute PI with a precision of 'prec' bits */
function calc_pi() {
    const CHUD_A = 13591409n;
    const CHUD_B = 545140134n;
    const CHUD_C = 640320n;
    const CHUD_C3 = 10939058860032000n; /* C^3/24 */
    const CHUD_BITS_PER_TERM = 47.11041313821584202247; /* log2(C/12)*3 */
    
    /* return [P, Q, G] */
    function chud_bs(a, b, need_G) {
        var c, P, Q, G, P1, Q1, G1, P2, Q2, G2;
        if (a == (b - 1n)) {
            G = (2n * b - 1n) * (6n * b - 1n) * (6n * b - 5n);
            P = BigFloat(G * (CHUD_B * b + CHUD_A));
            if (b & 1n)
                P = -P;
            G = BigFloat(G);
            Q = BigFloat(b * b * b * CHUD_C3);
        } else {
            c = (a + b) >> 1n;
            [P1, Q1, G1] = chud_bs(a, c, true);
            [P2, Q2, G2] = chud_bs(c, b, need_G);
            P = P1 * Q2 + P2 * G1;
            Q = Q1 * Q2;
            if (need_G)
                G = G1 * G2;
            else
                G = 0l;
        }
        return [P, Q, G];
    }

    var n, P, Q, G;
    /* number of serie terms */
    n = BigInt(Math.ceil(BigFloatEnv.prec / CHUD_BITS_PER_TERM)) + 10n;
    [P, Q, G] = chud_bs(0n, n, false);
    Q = Q / (P + Q * BigFloat(CHUD_A));
    G = BigFloat((CHUD_C / 12n)) * BigFloat.sqrt(BigFloat(CHUD_C));
    return Q * G;
}

(function() {
    var r, n_digits, n_bits;
    if (typeof scriptArgs != "undefined") {
        if (scriptArgs.length < 2) {
            print("usage: pi n_digits");
            return;
        }
        n_digits = scriptArgs[1];
    } else {
        n_digits = 1000;
    }
    n_bits = Math.ceil(n_digits * Math.log2(10));
    /* we add more bits to reduce the probability of bad rounding for
       the last digits */
    BigFloatEnv.setPrec( () => {
        r = calc_pi();
        print(r.toFixed(n_digits, BigFloatEnv.RNDZ));
    }, n_bits + 32);
})();
