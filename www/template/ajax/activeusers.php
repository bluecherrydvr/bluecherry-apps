<?php defined('INDVR') or exit(); ?>

<h1 id="header" class="header"><?php echo ACTIVE_USERS_HEADER; ?></h1>
<hr>
<p><?php echo ACTIVE_USERS_SUBHEADER; ?></p>

<?php
		echo '<table id="active-user-table"><tr class="header"><th>'.USERS_TABLE_HEADER_NAME.'</th><th>'.AU_IP.'</th><th></th><th></th><th></th></tr>';
		foreach($this->users as $k => $user){
			echo "<tr class='".(($f) ? 'low' : 'high')."' id={$user->data[0]['id']}><td>{$user->data[0]['name']}</td><td>{$user->data[0]['ip']}</td><td>".(($user->data[0]['from_client']) ? RCLIENT : WEB)."</td><td class='".(($user->data[0]['kick']) ? 'kicked' : "kick")."' id='{$user->data[0]['ip']}'>".(($user->data[0]['kick']) ? AU_KICKED : AU_KICK)."</td><td class='ban' id={$user->data[0]['id']}>".AU_BAN."</td></tr>";
			$f = !$f;
		}
		echo '</table>';
?>