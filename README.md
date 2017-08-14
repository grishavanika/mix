# MIX

[![Build Status (Windows)](https://ci.appveyor.com/api/projects/status/github/grishavanika/mix?svg=true)](https://ci.appveyor.com/project/grishavanika/mix)
[![Build Status (Linux)](https://travis-ci.org/grishavanika/mix.svg)](https://travis-ci.org/grishavanika/mix)


###### TODO

- add index register influence to LD*/ST*/ADD/SUB and similar tests:
	* `LDA 2000,3(1:3)`
- remove googletest third_party files and use git submodule instead
- add Travis CI build support

- MIXAL should process text input to byte-code. For example:
    ```
    LDA 2000
    LDA -2000,4
    ```
	will generate next commands:
    ```
    +2000 2 3 8
    -2000 4 5 8
    ```

that byte code can be saved to some intermediate result and executed later

- port interpreter (when it will exist!) to JavaScript (with **[emscripten](http://kripken.github.io/emscripten-site/)**)
to play with it in web-page
