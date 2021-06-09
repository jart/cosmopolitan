/* PIPZIP REXX     Rexx filter to use ZIP                             */
/*                 Author : George Petrov, 8 May 1995                 */

parse arg opts
'callpipe *:',
   '| specs w1 1 /./ n w2 n',
   '| join * / /',
   '| specs /zip 'opts'/ 1 1-* nw',
   '| cms',
   '| *:'

exit rc















