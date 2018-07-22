# chatRoom

## dayOne
1. 首先实现一个简单的聊天室服务器，基于之前所学的Linux网络编程的知识
2. 客户端可以自己实现，也可以直接使用nc命令或telnet命令来充当客户端

## dayTwo
学习qt，并实现一个能和服务端连接上并且能交流的一个TCP客户端demo

### Linux环境下运行的TCP客户端:
![](https://github.com/liu-jianhao/chatRoom/blob/master/dayTwo/communication.png)

### Windows7环境下运行的TCP客户端:
![](https://github.com/liu-jianhao/chatRoom/blob/master/dayTwo/communication2.png)

### 与dayOne实现的服务器一起运行:
![](https://github.com/liu-jianhao/chatRoom/blob/master/dayTwo/TcpClientV0.01.png)


## dayThree
学习qt，然后写出一个图形界面的客户端，暂时先不考虑没不美观、只考虑功能实现

### Linux环境下运行的客户端:
![](https://github.com/liu-jianhao/chatRoom/blob/master/dayThree/TcpClient.png)

### Windows7环境下运行的客户端:
![](https://github.com/liu-jianhao/chatRoom/blob/master/dayThree/TcpClient2.png)

### 与dayOne实现的服务器一起运行:
![](https://github.com/liu-jianhao/chatRoom/blob/master/dayThree/chatRoomV0.1.png)

## dayFour
今天把注意力放在服务器，之前写的服务器有点看不过去，现在写一个reactor模式的聊天服务器，这样服务器的并发性能更好


## dayFive
今天继续修改dayFour写的服务器，加上日志的功能
为了方便（偷懒），日志采用开源库[spdlog](https://github.com/gabime/spdlog/)


## daySix
今天再把重心转向客户端，我们都知道一般的聊天软件肯定都要账号和密码，我们今天就实现它

### ChatRoom与Chat+mysql中的服务器一起运行:
#### (Chat+MySQL是由之前的服务器加上了MySQL数据库的操作，还不完善)
![](https://github.com/liu-jianhao/chatRoom/blob/master/daySix/ChatRoomV0.1.PNG)


## 注意！
1. 在dayFive时添加的依赖库要自行添加，不然直接make会报错
2. 在客户端中要注意可能要修改IP地址，这取决于你怎么实验，我的服务器端已经部署到阿里云服务器上了，IP地址为120.79.214.120
3. 由于我还在学习当中，代码可能不怎么好看，而且客户端实在有些难看。不过如果你喜欢的话，可以顺手点一下star。
