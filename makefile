all:
	cd dataprocessing/;\
	cargo build --release --target=asmjs-unknown-emscripten
	cp dataprocessing/target/asmjs-unknown-emscripten/release/dataprocessing.js web/bin/dataprocessing.js
