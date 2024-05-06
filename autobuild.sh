# 一键编译脚本，执行这个脚本可以一键编译整个项目
#!/bin/bash

# 更新apt包列表
sudo apt update

# 安装 git 
sudo apt install git -y

# 安装 make 命令
sudo apt install make -y

# 安装gcc工具链
sudo apt install -y build-essential

# 安装CMake
sudo apt install -y cmake

# 安装mysql 数据库并且配置信息用户信息，配置一个 root 用户，密码为123456
sudo apt-get install -y mysql-server 
sudo apt-get install -y libmysqlclient-dev

#对于全新安装，您需要运行包含的安全脚本。这会更改一些不太安全的默认选项，
#例如远程 root 登录和示例用户。在旧版本的 MySQL 上，您还需要手动初始化数据目录，但现在这是自动完成的
#这将带您完成一系列提示，您可以在其中对 MySQL 安装的安全选项进行一些更改。第一个提示会询问您是否要设置验证密码插件，该插件可用于测试您的 MySQL 密码的强度。
#@无论您如何选择，下一个提示都是为 MySQL root 用户设置密码。输入并确认您选择的安全密码。
#从那里，您可以按Y，然后ENTER接受所有后续问题的默认值。这将删除一些匿名用户和测试数据库，禁用远程 root 登录，并加载这些新规则，
# 以便 MySQL 立即尊重您所做的更改。
sudo mysql_secure_installation 
# 按照自己的喜爱进行选择

echo "自己配置mysql的用户"
# 配置mysql的基本信息
sudo mysql < ./thirdparty/config.sql

# 查看此时的mysql的状态
systemctl status mysql 

# 提示用户是否继续执行脚本
read -p "是否继续执行脚本？(Y/N): " choice

# 判断用户输入
if [ "$choice" == "Y" ] || [ "$choice" == "y" ]; then
    echo "MYSQL配置完成 , 继续执行脚本..."
else
    echo "脚本终止,先把MYSQL的配置问题解决....."
    exit 0
fi

# 向mysql 数据库中导入表格
# 提示用户输入MySQL用户名和密码
read -p "请输入MySQL用户名: " username
read -s -p "请输入MySQL密码: " password

# 执行主SQL脚本文件
mysql -u $username -p$password < ./thirdparty/table.sql
# mysql: [Warning] Using a password on the command line interface can be insecure. 这个错误不必理会

echo "MYSQL数据库已经安装好了,并且项目所需的mysql中的表结构也已经配置.......\n"



# 安装 boost 库
# 创建临时目录
sudo mkdir -p /tmp/boost_install
sudo cd /tmp/boost_install

# 下载Boost源码库
sudo wget  https://boostorg.jfrog.io/artifactory/main/release/1.85.0/source/boost_1_85_0.tar.gz
sudo tar -xzf boost_1_85_0.tar.gz

# 进入Boost源码目录
sudo cd boost_1_85_0

# 配置Boost编译选项
sudo ./bootstrap.sh --prefix=/usr/local

# 编译Boost
sudo ./b2 install


# 清理临时目录
sudo cd /tmp
sudo rm -rf /tmp/boost_install




# 定义muduo库的下载地址和版本号
MUDUO_URL="https://github.com/chenshuo/muduo.git"
MUDUO_VERSION="master"
# 创建一个临时目录用于存放源码和编译文件
TEMP_DIR=$(mktemp -d)
cd $TEMP_DIR

# 下载muduo库的源码
git clone $MUDUO_URL
cd muduo
git checkout $MUDUO_VERSION

# 创建一个build目录用于编译
mkdir build
cd build

# 使用cmake进行配置
cmake ..

# 编译并安装muduo库
make
sudo make install

# 清理临时目录
rm -rf $TEMP_DIR

echo "muduo库已成功下载、编译并安装"



# 安装 nginx + tcp 负载均衡模块
sudo mkdir -p /tmp/nginx_install
cd  /tmp/nginx_install 


# 安装编译依赖
apt-get install -y gcc make libpcre3 libpcre3-dev openssl zlib1g zlib1g-dev libgd-dev libgdal-dev 

# 下载nginx 源码
wget https://nginx.org/download/nginx-1.22.1.tar.gz

# 解压源代码并进入目录
tar -zxvf nginx-1.22.1.tar.gz

#进入下载好的nginx源码目录
cd nginx-1.22.1

# 配置编译选项
./configure --with-stream --prefix=/usr/local/nginx 

# 编译和安装
make && make install

# 启动 Nginx 服务
cd /usr/local/nginx/sbin
./nginx

# 清理临时目录
cd /tmp/
sudo rm -rf /tmp/nginx_install 

# 验证安装
echo "Nginx 安装完成" 

set -x


mkdir ./build
rm -rf `pwd`/build/*
cd `pwd`/build &&
	cmake .. &&
	make