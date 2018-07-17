## daySix
1. 今天再把重心转向客户端，我们都知道一般的聊天软件肯定都要账号和密码，我们今天就实现它
（但暂时不考虑和服务器之间的交互，这个问题留到daySeven）
2. 然后实现聊天客户端的升级（突然意识到聊天应该用UDP，不用经过服务器直接聊天，而注册登录验证用TCP）


### ChatRoom与Chat+mysql中的服务器一起运行:
#### (Chat+mysql是由之前的服务器加上了MySQL数据库的操作，还不完善，而且聊天室的聊天也不用经过服务器了，之后还会有大修改)
![](https://github.com/liu-jianhao/chatRoom/blob/master/daySix/ChatRoomV0.1.PNG)
