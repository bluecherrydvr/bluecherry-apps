<div class="row">
    <div class="col-lg-12">
        <h1 class="page-header">Playback</h1>
    </div>
</div>

<div class="row">
    <div class="col-md-4 col-lg-3">
        <div class="panel panel-default">
            <div class="panel-heading"><?php echo STS_PICKER_HEADER; ?></div>
            <div class="panel-body">
            <form action="/playback" method="POST" id="playback-form-search" >
		<div class="form-group">
		    <label><?php echo STS_START_DATE; ?></label>
		    <div class="input-group date form-datetime">
			<input class="form-control" type="text" name="start" value="" readonly>
			<span class="input-group-addon"><span class="glyphicon glyphicon-th"></span></span>
		    </div>
		</div>
		<div class="form-group">
		    <label><?php echo STS_END_DATE; ?></label>
		    <div class="input-group date form-datetime">
			<input class="form-control" type="text" name="end" value="" readonly>
			<span class="input-group-addon"><span class="glyphicon glyphicon-th"></span></span>
		    </div>
		</div>
		<div class="form-group">
		    <label>Quick Date</label>
		    <div>
			<select name="quick_date" class="form-control change-event" data-function="playbackSetDate">
			    <option value="today">Today</option>
			    <option value="yesterday">Yesterday</option>
			    <option value="this_week">This Week</option>
			    <option value="last_week">Last Week</option>
			</select>
		    </div>
		</div>
		<div class="form-group">
		    <label>Camera(s)</label>
		    <select class="form-control" name="device_id">
			<?php echo ((empty($cameras))) ? '<option value="0">There are not any events</option>' : '' ; ?>
			<?php foreach ($cameras as $key => $val) { ?>
			<option value="<?php echo $val['id'] ?>"><?php echo $val['device_name']; ?></option>
			<?php } ?>
		    </select>
		</div>
		<div class="form-group">
		    <div class="checkbox">
			<label>
			    <input type="checkbox" name="motion_events" checked><?php echo STS_ET_M; ?>
			</label>
			<label>
			    <input type="checkbox" name="continuous_events"><?php echo STS_ET_C; ?>
			</label>
		    </div>
		</div>
		<button class="btn btn-success btn-block send-req-form" type="submit" data-type-data="HTML" data-func-after="playbackSetResult">Get</button>
            </form>
            </div>
        </div>
    </div>
    <div class="col-md-8 col-lg-9">
        <div id="playback-player-bl" style="display: none;">
        <div class="panel panel-default">
            <div class="panel-heading"></div>
            <div class="panel-body">
                <div id="playback-player"></div>
                <div>
                    <br>
                    <a href="javascript:void();" class="btn btn-primary click-event" data-function="playbackReloadVideo">Reload</a>
                    <a href="" class="btn btn-primary" id="playback-player-download" target="_blank">Download MP4 file</a>
                    <a href="" class="btn btn-primary" id="playback-player-download-mkv" target="_blank">Download MKV file</a>
                </div>
            </div>
        </div>
	    </div>
        <div id="playback-results"></div>
    </div>
</div>
<?php

addJs('
$(function() {
    $(".form-datetime").datetimepicker({
        format: var_date_format,
        autoclose: true
    });

    playbackSetDate(null);
});
');
?>
