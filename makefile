all:
	source ~/emsdk-portable/emsdk_env.sh;\
	cd dataprocessing/;\
	emcc dataprocessing.cpp -Oz -s WASM=1 -s EXPORTED_FUNCTIONS="['_fn_test']"
	cp dataprocessing/a.out.js web/bin/dataprocessing.js
	sed -i -e 's/a.out./bin\/dataprocessing./g' web/bin/dataprocessing.js
	cp dataprocessing/a.out.wasm web/bin/dataprocessing.wasm
