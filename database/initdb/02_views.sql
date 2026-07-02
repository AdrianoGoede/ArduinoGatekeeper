CREATE VIEW v_access_log AS
SELECT
    log.id AS log_id,
    CONCAT(usr.id, ' - ', usr.label) AS user,
    CONCAT(doo.id, ' - ', doo.label) AS door,
    granted AS granted,
    scanned_at AS scanned_at
FROM access_log log
JOIN users usr ON usr.id = log.user_id
JOIN doors doo ON doo.id = log.door_id
ORDER BY log.scanned_at ASC;