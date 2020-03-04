# Snake_Game_STM32
贪吃蛇游戏-基于STM32

## 成品展示
![成品展示](https://github.com/Sunnycee99/Snake_Game_STM32/blob/master/%E6%88%90%E5%93%81%E5%B1%95%E7%A4%BA.gif)

## 显示屏符号说明
- 最左边上下8格为贪吃蛇游戏区域；
- **B**：最高游戏分数；
- **Len**：贪吃蛇当前长度；
- **Lv**：当前游戏难度（一共4个难度等级）。

## 按键操作说明
- **Key1** 长按：重新开始游戏并清除最高纪录；
- **Key1** 短按：重新开始游戏不清除记录；
仅 **Key1** 有长短按区别，其他两按键只有一个功能：
- **Key2**：更改游戏难度；
- **Key3**：游戏暂停/开始。

## 游戏主要功能
- 活动区域为8个字符方块；
- 蛇身长度显示；
- 可调难度等级；
- 记录最高游戏分数；
- 通过角度传感器控制蛇移动。

## 所需外设
- LCD1602（用于显示）；
- AT24C02（用于存储最高纪录）；
- 三个按键（用于暂停、开始等）；
- ADXL362加速度传感器（用于控制贪吃蛇移动）。

## 引脚对应图
详见 **onenet_development_kit_m6312_v3r2.pdf** 。
