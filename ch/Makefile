PACKAGE = chming

create:
	ch ./pkgcreate.ch
install:
	ch pkginstall.ch $(PACKAGE)
uninstall:
	ch pkginstall.ch -u $(PACKAGE)
rmpkg:
	rm -rf $(PACKAGE)
clean:
	rm -f  c/*.c
	rm -f  chfcreate/*.h
	cd c;  make clean
