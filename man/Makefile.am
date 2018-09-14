man1_MANS = m.1 m-print.1 m-update.1 m-search.1

m-%.1: ../src/m
	help2man -s 1 --manual "m Manual" --name="$(basename $@)" -N --no-discard-stderr -h '--help=$*' $< > $@

m.%: ../src/m
	help2man -s $* -N --no-discard-stderr $< > $@
