[English](/README_en.md)

# 红龙加热台固件

![](/photos/1.jpg)
![](/photos/2.jpg)

QQ 交流群：1058528742

## 硬件方案

硬件开源地址：https://oshwhub.com/elisaday/high-color-value-low-cost-reflow

- 主控 STC8H1K08，返回按钮外加 EC11 旋转编码器的操作方式
- 温度采样使用 PT1000 热敏电阻
- CNC 加工的 3.5mm 厚度的 6061 铝板
- 散热风扇为 6 x 6 x 2.5cm，12V 0.3A
- 两个 230 欧 15 x 70mm 的 MCH 陶瓷加热片
- 电源为 220V 市电，系统额定功率 420W

## 软件部分

- 多级中文菜单
- 支持恒温模式、回流焊模式
- 恒温模式支持设定温度
- 回流焊模式支持各阶段温度和时间设定
- 温控 PID 算法参数可设置
