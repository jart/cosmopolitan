/*
 * PI computation in Javascript using the QuickJS bigdecimal type
 * (decimal floating point)
 */
"use strict";

/* compute PI with a precision of 'prec' digits */
function calc_pi(prec) {
    const CHUD_A = 13591409m;
    const CHUD_B = 545140134m;
    const CHUD_C = 640320m;
    const CHUD_C3 = 10939058860032000m; /* C^3/24 */
    const CHUD_DIGITS_PER_TERM = 14.18164746272548; /* log10(C/12)*3 */
    
    /* return [P, Q, G] */
    function chud_bs(a, b, need_G) {
        var c, P, Q, G, P1, Q1, G1, P2, Q2, G2, b1;
        if (a == (b - 1n)) {
            b1 = BigDecimal(b);
            G = (2m * b1 - 1m) * (6m * b1 - 1m) * (6m * b1 - 5m);
            P = G * (CHUD_B * b1 + CHUD_A);
            if (b & 1n)
                P = -P;
            G = G;
            Q = b1 * b1 * b1 * CHUD_C3;
        } else {
            c = (a + b) >> 1n;
            [P1, Q1, G1] = chud_bs(a, c, true);
            [P2, Q2, G2] = chud_bs(c, b, need_G);
            P = P1 * Q2 + P2 * G1;
            Q = Q1 * Q2;
            if (need_G)
                G = G1 * G2;
            else
                G = 0m;
        }
        return [P, Q, G];
    }

    var n, P, Q, G;
    /* number of serie terms */
    n = BigInt(Math.ceil(prec / CHUD_DIGITS_PER_TERM)) + 10n;
    [P, Q, G] = chud_bs(0n, n, false);
    Q = BigDecimal.div(Q, (P + Q * CHUD_A),
                       { roundingMode: "half-even",
                         maximumSignificantDigits: prec });
    G = (CHUD_C / 12m) * BigDecimal.sqrt(CHUD_C,
                                         { roundingMode: "half-even",
                                           maximumSignificantDigits: prec });
    return Q * G;
}

(function() {
    var r, n_digits, n_bits;
    if (typeof scriptArgs != "undefined") {
        if (scriptArgs.length < 2) {
            print("usage: pi n_digits");
            return;
        }
        n_digits = scriptArgs[1] | 0;
    } else {
        n_digits = 1000;
    }
    /* we add more digits to reduce the probability of bad rounding for
       the last digits */
    r = calc_pi(n_digits + 20);
    print(r.toFixed(n_digits, "down"));
})();
