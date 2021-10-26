$! VMS_EXPORT_SYMBOL_TEST.COM
$!
$! Verify the VMS_EXPORT_SYMBOL.C module
$!
$! 22-May-2014   J. Malmberg
$!
$!=========================================================================
$!
$ cc/names=(as_is)/define=(DEBUG=1,_POSIX_EXIT=1) vms_export_symbol.c
$!
$ link vms_export_symbol
$!
$ delete vms_export_symbol.obj;*
$!
$! Need a foreign command to test.
$ vms_export_symbol := $sys$disk:[]vms_export_symbol.exe
$ save_export_symbol = vms_export_symbol
$!
$ vms_export_symbol
$ if $severity .ne. 1
$ then
$    write sys$output "Test program failed!";
$ endif
$!
$ if vms_export_symbol .nes. save_export_symbol
$ then
$   write sys$output "Test failed to restore foreign command!"
$ endif
$ if f$type(test_export_symbol) .nes. ""
$ then
$   write sys$output "Test failed to clear exported symbol!"
$ endif
$ if f$type(test_putenv_symbol) .nes. ""
$ then
$   write sys$output "Test failed to clear putenv exported symbol!"
$ endif
$!
