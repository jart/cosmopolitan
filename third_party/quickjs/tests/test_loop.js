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

// load more elaborate version of assert if available
try { __loadScript("test_assert.js"); } catch(e) {}

/*----------------*/

function test_while()
{
    var i, c;
    i = 0;
    c = 0;
    while (i < 3) {
        c++;
        i++;
    }
    assert(c === 3);
}

function test_while_break()
{
    var i, c;
    i = 0;
    c = 0;
    while (i < 3) {
        c++;
        if (i == 1)
            break;
        i++;
    }
    assert(c === 2 && i === 1);
}

function test_do_while()
{
    var i, c;
    i = 0;
    c = 0;
    do {
        c++;
        i++;
    } while (i < 3);
    assert(c === 3 && i === 3);
}

function test_for()
{
    var i, c;
    c = 0;
    for(i = 0; i < 3; i++) {
        c++;
    }
    assert(c === 3 && i === 3);

    c = 0;
    for(var j = 0; j < 3; j++) {
        c++;
    }
    assert(c === 3 && j === 3);
}

function test_for_in()
{
    var i, tab, a, b;

    tab = [];
    for(i in {x:1, y: 2}) {
        tab.push(i);
    }
    assert(tab.toString(), "x,y", "for_in");

    /* prototype chain test */
    a = {x:2, y: 2, "1": 3};
    b = {"4" : 3 };
    Object.setPrototypeOf(a, b);
    tab = [];
    for(i in a) {
        tab.push(i);
    }
    assert(tab.toString(), "1,x,y,4", "for_in");

    /* non enumerable properties hide enumerables ones in the
       prototype chain */
    a = {y: 2, "1": 3};
    Object.defineProperty(a, "x", { value: 1 });
    b = {"x" : 3 };
    Object.setPrototypeOf(a, b);
    tab = [];
    for(i in a) {
        tab.push(i);
    }
    assert(tab.toString(), "1,y", "for_in");

    /* array optimization */
    a = [];
    for(i = 0; i < 10; i++)
        a.push(i);
    tab = [];
    for(i in a) {
        tab.push(i);
    }
    assert(tab.toString(), "0,1,2,3,4,5,6,7,8,9", "for_in");

    /* iterate with a field */
    a={x:0};
    tab = [];
    for(a.x in {x:1, y: 2}) {
        tab.push(a.x);
    }
    assert(tab.toString(), "x,y", "for_in");

    /* iterate with a variable field */
    a=[0];
    tab = [];
    for(a[0] in {x:1, y: 2}) {
        tab.push(a[0]);
    }
    assert(tab.toString(), "x,y", "for_in");

    /* variable definition in the for in */
    tab = [];
    for(var j in {x:1, y: 2}) {
        tab.push(j);
    }
    assert(tab.toString(), "x,y", "for_in");

    /* variable assigment in the for in */
    tab = [];
    for(var k = 2 in {x:1, y: 2}) {
        tab.push(k);
    }
    assert(tab.toString(), "x,y", "for_in");
}

function test_for_in2()
{
    var i;
    tab = [];
    for(i in {x:1, y: 2, z:3}) {
        if (i === "y")
            continue;
        tab.push(i);
    }
    assert(tab.toString() == "x,z");

    tab = [];
    for(i in {x:1, y: 2, z:3}) {
        if (i === "z")
            break;
        tab.push(i);
    }
    assert(tab.toString() == "x,y");
}

function test_for_break()
{
    var i, c;
    c = 0;
    L1: for(i = 0; i < 3; i++) {
        c++;
        if (i == 0)
            continue;
        while (1) {
            break L1;
        }
    }
    assert(c === 2 && i === 1);
}

function test_switch1()
{
    var i, a, s;
    s = "";
    for(i = 0; i < 3; i++) {
        a = "?";
        switch(i) {
        case 0:
            a = "a";
            break;
        case 1:
            a = "b";
            break;
        default:
            a = "c";
            break;
        }
        s += a;
    }
    assert(s === "abc" && i === 3);
}

function test_switch2()
{
    var i, a, s;
    s = "";
    for(i = 0; i < 4; i++) {
        a = "?";
        switch(i) {
        case 0:
            a = "a";
            break;
        case 1:
            a = "b";
            break;
        case 2:
            continue;
        default:
            a = "" + i;
            break;
        }
        s += a;
    }
    assert(s === "ab3" && i === 4);
}

function test_try_catch1()
{
    try {
        throw "hello";
    } catch (e) {
        assert(e, "hello", "catch");
        return;
    }
    assert(false, "catch");
}

function test_try_catch2()
{
    var a;
    try {
        a = 1;
    } catch (e) {
        a = 2;
    }
    assert(a, 1, "catch");
}

function test_try_catch3()
{
    var s;
    s = "";
    try {
        s += "t";
    } catch (e) {
        s += "c";
    } finally {
        s += "f";
    }
    assert(s, "tf", "catch");
}

function test_try_catch4()
{
    var s;
    s = "";
    try {
        s += "t";
        throw "c";
    } catch (e) {
        s += e;
    } finally {
        s += "f";
    }
    assert(s, "tcf", "catch");
}

function test_try_catch5()
{
    var s;
    s = "";
    for(;;) {
        try {
            s += "t";
            break;
            s += "b";
        } finally {
            s += "f";
        }
    }
    assert(s, "tf", "catch");
}

function test_try_catch6()
{
    function f() {
        try {
            s += 't';
            return 1;
        } finally {
            s += "f";
        }
    }
    var s = "";
    assert(f() === 1);
    assert(s, "tf", "catch6");
}

function test_try_catch7()
{
    var s;
    s = "";

    try {
        try {
            s += "t";
            throw "a";
        } finally {
            s += "f";
        }
    } catch(e) {
        s += e;
    } finally {
        s += "g";
    }
    assert(s, "tfag", "catch");
}

function test_try_catch8()
{
    var i, s;
    
    s = "";
    for(var i in {x:1, y:2}) {
        try {
            s += i;
            throw "a";
        } catch (e) {
            s += e;
        } finally {
            s += "f";
        }
    }
    assert(s === "xafyaf");
}

test_while();
test_while_break();
test_do_while();
test_for();
test_for_break();
test_switch1();
test_switch2();
test_for_in();
test_for_in2();

test_try_catch1();
test_try_catch2();
test_try_catch3();
test_try_catch4();
test_try_catch5();
test_try_catch6();
test_try_catch7();
test_try_catch8();
