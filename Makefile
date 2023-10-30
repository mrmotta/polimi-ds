.PHONY: all clean

default: all

all:
	@$(MAKE) -j $(nproc) -C src/client/ all
	@$(MAKE) -j $(nproc) -C src/server/ all

clean:
	@$(MAKE) -C src/client/ clean
	@$(MAKE) -C src/server/ clean
	@rm -rf build/obj
