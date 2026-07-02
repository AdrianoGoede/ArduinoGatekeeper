CREATE TABLE users (
    id SERIAL NOT NULL PRIMARY KEY,
    card_id VARCHAR(10) UNIQUE NOT NULL,
    label VARCHAR(200) NOT NULL,
    authorized BOOLEAN DEFAULT true,
    created_at TIMESTAMPTZ NOT NULL DEFAULT now(),
    revoked_at TIMESTAMPTZ
);

CREATE TABLE doors (
    id SERIAL NOT NULL PRIMARY KEY,
    label VARCHAR(100) NOT NULL,
    created_at TIMESTAMPTZ NOT NULL DEFAULT now(),
    revoked_at TIMESTAMPTZ
);

CREATE TABLE access_log (
    id BIGSERIAL NOT NULL PRIMARY KEY,
    user_id INT NOT NULL,
    door_id INT NOT NULL,
    granted BOOLEAN NOT NULL,
    scanned_at TIMESTAMPTZ NOT NULL DEFAULT now(),
    FOREIGN KEY (user_id) REFERENCES users (id),
    FOREIGN KEY (door_id) REFERENCES doors (id)
);