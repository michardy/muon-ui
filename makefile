release:
	mkdir release
	mkdir release/bin
	mkdir release/img
	mkdir release/js
	mkdir release/css
	/bin/bash;\
	source ~/emsdk-portable/emsdk_env.sh;\
	cd dataprocessing/;\
	emcc dataprocessing.cpp -Oz -s WASM=1 -s EXPORTED_FUNCTIONS="['_queue_push']"
	cp dataprocessing/a.out.js release/bin/dataprocessing.js
	sed -i -e 's/a.out./bin\/dataprocessing./g' release/bin/dataprocessing.js
	cp dataprocessing/a.out.wasm release/bin/dataprocessing.wasm
	cp dev/index.html release/index.html
	uglifyjs dev/js/main.js -c -m -o release/js/main.js
	uglifycss dev/css/style.css > release/css/style.css
	cd dev/img;\
	for i in *.png; do optipng -o7 --dir ../release/img "$i"; done

update:
	/bin/bash;\
	source ~/emsdk-portable/emsdk_env.sh;\
	cd dataprocessing/;\
	emcc dataprocessing.cpp -Oz -s WASM=1 -s EXPORTED_FUNCTIONS="['_queue_push']"
	cp dataprocessing/a.out.js dev/bin/dataprocessing.js
	sed -i -e 's/a.out./bin\/dataprocessing./g' dev/bin/dataprocessing.js
	cp dataprocessing/a.out.wasm dev/bin/dataprocessing.wasm
