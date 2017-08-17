1. White-spaces can be freely used to start the line of code
2. Any amount of white-spaces can be used to separate LABEL/OPERATION/ADDRESS fields


Here are few valid lines of MIXAL code:
``` {.asm .numberLines}
|    * comment with with white-spaces at the beginning
|  LABEL     SUB
|     LDA 1000
|    IN OUT    16         note: "IN" is a LABEL
```

3. Since of allowing white spaces everywhere, MIXAL will fail to parse next line:
    ``` {.asm .numberLines}
    |CMPA     1001         * cmp flag changed
    *^^^^     ^^^^^^^      ^^^^^^^^^^^^^^^^^^^
    *Command  Address      Comment
    ```
because "`1001         * cmp`" will be treated as expression "`1001*cmp`" where
`cmp` is invalid MIX(AL) symbol


