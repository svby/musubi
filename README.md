# musubi

Graphics wrapper in modern C++. Written from scratch for maximum usability.

## Building

```sh
mkdir build
cd build
cmake .. && make all install
```

This will install `libmusubi` to /usr/local by default.

## Components

### `libmusubi`

`libmusubi` is the main "engine" component and currently consists of:

 - graphics abstractions (implementations: SDL2)

## License

musubi is licensed under the LGPL.

Licenses for its source dependencies are located in `licenses/`.

### Dependencies

#### Build scripts

 - [`LibFindMacros`](https://github.com/Tronic/cmake-modules) (public domain)
 
#### `libmusubi`

 - [SDL 2](https://www.libsdl.org/) (zlib)
 - [Epoxy](https://github.com/anholt/libepoxy/blob/master/COPYING) (MIT)
