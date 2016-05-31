<div class="row">
    <div class="col-lg-12">
        <h1 class="page-header"><?php echo VA_HEADER; ?>
    
        <ol class="breadcrumb">
            <li><a href="/devices" class="ajax-content"><?php echo ALL_DEVICES; ?></a></li>
           <li class="active"><?php echo VIDEO_ADJ; ?>: <b><?php echo (($device_data[0]['device_name']) ? $device_data[0]['device_name'] : $device_data[0]['id']); ?></b></li>
        </ol>
        </h1>
    </div>
</div>

<div class="row">
    <div class="col-lg-12">
        
    <form action="/ajax/update.php" method="post" class="form-horizontal">
        <div class="form-group">
            <div class="col-lg-8 col-lg-offset-4 col-md-8 col-md-offset-4">
                <img id="liveViewImg" width='352' src='/media/mjpeg.php?id=<?php echo $_GET['id']; ?>' />
            </div>
        </div>
                    
        <?php if ($device_data[0]['driver'] != 'tw5864') { ?>
        <div class="form-group">
            <label class="col-lg-4 col-md-4 control-label"><?php echo VA_AUDIO_ENABLE; ?></label>

            <div class="col-lg-8 col-md-8 form-control-static">
                <input type="checkbox" name="audio_disabled" class="click-event" data-function="videoadjAudio" data-id="<?php echo $device_data[0]['id']; ?>" <?php echo !($device_data[0]['audio_disabled']) ? ' checked="checked"':''; ?>  />
            </div>
        </div>

        <div class="form-group">
            <div class="col-lg-12 col-md-12">
                <div class="alert alert-warning">Audio support is currently experimental. It may cause problems and numerous errors in the log files. Use at your own risk.</div>
            </div>
        </div>

        <div class="form-group">
            <label class="col-lg-4 control-label"><?php echo VA_AUDIO; ?></label>

            <div class="col-lg-8">
                <div class="bfh-slider" data-mode="update" data-id="<?php echo $device_data[0]['id']; ?>" data-name="audio_volume" data-value="<?php echo $device_data[0]['audio_volume']; ?>"></div>
            </div>
        </div>
        <?php } ?>

        <div class="form-group">
            <label class="col-lg-4 control-label"><?php echo VA_BRIGHTNESS; ?></label>

            <div class="col-lg-8">
                <div class="bfh-slider" data-id="<?php echo $device_data[0]['id']; ?>" data-name="brightness" data-value="<?php echo $device_data[0]['brightness']; ?>"></div>
            </div>
        </div>

        <div class="form-group">
            <label class="col-lg-4 control-label"><?php echo VA_HUE; ?></label>

            <div class="col-lg-8">
                <div class="bfh-slider" data-id="<?php echo $device_data[0]['id']; ?>" data-name="hue" data-value="<?php echo $device_data[0]['hue']; ?>"></div>
            </div>
        </div>

        <div class="form-group">
            <label class="col-lg-4 control-label"><?php echo VA_SATURATION; ?></label>

            <div class="col-lg-8">
                <div class="bfh-slider" data-id="<?php echo $device_data[0]['id']; ?>" data-name="saturation" data-value="<?php echo $device_data[0]['saturation']; ?>"></div>
            </div>
        </div>

        <div class="form-group">
            <label class="col-lg-4 control-label"><?php echo VA_CONTRAST; ?></label>

            <div class="col-lg-8">
                <div class="bfh-slider" data-id="<?php echo $device_data[0]['id']; ?>" data-name="contrast" data-value="<?php echo $device_data[0]['contrast']; ?>"></div>
            </div>
        </div>

        <div class="form-group">
            <label class="col-lg-4 control-label"><?php echo VA_VIDEO_QUALITY; ?></label>

            <div class="col-lg-8">
                <div class="bfh-slider" data-id="<?php echo $device_data[0]['id']; ?>" data-name="video_quality" data-value="<?php echo $device_data[0]['video_quality']; ?>"></div>
            </div>
        </div>

        <div class="form-group">
            <div class="col-lg-8 col-lg-offset-4 col-md-8 col-md-offset-4">
                <button type="button" class="btn btn-success click-event" data-function="videoadjReset"><?php echo VA_SETDEFAULT; ?></button>
            </div>
        </div>

    </form>

    </div>
</div>
<br><br>

<?php 
addJs("
$(function() {
    $('div.bfh-slider').each(function () {
        var \$slider;
        \$slider = $(this);
        \$slider.bfhslider(\$slider.data());
    });
});
");
?>

