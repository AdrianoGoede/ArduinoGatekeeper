CREATE VIEW v_access_log AS
SELECT
    log.id AS log_id,
    CONCAT(usr.id, ' - ', usr.label) AS user,
    CONCAT(doo.id, ' - ', doo.label) AS door,
    log.granted AS granted,
    log.created_at AS created_at
FROM access_log log
JOIN users usr ON usr.id = log.user_id
JOIN doors doo ON doo.id = log.door_id
ORDER BY log.created_at ASC;