-- 参考设置，将root 用户设置为可以远程登录，并且将root 用户的验证方式从默认的`auth_socket` 改为密码验证
-- 密码设置为hsa101610 [注: 一定要符合要求的密码强度]
SELECT user,authentication_string,plugin,host FROM mysql.user;
use mysql
update user set host = '%' where user = 'root';
#刷新cache中配置
flush privileges;
select host, user from user;
ALTER USER 'root'@'%' IDENTIFIED WITH MYSQL_NATIVE_PASSWORD BY 'hsa101610'    
exit