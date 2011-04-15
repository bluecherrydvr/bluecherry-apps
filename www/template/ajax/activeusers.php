<?php defined('INDVR') or exit(); ?>

<h1 id="header" class="header"><?php echo ACTIVE_USERS_HEADER; ?></h1>
<hr>
<p><?php echo ACTIVE_USERS_SUBHEADER; ?></p>

<?php
		echo '<table id="active-user-table"><tr class="header"><th>'.USERS_TABLE_HEADER_NAME.'</th><th>'.AU_IP.'</th><th></th><th></th><th></th></tr>';
		foreach($this->users as $k => $user){
			echo "<tr class='".(($f) ? 'low' : 'high')."' id={$user->info['id']}><td>{$user->info['name']}</td><td>{$user->info['ip']}</td><td>".(($user->info['from_client']) ? RCLIENT : WEB)."</td><td class='".(($user->info['kick']) ? 'kicked' : "kick")."' id='{$user->info['ip']}'>".(($user->info['kick']) ? AU_KICKED : AU_KICK)."</td></tr>";
			$f = !$f;
		}
		echo '</table>';
?>