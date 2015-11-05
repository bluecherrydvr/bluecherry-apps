
<?php if ($statistics->type == 'html') { ?>

    <div class="panel panel-default">
        <div class="panel-heading"><?php echo str_replace('%TYPE%', $statistics->event_type, STS_TOTAL_EVENTS); ?> <b><?php echo $statistics->total_records; ?></b></div>
        <div class="panel-body">
            <table class="table table-striped">
                <thead>
                    <tr>
                        <th><?php echo $data['primary_grouping']; ?></th>
                        <?php echo (($data['primary_grouping'] != $data['secondary_grouping']) ?  "<th>".$data['secondary_grouping']."</th>" : ""); ?>
                        <th><?php echo STS_NUM_EVENTS; ?></th>
                        <th><?php echo STS_PERCENTAGE_OF_TOTAL; ?></th>
                    </tr>
                </thead>
                <tbody>
                <?php
        			foreach($statistics->results as $key => $result){
        				$primary_grouping_value = '';
        				if ($key!=0 && $result[$data['primary_grouping']]!= $statistics->results[$key-1][$data['primary_grouping']]){ #change in primary grouping
        					$primary_grouping_td = 'primarySeparator';
        					$secondary_grouping_td = 'primarySeparator';
        					$primary_grouping_value = $result[$data['primary_grouping']];
        				} elseif ($key == 0){
        					$primary_grouping_value = $result[$data['primary_grouping']];
        					$primary_grouping_td = 'nonSeparated';
        				} else {
        					$primary_grouping_td = 'nonSeparated';
        					$secondary_grouping_td = '';
                        }
                ?>
                    <tr>
                        <td><?php echo $primary_grouping_value; ?></td>
                        <?php echo (($data['primary_grouping'] != $data['secondary_grouping']) ? "<td>".$result[$data['secondary_grouping']]."</td>" : ""); ?>
                        <td ><?php echo $result['counter']; ?></td>
                        <td><?php echo round($result['counter']/$statistics->total_records*100, 2); ?>%</td>
                    </tr>
                <?php } ?>
                </tbody>
            </table>
        </div>
        </div>
    </div>


<?php 
    } else { 
?>
<div class="row">
    <div class="col-lg-12">
        <h1 class="page-header"><?php echo STS_HEADER; ?>
    
        <ol class="breadcrumb">
            <li class="active"><?php echo STS_SUBHEADER; ?></li>
        </ol>
        </h1>
    </div>
</div>

<div class="row">
    <div class="col-lg-12">
    <div class="panel panel-default">
        <div class="panel-heading"><?php echo STS_PICKER_HEADER; ?></div>
        <div class="panel-body">

        <form action="/ajax/statistics.php" method="POST">
        <div class="row">
            <div class="col-lg-6 col-md-6">
                <div class="form-group" style="position: relative;">
                    <label><?php echo STS_START_DATE; ?></label>
                    <div class="input-group date form-datetime">
                        <input class="form-control" type="text" name="start" value="<?php echo $statistics->result['first_event']; ?>" readonly>
    					<span class="input-group-addon"><span class="glyphicon glyphicon-th"></span></span>
                    </div>
                </div>
                <div class="form-group">
                    <label><?php echo STS_END_DATE; ?></label>
                    <div class="input-group date form-datetime" data-date="1979-09-16T05:25:07Z" data-date-format="dd MM yyyy - HH:ii p">
                        <input class="form-control" type="text" name="end" value="<?php echo $statistics->result['last_event']; ?>" readonly>
    					<span class="input-group-addon"><span class="glyphicon glyphicon-th"></span></span>
                    </div>
                </div>
            </div>
            <div class="col-lg-6 col-md-6">
                <div class="form-group">
                    <label><?php echo STS_PR_GRP; ?></label>
                    <?php echo arrayToSelect(array_keys($query_params), 'month', 'primary_grouping'); ?>
                </div>
                <div class="form-group">
                    <label><?php echo STS_SC_GRP; ?></label>
                    <?php echo arrayToSelect(array_keys($query_params), 'day', 'secondary_grouping'); ?>
                </div>
            </div>
        </div>
        <div class="row">
            <div class="col-lg-6 col-md-6">
                <div class="form-group">
                    <div class="checkbox">
                        <label>
                            <input type="checkbox" name="all_events" checked="checked"><?php echo STS_ET_ALL; ?>
                        </label>
                        <label>
                            <input type="checkbox" name="motion_events"><?php echo STS_ET_M; ?>
                        </label>
                        <label>
                            <input type="checkbox" name="continuous_events"><?php echo STS_ET_C; ?>
                        </label>
                    </div>
                </div>
            </div>

            <div class="col-lg-6 col-md-6">
                <button class="btn btn-success btn-block send-req-form" type="submit" data-type-data="HTML" data-func-after="statRes"> <?php echo STS_SUBMIT; ?></button>
            </div>


        </div>
        

        </form>
        </div>
    </div>
    </div>
</div>

<div class="row">
    <div class="col-lg-12" id="statistics-res">
    </div>
</div>


<?php

addJs('
$(function() {
    $(".form-datetime").datetimepicker({
        format: "yyyy-mm-dd hh:ii:ss",
        autoclose: true
    });
});
');
?>


<?php } ?>
    
