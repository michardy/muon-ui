SHELL := /bin/bash

release:
	mkdir release
	mkdir release/bin
	mkdir release/img
	mkdir release/js
	mkdir release/css
	source ~/emsdk-portable/emsdk_env.sh;\
	cd dataprocessing/;\
	emcc dataprocessing.cpp -Oz -s WASM=1 -s EXPORTED_FUNCTIONS="['_queue_push']";
	cp dataprocessing/a.out.js release/bin/dataprocessing.js
	sed -i -e 's/a.out./bin\/dataprocessing./g' release/bin/dataprocessing.js
	cp dataprocessing/a.out.wasm release/bin/dataprocessing.wasm
	cp dev/index.html release/index.html
	cp -r dev/img release
	uglifyjs dev/js/main.js -c -m -o release/js/main.js
	uglifycss dev/css/style.css > release/css/style.css

update:
	source ~/emsdk-portable/emsdk_env.sh;\
	cd dataprocessing/;\
	emcc dataprocessing.cpp -Oz -s WASM=1 -s EXPORTED_FUNCTIONS="['_queue_push']"
	cp dataprocessing/a.out.js dev/bin/dataprocessing.js
	sed -i -e 's/a.out./bin\/dataprocessing./g' dev/bin/dataprocessing.js
	cp dataprocessing/a.out.wasm dev/bin/dataprocessing.wasm
