/* 用root用户登录系统，执行脚本*/
/*root pwd:123456*/
/*创建数据库*/
create database chat;

/*选择数据库*/
use chat;

/*增加用户chat_root*/
/*创建用户'chat_root'密码为'asdzxc123' 拥有操作数据库chat的所有权限*/
	create user char_root@"%" identified by 'asdzxc123';
	grant all privileges on chat.* to char_root@"%";
	flush privileges;

/* grant select,insert,update,delete on mydb61.* to dbuser61@localhost identified by "dbuser61";*/
/* grant select,insert,update,delete on mydb61.* to dbuser61@'%' identified by "dbuser61";*/

/*创建表*/

/*创建user表*/
DROP TABLE IF EXISTS `user`;
CREATE TABLE IF NOT EXISTS `user`(
	`id` int PRIMARY KEY AUTO_INCREMENT comment '用户id',
	`name`	varchar(50)	NOT NULL UNIQUE comment '用户名',
	`password` varchar(50) NOT NULL comment '用户密码',
	`state`	enum('online','offline') DEFAULT 'offline' comment '当前登录状态'
); 
commit;

/*创建friend表*/
DROP TABLE IF EXISTS `friend`;
CREATE TABLE IF NOT EXISTS `friend`(
	`userid` int NOT NULL comment '用户id',
	`friendid` int NOT NULL comment '朋友id',
	PRIMARY KEY(`userid`,`friendid`)
);
commit;

/*创建allgroup表*/
DROP TABLE IF EXISTS `allgroup`;
CREATE TABLE IF NOT EXISTS `allgroup`(
	`id` int PRIMARY KEY AUTO_INCREMENT comment '组id',
	`groupname` varchar(50) NOT NULL UNIQUE comment '组名称',
	`groupdesc` varchar(200) DEFAULT '' comment '组功能描述'
);
commit;

/*创建groupuser表*/
DROP TABLE IF EXISTS `groupuser`;
CREATE TABLE IF NOT EXISTS `groupuser`(
	`groupid` int NOT NULL comment '组id',
	`userid` int NOT NULL comment '用户id',
	`grouprole`	enum('creator','normal') DEFAULT 'normal' comment '组内角色',
	PRIMARY KEY(`groupid`,`userid`)
);
commit;

/*创建offlinemessage表*/
DROP TABLE IF EXISTS `offlinemessage`;
CREATE TABLE IF NOT EXISTS `offlinemessage`(
	`userid` int PRIMARY KEY NOT NULL comment '用户id',
	`message` varchar(500) NOT NULL comment '离线消息'
); 
commit;

	
	
	
	
	