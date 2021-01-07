buildtests:
	cmake -S . -B ./build/tests && cd ./build/tests && make

runtests:
	./build/tests/src/arch/local_tests/tests

buildespidf:
	-mkdir build/
	cp -r ./src/arch/espidf/ build/
	cp -rn ./src/common/* build/espidf/components/loadCellAmp/
	-rm -r ../test_load_cell_amp_espidf/hello_world/components
	cp -rn ./build/espidf/components ../test_load_cell_amp_espidf/hello_world/components


