<?php defined('INDVR') or exit(); ?>

<div id="user-table">
<h1 id="header" class="header"><?php echo ($dvr_users->all) ? USERS_ALL_TABLE_HEADER : USERS_DETAIL_TABLE_HEADER.$dvr_users->data[0]['name']; ?></h1><hr>
<p><?php (!$dvr_users->data['new']) or $dvr_users->data[0]['name']='Create new user'; echo ($dvr_users->all) ? USERS_ALL_TABLE_SUBHEADER : '<a href="#" id="allusers">'.USERS_LIST.'</a> '.BB_MAP_ARROW.' '.$dvr_users->data[0]['name']; ?></p>
<?php
	#if all => show table with users
	if ($dvr_users->all){
		echo '<div id="newUser">'.USERS_NEW.'</div>';
		echo '<table><tr class="header"><th>'.USERS_TABLE_HEADER_NAME.'</th><th>'.USERS_TABLE_HEADER_LOGIN.'</th><th>'.USERS_TABLE_HEADER_EMAIL.'</th><th>'.USERS_TABLE_HEADER_STATUS.'</th></tr>';
		foreach($dvr_users->data as $id => $info){
			echo "<tr class='".(($f) ? 'low' : 'high')."' id={$info['id']}><td>{$info['name']}</td><td>{$info['username']}</td><td>{$info['email']}</td><td>".(($info['access_setup'])  ? USERS_STATUS_SETUP : USERS_STATUS_VIEWER)."</td><td class='deleteUser'></td></tr>";
			$f = !$f;
		}
		echo '</table>';
	} else { //single user
		echo "	<div>
					<FORM method='Post' action='/ajax/update.php' id='settingsForm'>
					<input type='Hidden' name='id' class='objId' value='{$dvr_users->data[0]['id']}'>
					<input type='Hidden' name='mode' value='".(($dvr_users->data['new']) ? 'newUser' : 'user')."'>
					".(($dvr_users->data['new']) ? "" : "<div id='deleteButton'>".DELETE_USER."</div>")."
					<div id='saveButton'>".SAVE_CHANGES."</div>
					<div>
						<div id='editAccessList'>Edit access list</div>
					</div>
					<div><label>".USERS_NAME."<span class='sub'>".USERS_NAME_EX."</span></label><input type='text' name='name' value='".(($dvr_users->data['new']) ? '' : $dvr_users->data[0]['name'])."'  /></div><br />
					<div><label>".USERS_LOGIN."<span class='sub'>".USERS_LOGIN_EX."</span></label><input type='text' name='username' value='".(($dvr_users->data['new']) ? '' : $dvr_users->data[0]['username'])."' /></div><br />
					<div><label>".USERS_PASSWORD."<span class='sub'>".USERS_PASSWORD_EX."</span></label><input type='password' name='password' value='".(($dvr_users->data['new']) ? '' : '__default__')."' /></div><br />
					<div><label>".USERS_EMAIL."<span class='sub'>".USERS_EMAIL_EX."</span></label><input type='text' name='email' value='{$dvr_users->data[0]['email']}' /></div><br />
					<div><label>".USERS_PHONE."<span class='sub'>".USERS_PHONE."</span></label><input type='text' name='phone' value='{$dvr_users->data[0]['phone']}' /></div><br />
					<div><label>".USERS_ACCESS_SETUP."<span class='sub'>".USERS_ACCESS_SETUP_EX."</span></label><input type='checkbox' name='access_setup' ".(($dvr_users->data[0]['access_setup']) ? 'checked' : '')." /></div><br />
					<div><label>".USERS_ACCESS_WEB."<span class='sub'>".USERS_ACCESS_WEB_EX."</span></label><input type='checkbox' name='access_web' ".(($dvr_users->data[0]['access_web']) ? 'checked' : '')." /></div><br />
					<div><label>".USERS_ACCESS_REMOTE."<span class='sub'>".USERS_ACCESS_REMOTE_EX."</span></label><input type='checkbox' name='access_remote' ".(($dvr_users->data[0]['access_remote']) ? 'checked' : '')." /></div><br />
					<div><label>".USERS_ACCESS_BACKUP."<span class='sub'>".USERS_ACCESS_BACKUP_EX."</span></label><input type='checkbox' name='access_backup' ".(($dvr_users->data[0]['access_backup']) ? 'checked' : '')." /></div><br />
					<div><label>".USERS_NOTES."<span class='sub'>".USERS_NOTES_EX."</span></label><textarea name='notes' class='notes'>".$dvr_users->data[0]['notes']."</textarea></div><br />
					</FORM>
				</div>
		";
	}
?>
</div>