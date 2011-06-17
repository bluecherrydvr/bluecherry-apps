<?php defined('INDVR') or exit(); ?>

<div id="user-table">
<h1 id="header" class="header"><?php (empty($dvr_users->data['new'])) or $dvr_users->data[0]['name']='Create new user'; echo ($dvr_users->all) ? USERS_ALL_TABLE_HEADER : USERS_DETAIL_TABLE_HEADER.$dvr_users->data[0]['name']; ?></h1><hr />
<p><?php  echo ($dvr_users->all) ? USERS_ALL_TABLE_SUBHEADER : '<a href="#" id="allusers">'.USERS_LIST.'</a> '.BB_MAP_ARROW.' '.$dvr_users->data[0]['name']; ?></p>
<?php
	#if all => show table with users
	if ($dvr_users->all){
		echo '<div id="newUser">'.USERS_NEW.'</div>';
		echo '<table><tr class="header"><th>'.USERS_TABLE_HEADER_NAME.'</th><th>'.USERS_TABLE_HEADER_LOGIN.'</th><th>'.USERS_TABLE_HEADER_EMAIL.'</th><th>'.USERS_TABLE_HEADER_STATUS.'</th></tr>';
		$f = 0;
		foreach($dvr_users->data as $id => $info){
			echo "<tr class='".(($f) ? 'low' : 'high')."' id='{$info['id']}'><td>{$info['name']}</td><td>{$info['username']}</td><td>{$info['email']}</td><td>".(($info['access_setup'])  ? USERS_STATUS_SETUP : USERS_STATUS_VIEWER)."</td><td class='deleteUser'></td></tr>";
			$f = !$f;
		}
		echo '</table>';
	} else { //single user
		echo "	<div>
					<FORM method='Post' action='/ajax/update.php' id='settingsForm'>
					<input type='Hidden' name='id' class='objId' value='".((!empty($dvr_users->data[0]['id'])) ? $dvr_users->data[0]['id'] : '')."' />
					<input type='Hidden' name='mode' value='".((!empty($dvr_users->data['new'])) ? 'newUser' : 'user')."' />
					".((!empty($dvr_users->data['new'])) ? "" : "<div id='deleteButton'>".DELETE_USER."</div>")."
					<div id='saveButton'>".SAVE_CHANGES."</div>
					<div>
						".((!empty($dvr_users->data['new'])) ? "" : "<div id='editAccessList'>Edit access list</div>")."
					</div>
					<div><label>".USERS_NAME."<span class='sub'>".USERS_NAME_EX."</span></label><input type='text' name='name' value='{$dvr_users->data[0]['name']}'  /></div><br />
					<div><label>".USERS_LOGIN."<span class='sub'>".USERS_LOGIN_EX."</span></label><input type='text' name='username' value='".((!empty($dvr_users->data['new'])) ? '' : $dvr_users->data[0]['username'])."' /><br />
					<div><label>".USERS_PASSWORD."<span class='sub'>".USERS_PASSWORD_EX."</span></label><input type='password' name='password' value='".((!empty($dvr_users->data['new'])) ? '' : '__default__')."' /></div><br />
					<div id='emails'>";
					foreach($dvr_users->data[0]['emails'] as $key => $email){
						echo "
							<span><label>".USERS_EMAIL."<span class='sub'>".USERS_EMAIL_EX."</span></label><input type='text' class='email' name='email[]' value='{$email['addrs']}' /><input type='text' class='limit' name='limit[]' value='{$email['limit']}' /></span><br />
						";
					}
					echo "<br /><label></label><span id='addEmail'>Add email</span></div>
					<div><label>".USERS_PHONE."<span class='sub'>".USERS_PHONE."</span></label><input type='text' name='phone' value='".((!empty($dvr_users->data[0]['phone'])) ? $dvr_users->data[0]['phone'] : '')."' /></div><br />
					<div><label>".USERS_ACCESS_SETUP."<span class='sub'>".USERS_ACCESS_SETUP_EX."</span></label><input type='checkbox' name='access_setup' ".((!empty($dvr_users->data[0]['access_setup'])) ? 'checked' : '')." /></div><br />
					<div><label>".USERS_ACCESS_WEB."<span class='sub'>".USERS_ACCESS_WEB_EX."</span></label><input type='checkbox' name='access_web' ".((!empty($dvr_users->data[0]['access_web'])) ? 'checked' : '')." /></div><br />
					<div><label>".USERS_ACCESS_REMOTE."<span class='sub'>".USERS_ACCESS_REMOTE_EX."</span></label><input type='checkbox' name='access_remote' ".((!empty($dvr_users->data[0]['access_remote'])) ? 'checked' : '')." /></div><br />
					<div><label>".USERS_ACCESS_BACKUP."<span class='sub'>".USERS_ACCESS_BACKUP_EX."</span></label><input type='checkbox' name='access_backup' ".((!empty($dvr_users->data[0]['access_backup'])) ? 'checked' : '')." /></div><br />
					<div><label>".USERS_NOTES."<span class='sub'>".USERS_NOTES_EX."</span></label><textarea name='notes' class='notes'>".((!empty($dvr_users->data[0]['notes'])) ? $dvr_users->data[0]['notes'] : '')."</textarea></div><br />
					</FORM>
				</div>
		";
	}
?>
</div>
