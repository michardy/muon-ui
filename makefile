all:
	source ~/emsdk-portable/emsdk_env.sh;\
	cd dataprocessing/;\
	cargo build --release --target=asmjs-unknown-emscripten
	cp dataprocessing/target/asmjs-unknown-emscripten/release/dataprocessing.js web/bin/dataprocessing.js
