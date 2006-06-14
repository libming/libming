all: $(TESTS)

refs: $(TESTS)
	for t in $(TESTS); do ./$$t;listswf $$t.swf >$$t.ref; done

gen: refs $(SWFS)
	for t in $(TESTS); do \
	swftophp $$t.swf $$t.swf >$$t.php ;\
	swftoperl $$t.swf $$t.swf >$$t.pl ;\
	swftopython $$t.swf $$t.swf >$$t.py ;\
	done


clean:
	$(RM) $(SWFS) $(TESTS)
