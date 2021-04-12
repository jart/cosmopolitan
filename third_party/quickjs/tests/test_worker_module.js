/* Worker code for test_worker.js */
import * as std from "std";
import * as os from "os";

var parent = os.Worker.parent;

function handle_msg(e) {
    var ev = e.data;
    //          print("child_recv", JSON.stringify(ev));
    switch(ev.type) {
    case "abort":
        parent.postMessage({ type: "done" });
        break;
    case "sab":
        /* modify the SharedArrayBuffer */
        ev.buf[2] = 10;
        parent.postMessage({ type: "sab_done", buf: ev.buf });
        break;
    }
}

function worker_main() {
    var i;
    
    parent.onmessage = handle_msg;
    for(i = 0; i < 10; i++) {
        parent.postMessage({ type: "num", num: i }); 
    }
}

worker_main();
