#
# File di compilazione globale per TuoProgetto
#
PERCORSO = /hosthome/Documenti/netkit/PaCuFileSystem

DIRS =  $(PERCORSO)/Client1 \
        $(PERCORSO)/Client2 \
        $(PERCORSO)/Client3 \
        $(PERCORSO)/Client4 \
        $(PERCORSO)/ServerNARUTO \
        $(PERCORSO)/ServerMR2 \
        $(PERCORSO)/ServerRUKIA \
        $(PERCORSO)/ServerZORO \
        $(PERCORSO)/ServerBACK \
        $(PERCORSO)/ServerACE \
        $(PERCORSO)/BootServer

MAKE = make

all:
	@echo "Generazione programma per TuoProgetto"
	@for i in $(DIRS) ; do \
		( echo "cd $$i" ; cd $$i ; $(MAKE) ) ; \
	done

clean:
	@echo "Cleaning object"
	@for i in $(DIRS) ; do \
		( echo "cd $$i" ; cd $$i ; $(MAKE) clean ) ; \
	done

clobber:
	@echo "Clobbering executables and libraries"
	@for i in $(DIRS) ; do \
		( echo "cd $$i" ; cd $$i ; $(MAKE) clobber ) ; \
	done
