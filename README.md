# osu-replayer

Viewer for osu! (stable, no lazer support) replays.
It is written in C++ and can be run both natively and in the browser with WASM and WebGL.

![Screenshot_20230123_232542](https://user-images.githubusercontent.com/18403034/214163969-9b8fb4dd-9b5b-4a09-9660-e5304fbb6ad8.png)


If the [API](https://github.com/ShaddyDC/osu-api-proxy/) is available,
a downloadable replay can be passed (like `https://replay.shaddy.dev/?replayer-score=3287277626`).
Both the replay and the corresponding beatmap will be fetched automatically, and the play will be shown.

## Building

Since the project uses standard CMake, many environments like [VSCode](https://code.visualstudio.com/docs/cpp/cmake-linux) will support it out-of-the-box.
To manually build it, follow these steps while having CMake and a C++ compiler installed, or if using [Nix](https://nixos.org/), 
use the provided dev environment with `nix develop`.

```console
$ git clone https://github.com/ShaddyDC/osu-replayer --recurse-submodules
$ cd osu-replayer
$ mkdir build && cd build
$ cmake .. && cmake --build .
```

## Using the flake package

If you're using [Nix](https://nixos.org/), the executable is exported in the flake.
However, since it relies on submodules, you have to explicitly enable those.

```console
$ nix shell "git+https://github.com/ShaddyDC/osu-replayer?submodules=1" -c osu_replayer
```
