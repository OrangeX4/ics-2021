lines_count_pa1=`find ./nemu | grep '\.c$\|\.h$' | xargs wc -l | grep 'total' | awk '{print substr($1, 1)}'`
git checkout pa0 >/dev/null 2>/dev/null
lines_count_pa0=`find ./nemu | grep '\.c$\|\.h$' | xargs wc -l | grep 'total' | awk '{print substr($1, 1)}'`
git checkout pa1 >/dev/null 2>/dev/null
expr="$lines_count_pa1 - $lines_count_pa0"
echo New lines: `expr $expr`