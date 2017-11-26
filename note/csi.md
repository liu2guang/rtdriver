CSI sequences(控制序列):

CSI序列是ESC序列中一种, 其他序列不用去看了用不到。

以下是CSI序列中比较常用的序列:
1. CUU序列:CSI [n] A 上移n格, 默认为1
2. CUD序列:CSI [n] B 下移n格, 默认为1 
3. CUF序列:CSI [n] C 前移n格, 默认为1 
4. CUB序列:CSI [n] D 后移n格, 默认为1
5. CNL序列:CSI [n] E 移动到下n行, 默认为1, CSI 1 E没有效果!
6. CPL序列:CSI [n] F 移动到上n行, 默认为1, 没有效果!
7. CHA序列:CSI [n] F 移动n绝对位置, 默认为1, 没有效果!
8. 






