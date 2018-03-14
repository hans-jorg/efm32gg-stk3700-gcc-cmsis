
.SILENT=
SUBPROJECTS=$(shell ls -d [0-9][0-9]-* )
MAKEFLAGS += --no-print-directory

all:
	@for f in $(SUBPROJECTS); \
	do \
	    echo Building $$f ; \
	   ( cd $$f ; \
	    make $(MAKEFLAGS) ) ; \
	done
clean:
	@for f in $(SUBPROJECTS); \
	do \
	    echo Cleaning $$f ; \
	   (  cd $$f ; \
	    make $(MAKEFLAGS) clean ) ; \
	done
docs:
	@for f in $(SUBPROJECTS); \
	do \
	    echo Generationg docs for $$f ; \
	   ( cd $$f ; \
	    make $(MAKEFLAGS) docs ) ; \
	done

docs-clean:
	@for f in $(SUBPROJECTS); \
	do \
	    echo Cleaning docs in $$f ; \
	   ( cd $$f ; \
	    make $(MAKEFLAGS) docs-clean ) ; \
	done

