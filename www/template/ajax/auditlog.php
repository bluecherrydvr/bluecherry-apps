
<div class="row">
    <div class="col-lg-12">
        <h1 class="page-header"><?php echo AUDITLOG_HEADER; ?></h1>
    </div>
</div>

<div class="row">
    <div class="col-lg-12">
        <a href="/downloadauditlogs" class="btn btn-success pull-right">Download CSV</a>
        <div class="clearfix"></div>
    </div>
</div>


<div class="row">
    <div class="col-lg-12">
	<table class="table table-striped">
	<thead>
	<tr>
		<th><?php echo AUDITLOG_DATETIME; ?></th><th><?php echo AUDITLOG_EVENTTYPE; ?></th>
		<th><?php echo AUDITLOG_USER; ?></th><th><?php echo AUDITLOG_DEVICE; ?></th>
		<th><?php echo AUDITLOG_IP; ?></th>
	</tr>
	</thead>
	<tbody>
<?php foreach($auditlog as $log_entry){ ?>
	<tr>
		<td><?php echo $log_entry['dt']; ?></td>
		<td><?php echo $log_entry['event_type']; ?></td>
		<td><?php echo $log_entry['user']; ?></td>
		<td><?php echo $log_entry['device']; ?></td>
                <td><?php echo $log_entry['ip']; ?></td>
	</tr>
<?php } ?>
	</tbody>
	</table>
    </div>
</div>

