option casemap:none 

include cpu.inc

.CODE
CheckSupportDS PROC uses rdx 
	mov eax,1
	cpuid
	bt edx,21		; ��edx[21]���Ƶ�CFλ
	setc al			; ����CF��־λ set byte
	movzx eax,al	; ��չal��eax

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
	jnc tag_end			; Ϊ0ֱ����ת������

	mov ecx,IA32_MISC_ENABLE	; IA32_MISC_ENABLE
	rdmsr
	bt eax,11		;IA32_MISC_ENABLE[11] Ϊ0��ʾ����. Ϊ1��ʾ������
	setnc al

tag_end:
	movzx eax,al	; ��չal��eax
	ret
CheckAvaliableBTS ENDP


CheckAvaliablePEBS PROC
	mov eax,1
	cpuid
	bt edx,21		
	jnc tag_end			; Ϊ0ֱ����ת������

	mov ecx,IA32_MISC_ENABLE	; IA32_MISC_ENABLE
	rdmsr
	bt eax,12		;IA32_MISC_ENABLE[12] Ϊ0��ʾ����. Ϊ1��ʾ������
	setnc al
	
tag_end:
	movzx eax,al	; ��չal��eax
	ret	
CheckAvaliablePEBS ENDP


; rcx = DebugStoreAreaBase
SetDebugStoreArea PROC uses rdx

	mov rax,rcx		; �� DebugStoreAreaBase ��λд��eax	  wrmsr�����rax��32λ
	mov rdx,rcx	
	shr rdx,32		; �� DebugStoreAreaBase ��32λд��edx  wrmsr�����rdx��32λ

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