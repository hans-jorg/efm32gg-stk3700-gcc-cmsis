##
# @brief Master Makefile
#

.SILENT=
SUBPROJECTS=$(shell ls -d [0-9][0-9]-* )
MAKEFLAGS += --no-print-directory

# Build all
all:
	@for f in $(SUBPROJECTS); \
	do \
	    echo Building $$f ; \
	   ( cd $$f ; \
	    make $(MAKEFLAGS) ) ; \
	done
# Clean all generated files
clean:
	@for f in $(SUBPROJECTS); \
	do \
	    echo Cleaning $$f ; \
	   (  cd $$f ; \
	    make $(MAKEFLAGS) clean ) ; \
	done
# Generate documentation
docs:
	@for f in $(SUBPROJECTS); \
	do \
	    echo Generationg docs for $$f ; \
	   ( cd $$f ; \
	    make $(MAKEFLAGS) docs ) ; \
	done

# Clean the generated documentation
docs-clean:
	@for f in $(SUBPROJECTS); \
	do \
	    echo Cleaning docs in $$f ; \
	   ( cd $$f ; \
	    make $(MAKEFLAGS) docs-clean ) ; \
	done
# Concatenate all READMEs
docs-all:
	cat [0-3][0-9]-*/README.md > README-ALL.md
