# 迷宫游戏

## 迷宫游戏0.1
2019/8/29

### todo:
#### 1. 从之前的代码基础上加入显示功能
遇到了一些错误: 
1. keil中没有引入.c文件, 导致下载程序进去毫无反应
2. 头文件#"reg52.h"和"STC15F2K60S2.H"时常混淆

#### 2. 加入迷宫生成逻辑, 并在oled上显示
没办法了, 好好的代码在keil上编译就错了, 奇怪奇怪奇怪...

#### 3. 人物点移动逻辑, 闪烁
1. 实现了闪烁
2. 实现了4像素点移动

#### 4. 触碰警示逻辑
1. 实现了触碰警示
2. 实现了到达终点播放连续声音


#### 5. 另一种迷宫生成算法(可能在小oled上显示效果更好)
1. 实现递归分割法地图, 效果确实更好

---

## 迷宫游戏0.2
2019/8/30

### todo:
1. 添加key1切换游戏模式
2. key1 模式条件: 游戏模式声音