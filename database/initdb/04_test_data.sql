INSERT INTO admins (label, active) VALUES
('Some Admin', true),
('Some Other Admin', false);

INSERT INTO users (card_id, label) VALUES
('000000', 'Some User'),
('111111', 'Some Other User');

INSERT INTO doors (label) VALUES
('Door 1'),
('Door 2');

INSERT INTO permissions (user_id, door_id) VALUES
(1, 1),
(1, 2),
(2, 2);