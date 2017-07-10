# muon-ui
In browser data visualization for QNet DAQ cards.  Uses [daq_mirror](https://github.com/michardy/daq_mirror).

## Installing

Clone this repository

``` sh
git clone https://github.com/michardy/muon-ui.git
```

Use your package manger to install:
- gcc
- make
- uglifyjs
- uglifycss
- optipng

Install emscripten:

``` sh
curl -O https://s3.amazonaws.com/mozilla-games/emscripten/releases/emsdk-portable.tar.gz
tar -xzf emsdk-portable.tar.gz
source emsdk_portable/emsdk_env.sh
emsdk update
emsdk install latest
emsdk activate latest
```

## Building
Use `make update` to build the dataprocessing program and copy it to the dev folder.  
Use `make release` to build the dataprocessing program, copy it to the dev folder, and minify and copy the dev folder to release.  

### Warning
This is a site with a makefile.  You have been warned.
