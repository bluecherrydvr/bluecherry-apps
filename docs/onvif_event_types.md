# Adding New ONVIF Event Types

This document outlines the process for adding support for new ONVIF event types in the Bluecherry DVR system.

## Overview

The Bluecherry DVR system supports ONVIF events from IP cameras. The system is designed to be extensible, allowing new event types to be added as they become available in the ONVIF standard or as manufacturers implement proprietary event types.

## Code Restrictions and Naming Conventions

When adding new ONVIF event types, keep the following restrictions in mind:

1. **Event Type Format**: Event types must follow these formatting rules:
   - Use single words with underscores for spaces (e.g., "smoke_detection" not "Smoke Detection")
   - All lowercase letters
   - No special characters except underscores
   - Example format: `43|smoke_detection|start`

2. **Special Characters**: Avoid using special characters in event type names, especially:
   - Apostrophes (`'`) - These can break SQL queries
   - Hyphens (`-`) - May cause issues in some contexts
   - Spaces - Use underscores instead (e.g., "smoke_detector" rather than "Smoke Detector")

3. **Case Sensitivity**: Event types are case-sensitive in the database. All event types should be lowercase.

4. **Length Limitations**: Keep event type names reasonably short (under 50 characters) to ensure compatibility with all database fields and UI elements.

5. **Database Constraints**: The `EventTypesCam` table uses the event type as a primary key, so each event type must be unique.

6. **Trigger File Format**: When writing to `/tmp/bluecherry_trigger`, the event type is used as a parameter. The format must be: `device_id|event_type|action`

7. **UI Display**: Consider how the event type will be displayed in the UI. You may want to add a mapping for display purposes if the database value is not user-friendly.

8. **File Naming**: Event types are used in recording file names. The system uses a simple format for file names:
   - Format: `HH-MM-SS-event_type.mp4`
   - Example: `14-30-45-motion.mp4`
   - The event type in the filename should be a simple, single word without underscores
   - Current supported event types in filenames: motion, vehicle, person, animal
   - When adding new event types, update the `media_file_path` function in `server/recorder.cpp` to handle the new event type

## Database Changes

When adding a new ONVIF event type, you need to:

1. Add the new event type to the `EventTypesCam` table:

```sql
INSERT INTO EventTypesCam VALUES ("new_event_type");
```

2. Create a new SQL upgrade script in the `misc/sql/mysql-upgrade` directory with an incremented version number (e.g., `3000014`).

Example script structure:
```bash
#!/bin/bash

echo '
-- Add new ONVIF event type
INSERT INTO EventTypesCam VALUES ("new_event_type");
' | mysql -h"$host" -D"$dbname" -u"$user" -p"$password"

exit 0
```

## Server Code Changes

The server code needs to be updated to handle the new event type:

1. Update the event type enumeration in the relevant header file (likely `server/onvif_events.h` or similar).

2. Add handling for the new event type in the event processing code.

3. Update any event type mapping or conversion functions.

## PHP/Web Interface Changes

The web interface needs to be updated to:

1. Display the new event type in the camera configuration page.

2. Allow users to enable/disable the new event type for each camera.

3. Update any event filtering or display code to properly handle the new event type.

## Testing

When adding a new ONVIF event type:

1. Test with a camera that supports the new event type.

2. Verify that events are properly recorded in the database.

3. Confirm that the web interface correctly displays and filters the new event type.

4. Ensure that the event triggers the appropriate actions (recording, notifications, etc.).

## Common ONVIF Event Types

The ONVIF standard defines several event types that cameras may support. Here are some common ones with their proper formatting:

- motion_detection
- tampering_detection
- audio_detection
- video_loss
- storage_failure
- network_loss
- person_detection
- animal_detection
- vehicle_detection
- line_crossing
- object_left
- object_removed
- object_counting
- face_detection
- license_plate_recognition
- audio_level_detection
- temperature_detection
- humidity_detection
- smoke_detection
- fire_detection
- water_detection

## Adding Manufacturer-Specific Event Types

Some manufacturers implement proprietary event types that extend the ONVIF standard. To add support for these:

1. Identify the event topic and payload structure from the manufacturer's documentation.

2. Add the event type to the database as described above, following the formatting rules.

3. Update the server code to parse and handle the specific event format.

4. Test with the manufacturer's camera to ensure proper event detection and handling.

## Troubleshooting

If events from a new event type are not being properly detected or processed:

1. Check the server logs for any errors or warnings.

2. Verify that the event type is correctly added to the `EventTypesCam` table.

3. Confirm that the camera is sending events in the expected format.

4. Use ONVIF testing tools to verify the camera's event capabilities.

5. Check that the event type is enabled for the camera in the database. 