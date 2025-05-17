bc_event_cam_t bc_event_cam_start(int id, time_t start_ts,
                                  bc_event_level_t level,
                                  bc_event_cam_type_t type,
                                  const char *media_file)
{
	// Debug output for event creation
	fprintf(stderr, "DEBUG: bc_event_cam_start called with: id=%d, level=%d, type=%d, media_file=%s\n", 
		id, level, type, media_file ? media_file : "NULL");
	fprintf(stderr, "DEBUG: Looking up event type string: '%s'\n", cam_type_to_str[type]);
	
	struct bc_event_cam *bce = (struct bc_event_cam*) malloc(sizeof(*bce));
	char media_id_str[24];
	int re;

	memset(bce, 0, sizeof(*bce));

	bce->id         = id;
	bce->start_time = start_ts;
	bce->level      = level;
	bce->type       = type;

	fprintf(stderr, "DEBUG: Event structure initialized\n");

	if (bc_db_start_trans()) {
		fprintf(stderr, "DEBUG: Failed to start database transaction\n");
		free(bce);
		return NULL;
	}

	if (media_file) {
		fprintf(stderr, "DEBUG: Processing media file: %s\n", media_file);
		strlcpy(bce->media.filepath, media_file, PATH_MAX);

		re = __bc_db_query("INSERT INTO Media (start,device_id,filepath) "
		                   "VALUES(%lu,%d,'%s')", bce->start_time, bce->id,
		                   bce->media.filepath);
		if (re) {
			fprintf(stderr, "DEBUG: Failed to insert media record\n");
			goto error;
		}

		bce->media.table_id = bc_db_last_insert_rowid();

		/* XXX: Lost connection? */
		if (!bce->media.table_id) {
			fprintf(stderr, "DEBUG: Failed to get last insert rowid\n");
			goto error;
		}

		snprintf(media_id_str, sizeof(media_id_str), "%lu", bce->media.table_id);
	} else {
		strcpy(media_id_str, "NULL");
	}

	fprintf(stderr, "DEBUG: About to insert event with type string: '%s'\n", cam_type_to_str[bce->type]);

	re = __bc_db_query("INSERT INTO EventsCam (time,level_id,"
	                   "device_id,type_id,length,media_id) VALUES(%lu,'%s',%d,"
	                   "'%s',-1,%s)", bce->start_time, level_to_str[bce->level],
	                   bce->id, cam_type_to_str[bce->type], media_id_str);
	if (re) {
		fprintf(stderr, "DEBUG: Failed to insert event record: %d\n", re);
		goto error;
	}

	bce->inserted = bc_db_last_insert_rowid();
	fprintf(stderr, "DEBUG: Successfully inserted event: %lu\n", bce->inserted);

	if (bc_db_commit_trans()) {
		fprintf(stderr, "DEBUG: Failed to commit transaction\n");
	error:
		bc_db_rollback_trans();
		free(bce);
		return NULL;
	}

	return bce;
} 