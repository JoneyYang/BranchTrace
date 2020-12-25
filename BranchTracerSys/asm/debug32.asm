.686P 
.model flat, stdcall
option casemap:none 

.code

_asm_cpuid PROC uses edi ebx id:dword,pCpuinfo:dword
	mov eax,id
	cpuid

	;Êä³ö²ÎÊý
	mov edi ,pCpuinfo;
	mov [edi+0],eax
 	mov [edi+04h],ecx
	mov [edi+08h],edx
 	mov [edi+0ch],ebx

	mov eax, 1
	ret
_asm_cpuid ENDP

_asm_vmxon PROC lowPart:dword,highPart:dword	
	sub esp,08h
	mov eax, lowPart
	mov [esp],eax
	mov eax, highPart
	mov [esp+4],eax
	vmxon qword ptr [esp]
	add esp,08h
	ret
_asm_vmxon ENDP

_asm_vmxoff PROC
	vmxoff
	ret
_asm_vmxoff ENDP

_asm_vmlaunch PROC
	vmlaunch
	ret 
_asm_vmlaunch ENDP

_asm_vmresume PROC
	vmresume
	ret
_asm_vmresume ENDP

_asm_vmcall PROC
	vmcall
	ret
_asm_vmcall ENDP

_asm_vmclear PROC lowPart:dword, highPart:dword
	sub esp,08h
	mov eax, lowPart
	mov [esp],eax
	mov eax, highPart
	mov [esp+4],eax
	vmclear qword ptr [esp]
	add esp,08h
	ret
_asm_vmclear ENDP

_asm_vmptrld PROC lowPart:dword, highPart:dword
	sub esp,08h

	mov eax, lowPart
	mov [esp],eax
	mov eax, highPart
	mov [esp+4],eax
	vmptrld qword ptr [esp]

	add esp,08h
	ret
_asm_vmptrld ENDP

_asm_vmread  PROC uses ecx filed:dword
	mov eax,filed
	vmread ecx,eax
	mov eax, ecx
	ret
_asm_vmread ENDP

_asm_vmwrite PROC uses ecx filed:dword, value:dword
	mov eax,filed
	mov ecx,value
	vmwrite eax,ecx
	ret 
_asm_vmwrite ENDP

END