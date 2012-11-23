CREATE DATABASE IF NOT EXISTS `%DBNAME%` DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;

USE `%DBNAME%`;

CREATE TABLE `device` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(45) NOT NULL,
  PRIMARY KEY (`name`),
  UNIQUE KEY `id_UNIQUE` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;

CREATE TABLE `region` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(45) NOT NULL,
  `base` int(10) unsigned NOT NULL,
  PRIMARY KEY (`name`),
  UNIQUE KEY `id_UNIQUE` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;

CREATE TABLE `base` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(45) NOT NULL,
  `region` int(11) NOT NULL,
  `device` int(11) NOT NULL,
  `offset` int(10) unsigned NOT NULL,
  PRIMARY KEY (`name`,`region`,`device`),
  UNIQUE KEY `id_UNIQUE` (`id`),
  KEY `fk_register_base_1_idx` (`region`),
  KEY `fk_device_base_2_idx` (`device`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;

CREATE TABLE `base_mult` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(45) NOT NULL,
  `base` int(11) NOT NULL,
  `offset` int(10) unsigned NOT NULL,
  PRIMARY KEY (`name`,`base`),
  UNIQUE KEY `id_UNIQUE` (`id`),
  KEY `fk_base_mult_1_idx` (`base`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;

CREATE TABLE `register` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(45) NOT NULL,
  `base` int(11) NOT NULL,
  `device` int(11) NOT NULL,
  `offset` int(10) unsigned NOT NULL,
  `size` enum('32BITS','16BITS','8BITS') NOT NULL,
  `extra` varchar(45) DEFAULT '.',
  PRIMARY KEY (`name`,`device`,`base`),
  UNIQUE KEY `id_UNIQUE` (`id`),
  KEY `fk_register_2_idx1` (`base`),
  KEY `fk_register_2_idx` (`device`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;

CREATE TABLE `register_mult` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(45) NOT NULL,
  `register` int(11) NOT NULL,
  `offset` int(10) unsigned NOT NULL,
  PRIMARY KEY (`name`,`register`),
  UNIQUE KEY `id_UNIQUE` (`id`),
  KEY `fk_register_base_1_idx` (`register`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;

CREATE TABLE `field` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(45) NOT NULL,
  `register` int(11) NOT NULL,
  `shift` int(10) unsigned NOT NULL,
  `size` int(10) unsigned NOT NULL,
  `access` enum('R','W','RW','RS','RC_W1') NOT NULL,
  PRIMARY KEY (`name`,`register`),
  UNIQUE KEY `id_UNIQUE` (`id`),
  KEY `fk_field_1_idx` (`register`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;

CREATE TABLE `field_mult` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(45) NOT NULL,
  `field` int(11) NOT NULL,
  `shift` int(10) unsigned NOT NULL,
  PRIMARY KEY (`name`,`field`),
  UNIQUE KEY `id_UNIQUE` (`id`),
  KEY `fk_field_mult_1_idx` (`field`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;

CREATE TABLE `enum` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(45) NOT NULL,
  `field` int(11) NOT NULL,
  `value` text NOT NULL,
  PRIMARY KEY (`name`,`field`),
  UNIQUE KEY `id_UNIQUE` (`id`),
  KEY `fk_enum_1_idx` (`field`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;


ALTER TABLE `base`
  ADD CONSTRAINT `fk_base_1` FOREIGN KEY (`region`) REFERENCES `region` (`id`) ON DELETE NO ACTION ON UPDATE NO ACTION,
  ADD CONSTRAINT `fk_base_2` FOREIGN KEY (`device`) REFERENCES `device` (`id`) ON DELETE NO ACTION ON UPDATE NO ACTION;

ALTER TABLE `base_mult`
  ADD CONSTRAINT `fk_base_mult_1` FOREIGN KEY (`base`) REFERENCES `base` (`id`) ON DELETE NO ACTION ON UPDATE NO ACTION;

ALTER TABLE `register`
  ADD CONSTRAINT `fk_register_1` FOREIGN KEY (`base`) REFERENCES `base` (`id`) ON DELETE NO ACTION ON UPDATE NO ACTION,
  ADD CONSTRAINT `fk_register_2` FOREIGN KEY (`device`) REFERENCES `device` (`id`) ON DELETE NO ACTION ON UPDATE NO ACTION;

ALTER TABLE `register_mult`
  ADD CONSTRAINT `fk_register_mult_1` FOREIGN KEY (`register`) REFERENCES `register` (`id`) ON DELETE NO ACTION ON UPDATE NO ACTION;

ALTER TABLE `field`
  ADD CONSTRAINT `fk_field_1` FOREIGN KEY (`register`) REFERENCES `register` (`id`) ON DELETE NO ACTION ON UPDATE NO ACTION;

ALTER TABLE `field_mult`
  ADD CONSTRAINT `fk_field_mult_1` FOREIGN KEY (`field`) REFERENCES `field` (`id`) ON DELETE NO ACTION ON UPDATE NO ACTION;

ALTER TABLE `enum`
  ADD CONSTRAINT `fk_enum_1` FOREIGN KEY (`field`) REFERENCES `field` (`id`) ON DELETE NO ACTION ON UPDATE NO ACTION;
