

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">

<html>
<head>
    <title><?php echo DVR_COMPANY_NAME.' '.DVR_DVR.PAGE_HEADER_SEPARATOR.PAGE_HEADER_VIEWER; ?></title>
    <link rel="stylesheet" href="/template/styles.viewer.css">
    <script type="text/javascript" src="./lib/mootools.js"></script>
    <script type="text/javascript" src="./lib/mootools-more.js"></script>
    <script type="text/javascript" src="./lib/mootools-plugins.js"></script>
    <script type="text/javascript" src="./lib/js/viewer.js"></script>
    <script type="text/javascript" src="/vars"></script>
    <script type="text/javascript" src="//cdn.jsdelivr.net/hls.js/latest/hls.min.js"></script>
    <script type="text/javascript">
        var session = <?php echo json_encode($_SESSION['layouts']); ?>;
    </script>
</head>
<body>
<div>
    <ul id="layoutsMenu">
        <li><a class="disabled"><?php echo CURR.': '.( ( !empty($_COOKIE['currentLayout']) ) ? $_COOKIE['currentLayout'] : NONE_SELECTED); ?></a></li>
        <li><a href="#save" class="save"><?php echo SAVE;?></a></li>
        <li class="separator"><a href="#saveAs" class="saveAs"><?php echo SAVE_AS;?></a></li>
        <li class="separator"><a href="#delete" class="delete"><?php echo DELETE_CAM; ?></a></li>
        <li class="separator"><a href="#clearAll" class="clearLyout"><?php echo CLEAR_LAYOUT; ?></a></li>
        <?php
        $layouts =  (!empty($lv->layout_list[0])) ? $lv->layout_list : array(NO_SAVED_LAYOUTS);
        foreach ($layouts as $key => $layout){
            echo "<li>".(($layout==NO_SAVED_LAYOUTS) ? "<a class='disabled'>{$layouts[0]}</a>" : "<a href='#load' class='{".htmlspecialchars($layout['layout_name'])."}'>{".htmlspecialchars($layout['layout_name'])."}")."</a></li>";
        }

        ?>
    </ul>
    <ul id="cameraList">
        <?php
        foreach($lv->devices as $key => $device){
            if (!in_array($device['id'], $lv->access_list))
                echo "<li><a href='#loadCam' id='{$device['id']}' class='".((!empty($device['ptz_control_path'])) ? "ptz" : "")."'>{$device['device_name']}</a></li>";
        };
        ?>
    </ul>
    <?php
    foreach($lv->devices as $key => $device){
        if (!empty($device['ptz_serial_values']) && !empty($device['presets'])){
            $tmp = "<ul id='presets-{$device['id']}' class='presetsList'><li><a href='#map' class='mapPreset' id='{$device['id']}'>".MAP_PRESET."</a></li>";
            foreach ($device['presets'] as $key => $preset){
                $tmp .= "<li><a href='#goto' name='{$device['id']}' id='{$preset['preset_id']}' class='loadPreset'>{$preset['preset_name']}</a><a href='#rename' name='{$device['id']}'  class='renamePreset' id='{$preset['preset_id']}'></a><a href='#delete' name='{$device['id']}' class='deletePreset' id='{$preset['preset_id']}'></a></li>";
            }
            $tmp .= "</ul>";
            print $tmp;
        }
    }
    ?>
    <div id='leftMenu'>
        <div id='logout'><?php echo LOGOUT; ?></div>
        <div id='profilePage' <?php echo (($current_user->info['access_setup']) ? 'style="display: none;"' : ''); ?>>Profile</div>
        <?php if ($current_user->info['access_setup']) { echo "<div id='backToAdmin'>".BACK_TO_ADMIN."</div>"; }; ?>
        <div id="playbackPage">
            Playback
        </div>
        <div id='layoutsControl'>
            <?php echo LAYOUTS;?>
        </div>
        <div id='gridAdjustments'>
            <ul>
                <li class='l1'></li>
                <li class='l4'></li>
                <li class='l9'></li>
                <li class='l16'></li>
                <li class='ar'></li>
                <li class='dr'></li>
                <li class='ac'></li>
                <li class='dc'></li>
            </ul>
        </div>
    </div>
    <input type="hidden" id='video_method' value='<?php echo $lv->video_method; ?>' />
    <div id='liveViewContainer'></div>
</div>
</body>
</html>
