# ExpressBleJoystick

从 [ExpressLRS](https://github.com/ExpressLRS/ExpressLRS)  blejoystick 分支精简而来，目的是实现用 esp32 蓝牙功能，将遥控器以蓝牙无线方式链接到电脑模拟器； 

目前仅实现了可用，代码还比较ugly，待重构整理； 


所需配件： 

esp32 开发板 一块，自行淘宝 ￥22
5-12v 降压 3.3v 降压板一块  ￥4
杜邦线 3根
合计约￥30以内

接线示意：

已x9dp 为例
```
（空）
（空）
（vcc）------（+）-------------(+)--------------(3v3)------------
              [  in bec out    ]                   [    esp32   ]
（GND）------（-）------------(gnd)-------------(gnd)          ]
（s.port）-----------------------------------  -(d13)-----------

既JR口 从上到下 ，依次使用vcc、gnd、s.port 三个插针，vcc、gnd 接bec 降压的 in 端 正负极，之后out端正负极接 esp32开发板的 3v3 和gnd 针脚， s.port线直接接esp32 的D13 针脚

```
启动成功后，在电脑添加蓝牙 “expresslrs ble joystick”

然后模拟器进行校准即可使用。目前测试成功的 模拟器有liftoff，其他尚未测试；