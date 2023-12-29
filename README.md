# dm9051_mcu_project
Davicom DM9051 project for MCU created on 20231214

//***  增加 usart3proc_fun.c 功能
path: sdk32f403Av2.0.9_dm9051_hello_link
usart3proc_main() function
;-----------------------------------------------------------
;PC_RX_BUF:STX,LEN_L,LEN_H,57H,CMD#,ADDR_L,ADDR_H,DATA0_L,DATA0_H,..... ,ETX,CRC_L,CRC_H  ;
;RESPONSE OP = 55H
;RESPONSE CMD# = 0A5H
;PROTOCOL:STX,05H,00H,OP,CMD#,ETX,CRC_L,CRC_H
;             |<--LENGTH-------->|
;             |<----CRC--------->|
;------------------------------------------------------------
ex: 02 07 00 55 23 41 42 03 45 46  -- HEX CRC16 error.
ex: 02 07 00 55 23 89 76 03 55 67  -- HEX CRC16 error.
ex: 02 07 00 55 23 41 42 03 4A FD  --- CRC16 is correct, process the data 
ex: 02 07 00 55 23 41 42 03 4A FD 41 42  --- length error 