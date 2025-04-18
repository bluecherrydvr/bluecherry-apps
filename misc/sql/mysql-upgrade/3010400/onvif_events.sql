-- Drop existing table if it exists
DROP TABLE IF EXISTS OnvifEvents;

-- Create new table with updated structure
CREATE TABLE OnvifEvents (
    id INT AUTO_INCREMENT PRIMARY KEY,
    device_id INT NOT NULL,
    event_time DATETIME NOT NULL,
    onvif_topic VARCHAR(255) NOT NULL,
    action VARCHAR(15) NOT NULL,
    INDEX idx_device_time (device_id, event_time),
    FOREIGN KEY (device_id) REFERENCES Devices(id) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Add new columns to Devices table for ONVIF event configuration
ALTER TABLE Devices 
ADD COLUMN onvif_event_types TEXT DEFAULT NULL COMMENT 'Comma-separated list of enabled ONVIF event types',
ADD COLUMN onvif_events_enabled TINYINT(1) NOT NULL DEFAULT 0,
ADD COLUMN onvif_port INT NOT NULL DEFAULT 80; 