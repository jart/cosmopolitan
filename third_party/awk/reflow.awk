# fmt - format
#    input:  text
#    output: text formatted into lines of <= 72 characters

BEGIN {
        maxlen = 72
}

/^[ \t]/ { printline(); print; next }      # verbatim
###/^ +/ { printline();  }			# whitespace == break

/./  { for (i = 1; i <= NF; i++) addword($i); next }

/^$/ { printline(); print "" }
END  { printline() }

function addword(w) {
    ## print "adding [", w, "] ", length(w), length(line), maxlen
    if (length(line) + length(w) > maxlen)
        printline()
    if (length(w) > 2 && ( w ~ /[\.!]["?)]?$/ || w ~ /[?!]"?$/) &&
		w !~ /^(Mr|Dr|Ms|Mrs|vs|Ph.D)\.$/)
        w = w " "
    line = line " " w
}

function printline() {
    if (length(line) > 0) {
        sub(/ +$/, "", line)
        print substr(line, 2)   # removes leading blank
        line = ""
    }
}
