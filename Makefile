build:
	cd $(PWD)/src && make 9cc

test: build
	cd $(PWD)/tests && make test

clean:
	cd $(PWD)/src && make clean
	cd $(PWD)/tests && make clean

remake: clean build
