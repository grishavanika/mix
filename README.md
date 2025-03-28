# MIX

- [MIX UI debugger](#mix-ui-debugger)
- [Translating "FIRST FIVE HUNDRED PRIMES"](#translating-first-five-hundred-primes)
- [Executing "FIRST FIVE HUNDRED PRIMES"](#executing-first-five-hundred-primes)
- [Integration with GNU MIX Development Kit (MDK)](#integration-with-gnu-mix-development-kit-mdk)
- [Build](#build)
- [Dependencies](#dependencies)
- [mixal command-line help](#mixal-command-line-help)
- [TODO](#todo)

##### MIX UI debugger

Supports:

 * breakpoints
 * change of execution pointer
 * single instruction step

![](mix_ui_progress.png)

##### Translating "FIRST FIVE HUNDRED PRIMES"

```
mixal --hide-details --interactive --file src/tests/mixal_code/program_primes.mixal
```

```
3000: |+| 0000|00|18|35|
3003: |+| 0001|00|00|49|
3004: |+| 0499|01|05|26|
3006: |+| 0002|00|00|50|
3007: |+| 0002|00|02|51|
3008: |+| 0000|00|02|48|
3009: |+| 0000|02|02|55|
3010: |-| 0001|03|05|04|
3011: |+| 3006|00|01|47|
3012: |-| 0001|03|05|56|
3013: |+| 0001|00|00|51|
3014: |+| 3008|00|06|39|
3015: |+| 3003|00|00|39|
3005: |+| 3016|00|01|41|
3017: |+| 2035|00|02|52|
3018: |-| 0050|00|02|53|
3019: |+| 0501|00|00|53|
3020: |-| 0001|05|05|08|
3021: |+| 0000|00|01|05|
3022: |+| 0000|04|12|31|
3023: |+| 0001|00|01|52|
3024: |+| 0050|00|01|53|
3025: |+| 3020|00|02|45|
3026: |+| 0000|04|18|37|
3027: |+| 0024|04|05|12|
3028: |+| 3019|00|00|45|
3029: |+| 0000|00|02|05|
   0: |+|             2|
3016: |+| 1995|00|18|37|
1995: |+|06|09|19|22|23|
1996: |+|00|06|09|25|05|
1997: |+|00|08|24|15|04|
1998: |+|19|05|04|00|17|
1999: |+|19|09|14|05|22|
2024: |+|          2035|
2049: |+|          2010|
3001: |+| 2050|00|05|09|
3002: |+| 2051|00|05|10|
2050: |-|           499|
2051: |+|             3|
```

##### Executing "FIRST FIVE HUNDRED PRIMES"

```
mixal --file src/tests/mixal_code/program_primes.mixal
```

![](first_500_primes.png)

##### Integration with GNU MIX Development Kit (MDK)

There is much more powerful project, called "[MDK]" that can be viewed as IDE for MIX
(no support for Windows).

Now, you can compile MIXAL code into MIX byte-code using MDK compiler
and then execute compiled program using interpreter from this project.

Here is how:

```
# Compile MIXAL code on Linux, using MDK
mixasm program_primes.mixal
# or the same, but without debug symbols
mixasm --ndebug program_primes.mixal
# (this will output compiled code in program_primes.mix file)
# now, execute it, using interpreter
#
mixal.exe --execute --mdk --file program_primes.mix
```

[MDK]: https://www.gnu.org/software/mdk/

##### Build

Standard CMake with VCPKG, see `build.cmd` for an example.

###### Dependencies

Handled by VCPKG and FetchContent (imgui).  
Core stuff (MIX VM, interpreter, parser, assembler) has no dependencies.  
Other things (tools, UI, tests) dependencies listed in third_party/ folder:

* (for command line tools) cxxopts: https://github.com/jarro2783/cxxopts.git
* (for, tests)             gtest  : https://github.com/google/googletest.git
* (for debugger UI)        imgui  : https://github.com/ocornut/imgui.git
* (for imgui)              sdl2   : https://github.com/SDL-mirror/SDL.git
* (for imgui, implicitly)  opengl : your OpenGL system provides

###### mixal command-line help

```
MIX interpreter/emulator/compilator
Usage:
  mixal [OPTION...]

  -h, --help          Show this help and exit
  -e, --execute       Compile and execute file with MIXAL code
  -i, --interactive   Compile MIXAL code line by line and print formatted MIX
                      byte-code
  -x, --hide-details  Hide additional information during interactive compile
  -f, --file arg      Input file (either MIXAL code or MIX byte-code)
  -m, --mdk           Interpret <file> as file with GNU MIX Development Kit
                      (MDK) format
```

