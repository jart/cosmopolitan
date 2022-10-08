CREATE TABLE land (
    ip INTEGER PRIMARY KEY,
    nick TEXT NOT NULL,
    created INTEGER
);

CREATE INDEX land_by_name ON land (nick);
CREATE INDEX land_by_created ON land (created DESC) WHERE created NOT NULL;
