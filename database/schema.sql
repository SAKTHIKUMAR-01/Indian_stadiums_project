-- Indian Stadium Management System
-- Database Schema Definition

CREATE DATABASE IF NOT EXISTS indian_stadium_db;
USE indian_stadium_db;

DROP TABLE IF EXISTS stadiums;

CREATE TABLE stadiums (
    stadium_id INT PRIMARY KEY AUTO_INCREMENT,
    stadium_name VARCHAR(150) NOT NULL,
    city VARCHAR(100) DEFAULT 'Unknown',
    state VARCHAR(100) DEFAULT 'Unknown',
    capacity INT DEFAULT 0,
    sport VARCHAR(100) DEFAULT 'Cricket',
    home_team VARCHAR(150) DEFAULT 'None',
    opened_year INT DEFAULT 0,
    surface VARCHAR(50) DEFAULT 'Grass'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
