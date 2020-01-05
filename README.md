# musubi

Simple application framework in modern C++. Written from scratch for maximum usability.

## Building

```sh
mkdir build
cd build
cmake .. && make all install
```

This will install `libmusubi` to /usr/local by default.

musubi requires C++17 features and support for older standards is not a goal.
It will likely make extensive use of C++20 (concepts, ranges, `std::format`, `std::numbers`, `consteval`, ...)
once the standard is published in the near future.

Currently, the library compiles and the sample runs fine on both Clang and GCC.

**Note:** The CMake scripts for this project assume SDL2 2.0.8 (March 2018) or later is installed.
Earlier versions of SDL [do not configure a `SDL2::SDL2` target][arch-sdlcmake], which musubi tries to import.

Particularly of note is that Ubuntu's current version of `libsdl2-dev` (as of eoan)
suffers from this issue; building/installing SDL2 from source is required here.
Rolling-release distributions (such as Arch or Gentoo) are up-to-date and therefore unaffected.

## Components

### `libmusubi`

`libmusubi` is the main "engine" component and currently consists of:

 - graphics abstractions
   - windowing (SDL2)
   - pixmaps
   - rendering
     - shapes (OpenGL)
     - textures (OpenGL)
 - lifecycle abstractions
   - an `application` owns `window`s, which own `screen`s
 - asset packing & managing
   - assets are automatically packed in `xz` archives by `mpack.py`, and can be loaded and read at runtime

Currently, `libmusubi` will assume and request OpenGL 3.3 or greater by default.
No abstractions for versions older than 3.0 are planned, due to API differences.

Once the default API is stable enough, all OpenGL abstractions will be given an abstract base type
so they will be easy to swap out with other implementations (i.e. other OpenGL versions,
possibly even other rendering APIs).

## Documentation

Documentation is generated using [Doxygen](http://www.doxygen.nl/).
To link to standard-library documentation, the [cppreference.com tag file](https://en.cppreference.com/w/Cppreference:Archives#Doxygen_tag_file)
can be placed in the directory from with `doxygen` is run.

Doxygen is invoked by a GitHub workflow on every commit to `master`;
the generated content is [pushed to GitHub pages](https://stuhlmeier.github.io/musubi).

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
 - [libarchive](https://github.com/libarchive/libarchive/)
 - [`nlohmann::json`](https://github.com/nlohmann/json) (MIT)

[arch-sdlcmake]: https://bbs.archlinux.org/viewtopic.php?pid=1777965#p1777965
