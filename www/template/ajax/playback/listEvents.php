
<div class="panel panel-default">
    <div class="panel-heading legend-block">
        <span><i class="fa fa-square fa-fw text-info"></i> Alarm</span>
        <span><i class="fa fa-square fa-fw text-danger"></i> Critical</span>
        <span><i class="fa fa-square fa-fw text-warning"></i> Warning</span>
        <span><i class="fa fa-square-o fa-fw legend-info"></i> Info</span>
    </div>
    <div class="panel-body">
        <div id="playback-events-list-bl">
        <table class="table table-striped table-condensed table-hover table-vert-align" id="playback-events-list">
            <thead>
                <tr>
                    <th data-sortable="true" data-field="date" data-sort-name="_date_data" data-sorter="playbackSorterTime">Date</th>
                    <th data-sortable="true">Event Type</th>
                    <th data-sortable="true">Event Duration</th>
                    <th data-sortable="true" data-field="size" data-sort-name="_size_data" data-sorter="playbackSorterBytes">File Size</th>
                </tr>
            </thead>
            <tbody>
            <?php foreach ($events as $val) { ?>
                <?php if ($val['media_available'] && ($val['end'] > 0)) { ?>
                <tr class="<?php echo $val['color']; ?> click-event" data-function="playbackSetVideo" data-id="<?php echo $val['media_id'] ?>" data-title="<?php echo $val['device_name'];  ?> - Recorded time: <?php echo date('m/d/Y h:i A', $val['time']);  ?>" data-media-id="<?php echo $val['media_id']; ?>">
                    <td data-time="<?php echo $val['time']; ?>"><?php echo $val['time_converted']; ?></td>
                    <td><?php echo $val['type_id']; ?></td>
                    <td>
                        <?php echo $val['duration']; ?>
                    </td>
                    <td data-bytes="<?php echo $val['media_size']; ?>"><?php echo $val['media_size_converted']; ?></td>
                </tr>
                <?php } else { ?>
                <tr class="<?php echo $val['color']; ?>">
                    <td data-time="<?php echo $val['time']; ?>"><?php echo $val['time_converted']; ?></td>
                    <td><?php echo $val['type_id']; ?></td>
                    <td>
                        <?php echo isset($val['details']) ? htmlspecialchars($val['details']) : 'N/A'; ?>
                    </td>
                    <td data-bytes="0">No media</td>
                </tr>
                <?php } ?>
            <?php } ?>
            </tbody>
        </table>
        </div>
    </div>
</div>

<script type="text/javascript">
$(function() {
    $('#playback-events-list').bootstrapTable();
});
</script>
