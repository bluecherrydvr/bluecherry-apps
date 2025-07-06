#!/bin/bash

echo '
-- Add indexes for optimized cleanup system (3.1.13)
-- These indexes will significantly improve cleanup performance on large systems

-- Index for Media table cleanup queries (archive + start)
-- This is the most critical index for cleanup performance
CREATE INDEX IF NOT EXISTS idx_media_cleanup ON Media (archive, start);

-- Index for Media table filepath lookups
-- Used when marking files as archived
CREATE INDEX IF NOT EXISTS idx_media_filepath ON Media (filepath);

-- Index for EventsCam table media_id lookups
-- Used when marking associated events as archived
CREATE INDEX IF NOT EXISTS idx_eventscam_media_id ON EventsCam (media_id);

-- Index for Media table archive status
-- Used for counting and filtering archived files
CREATE INDEX IF NOT EXISTS idx_media_archive ON Media (archive);

-- Index for Media table start ordering
-- Used for getting oldest files first
CREATE INDEX IF NOT EXISTS idx_media_start ON Media (start);

-- Index for Storage table priority ordering
-- Used when checking storage thresholds
CREATE INDEX IF NOT EXISTS idx_storage_priority ON Storage (priority);

-- Add comments to document the indexes
-- Note: MySQL doesn'\''t support comments on indexes, so we'\''ll add them as table comments
ALTER TABLE Media COMMENT = '\''Media table with cleanup optimization indexes: idx_media_cleanup(archive,start), idx_media_filepath(filepath), idx_media_archive(archive), idx_media_start(start)'\'';
ALTER TABLE EventsCam COMMENT = '\''EventsCam table with cleanup optimization index: idx_eventscam_media_id(media_id)'\'';
ALTER TABLE Storage COMMENT = '\''Storage table with cleanup optimization index: idx_storage_priority(priority)'\'';
' | mysql -h"$host" -D"$dbname" -u"$user" -p"$password"

exit 0 