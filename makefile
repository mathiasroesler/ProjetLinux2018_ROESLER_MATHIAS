export CC = gcc
export CFLAGS =
export LDFLAGS = 
DIR1 = server 
DIR2 = client
EXEC = project

all: $(EXEC)

$(EXEC): 
	@(cd $(DIR1) && $(MAKE))
	@(cd $(DIR2) && $(MAKE))


.PHONY: clean mrproper

clean:
	@(cd $(DIR1) && $(MAKE) $@)
	@(cd $(DIR2) && $(MAKE) $@)

mrproper: clean
	@(cd $(DIR1) && $(MAKE) $@)
	@(cd $(DIR2) && $(MAKE) $@)
