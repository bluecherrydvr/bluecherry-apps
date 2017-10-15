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
    <form action="/ajax/gpio.php" method="post" class="form-horizontal" id="gpio-submit">

        <!-- <input type="hidden" name="id" value="<?php echo ($global) ? 'global' : $device_schedule->schedule_data[0]['id']; ?>" />
        <input type="hidden" name="<?php echo (!$global) ? 'schedule' : 'G_DEV_SCED' ; ?>" id="motion-map" value="<?php echo $device_schedule->schedule_data[0]['schedule']; ?>" />
        <input type="hidden" name="type" value="Devices" />
        <input type="hidden" name="mode" value="<?php echo (!$global) ? 'update' : 'global' ; ?>" /> --!>

        <div class="form-group">
            <div class="col-lg-12">
                <button class="btn btn-success pull-right send-req-form" type="submit"><i class="fa fa-check fa-fw"></i> <?php echo SAVE_CHANGES; ?></button>
                <div class="clearfix"></div>
            </div>
        </div>

	<div class="form-group">
	<div class="col-lg-12">
	  <div class="panel panel-default">
	    <div class="panel-heading"><?php echo GPIO_STATE; ?></div>
            <div class="panel-body">
		<div class="col-lg-4 col-md-4">
		  <div class="panel panel-default">
		    <div class="panel-heading"><?php echo GPIO_RELAYS; ?></div>
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
		    <div class="panel-heading"><?php echo GPIO_SENSORS; ?></div>
		     <div class="panel-body">
                        <table class="table"><tbody>
                        <?php foreach ($pinstates as $card_id => $cardpins) { ?>
                        <tr><td><?php echo GPIO_CARD.$cards[$card_id]->info['order']; ?></td></tr>
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

        <div class="form-group">
        <div class="col-lg-12">
          <div class="panel panel-default">
            <div class="panel-heading"><?php echo GPIO_SENSORS_TO_RELAYS_MAP; ?></div>
            <div class="panel-body">
		<ul class="nav nav-tabs">
		<?php foreach($cards as $card_id => $card)
			echo "<li ".( $card->info['order'] == 1 ? "class=\"active\"" : "")."><a data-toggle=\"tab\" href=\"#relays_navtab_card".$card_id."\">".GPIO_CARD.$card->info['order']."</a></li>"; ?>
		</ul>
		<div class="tab-content">
			<?php foreach($cards as $card_id => $card) { ?>
			<div id="relays_navtab_card<?php echo $card_id; ?>" class="tab-pane fade in <?php echo ($card->info['order'] == 1 ? "active" : ""); ?>">
			<!-- sensors to relays table --!>
			<table class="table table-bordered">
			<thead>
			<tr><th></th><th colspan=16><?php echo GPIO_SENSORS_ON_CARD.$card->info['order']; ?></th></tr>
			<tr><th>Relays</th><?php foreach ($pinstates[$card_id]['sensors'] as $pin_id => $pin) echo "<th>".$pin_id."</th>"; ?></tr>
			</thead>
			<tbody>
			<?php foreach($relay_mappings[$card_id] as $relay_card_id => $relaycard)
				foreach($relaycard as $relay_pin_id => $relay_pin) {?>
				<tr>
					<td><?php echo GPIO_RELAY.$relay_pin_id.GPIO_ON_CARD.$cards[$relay_card_id]->info['order']; ?> </td>
					<?php foreach ($relay_pin as $sensor_pin_id => $sensor_pin)
							echo "<td><input type='checkbox' name=\"".substr($sensor_pin, 1)."\" ".
							(substr($sensor_pin, 0, 1) == 1 ?  'checked' : '')
							." /></td>"; ?>
				</tr>
			<?php } ?>
			</tbody>
			</table>
			</div>
			<?php } ?>
		</div>
	    </div>
	  </div>
	</div>
	</div>

        <div class="form-group">
        <div class="col-lg-12">
          <div class="panel panel-default">
            <div class="panel-heading"><?php echo GPIO_SENSORS_TO_CAMERAS_MAP; ?></div>
            <div class="panel-body">
		<ul class="nav nav-tabs">
                <?php foreach($cards as $card_id => $card)
                        echo "<li ".( $card->info['order'] == 1 ? "class=\"active\"" : "")."><a data-toggle=\"tab\" href=\"#cameras_navtab_card".$card_id."\">".GPIO_CARD.$card->info['order']."</a></li>"; ?>
                </ul>
                <div class="tab-content">
                        <?php foreach($cards as $card_id => $card) { ?>
                        <div id="cameras_navtab_card<?php echo $card_id; ?>" class="tab-pane fade in <?php echo ($card->info['order'] == 1 ? "active" : ""); ?>">
			<!-- sensors to cameras table --!>
			<table class="table table-bordered">
			<thead>
                        <tr><th></th><th colspan=16><?php echo GPIO_SENSORS_ON_CARD.$card->info['order']; ?></th></tr>
                        <tr><th>Dispositivos</th><?php foreach ($pinstates[$card_id]['sensors'] as $pin_id => $pin) echo "<th>".$pin_id."</th>"; ?></tr>
                        </thead>
                        <tbody>
			<?php foreach($camera_mappings[$card_id] as $device_id => $device) { ?>
				<tr>
				<td><?php echo $device['device_name'];unset($device['device_name']); ?></td>
				<?php foreach($device as $sensor_pin_id => $sensor_pin) {
					//if ($sensor_pin_id != 'device_name')
					echo "<td><input type='checkbox' name=\"".substr($sensor_pin, 1)."\" ".
					(substr($sensor_pin, 0, 1) == 1 ?  'checked' : '')
					."/></td>" ?>
				<?php } ?>
				</tr>
			<?php } ?>
			</tbody>
                        </table>
                        </div>
                        <?php } ?>
                </div>

            </div>
          </div>
        </div>
        </div>


    </form>
    </div>
</div>

