### BrachTrace 

BrachTraceExe              示例程序
BranchTrace-x64plugin-wtl  x64dbg插件
BranchTrace-x64plugin      x64dbg插件(已弃用)
BranchTracerSys            核心驱动程序

#### 思路
1. 利用BTS将CPU的执行过程记录到内存中。
2. 硬件级别是没有线程这种概念的，所以需要想办法将某个线程与CPU关联。
3. 对Windows内核核心函数SwapContext挂钩，目标线程开始执行时开启BTS,目标线程被切则关闭BTS
4. 抓取到数据后，在x64dbg中实现可视化界面。
