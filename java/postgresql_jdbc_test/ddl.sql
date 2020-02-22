CREATE TABLE entity_table (
    guid    uuid   NOT NULL,
    version bigint NOT NULL,
    
    UNIQUE(guid, version)
);

CREATE TYPE entity_key AS (
    guid    uuid,
    version bigint
);
