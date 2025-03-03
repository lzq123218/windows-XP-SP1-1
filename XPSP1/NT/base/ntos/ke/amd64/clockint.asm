       title  "Interval Clock Interrupt"
;++
;
; Copyright (c) 2000  Microsoft Corporation
;
; Module Name:
;
;   clockint.asm
;
; Abstract:
;
;   This module implements the architecture dependent code necessary to
;   process the interval clock interrupt.
;
; Author:
;
;   David N. Cutler (davec) 12-Sep-2000
;
; Environment:
;
;   Kernel mode only.
;
;--

include ksamd64.inc

        extern  KdDebuggerEnabled:byte
        extern  KeMaximumIncrement:dword
        extern  KeTimeAdjustment:dword
        extern  KeTickCount:qword
        extern  KiAdjustDpcThreshold:dword
        extern  KiCheckBreakInRequest:proc
        extern  KiIdealDpcRate:dword
        extern  KiMaximumDpcQueueDepth:dword
        extern  KiTickOffset:dword
        extern  KiTimerTableListHead:qword
        extern  __imp_HalRequestSoftwareInterrupt:qword

        subttl  "Update System Time"
;++
;
; VOID
; KeUpdateSystemTime (
;     IN ULONG64 Increment
;     )
;
; Routine Description:
;
;   This routine is called as the result of an interrupt generated by the
;   interval timer. Its function is to update the interrupt time, update the
;   system time, and check to determine if a timer has expired.
;
;   N.B. This routine is executed on a single processor in a multiprocess
;        system. The remainder of the processors only execute the quantum end
;        and runtime update code.
;
; Arguments:
;
;   TrapFrame (rcx) - Supplies the address of a trap frame.
;
;   Increment (rcx) - Supplies the time increment value in 100 nanosecond
;       units.
;
; Return Value:
;
;   None.
;
;--

UsFrame struct
        P1Home  dq ?                    ; request IRQL parameter
        Fill    dq ?                    ; fill to 8 mod 16
        SavedRbp dq ?                   ; saved register RBP
UsFrame ends

        NESTED_ENTRY KeUpdateSystemTime, _TEXT$00

        push_reg rbp                    ; save nonvolatile register
        alloc_stack (sizeof UsFrame- (1 * 8)); allocate stack frame

        END_PROLOGUE

        lea     rbp, 128[rcx]           ; set frame pointer address

;
; Check if the current clock tick should be skipped.
;
; Skip tick is set when the kernel debugger is entered.
;

if DBG

        cmp     byte ptr gs:[PcSkipTick], 0 ; check if tick should be skipped
        jnz     KiUS60                  ; if nz, skip clock tick

endif

;
; Update interrupt time.
;

        mov     rcx, USER_SHARED_DATA   ; get user shared data address
        lea     r11, KiTimerTableListHead ; get timer table address
        mov     r8, UsInterruptTime[rcx] ; get interrupt time
        add     r8, rdx                 ; add time increment
        mov     rax, r8                 ; isolate high part of interrupt time
        shr     rax, 32                 ;
        mov     UsInterruptTime + 8[rcx], eax ; store high 2 interrupt time
        mov     UsInterruptTime[rcx], r8 ; store interrupt time
        mov     r10, KeTickCount        ; get tick count value
        sub     KiTickOffset, edx       ; subtract time increment
        jg      short KiUS20            ; if greater, not complete tick

;
; Update system time.
;

        mov     r9d, UsSystemTime + 0[rcx] ; get low interrupt time
        mov     eax, UsSystemTime + 4[rcx] ; get high interrupt time
        add     r9d, KeTimeAdjustment   ; add time increment
        adc     eax, 0                  ; propagate carry
        mov     UsSystemTime + 8[rcx], eax ; store high 2 interrupt time
        mov     UsSystemTime + 0[rcx], r9d ; store low interrupt time
        mov     UsSystemTime + 4[rcx], eax ; store high 1 interrupt time

;
; Update tick count.
;

        inc     KeTickCount             ; update tick count value
        inc     dword ptr UsTickCountLow[rcx] ; update low tick count value

;
; Check to determine if a timer has expired.
;

        mov     rcx, r10                ; copy tick count value
        and     ecx, TIMER_TABLE_SIZE - 1 ; isolate current hand value
        shl     ecx, 4                  ; compute listhead offset
        lea     rcx, [r11][rcx]         ; get listhead address
        mov     r9, LsFlink[rcx]        ; get first entry address
        cmp     r9, rcx                 ; check if list is empty
        je      short KiUS10            ; if e, list is empty
        cmp     r8, (TiDueTime - TiTimerListEntry)[r9] ; compare due time
        jae     short KiUS30            ; if ae, timer has expired
KiUS10: inc     r10                     ; advance tick count value

;
; Check to determine if a timer has expired.
;

KiUS20: mov     rcx, r10                ; copy tick count value
        and     ecx, TIMER_TABLE_SIZE - 1 ; isolate current hand value
        shl     ecx, 4                  ; compute listhead offset
        lea     rcx, [r11][rcx]         ; get listhead addrees
        mov     r9, LsFlink[rcx]        ; get first entry address
        cmp     r9, rcx                 ; check if list is empty
        je      short KiUS40            ; if equal, list is empty
        cmp     r8, (TiDueTime - TiTimerListEntry)[r9] ; compare due time
        jb      short KiUS40            ; if b, timer has not expired

;
; A timer has expired.
;
; Set the timer hand value in the current processor block if it is not already
; set.
;

KiUS30: mov     rdx, gs:[PcCurrentPrcb] ; get current processor block address
        cmp     dword ptr PbTimerHand[rdx], 0 ; check if expiration active
        jne     short KiUS40            ; if ne, expiration already active
	and	r10d, TIMER_TABLE_SIZE - 1 ; isolate current hand value
        inc     r10d                    ; increment timer hand value
        mov     PbTimerHand[rdx], r10d  ; set timer hand value
        mov     cl, DISPATCH_LEVEL      ; request dispatch interrupt
        call    __imp_HalRequestSoftwareInterrupt ;

;
; If the debugger is enabled, check if a break is requested.
;

KiUS40: cmp     KdDebuggerEnabled, 0    ; check if a debugger is enabled
        je      short KiUS50            ; if e, debugger is not enabled
        call    KiCheckBreakInRequest   ; check for break in request

;
; Check to determine if a full tick has expired.
;

KiUS50: cmp     KiTickOffset, 0         ; check if full tick has expired
        jg      short KiUS70            ; if g, not a full tick
        mov     eax, KeMaximumIncrement ; get maximum time incrmeent
        add     KiTickOffset, eax       ; add maximum time to residue
        mov     rcx, rbp                ; set trap frame address
        call    KeUpdateRunTime         ; update runtime

if DBG

KiUS60: mov     byte ptr gs:[PcSkipTick], 0 ; clear skip tick indicator

endif

KiUS70: add     rsp, sizeof UsFrame- (1 * 8) ; deallocate stack frame
        pop     rbp                     ; restore nonvolatile register
        ret                             ; return

        NESTED_END KeUpdateSystemTime, _TEXT$00

        subttl  "Update Thread and Process Runtime"
;++
;
; Routine Description:
;
;   This routine is called as the result of the interval timer interrupt on
;   all processors in the system. Its function is update the runtime of the
;   current thread, update the runtime of the current thread's process, and
;   decrement the current thread's quantum. This routine also implements DPC
;   interrupt moderation.
;
;   N.B. This routine is executed on all processors in a multiprocessor
;        system.
;
; Arguments:
;
;   rcx - Supplies the address of a trap frame.
;
; Return Value:
;
;   None.
;
;--

UrFrame struct
        P1Home  dq ?                    ; request IRQL parameter
        Fill    dq ?                    ; fill to 8 mod 16
        SavedRdi dq ?                   ; saved register RDI
        SavedRsi dq ?                   ; saved register RSI
        savedRbp dq ?                   ; saved register RBP
UrFrame ends

        NESTED_ENTRY KeUpdateRunTime, _TEXT$00

        push_reg rbp                    ; save nonvolatile registers
        push_reg rsi                    ;
        push_reg rdi                    ;
        alloc_stack (sizeof UrFrame - (3 * 8)) ; allocate stack frame

        END_PROLOGUE

        lea     rbp, 128[rcx]           ; set frame pointer address

;
; Check if the current clock tick should be skipped.
;
; Skip tick is set when the kernel debugger is entered.
;

if DBG

        cmp     byte ptr gs:[PcSkipTick], 0 ; check if tick should be skipped
        jnz     KiUR70                  ; if nz, skip clock tick

endif

;
; Update time counter based on previous mode, IRQL level, and whether there
; is currently a DPC active.
;

        mov     rsi, gs:[PcCurrentPrcb]  ; get current processor block address
        mov     rdi, PbCurrentThread[rsi] ; get current thread address
        mov     rdx, ThApcState + AsProcess[rdi] ; get current process address
        test    byte ptr TrSegCs[rbp], MODE_MASK ; check if previous mode user
        jnz     short KiUR30            ; if nz, previous mode user

;
; Update the total time spent in kernel mode.
;

        inc     dword ptr PbKernelTime[rsi] ; increment kernel time
        cmp     byte ptr TrPreviousIrql[rbp], DISPATCH_LEVEL ; check IRQL level
        jb      short KiUR20            ; if b, previous IRQL below DPC level
        ja      short KiUR10            ; if a, previous IRQL above DPC level
        cmp     dword ptr PbDpcRoutineActive[rsi], 0 ; check if DPC routine active
        je      short KiUR20            ; if e, no DPC routine active
        inc     dword ptr PbDpcTime[rsi] ; increment time at DPC level
        jmp     short KiUR40            ; finish in common code

;
; Update the time spent at interrupt time for this processor
;

KiUR10: inc     dword ptr PbInterruptTime[rsi] ; increment interrupt time
        jmp     short KiUR40            ; finish in common code

;
; Update the time spent in kernel mode for the current thread and the current
; process.
;

KiUR20: inc     dword ptr ThKernelTime[rdi] ; increment time in kernel mode

ifndef NT_UP

   lock inc     dword ptr PrKernelTime[rdx] ; increment time in kernel mode

else

        inc     dword ptr PrKernelTime[rdx] ; increment time in kernel mode

endif
        jmp     short KiUR40            ; finish in common code

;
; Update total time spent in user mode and update the time spent inuser mode
; for the current thread and the current process.
;

KiUR30: inc     dword ptr PbUserTime[rsi] ; increment time in user mode
        inc     dword ptr ThUserTime[rdi] ; increment time is user mode

ifndef NT_UP

   lock inc     dword ptr PrUserTime[rdx] ; increment time in user mode

else

        inc     dword ptr PrUserTime[rdx] ; increment time in user mode

endif

;
; Update the DPC request rate which is computed as the average between the
; previous rate and the current rate.
;

KiUR40: mov     ecx, PbDpcCount[rsi]    ; get current DPC count
        mov     edx, PbDpcLastCount[rsi] ; get last DPC count
        mov     PbDpcLastCount[rsi], ecx ; set last DPC count
        sub     ecx, edx                ; compute count during interval
        add     ecx, PbDpcRequestRate[rsi] ; compute sum
        shr     ecx, 1                  ; average current and last
        mov     PbDpcRequestRate[rsi], ecx ; set new DPC request rate

;
; If the current DPC queue depth is not zero, a DPC routine is not active,
; and a DPC interrupt has not been requested, then request a dispatch
; interrupt, decrement the maximum DPC queue depth, and reset the threshold
; counter if appropriate.
;

        cmp     dword ptr PbDpcQueueDepth[rsi], 0 ; check if queue depth zero
        je      short KiUR50            ; if e, DPC queue depth is zero
        cmp     dword ptr PbDpcRoutineActive[rsi], 0 ; check if DPC routine active
        jne     short KiUR50            ; if ne, DPC routine active
        cmp     dword ptr PbDpcInterruptRequested[rsi], 0 ; check if interrupt
        jne     short KiUR50            ; if ne, interrupt requested
        mov     cl, DISPATCH_LEVEL      ; request a dispatch interrupt
        call    __imp_HalRequestSoftwareInterrupt ;
        mov     ecx, PbDpcRequestRate[rsi] ; get DPC request rate
        mov     edx, KiAdjustDpcThreshold ; reset initial threshold counter
        mov     PbAdjustDpcThreshold[rsi], edx ;
        cmp     ecx, KiIdealDpcRate     ; check if current rate less than ideal
        jge     short KiUR60            ; if ge, rate greater or equal ideal
        cmp     dword ptr PbMaximumDpcQueueDepth[rsi], 1 ; check if maximum depth one
        je      short KiUR60            ; if e, maximum depth is one
        dec     dword ptr PbMaximumDpcQueueDepth[rsi] ; decrement depth
        jmp     short KiUR60            ;

;
; The DPC queue is empty or a DPC routine is active or a DPC interrupt has
; been requested. Count down the adjustment threshold and if the count reaches
; zero, then increment the maximum DPC queue depth, but not above the initial
; value and reset the adjustment threshold value.
;

KiUR50: dec     dword ptr PbAdjustDpcThreshold[rsi] ; decrement threshold
        jnz     short KiUR60            ; if nz, threshold not zero
        mov     ecx, KiAdjustDpcThreshold ; reset initial threshold counter
        mov     PbAdjustDpcThreshold[rsi], ecx ;
        mov     ecx, KiMaximumDpcQueueDepth ; get maximum DPC queue depth
        cmp     ecx, PbMaximumDpcQueueDepth[rsi] ; check if depth at maximum level
        je      short KiUR60            ; if e, aleady a maximum level
        inc     dword ptr PbMaximumDpcQueueDepth[rsi] ; increment maximum depth

;
; Decrement current thread quantum and check to determine if a quantum end
; has occurred.
;

KiUR60: sub     byte ptr ThQuantum[rdi], CLOCK_QUANTUM_DECREMENT ; decrement quantum
        jg      short KiUR80            ; if g, time remaining on quantum

;
; Set quantum end flag and initiate a dispather interrupt on the current
; processor.
;

        cmp     rdi, PbIdleThread[rsi]  ; check if idle thread
        je      short KiUR80            ; if e, idle thread
        inc     dword ptr PbQuantumEnd[rsi] ; set quantum end indicator
        mov     cl, DISPATCH_LEVEL      ; request dispatch interrupt
        call    __imp_HalRequestSoftwareInterrupt ;

if DBG

KiUR70: mov     byte ptr gs:[PcSkipTick], 0 ; clear skip tick indicator

endif

KiUR80: add     rsp, sizeof UrFrame - (3 * 8) ; deallocate stack frame
        pop     rdi                     ; restore nonvolatile registers
        pop     rsi                     ;
        pop     rbp                     ;
        ret                             ; return

        NESTED_END KeUpdateRunTime, _TEXT$00

        end
