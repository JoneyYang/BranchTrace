#pragma once

EXTERN_C_START

/* check support Debug Store*/
BOOLEAN __stdcall CheckSupportDS();

/* check support 64bit Debug Store*/
BOOLEAN __stdcall CheckSupportDS64();

/* check BTS is avaliable*/
BOOLEAN __stdcall CheckAvaliableBTS();

/* check PEBS is avaliable*/
BOOLEAN __stdcall CheckAvaliablePEBS();

/* Set IA32_DS_AREA*/
BOOLEAN __stdcall SetDebugStoreArea(PVOID DebugStoreAreaBase);

/*¿ªÆôBTS */
BOOLEAN EnableBTS();

/*¹Ø±ÕBTS*/
BOOLEAN DisableBTS();


VOID TestJmp();

EXTERN_C_END