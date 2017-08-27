<div class="row">
    <div class="col-lg-12">
        <h1 class="page-header"><?php echo GPIO; ?>
    
        <ol class="breadcrumb">
            <li><a href="/ajax/devices.php" class="ajax-content"><?php echo ALL_DEVICES; ?></a></li>
            <li class="active">GPIO</li>
        </ol>
        </h1>
    </div>
</div>



<div class="row">
    <div class="col-lg-12">
    <form action="/ajax/update.php" method="post" class="form-horizontal" id="motion-submit">

        <input type="hidden" name="id" value="<?php echo ($global) ? 'global' : $device_schedule->schedule_data[0]['id']; ?>" />
        <input type="hidden" name="<?php echo (!$global) ? 'schedule' : 'G_DEV_SCED' ; ?>" id="motion-map" value="<?php echo $device_schedule->schedule_data[0]['schedule']; ?>" />
        <input type="hidden" name="type" value="Devices" />
        <input type="hidden" name="mode" value="<?php echo (!$global) ? 'update' : 'global' ; ?>" />

        <div class="form-group">
            <div class="col-lg-12">
                <button class="btn btn-success pull-right send-req-form" type="submit" data-func="getMotionMap"><i class="fa fa-check fa-fw"></i> <?php echo SAVE_CHANGES; ?></button>
                <div class="clearfix"></div>
            </div>
        </div>

	<div class="form-group">
	<div class="col-lg-12">
	  <div class="panel panel-default">
	    <div class="panel-heading">GPIO State</div>
            <div class="panel-body">
		<div class="col-lg-4 col-md-4">
		  <div class="panel panel-default">
		    <div class="panel-heading">Relays</div>
		    <div class="panel-body">
			<table class="table"><tbody>
			<?php foreach ($pinstates as $card_id => $cardpins) { ?>
			<tr><td><?php echo "Card ".$cards[$card_id]->info['order']; ?></td></tr>
				<tr><td>
				<table class="table table-bordered">
				<thead>
				<tr class="info"><?php foreach ($cardpins['relays'] as $pin_id => $pin) echo "<th>".$pin_id."</th>"; ?></tr>
				</thead>
				<tbody>
				<tr><?php foreach ($cardpins['relays'] as $pin_id => $pin) echo ( $pin == 1 ? "<td class='danger'>" :"<td>").$pin."</td>"; ?></tr>
				</tbody>
				</table>
				</td></tr>
			<?php }  ?>
			</tbody></table>
		    </div>
		  </div>
		<a href="/gpio?mode=reset" class="btn btn-success pull-left" ><i class="fa fa-check fa-fw"></i> <?php echo GPIO_RESET_RELAYS; ?></a>
		</div>
		<div class="col-lg-8 col-md-8">
		  <div class="panel panel-default">
		    <div class="panel-heading">Sensors</div>
		     <div class="panel-body">
                        <table class="table"><tbody>
                        <?php foreach ($pinstates as $card_id => $cardpins) { ?>
                        <tr><td><?php echo "Card ".$cards[$card_id]->info['order']; ?></td></tr>
                                <tr><td>
                                <table class="table table-bordered">
                                <thead>
                                <tr class="info"><?php foreach ($cardpins['sensors'] as $pin_id => $pin) echo "<th>".$pin_id."</th>"; ?></tr>
                                </thead>
                                <tbody>
                                <tr><?php foreach ($cardpins['sensors'] as $pin_id => $pin) echo ($pin == 0 ? "<td class='danger'>" : "<td>").$pin."</td>"; ?></tr>
                                </tbody>
                                </table>
                                </td></tr>
                        <?php }  ?>
                        </tbody></table>
		     </div>
		  </div>
		</div>
	    </div>
          </div>
	</div>
	</div>


    </form>
    </div>
</div>

