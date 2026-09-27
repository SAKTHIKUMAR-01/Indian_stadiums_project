-- Indian Stadium Management System
-- Sample Seed Data

USE indian_stadium_db;

INSERT INTO stadiums (stadium_name, city, state, capacity, sport, home_team, opened_year, surface) VALUES
('Narendra Modi Stadium', 'Ahmedabad', 'Gujarat', 132000, 'Cricket', 'Gujarat Titans', 1983, 'Grass'),
('Eden Gardens', 'Kolkata', 'West Bengal', 68000, 'Cricket', 'Kolkata Knight Riders', 1864, 'Grass'),
('Naya Raipur International Cricket Stadium', 'Raipur', 'Chhattisgarh', 65000, 'Cricket', 'Chhattisgarh Cricket Team', 2008, 'Grass'),
('Jawaharlal Nehru Stadium', 'Kochi', 'Kerala', 60000, 'Multi-purpose', 'Kerala Blasters FC', 1996, 'Grass'),
('DY Patil Stadium', 'Navi Mumbai', 'Maharashtra', 55000, 'Cricket', 'Mumbai City FC', 2008, 'Turf'),
('Greenfield International Stadium', 'Thiruvananthapuram', 'Kerala', 55000, 'Multi-purpose', 'Kerala Cricket Team', 2015, 'Grass'),
('Bharat Ratna Shri Atal Bihari Vajpayee Ekana Cricket Stadium', 'Lucknow', 'Uttar Pradesh', 50000, 'Cricket', 'Lucknow Super Giants', 2017, 'Grass'),
('MA Chidambaram Stadium', 'Chennai', 'Tamil Nadu', 50000, 'Cricket', 'Chennai Super Kings', 1916, 'Grass'),
('M. Chinnaswamy Stadium', 'Bengaluru', 'Karnataka', 40000, 'Cricket', 'Royal Challengers Bengaluru', 1969, 'Grass'),
('Wankhede Stadium', 'Mumbai', 'Maharashtra', 33000, 'Cricket', 'Mumbai Indians', 1974, 'Grass');
