# CppChatServer


如何添加脚本，从而在一台环境未配置或部分配置的linux机器上，当从github拉取该项目之后，执行该脚本，就直接将项目所依赖的环境，以及项目直接编译完成
项目所依赖的环境如下:
1. mysql数据库以及  mysql数据库表结构的导入 以及 mysql开发包的安装
2. cmake 工具
3. gcc 工具链
4. muduo网络库的编译与安装
5. nginx + tcp 负载均衡模块
6. redis 数据库
7. boost 
8. make 命令
9. 安装hiredis 开发包


本项目的自动脚本可以实现在ubuntu22.04虚拟机上一键编译，首先会自动安装以上所有的环境，之后再编译整个项目，其中需要配置mysql数据库的密码，完全凭借自己的喜好，并且自动建立了该项目所需要的表结构。

##### 注意，脚本并未配置nginx 的tcp 负载均衡，需要自行配置。编辑`usr/local/nginx/conf/nginx.cnf`文件 ，下面给出示例
```
stream{
        upstream MyServer {
                ip_hash ;   # 使用IP进行哈希计算的负载均衡算法，保证一个客户端多次连接的时候，连接的一定是同一台服务器
                server 127.0.0.1:6000 weight=1 max_fails=3 fail_timeout=30s;
                server 127.0.0.1:6002 weight=1 max_fails=3 fail_timeout=30s;
        #       server www.baidu.com:80;
        }
        server{
                proxy_connect_timeout 1s ;
                #proxy_timeout 3s;
                listen 8000 ;
                proxy_pass MyServer ;
                tcp_nodelay on;
        }
}
```
编辑好之后执行:
```
cd ../sbin
./nginx -s reload 
```
实现nginx的热启动。

### 注意:`autobuild.sh`的第178行的路径需要修改为你的CppChatServer的目录。


配置集群环境，对该项目进行测试:
1. 使用Ubuntu-22.04镜像 创建4台虚拟机，测试这四台虚拟机能相互通信
2. 安装git 并且从远程仓库拉取项目 ，执行一键配置脚本，编译项目。
3. 两台作为聊天服务器集群，两台作为客户端进行通信模拟, 如果通信成功，说明代码没得问题。 

### 主要技术点

+ 使用`muduo` 网络库作为项目的核心模块，提供高并发网络IO服务，解耦网络和业务模块代码
+ 使用`nlohmann`编写的C++`json.hpp`库，作为私有通信协议
+ 配置nginx基于tcp的负载均衡，实现聊天服务器的集群功能，提高后端的并发能力。

集群的优点:
```
高可伸缩性：服务器集群具有很强的可伸缩性。随着需求和负荷的增长，可以向集群系统添加更多的服务器。在这样的配置中，可以有多台服务器执行相同的应用和数据库操作。

高可用性：高可用性是指，在不需要操作者干预的情况下，防止系统发生故障或从故障中自动恢复的能力。通过把故障服务器上的应用程序转移到备份服务器上运行，集群系统能够把正常运行时间提高到大于99.9%，大大减少服务器和应用程序的停机时间。

高可管理性：系统管理员可以从远程管理一个、甚至一组集群，就好像在单机系统中一样。
```
+ 基于redis的发布-订阅功能，实现跨服务器的消息通信
+ 使用mysql关系型数据库作为项目数据的落地存储






#### 参考链接
[muduo网络库简介](https://zhuanlan.zhihu.com/p/495016351)

[nlohmann json的基本使用教程](https://www.jb51.net/article/261677.htm)

[Mysql C API](https://www.mysqlzh.com/api/50.html)

[Nginx Tcp 负载均衡](https://zhuanlan.zhihu.com/p/365910883)