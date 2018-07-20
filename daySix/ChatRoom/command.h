#ifndef COMMAND_H
#define COMMAND_H

#define REGISTER    "1"
#define LOGIN       "2"

//Msg:聊天信息，UsrEnter:新用户进入，UsrLeft:用户退出，FileName:文件名，Refuse:拒绝接收文件
enum MsgType{Msg, UsrEnter, UsrLeft, FileName, Refuse};


#endif // COMMAND_H
