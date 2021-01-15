buildtests:
	cmake -S . -B ./build/tests && cd ./build/tests && make

runtests:
	./build/tests/src/arch/local_tests/tests

espidfbuild:
	-mkdir build/
	cp -r ./src/arch/espidf/ build/
	cp -rn ./src/common/* build/espidf/components/loadCellAmp/
	# -rm -r ../test_load_cell_amp_espidf/hello_world/components
	# cp -rn ./build/espidf/components ../test_load_cell_amp_espidf/hello_world/components
	# cd ../test_load_cell_amp_espidf/hello_world && idf.py reconfigure build

espidfflashmonitor: espidfbuild
	cd ../test_load_cell_amp_espidf/hello_world && idf.py flash monitor

espidftest: espidfbuild
	-rm -r ${IDF_PATH}/components/loadCellAmp
	cp -r ./build/espidf/components/loadCellAmp ${IDF_PATH}/components/loadCellAmp
	cd ${IDF_PATH}/tools/unit-test-app && idf.py -T loadCellAmp reconfigure build flash monitor


.PHONY = espidfbuild buildtests runtests espidfflashmonitor espidftest
