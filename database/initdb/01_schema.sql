CREATE TABLE admins (
    id SERIAL NOT NULL PRIMARY KEY,
    label VARCHAR(200) NOT NULL,
    active BOOLEAN DEFAULT true,
    created_at TIMESTAMP NOT NULL DEFAULT now()
);

CREATE TABLE users (
    id SERIAL NOT NULL PRIMARY KEY,
    card_id VARCHAR(8) UNIQUE NOT NULL,
    label VARCHAR(200) NOT NULL,
    created_at TIMESTAMP NOT NULL DEFAULT now()
);

CREATE TABLE doors (
    id SERIAL NOT NULL PRIMARY KEY,
    label VARCHAR(100) NOT NULL,
    created_at TIMESTAMP NOT NULL DEFAULT now()
);

CREATE TABLE permissions (
    user_id INT NOT NULL,
    door_id INT NOT NULL,
    created_at TIMESTAMP NOT NULL DEFAULT now(),
    PRIMARY KEY (user_id, door_id),
    FOREIGN KEY (user_id) REFERENCES users (id),
    FOREIGN KEY (door_id) REFERENCES doors (id)
);

CREATE TABLE door_log (
    id BIGSERIAL NOT NULL PRIMARY KEY,
    door_id INT NOT NULL,
    online BOOLEAN NOT NULL,
    created_at TIMESTAMP NOT NULL DEFAULT now(),
    FOREIGN KEY (door_id) REFERENCES doors (id)
);

CREATE TABLE access_log (
    id BIGSERIAL NOT NULL PRIMARY KEY,
    user_id INT NOT NULL,
    door_id INT NOT NULL,
    granted BOOLEAN NOT NULL,
    created_at TIMESTAMP NOT NULL DEFAULT now(),
    FOREIGN KEY (user_id) REFERENCES users (id),
    FOREIGN KEY (door_id) REFERENCES doors (id)
);