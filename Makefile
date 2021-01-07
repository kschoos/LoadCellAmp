SHELL := /bin/bash

buildtests:
	cmake -S . -B ./build/tests && cd ./build/tests && make

runtests:
	./build/tests/src/arch/local_tests/tests

setupespidftestenv:
	@if [ -z "${IDF_PATH}" ] ; then\
		echo "Please set IDF_PATH variable before continuing. You may not have esp-idf installed correctly or just forgot to source exports.sh";\
	else \
		mkdir build/espidftest; \
		cd build/espidftest; \
		cp -r ${IDF_PATH}/examples/get-started/hello_world .; \
	fi

buildespidf:
	-mkdir build/
	cp -r ./src/arch/espidf/ build/
	cp -rn ./src/common/* build/espidf/components/loadCellAmp/
	-rm -r ../test_load_cell_amp_espidf/hello_world/components
	cp -rn ./build/espidf/components ../test_load_cell_amp_espidf/hello_world/components
	cd ../test_load_cell_amp_espidf/hello_world/ && idf.py reconfigure build

flashmonitorespidf: buildespidf
	cd ../test_load_cell_amp_espidf/hello_world && idf.py flash monitor


.PHONY: buildespidf flashmonitorespidf runtests buildtests

