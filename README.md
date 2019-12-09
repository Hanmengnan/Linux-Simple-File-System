## Linux 简单文件系统实现

<!--与实际Linux的文件系统的实现方法存在差异，实现的功能也很少，只是从自己的理解层面和实现的难易角度出发，实现了一个简单的系统-->

### 前言

[借鉴于Linux文件系统的实现](https://www.cnblogs.com/pipci/p/10179502.html) 

#### 存储设备分区

文件系统的最终目的是把大量数据有组织的放入持久性的存储设备中，比如硬盘和磁盘。这些存储设备与内存不同。它们的存储能力具有持久性，不会因为断电而消失；存储量大，但读取速度慢。



观察常见存储设备。最开始的区域是MBR，用于Linux开机启动(参考[Linux开机启动](http://www.cnblogs.com/vamei/archive/2012/09/05/2672039.html))。剩余的空间可能分成数个分区(partition)。每个分区有一个相关的分区表(Partition table)，记录分区的相关信息。这个分区表是储存在分区之外的。分区表说明了对应分区的起始位置和分区的大小。


![img](https://images0.cnblogs.com/blog/413416/201402/251719082508830.png)

 

我们在Windows系统常常看到C分区、D分区等。Linux系统下也可以有多个分区，但都被挂载在同一个文件系统树上。

数据被存入到某个分区中。一个典型的Linux分区包含有下面各个部分:

![img](https://images0.cnblogs.com/blog/413416/201402/250221581092754.png)

 

分区的第一个部分是启动区(Boot block)，它主要是为计算机开机服务的。[Linux开机启动](http://www.cnblogs.com/vamei/archive/2012/09/05/2672039.html)后，会首先载入MBR，随后MBR从某个硬盘的启动区加载程序。该程序负责进一步的操作系统的加载和启动。为了方便管理，即使某个分区中没有安装操作系统，Linux也会在该分区预留启动区。

启动区之后的是超级区(Super block)。它存储有文件系统的相关信息，包括文件系统的类型，inode的数目，数据块的数目。

随后是多个inodes，它们是实现文件存储的关键。在Linux系统中，一个文件可以分成几个数据块存储，就好像是分散在各地的龙珠一样。为了顺利的收集齐龙珠，我们需要一个“雷达”的指引：该文件对应的inode。每个文件对应一个inode。这个inode中包含多个指针，指向属于该文件各个数据块。当操作系统需要读取文件时，只需要对应inode的"地图"，收集起分散的数据块，就可以收获我们的文件了。

#### inode简介

正如上一节中提到的，inode储存由一些指针，这些指针指向存储设备中的一些数据块，文件的内容就储存在这些数据块中。当Linux想要打开一个文件时，只需要找到文件对应的inode，然后沿着指针，将所有的数据块收集起来，就可以在内存中组成一个文件的数据了。

![img](https://images0.cnblogs.com/blog/413416/201402/251115315292334.png)

 																			    

### inode示例 

在Linux中，我们通过解析路径，根据沿途的目录文件来找到某个文件。目录中的条目除了所包含的文件名，还有对应的inode编号。当我们输入$cat /var/test.txt时，Linux将在根目录文件中找到var这个目录文件的inode编号，然后根据inode合成var的数据。随后，根据var中的记录，找到text.txt的inode编号，沿着inode中的指针，收集数据块，合成text.txt的数据。整个过程中，我们参考了三个inode：根目录文件，var目录文件，text.txt文件的inodes。

在Linux下，可以使用$stat filename，来查询某个文件对应的inode编号。

​                                               ![img](https://images0.cnblogs.com/blog/413416/201402/251216497524884.png)

在存储设备中实际上存储为：

![img](https://images0.cnblogs.com/blog/413416/201402/252007477914143.png)

 

当我们读取一个文件时，实际上是在目录中找到了这个文件的inode编号，然后根据inode的指针，把数据块组合起来，放入内存供进一步的处理。当我们写入一个文件时，是分配一个空白inode给该文件，将其inode编号记入该文件所属的目录，然后选取空白的数据块，让inode的指针指像这些数据块，并放入内存中的数据。



### 文件系统设计

#### 运行效果
![5.png](https://img.hacpai.com/file/2019/12/5-196f7776.png)
![6.png](https://img.hacpai.com/file/2019/12/6-9808a910.png)



#### 系统层次

![image20191208161446285.png](https://img.hacpai.com/file/2019/12/image20191208161446285-60143a6a.png)


如图所示，文件系统的主要数据结构为“森林”，由于Linux系统没有盘符的概念，默认的首层目录即存在文件，故森林中的每一棵树为首层目录下的文件夹及文件。并且每棵树之间靠**兄弟指针**相互关联，但此处的指针为单向指针，即由该层中的**第一个节点**（文件/文件夹）出发，依次指向该层中后一个节点（文件/文件夹），也即第一个指向第二个，第二个指向第三个。

而不同层次之间通过父亲指针和儿子指针连接，父节点通过**子孙指针**指向下一层的首节点，而下一层的所有子节点通过**父亲指针**指向父节点。

#### 数据结构

```cpp
typedef struct item {
	char name[10];//文件名
	int type;//文件类型（文件/文件夹）
	int size;//文件大小
	int code;//文件的状态码
	int  datab;//文件内容开始指针
	int datae;//文件内容结束指针
	int father;//父文件夹指针
	int son;//子目录指针
	int brother;//同级指针
}item;
```
![image20191208155545227.png](https://img.hacpai.com/file/2019/12/image20191208155545227-b610679a.png)


#### 实现功能

| login  | 用户登录 |
| :----: | :------: |
| logout | 拥护注销 |
| Create | 新建文件 |
| Delete | 删除文件 |
|  Open  | 打开文件 |
| Close  | 关闭文件 |
|  Read  |  读文件  |
| Write  |  写文件  |
|  Dir   |  列目录  |



#### 函数原型

- **void userlogin()** 用户登录
- **void userlogout()** 用户注销

- **void cd(string filepath)**  进入文件夹
- **void createfile(string filename)** 新建文件/文件夹
- **void removefile(string filename**移除文件/文件夹
- **void fileopen(string filename)** 文件打开
- **void fileclose(string filename** 文件关闭
- **void readf(string filename** 读文件内容
- **void writef(string data, string filename)** 写文件内容
- **void dir()** 列出文件目录
- **int search(string file)** 在该层目录中搜索文件
- **int find_writeposition(** 寻找下一个写入位置
- *void init(** 初始化磁盘（提前建好一些文件）

#### 功能实现

虚拟文件系统通过两个二进制文件实现，**disk**与**data**，disk用于记录文件系统的层次结构，data文件用于记录文件内容。文件指针为数据在文件中的偏移量，通过移动读写的位置，可以将目标数据读出与写入。

用全局变量**userstate**标识用户当前状态，未登录的状态下无法进行进一步操作。

用全局变量**currentcd**记录目前所处的层次信息，该值为某一层首节点的偏移量。currentcd为负时意味着进入的空文件夹，而currentcd表示的为已进入文件夹信息的偏移量。

用全局变量**dataposition**指示data文件继续写入的位置，为文件读写提供服务，每次程序运行时初始化为data文件的末尾，并随着文件写入持续增加。

##### CD命令

![1.png](https://img.hacpai.com/file/2019/12/1-59a2425f.png)


##### removef命令
![批注20191208223344.png](https://img.hacpai.com/file/2019/12/批注20191208223344-f6edf591.png)


##### createf 命令
![批注20191208223713.png](https://img.hacpai.com/file/2019/12/批注20191208223713-7be0ad2c.png)

