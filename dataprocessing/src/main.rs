#![feature(link_args)]
#[link_args = "-s EXPORTED_FUNCTIONS=['_add_to_queue']"]

use std::os::raw::c_char;


extern {
    pub fn emscripten_asm_const(s: *const c_char);
}

#[no_mangle]
pub fn add_to_queue(wsin:&str) -> u32 {
	println!("Hello World");
	let code : &'static [u8] = b"console.log('Hello Rust!');\0";
	unsafe {
		emscripten_asm_const(&code[0] as *const c_char);
	}
	42
}

fn main() {
}
