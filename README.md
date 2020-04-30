# SandraVM
Virtual machine.
Not completed now.
![Image alt](https://github.com/F11GAR0/SandraVM/blob/master/view.png)
____

# Syntax example

```asm
.var
    _global a
    _global b
.code
    mov sanax, 12
    mov a, sanax
    mov sanbx, 7
    mov b, sanbx
    jmp @out
    add sanax, 4
    add sanbx, 2
    mov a, sanax
    mov b, sanbx
    :out
    jmp @trueout
    add sanax, 1
    add sanbx, 7
    :trueout
    out sanax
    out sanbx
```
