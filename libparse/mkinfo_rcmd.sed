1i\
\
\
cmd_info_t trimble_rcmds[] = {
s!^#define[ \	][ \	]*\(CMD_R[^ \	]*\)[ \	][ \	]*\([^ \	]*\)[ \	][ \	]*/\*[ \	][ \	]*\(.*\)[ \	]*:\([^:]*\):\([^:]*\)[ \	][ \	]*\*/!\	{ \1, "\1", "\3 (\2)", "\4", \5 },!p
$a\
\	{ 0xFF, "", "" }\
};
