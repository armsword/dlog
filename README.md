## dlog

### 简介
dlog是一个线程安全、高性能的C++日志库。此日志库主要为了满足我日常造轮子需求以及学习交流之用，如果你需要一个轻量级的C++日志库时，dlog 也许可以满足你的需求。

### 编译
执行build.sh，在上层目录里会生成build文件夹，测试的可执行文件在release/bin目录下。

### 使用方法
将etc文件下的dlog.json扔到可执行文件的当前目录（当然只要能让可执行文件找到dlog.json即可），每个函数包含logger文件夹下的Log.h头文件，在主函数里调用DLOG_INIT初始化一次，之后在每个需要打印log的文件里调用DLOG_LOG即可。
如实例所示：

```
#include <dlog/logger/Log.h>
#include <unistd.h>
using namespace dlog::logger;

int main() {
    DLOG_INIT();

    DLOG_LOG(WARN, "test the log level using log lib!");
    DLOG_LOG(DEBUG, "Hello World, %d",2016);
    DLOG_LOG(INFO, "test Log C/C++ lib");
    DLOG_LOG(ERROR, "Hello everyone, this is my blog: http://armsword.com/; Welcome to visit it,Thank you!");

    return 0;
}

```
### 输出样例
目录名：dlog.log.20160911.0

日志内容：

```

[2016-09-11 15:05:30.510] [WARN] [4246443808] [/home/Github/dlog/example/LogTest.cpp:8] [main] test the log level using log lib!
[2016-09-11 15:05:30.510] [DEBUG] [4246443808] [/home/Github/dlog/example/LogTest.cpp:9] [main] Hello World, 2016
[2016-09-11 15:05:30.510] [INFO] [4246443808] [/home/Github/dlog/example/LogTest.cpp:10] [main] test Log C/C++ lib
[2016-09-11 15:05:30.510] [ERROR] [4246443808] [/home/Github/dlog/example/LogTest.cpp:11] [main] Hello everyone, this is my blog: http://armsword.com/; Welcome to visit it,Thank you!

```
### 该日志库功能
- 包含四种日志级别，分别为WARN、DEBUG、INFO、ERROR，日志级别大小依次递增
- 可配置输出日志路径
- 可配置输出日志前缀
- 可定义输出的日志级别，默认DEBUG
- 可定义日志文件切分大小
- 支持多线程程序
- 可定义日志往磁盘刷新的方式
- 支持每天切换新的日志文件
- 支持log文件被删除时，从新建立日志文件

### dlog.json配置

```
{
    "log_path": "./log",    // 日志路径
    "log_prefix": "dlog",   // 日志前缀
    "log_level": "DEBUG",   // 输出日志级别，DEBUG表示大于等于DEBUG级别的日志都打印
    "max_file_size": 200,   // 日志切分大小，单位m
    "async_flush": true     // 日志往磁盘刷新方式，true表示异步，false表示同步，建议选择true
}

```
### 性能
在测试机上测试了下机器的硬盘真实io写速度
```
time dd if=/dev/zero of=test.dbf bs=8k count=300000 oflag=direct
记录了300000+0 的读入
记录了300000+0 的写出
2457600000字节(2.5 GB)已复制，38.0385 秒，64.6 MB/秒

注：oflag=direct 表示使用DirectIO方式，不使用文件系统的buffer等
```
而我用dlog写入3.5G（1000W条数据），用时大约61、62s，计算下来，写速度大约58M/s，与上面测试的磁盘io数据64.6 MB/s比起来，性能还算不错，当然由于时间问题，我测试还不够充分，以后有机会继续优化下再测试看下。

[一些性能优化方法](http://armsword.com/2016/09/10/a-high-performace-and-thread-safe-cpp-log-lib/)

