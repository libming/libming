all: $(TESTS_PROGS)

#
# Warning!!!!
# This is a cheat. Reference results should never be generated using the
# same code that it is testing. Due to the large number of tests involved,
# and the likelyhood of listswf output changing as things get fixed, we
# need a reasonable way to avoid having to hand edit a huge number of
# reference files. The compromise, is that all new reference files, and
# all changes to existing files must be carefully inspected by hand before
# committing them.
#
refs: $(C_PROGS)
	for t in $(C_PROGS); do ./$$t;listswf $$t.swf >$$t.ref; done

gen: refs $(SWFS)
	for t in $(C_PROGS); do \
	swftophp $$t.swf $$t.swf >$$t.php ;\
	swftoperl $$t.swf $$t.swf >$$t.pl ;\
	swftopython $$t.swf $$t.swf >$$t.py ;\
	swftocxx $$t.swf $$t.swf >$$t-cxx.C ;\
	done
