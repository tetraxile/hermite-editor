# Hermite animation curve editor

This is a GUI editor for the type of animation curves found in [BFLAN](https://wiki.wexosmk.xyz/index.php/BFLAN_(File_Format)) files.

The "Copy to clipboard" button will copy the data currently represented by the editor's keyframes, in the XML format used by KillzXGaming's [LayoutLibrary.CLI](https://github.com/KillzXGaming/LayoutLibrary).

## Usage

### Building

```
mkdir build && cd build
cmake ..
make
```

When run, the program expects the working directory to be `build/`, as it looks for the font at `../assets/`.

### Controls

Most interactions just use left click, the only exceptions are:
* If `SHIFT` is held while moving a keyframe up or down, or moving a slope control, the values associated with them will snap to multiples of 0.1.
* Right clicking anywhere will deselect the current keyframe, or cancel the `Add keyframe` action.
* Pressing `ESCAPE` will terminate the program.

## Dependencies

* [raylib](https://github.com/raysan5/raylib)

## License

The licenses found in the [LICENSE](LICENSE) file apply only to the source files in the [src/](src) directory.

## Credits

* https://github.com/KillzXGaming/LayoutLibrary
* https://github.com/KillzXGaming/Switch-Toolbox
* https://github.com/gdkchan/SPICA
