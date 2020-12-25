option casemap:none 

include cpu.inc

.CODE
CheckSupportDS PROC uses rdx 
	mov eax,1
	cpuid
	bt edx,21		; 将edx[21]复制到CF位
	setc al			; 根据CF标志位 set byte
	movzx eax,al	; 拓展al到eax

	ret
CheckSupportDS ENDP

CheckSupportDS64 PROC
	mov eax,1
	cpuid
	bt ecx,2			;ecx[2]:DEST64
	setc al			
	movzx eax,al
	ret
CheckSupportDS64 ENDP

CheckAvaliableBTS PROC
	mov eax,1
	cpuid
	bt edx,21		
	jnc tag_end			; 为0直接跳转到返回

	mov ecx,IA32_MISC_ENABLE	; IA32_MISC_ENABLE
	rdmsr
	bt eax,11		;IA32_MISC_ENABLE[11] 为0表示可用. 为1表示不可用
	setnc al

tag_end:
	movzx eax,al	; 拓展al到eax
	ret
CheckAvaliableBTS ENDP


CheckAvaliablePEBS PROC
	mov eax,1
	cpuid
	bt edx,21		
	jnc tag_end			; 为0直接跳转到返回

	mov ecx,IA32_MISC_ENABLE	; IA32_MISC_ENABLE
	rdmsr
	bt eax,12		;IA32_MISC_ENABLE[12] 为0表示可用. 为1表示不可用
	setnc al
	
tag_end:
	movzx eax,al	; 拓展al到eax
	ret	
CheckAvaliablePEBS ENDP


; rcx = DebugStoreAreaBase
SetDebugStoreArea PROC uses rdx

	mov rax,rcx		; 将 DebugStoreAreaBase 低位写入eax	  wrmsr会忽略rax高32位
	mov rdx,rcx	
	shr rdx,32		; 将 DebugStoreAreaBase 高32位写入edx  wrmsr会忽略rdx高32位

	mov ecx,IA32_DS_AREA
	wrmsr			; write msr   EDX:EAX -> msr

	ret
SetDebugStoreArea ENDP;

EnableBTS PROC uses rcx rdx
	mov ecx, IA32_DEBUGCTL
	rdmsr    		; read msr     msr -> EDX:EAX
	or eax,2C0h		; TR(bit6)=1, BTS(bit7)=1   BTS_OFF_OS(bit9) 
	wrmsr
	ret
EnableBTS ENDP

DisableBTS PROC  uses rcx rdx
	mov ecx, IA32_DEBUGCTL
	rdmsr					; read msr     msr -> EDX:EAX
	and eax,0FFFFFF3Fh		; TR=0, BTS=0
	wrmsr
	ret
DisableBTS ENDP



TestJmp PROC

lable1:
	jmp lable2
lable2:
	jmp lable3
lable3:
	jmp lable4
lable4:
	jmp lable5
lable5:
	jmp lable6
lable6:
	jmp lable7
lable7:
	jmp lable8
lable8:
	jmp lable9
lable9:
	jmp lable10
lable10:
	jmp lable11
lable11:

	ret	
TestJmp ENDP



END