
1. There are no limits for length of `LABEL` or `OPERATION` or `ADDRESS` columns
of MIXAL line of code.
2. White-spaces can be freely used to start the line of code. Here are few valid
lines of MIXAL code:
    ``` {.asm .numberLines}
    |    * comment for with white-spaces at the beginning
    |  LABEL SUB
    |     LDA 1000
    ```
3. Because of previous differences, it's impossible to handle
comments at the end of line (can't find difference between binary multiply (`*`)
symbol and beginning-of-the-comment symbol (`*`)). E.g., this will **fail** to parse:
    ``` asm 
    ADD 1 * comment
    ```

