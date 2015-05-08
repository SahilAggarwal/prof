cat ${1+"$@"} |
	sed -n 's/^#[ 	]*define[ 	][ 	]*SYS_\([^ 	]*\)[ 	]*[^0-9]*\([0-9]*\).*$/\1 \2/p
s/^#[ 	]*define[ 	][ 	]*__NR_\([^ 	]*\)[ 	]*[^0-9]*\([0-9]*\).*$/\1 \2/p
s/^#[ ]*define[ ][ ]*__NR_\([^ ]*\)[ ]*[^0-9()]*(__NR_Linux + \([0-9]*\))$/\1 \2/p' |
	sort -k2n | uniq |
	awk '
		BEGIN {
		h = "#ifndef _H_SYSCALLENT\n#define _H_SYSCALLENT\nchar *syscalls[] = { "
		print h 		
		}
		
		{
		s = "\"" $1 "\","
		print s
		}

		END {
		f = " };\n#endif"
		print f
		}

	'

