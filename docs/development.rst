***********************
Developer documentation
***********************

API

======================================

Using the Bluecherry API to control or view a camera requires knowing the device ID. Currently this can be found in the XML file.

Note: The XML file will only generate the device IDs that the user has access to. Make sure you have access to all cameras, or that you are using an administrator login if you plan on controlling all cameras.

``https://login:password@bluecherry-server/ajax/devices.php?XML=true&short=true``


Live viewing
------------

**RTSP (TCP) connection**

``rtsp://login:password@bluecherry-server:7002/live/$deviceid``

**Return Multi-part JPEG (MJPEG)**

Once you obtain the device id, you can obtain a multi-part JPEG image from this URL:

``https://login:password@bluecherry-server/media/mjpeg.php?id={camera_id}&multipart=true``

**Return single image (JPEG)**

Obtain one image from the server:
``https://login:password@bluecherry-server/media/mjpeg.php?id={camera_id}``

Controlling PTZ cameras
-----------------------

Below is a example URL for controlling a Pan / Tilt / Zoom camera through the Bluecherry API.

``https://login:password@bluecherry-server/media/ptz.php?id={camera_id}&command={command}&panspeed={speed}&tiltspeed={speed}&pan={pan_command}&tilt={tilt_command}&zoom={zoom-command}&duration={duration}&preset={preset_id}&name={preset_name}``

**Supported Pan / Tilt / Zoom commands:**

query – display XML with camera information/PTZ capabilities and presets move – pan/tilt/zoom using parameters stop – stop movement save – save present rename – rename a preset go – go to specified preset clear – clear preset

**Pan / Tilt / Zoom Variables:**

preset_id – id of the preset preset_name --new preset name speed: pan/tilt speed pan_command:(l)eft /®ight
tilt_command:(u)p /(d)own
zoom_command:(t)ight /(w)ide duration:for how long to execute command; -1 for continuous

Trigger recordings
------------------
Trigger recording:

``https://login:password@bluecherry-server:7001/media/trigger.php?camera_id={camera_id}&description=TEST``

 Camera must be set to Trigger (‘T’) in the recording schedule. Description will show in the logs that a trigger was recording with the description you inserted.

