buildtests:
	cmake -S . -B ./build/tests && cd ./build/tests && make

runtests:
	./build/tests/src/arch/local_tests/tests

espidfbuild:
	-rm -r build/espidf
	-mkdir -p build/espidf
	cp -r ./src/arch/espidf/ build/
	cp -rn ./src/common/* build/espidf/components/loadCellAmp/

espidftest: espidfbuild
	-rm -r ${IDF_PATH}/components/loadCellAmp
	cp -r ./build/espidf/components/loadCellAmp ${IDF_PATH}/components/loadCellAmp
	cd ${IDF_PATH}/tools/unit-test-app && idf.py -T loadCellAmp reconfigure build flash monitor

espidfclean:
	cd ${IDF_PATH}/tools/unit-test-app && idf.py clean

.PHONY = espidfbuild buildtests runtests espidfflashmonitor espidftest
