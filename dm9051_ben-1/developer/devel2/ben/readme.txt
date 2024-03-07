/**
  **************************************************************************
  * @file     readme.txt
  * @version  AT32F4_DM9051_Lw212_mqtt_r2306_rc1.0
  * @date     2023-05-30
  **************************************************************************
  */

  this demo is based on the at32f415-start board, in this demo, shows the mqtt client
  operating flow for at32f4xx series with DM9051. 

(1)

[msg-out]
mqtt_publish [DeviceTopic] 2.50 Hello mqtt server!! 009
set plsh time [3000]
[[ tcp slowtmr (pcb->rtime >= pcb->rto) 3 >= 6, pcb-rtime 3 reset to 0
set plsh time [0]

[msg-out]
mqtt_publish [DeviceTopic] 2.50 Hello mqtt server!! 010
set plsh time [3000]
[[ tcp slowtmr (pcb->rtime >= pcb->rto) 6 >= 12, pcb-rtime 6 reset to 0
set plsh time [0]

[msg-out]
mqtt_publish [DeviceTopic] 2.50 Hello mqtt server!! 011
set plsh time [3000]
set plsh time [0]

[msg-out]
mqtt_publish [DeviceTopic] 2.50 Hello mqtt server!! 012
set plsh time [3000]
[[ tcp slowtmr (pcb->rtime >= pcb->rto) 12 >= 24, pcb-rtime 12 reset to 0  <--------------------------  THE MEANINGS ,HOW TO PRE-STOP PUBLISH NEXT...
set plsh time [0]  <--------------------------- START REQUEST A PUBLISH ,WITH ERROR RETURN!! 
_[mqtt_publish] mqtt_create_request = NULL
_at32_mqtt_publish: mqtt_publish err = Out of memory error.
?During MQTT-Publishing.. ,if mqtt_publish fail && 'flag_link' is true].nvic_system_reset()...
@example  mqtt_client
@version  AT32F4_DM9051_Lw212_mqtt_r2307_rc1.2
@date     2023-07-27
dm9051_init
mac: 00:60:6e:00:01:25
ip: 192.168.1.37
msk: 255.255.255.0
gw: 192.168.1.254
timeouts num          12

at32_mqtt_client: New!
dm9051 link up
mqttc-connecting ...

(2)
[msg-out]
mqtt_publish [DeviceTopic] 2.50 Hello mqtt server!! 007
set plsh time [3000]
set plsh time [0]

[msg-out]
mqtt_publish [DeviceTopic] 2.50 Hello mqtt server!! 008
set plsh time [3000]
[[ tcp slowtmr (pcb->rtime >= pcb->rto) 3 >= 6, pcb-rtime 3 reset to 0
set plsh time [0]

[msg-out]
mqtt_publish [DeviceTopic] 2.50 Hello mqtt server!! 009
set plsh time [3000]
[[ tcp slowtmr (pcb->rtime >= pcb->rto) 6 >= 12, pcb-rtime 6 reset to 0
set plsh time [0]

[msg-out]
mqtt_publish [DeviceTopic] 2.50 Hello mqtt server!! 010
set plsh time [3000]
set plsh time [0]

[msg-out]
mqtt_publish [DeviceTopic] 2.50 Hello mqtt server!! 011
set plsh time [3000]
[[ tcp slowtmr (pcb->rtime >= pcb->rto) 12 >= 24, pcb-rtime 12 reset to 0
set plsh time [0]
_[mqtt_publish] mqtt_create_request = NULL
_at32_mqtt_publish: mqtt_publish err = Out of memory error.
?During MQTT-Publishing.. ,if mqtt_publish fail && 'flag_link' is true].nvic_system_reset()...
@example  mqtt_client
@version  AT32F4_DM9051_Lw212_mqtt_r2307_rc1.2
@date     2023-07-27
dm9051_init
mac: 00:60:6e:00:01:25
ip: 192.168.1.37
msk: 255.255.255.0
gw: 192.168.1.254
timeouts num          12

at32_mqtt_client: New!
dm9051 link up
mqttc-connecting ...

times: [  0] [250] [690] [690] 4
[[ tcp slowtmr (pcb->rtime >= pcb->rto) 10 >= 6, pcb-rtime 10 reset to 0
[[ tcp slowtmr (pcb->rtime >= pcb->rto) 10 >= 6, pcb-rtime 10 reset to 0

mqttc-connecting ...

times: [  0] [250] [690] [690] 4
[[ tcp slowtmr (pcb->rtime >= pcb->rto) 10 >= 6, pcb-rtime 10 reset to 0
[[ tcp slowtmr (pcb->rtime >= pcb->rto) 10 >= 6, pcb-rtime 10 reset to 0
mqttc-connecting ...

times: [  0] [250] [690] [690] 4
[[ tcp slowtmr (pcb->rtime >= pcb->rto) 10 >= 6, pcb-rtime 10 reset to 0
[[ tcp slowtmr (pcb->rtime >= pcb->rto) 10 >= 6, pcb-rtime 10 reset to 0
mqttc-connecting ...

times: [  0] [250] [690] [690] 4
[[ tcp slowtmr (pcb->rtime >= pcb->rto) 10 >= 6, pcb-rtime 10 reset to 0
[[ tcp slowtmr (pcb->rtime >= pcb->rto) 10 >= 6, pcb-rtime 10 reset to 0
mqttc-connecting ...

times: [  0] [250] [690] [690] 4
[[ tcp slowtmr (pcb->rtime >= pcb->rto) 10 >= 6, pcb-rtime 10 reset to 0
[[ tcp slowtmr (pcb->rtime >= pcb->rto) 10 >= 6, pcb-rtime 10 reset to 0
mqttc-connecting ...

times: [  0] [250] [690] [690] 4
[[ tcp slowtmr (pcb->rtime >= pcb->rto) 10 >= 6, pcb-rtime 10 reset to 0
[[ tcp slowtmr (pcb->rtime >= pcb->rto) 10 >= 6, pcb-rtime 10 reset to 0
...

(10) MQTT_CLIENT_PUBLISH_MS						3000  //.16000

[msg-out]
mqtt_publish [DeviceTopic] 2.50 Hello mqtt server!! 000
set plsh hdlr time [3000 ms]
service plsh hdlr, time 3000

[msg-out]
mqtt_publish [DeviceTopic] 2.50 Hello mqtt server!! 001
set plsh hdlr time [3000 ms]
service plsh hdlr, time 3000

[msg-out]
mqtt_publish [DeviceTopic] 2.50 Hello mqtt server!! 002
set plsh hdlr time [3000 ms]
service plsh hdlr, time 3000

[msg-out]
mqtt_publish [DeviceTopic] 2.50 Hello mqtt server!! 003
set plsh hdlr time [3000 ms]
service plsh hdlr, time 3000

[msg-out]
mqtt_publish [DeviceTopic] 2.50 Hello mqtt server!! 004
set plsh hdlr time [3000 ms]
service plsh hdlr, time 3000

[msg-out]
mqtt_publish [DeviceTopic] 2.50 Hello mqtt server!! 005
set plsh hdlr time [3000 ms]
service plsh hdlr, time 3000

[msg-out]
mqtt_publish [DeviceTopic] 2.50 Hello mqtt server!! 006
set plsh hdlr time [3000 ms]
service plsh hdlr, time 3000

[msg-out]
mqtt_publish [DeviceTopic] 2.50 Hello mqtt server!! 007
set plsh hdlr time [3000 ms]
service plsh hdlr, time 3000
_[mqtt_publish] mqtt_create_request = NULL, err_t = Out of memory error.

[_mqtt_close]
Connect Close, status = Connection closed. (reason MQTT_CONNECT_DISCONNECTED)
mqttc-connecting ...

times: [  0] [140] [520] [520] 4

[_mqtt_close]
Connect Close, status = Connection closed. (reason MQTT_CONNECT_DISCONNECTED)
mqttc-connecting ...


[_mqtt_close]
Connect Close, status = Connection closed. (reason MQTT_CONNECT_DISCONNECTED)
mqttc-connecting ...




(11) MQTT_CLIENT_PUBLISH_MS						16000  //.3000

[msg-out]
mqtt_publish [DeviceTopic] 2.50 Hello mqtt server!! 100
set plsh hdlr time [16000 ms]
service plsh hdlr, time 16000

[msg-out]
mqtt_publish [DeviceTopic] 2.50 Hello mqtt server!! 101
set plsh hdlr time [16000 ms]
service plsh hdlr, time 16000

[msg-out]
mqtt_publish [DeviceTopic] 2.50 Hello mqtt server!! 102
set plsh hdlr time [16000 ms]
_mqtt_client_pub_request_cb: c002: Publish FAIL, result = Timeout.
_take_mqtt_client_connect: connect mqtt Fail, Already connected.

service plsh hdlr, time 16000

[msg-out]
mqtt_publish [DeviceTopic] 2.50 Hello mqtt server!! 103
set plsh hdlr time [16000 ms]
_mqtt_client_pub_request_cb: c002: Publish FAIL, result = Timeout.
_take_mqtt_client_connect: connect mqtt Fail, Already connected.

service plsh hdlr, time 16000

[msg-out]
mqtt_publish [DeviceTopic] 2.50 Hello mqtt server!! 104
set plsh hdlr time [16000 ms]
_mqtt_client_pub_request_cb: c002: Publish FAIL, result = Timeout.
_take_mqtt_client_connect: connect mqtt Fail, Already connected.

service plsh hdlr, time 16000

[msg-out]
mqtt_publish [DeviceTopic] 2.50 Hello mqtt server!! 105
set plsh hdlr time [16000 ms]
_mqtt_client_pub_request_cb: c002: Publish FAIL, result = Timeout.
_take_mqtt_client_connect: connect mqtt Fail, Already connected.

service plsh hdlr, time 16000

[msg-out]
mqtt_publish [DeviceTopic] 2.50 Hello mqtt server!! 106
set plsh hdlr time [16000 ms]
_mqtt_client_pub_request_cb: c002: Publish FAIL, result = Timeout.
_take_mqtt_client_connect: connect mqtt Fail, Already connected.

service plsh hdlr, time 16000

[msg-out]
mqtt_publish [DeviceTopic] 2.50 Hello mqtt server!! 107
set plsh hdlr time [16000 ms]
_mqtt_client_pub_request_cb: c002: Publish FAIL, result = Timeout.
_take_mqtt_client_connect: connect mqtt Fail, Already connected.

service plsh hdlr, time 16000

[msg-out]
mqtt_publish [DeviceTopic] 2.50 Hello mqtt server!! 108
set plsh hdlr time [16000 ms]
_mqtt_client_pub_request_cb: c002: Publish FAIL, result = Timeout.
_take_mqtt_client_connect: connect mqtt Fail, Already connected.


[_mqtt_close]
Connect Close, status = Timeout. (reason MQTT_CONNECT_TIMEOUT)
mqttc-connecting ...

times: [  0] [200] [220] [220] 4

[_mqtt_close]
Connect Close, status = Connection closed. (reason MQTT_CONNECT_DISCONNECTED)
mqttc-connecting ...


[_mqtt_close]
Connect Close, status = Connection closed. (reason MQTT_CONNECT_DISCONNECTED)
mqttc-connecting ...


[_mqtt_close]
Connect Close, status = Connection closed. (reason MQTT_CONNECT_DISCONNECTED)
mqttc-connecting ...


(11-1)

[msg-out]
mqtt_publish [DeviceTopic] 2.50 Hello mqtt server!! 010
timeout_diff 30
[timeout_diff list] 30
set plsh hdlr time [16000 ms]
service plsh hdlr, time 16000
Create Request () 0 / 4 ok

[msg-out]
mqtt_publish [DeviceTopic] 2.50 Hello mqtt server!! 011
timeout_diff 30
[timeout_diff list] 30
set plsh hdlr time [16000 ms]
service plsh hdlr, time 16000
Create Request () 0 / 4 ok

[msg-out]
mqtt_publish [DeviceTopic] 2.50 Hello mqtt server!! 012
timeout_diff 30
[timeout_diff list] 30
set plsh hdlr time [16000 ms]
service plsh hdlr, time 16000
Create Request () 1 / 4 ok

[msg-out]
mqtt_publish [DeviceTopic] 2.50 Hello mqtt server!! 013
timeout_diff 15
[timeout_diff list] 15 15
set plsh hdlr time [16000 ms]
_mqtt_client_pub_request_cb: c002: Publish FAIL, result = Timeout.
_take_mqtt_client_connect: connect mqtt Fail, Already connected.

service plsh hdlr, time 16000
Create Request () 0 / 4 ok

[msg-out]
mqtt_publish [DeviceTopic] 2.50 Hello mqtt server!! 014
timeout_diff 20
[timeout_diff list] 10 20
set plsh hdlr time [16000 ms]
_mqtt_client_pub_request_cb: c002: Publish FAIL, result = Timeout.
_take_mqtt_client_connect: connect mqtt Fail, Already connected.

service plsh hdlr, time 16000
Create Request () 1 / 4 ok

[msg-out]
mqtt_publish [DeviceTopic] 2.50 Hello mqtt server!! 015
timeout_diff 15
[timeout_diff list] 15 15
set plsh hdlr time [16000 ms]
_mqtt_client_pub_request_cb: c002: Publish FAIL, result = Timeout.
_take_mqtt_client_connect: connect mqtt Fail, Already connected.

service plsh hdlr, time 16000
Create Request () 0 / 4 ok

[msg-out]
mqtt_publish [DeviceTopic] 2.50 Hello mqtt server!! 016
timeout_diff 15
[timeout_diff list] 15 15
set plsh hdlr time [16000 ms]
_mqtt_client_pub_request_cb: c002: Publish FAIL, result = Timeout.
_take_mqtt_client_connect: connect mqtt Fail, Already connected.

service plsh hdlr, time 16000
Create Request () 1 / 4 ok

[msg-out]
mqtt_publish [DeviceTopic] 2.50 Hello mqtt server!! 017
timeout_diff 15
[timeout_diff list] 15 15
set plsh hdlr time [16000 ms]
_mqtt_client_pub_request_cb: c002: Publish FAIL, result = Timeout.
_take_mqtt_client_connect: connect mqtt Fail, Already connected.

service plsh hdlr, time 16000
Create Request () 0 / 4 ok

[msg-out]
mqtt_publish [DeviceTopic] 2.50 Hello mqtt server!! 018
timeout_diff 15
[timeout_diff list] 15 15
set plsh hdlr time [16000 ms]

_mqtt_client_pub_request_cb: c002: Publish FAIL, result = Timeout.
_take_mqtt_client_connect: connect mqtt Fail, Already connected.

service plsh hdlr, time 16000
Create Request () 1 / 4 ok

[msg-out]
mqtt_publish [DeviceTopic] 2.50 Hello mqtt server!! 019
timeout_diff 20
[timeout_diff list] 10 20
set plsh hdlr time [16000 ms]

_mqtt_client_pub_request_cb: c002: Publish FAIL, result = Timeout.
_take_mqtt_client_connect: connect mqtt Fail, Already connected.


[_mqtt_close]
Connect Close, status = Timeout. (reason MQTT_CONNECT_TIMEOUT)
mqttc-connecting ...

times: [100] [150] [520] [520] 4

[_mqtt_close]
Connect Close, status = Connection closed. (reason MQTT_CONNECT_DISCONNECTED)
mqttc-connecting ...

