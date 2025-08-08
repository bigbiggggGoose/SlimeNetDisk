-- 1. 创建数据库
CREATE DATABASE IF NOT EXISTS `clouddisk`
  CHARACTER SET utf8mb4
  COLLATE utf8mb4_unicode_ci;
USE `clouddisk`;

-- 2. 用户表
DROP TABLE IF EXISTS `user_info`;
CREATE TABLE `user_info` (
  `id`           INT          NOT NULL AUTO_INCREMENT,
  `user_name`    VARCHAR(64)  NOT NULL,
  `nick_name`    VARCHAR(64)  NOT NULL,
  `password`     VARCHAR(255) NULL,
  `phone`        VARCHAR(32)  NULL,
  `email`        VARCHAR(128) NULL,
  `create_time`  DATETIME     NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  UNIQUE KEY `ux_user_name` (`user_name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 3. 文件表
DROP TABLE IF EXISTS `file_info`;
CREATE TABLE `file_info` (
  `id`           INT          NOT NULL AUTO_INCREMENT,
  `user_id`      INT          NOT NULL,
  `file_name`    VARCHAR(255) NOT NULL,
  `md5`          CHAR(32)     NOT NULL,
  `size`         BIGINT       NOT NULL,
  `url`          VARCHAR(512) NOT NULL,
  `type`         VARCHAR(64)  NULL,
  `share_status` TINYINT      NOT NULL DEFAULT 0,
  `share_time`   TIMESTAMP    NULL,
  `pv`           INT          NOT NULL DEFAULT 0,
  `create_time`  DATETIME     NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  KEY `idx_user_id` (`user_id`),
  KEY `idx_md5`     (`md5`),
  CONSTRAINT `fk_file_user`
    FOREIGN KEY (`user_id`) REFERENCES `user_info`(`id`)
      ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
