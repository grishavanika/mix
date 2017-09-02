# MIX

[![Build Status (Windows)](https://ci.appveyor.com/api/projects/status/github/grishavanika/mix?svg=true)](https://ci.appveyor.com/project/grishavanika/mix)
[![Build Status (Linux)](https://travis-ci.org/grishavanika/mix.svg)](https://travis-ci.org/grishavanika/mix)


###### TODO

- add index register influence to LD/ST/ADD/SUB and similar tests:
	* `LDA 2000,3(1:3)`
- remove googletest third_party files and use git submodule instead
- add Travis CI build support for **Clang**
- port interpreter to JavaScript (with **[emscripten](http://kripken.github.io/emscripten-site/)**)
to play with it in web-page
- use WSL to build & debug *linux version with Visual Studio

##### Translating "FIRST FIVE HUNDRED PRIMES"

How to translate:
    ```
    mixal --hide-details --file src/tests/mixal_code/program_primes.mixal
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

How to run:
    ```
    mixal --execute --file src/tests/mixal_code/program_primes.mixal
    ```

![](docs/first_500_primes.png)



##### Building

- To use it with MinGW please install STL's port from https://nuwen.net/mingw.html.
And then:
    ```
    set path=%path%;C:\Programs\mingw\bin
    cmake -G "MinGW Makefiles" ..
    ```

- To use it with Clang on Windows you need to install latest one
from http://llvm.org/builds/ (5.0.0 for the moment).
And then:
    ```
    cmake -G "Visual Studio 14 2015 Win64" -T "LLVM-vs2014" ..
    ```

- See also `scripts/generate_proj.bat` and `scripts/build.*(sh|bat)`
