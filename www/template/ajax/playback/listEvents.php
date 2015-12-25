
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
                    <th>Date</th>
                    <th>Event Type</th>
                    <th>Event Duration</th>
                    <th>File Size</th>
                </tr>
            </thead>
            <tbody>
            <?php foreach ($events as $val) { ?>
                <?php if ($val['media_available'] && ($val['end'] > 0)) { ?>
                <tr class="<?php echo $val['color']; ?> click-event" data-function="playbackSetVideo" data-id="<?php echo $val['media_id'] ?>" data-title="<?php echo $val['device_name'];  ?> - <?php echo $val['time'];  ?>" data-media-id="<?php echo $val['media_id']; ?>">
                    <td><?php echo $val['time']; ?></td>
                    <td><?php echo $val['type_id']; ?></td>
                    <td>
                        <?php echo $val['duration']; ?>
                    </td>
                    <td><?php echo $val['media_size']; ?></td>
                </tr>
                <?php } ?>
            <?php } ?>
            </tbody>
        </table>
        </div>
    </div>
</div>
