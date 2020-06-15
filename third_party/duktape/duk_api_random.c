/*
 *  Random numbers
 */

#include "third_party/duktape/duk_internal.h"

DUK_EXTERNAL duk_double_t duk_random(duk_hthread *thr) {
	return (duk_double_t) DUK_UTIL_GET_RANDOM_DOUBLE(thr);
}
