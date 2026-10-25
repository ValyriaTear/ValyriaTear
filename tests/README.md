# ImageMemory SDL surface regression

Run `tools/test-image-memory-surface.sh` from any directory. It compiles the
production `image_base.cpp` with an OpenGL upload stub, so it needs the game's
SDL2, SDL2_image, SDL2_ttf, PNG and GLEW development packages, pkg-config, and a
compiler/linker supporting `-ffunction-sections` and `--gc-sections`. It does not
need an X server or an OpenGL context.

Four synthetic surfaces cover padded and packed 24-bit RGB and 32-bit RGBA rows.
The RGBA pixels have distinct color and alpha bytes. The test verifies every
uploaded pixel, dimensions and format, and verifies that copying leaves the
source pixels and padding unchanged. The previous contiguous SDL-surface copy
fails the first padded-row case. Three empty surfaces cover zero width, zero
height and both. The test enables standard-library assertions to detect invalid
access to an empty pixel vector.

Thirty-six additional cases render four strings (narrow glyphs, wide glyphs,
`DEUTSCH`, and accented/Greek characters) at three sizes through each of the
three bundled font families used for titles, body text and map headings. Their
actual SDL_ttf surface rows must reach the upload unchanged, with padding
excluded. Run with `LD_PRELOAD` unset to exercise the system SDL_ttf directly.

Each of these 43 cases runs with incoming OpenGL unpack alignments 4, 8, 2 and 1.
The stub rejects a row stride that would introduce padding and verifies that the
upload restores the incoming alignment. Empty images must skip the upload.

Run `tools/test-image-memory-surface.sh --opengl` with an available SDL OpenGL
display to replace the stub with real texture upload and readback. It compares
every RGB/RGBA byte (including alpha), checks for GL errors and checks unpack
alignment restoration at all four settings. This mode needs an OpenGL context;
a private software GL display works. The readback explicitly uses packed rows.

Neither mode exercises the two immediate TextSupervisor draw overloads. Those
paths should also be checked in the running game, especially growing dialogue
text and a second frame with unchanged text dimensions.
