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

 - graphics abstractions
   - windowing (SDL2)
   - rendering
     - shapes (OpenGL)

Currently, `libmusubi` will assume and request OpenGL 3.3 or greater by default.
No abstractions for versions older than 3.0 are planned, due to API differences.

Once the default API is stable enough, all OpenGL abstractions will be given an abstract base type
so they will be easy to swap out with other implementations (i.e. other OpenGL versions,
possibly even other rendering APIs).

## License

musubi is licensed under the LGPL (version 3).

Licenses for its source dependencies are located in `licenses/`.

### Dependencies

#### Build scripts

 - [`LibFindMacros`](https://github.com/Tronic/cmake-modules) (public domain)
 
#### `libmusubi`

 - [SDL 2](https://www.libsdl.org/) (zlib)
 - [Epoxy](https://github.com/anholt/libepoxy) (MIT)
 - [GLM](https://github.com/g-truc/glm) (Happy Bunny)
