/* The goal of the include/alt-extra directory is to test what happens
 * if certain files come _after_ the normal include directory.
 * Make sure that if the alt-extra directory comes before the normal
 * directory (so we wouldn't be achieving our test objective), the build
 * will fail.
 */
#error "The normal include directory must come first in the include path"
