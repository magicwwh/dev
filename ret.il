addr 0x0 @asm "xchg   %eax,(%esp)"
label pc_0x0
T_t:u32 = R_EAX_32:u32
R_EAX_32:u32 = mem32:u32?u8[R_ESP_32:u32, e_little]:u32
mem32:u32?u8 = mem32:u32?u8 with [R_ESP_32:u32, e_little]:u32 = T_t:u32
addr 0x3 @asm "cmp    %gs:0x0,%eax"
label pc_0x3
T_t_109:u32 =
  R_EAX_32:u32 - mem32:u32?u8[R_GS_BASE_32:u32 + 0:u32, e_little]:u32
R_CF:bool =
  R_EAX_32:u32 < mem32:u32?u8[R_GS_BASE_32:u32 + 0:u32, e_little]:u32
R_OF:bool =
  high:bool((R_EAX_32:u32 ^
            mem32:u32?u8[R_GS_BASE_32:u32 + 0:u32, e_little]:u32) &
            (R_EAX_32:u32 ^ T_t_109:u32))
R_AF:bool =
  0x10:u32 ==
  (0x10:u32 &
  (T_t_109:u32 ^ R_EAX_32:u32 ^
  mem32:u32?u8[R_GS_BASE_32:u32 + 0:u32, e_little]:u32))
R_PF:bool =
  ~low:bool(let T_acc:u32 := T_t_109:u32 >> 4:u32 ^ T_t_109:u32 in
            let T_acc:u32 := T_acc:u32 >> 2:u32 ^ T_acc:u32 in
            T_acc:u32 >> 1:u32 ^ T_acc:u32)
R_SF:bool = high:bool(T_t_109:u32)
R_ZF:bool = 0:u32 == T_t_109:u32
addr 0xa @asm "jb     0x000000000000001f"
label pc_0xa
cjmp R_CF:bool, 0x1f:u32, "nocjmp0"
label nocjmp0
addr 0xc @asm "and    0xf,%al"
label pc_0xc
R_EAX_32:u32 =
  concat:[extract:31:8:[R_EAX_32:u32]][
   low:u8(R_EAX_32:u32) & mem32:u32?u8[0xf:u32, e_little]:u8]
R_OF:bool = false
R_CF:bool = false
R_AF:bool = unknown "AF is undefined after and":bool
R_PF:bool =
  ~low:bool(let T_acc_111:u8 :=
                                 low:u8(R_EAX_32:u32) >> 4:u8 ^
                                 low:u8(R_EAX_32:u32) in
            let T_acc_111:u8 := T_acc_111:u8 >> 2:u8 ^ T_acc_111:u8 in
            T_acc_111:u8 >> 1:u8 ^ T_acc_111:u8)
R_SF:bool = high:bool(low:u8(R_EAX_32:u32))
R_ZF:bool = 0:u8 == low:u8(R_EAX_32:u32)
addr 0x12 @asm "cmp    %gs:0x4,%eax"
label pc_0x12
T_t_112:u32 =
  R_EAX_32:u32 - mem32:u32?u8[R_GS_BASE_32:u32 + 4:u32, e_little]:u32
R_CF:bool =
  R_EAX_32:u32 < mem32:u32?u8[R_GS_BASE_32:u32 + 4:u32, e_little]:u32
R_OF:bool =
  high:bool((R_EAX_32:u32 ^
            mem32:u32?u8[R_GS_BASE_32:u32 + 4:u32, e_little]:u32) &
            (R_EAX_32:u32 ^ T_t_112:u32))
R_AF:bool =
  0x10:u32 ==
  (0x10:u32 &
  (T_t_112:u32 ^ R_EAX_32:u32 ^
  mem32:u32?u8[R_GS_BASE_32:u32 + 4:u32, e_little]:u32))
R_PF:bool =
  ~low:bool(let T_acc_113:u32 := T_t_112:u32 >> 4:u32 ^ T_t_112:u32 in
            let T_acc_113:u32 := T_acc_113:u32 >> 2:u32 ^ T_acc_113:u32 in
            T_acc_113:u32 >> 1:u32 ^ T_acc_113:u32)
R_SF:bool = high:bool(T_t_112:u32)
R_ZF:bool = 0:u32 == T_t_112:u32
addr 0x19 @asm "jae    0x000000000000001f"
label pc_0x19
cjmp ~R_CF:bool, 0x1f:u32, "nocjmp1"
label nocjmp1
addr 0x1b @asm "xchg   %eax,(%esp)"
label pc_0x1b
T_t_114:u32 = R_EAX_32:u32
R_EAX_32:u32 = mem32:u32?u8[R_ESP_32:u32, e_little]:u32
mem32:u32?u8 = mem32:u32?u8 with [R_ESP_32:u32, e_little]:u32 = T_t_114:u32
addr 0x1e @asm "ret    "
label pc_0x1e
T_ra:u32 = mem32:u32?u8[R_ESP_32:u32, e_little]:u32
R_ESP_32:u32 = R_ESP_32:u32 + 4:u32
jmp T_ra:u32 @str "ret"
addr 0x1f @asm "jmp    0x0000000000000114"
label pc_0x1f
jmp 0x114:u32


