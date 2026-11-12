        AREA STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem SPACE 0x4000
__initial_sp
        AREA RESET, DATA, READONLY
        EXPORT __Vectors
__Vectors
        DCD __initial_sp
        DCD Reset_Handler
        AREA |.text|, CODE, READONLY
        THUMB
        EXPORT Reset_Handler
        IMPORT __main
Reset_Handler PROC
        LDR R0, =__main
        BX R0
        ENDP
        END
