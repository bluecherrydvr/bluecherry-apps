<?php defined('INDVR') or exit(); ?>

<div class="row">
    <div class="col-lg-12">
        <h1 class="page-header"><?php (empty($dvr_users->data['new'])) or $dvr_users->data[0]['name']='Create new user'; echo ($dvr_users->all) ? USERS_ALL_TABLE_HEADER : USERS_DETAIL_TABLE_HEADER.$dvr_users->data[0]['name']; ?>

        <ol class="breadcrumb">
            <?php  echo ($dvr_users->all) ? '<li class="active">'.USERS_ALL_TABLE_SUBHEADER.'</li>' : '<li><a href="/ajax/users.php" class="ajax-content">'.USERS_LIST.'</a></li> <li class="active">'.$dvr_users->data[0]['name'].'</li>'; ?>
        </ol>
        </h1>
    </div>
</div>


<?php
	#if all => show table with users
    if ($dvr_users->all) { ?>
<div class="row">
    <div class="col-lg-12">
        <a href="/ajax/users.php?id=new" class="btn btn-success pull-right ajax-content"><i class="fa fa-plus fa-fw"></i>  <?php echo USERS_NEW; ?></a>
        <div class="clearfix"></div>
    </div>
</div>
<br>

<div class="row">
    <div class="col-lg-12">
        <table class="table table-striped table-hover table-vert-align" id="users-list-bl">
            <thead>
                <tr>
                    <th><?php echo USERS_TABLE_HEADER_NAME; ?></th>
                    <th><?php echo USERS_TABLE_HEADER_LOGIN; ?></th>
                    <th><?php echo USERS_TABLE_HEADER_EMAIL; ?></th>
                    <th><?php echo USERS_TABLE_HEADER_STATUS; ?></th>
                    <th></th>
                <tr>
            </thead>
            <tbody>
            <?php
		        foreach($dvr_users->data as $id => $info){
        			$email_list = ''; $emails = ''; $email = ''; $tmp = '';
    	    		$emails = explode('|', $info['email']);
	    	    	foreach($emails as $id => $email){
    	    			$email = explode(':', $email);
        				$tmp[$id] = $email[0];
        			}
                    $email_list = implode(', ', $tmp);
            ?>
                <tr class="users-list-bl-item ajax-content" data-url="/ajax/users.php?id=<?php echo $info['id']; ?>">
                    <td><?php echo $info['name']; ?></td><td><?php echo $info['username']; ?></td>
                    <td><?php echo $email_list; ?></td>
                    <td><?php echo (($info['access_setup'])  ? USERS_STATUS_SETUP : USERS_STATUS_VIEWER); ?></td>
                    <td class="users-del">
                        <form action="/ajax/update.php" method="POST">
                            <input type="hidden" name="mode" value="deleteUser">
                            <input type="hidden" name="id" value="<?php echo $info['id']; ?>">
                            <button type="submit" class="btn btn-danger send-req-form" data-confirm="Delete this user (ID: <?php echo $info['id'];?>)?" data-loading-text="..." data-func="delTr"><i class="fa fa-times fa-fw"></i></a>
                        </form>
                    </td>
                </tr>
            <?php  $f = !$f; } ?>
            </tbody>
        </table>
    </div>
</div>
    <?php } else { //single user ?>


<div class="row">
    <div class="col-lg-12">
    <div class="btn-toolbar">
        <?php if (empty($dvr_users->data['new'])) { ?>

            <form action="/ajax/update.php" method="POST" class="pull-left">
                <input type="hidden" name="mode" value="deleteUser">
                <input type="hidden" name="id" value="<?php echo $dvr_users->data[0]['id']; ?>">
                <button type="submit" class="btn btn-danger send-req-form" data-confirm="Delete this user (ID: <?php echo $dvr_users->data[0]['id'];?>)?"><i class="fa fa-times fa-fw"></i><?php echo DELETE_USER; ?></button>
            </form>

            <a href="javascript:void(0);" class="btn btn-primary pull-left"><i class="fa fa-video-camera fa-fw"></i> Edit access list</a>

        <?php } ?>

        <button class="btn btn-success pull-right send-req-form" type="submit" data-form-id="users-form-save" data-func-after="usersAfterAdd"><i class="fa fa-check fa-fw"></i> <?php echo SAVE_CHANGES; ?></button>
        <div class="clearfix"></div>
    </div>
    </div>
</div>
<br>


<div class="row">
    <div class="col-lg-12">
        <form action="/ajax/update.php" method="POST" id="users-form-save" class="form-horizontal">

            <input type="hidden" name="id" value="<?php echo ((!empty($dvr_users->data[0]['id'])) ? $dvr_users->data[0]['id'] : ''); ?>" />
            <input type="hidden" name="mode" value="<?php echo ((!empty($dvr_users->data['new'])) ? 'newUser' : 'user'); ?>" />

            <div class="panel panel-default">
                <div class="panel-body">
                    
                    <div class="form-group">
                        <label class="col-lg-4 control-label"><?php echo USERS_NAME; ?><p class='help-block'><small><?php echo USERS_NAME_EX; ?></small></p></label>

                        <div class="col-lg-6">
                            <input class="form-control" type='text' name='name' value='<?php echo $dvr_users->data[0]['name']; ?>'  />
                        </div>
                    </div>
                    
                    <div class="form-group">
                        <label class="col-lg-4 control-label"><?php echo USERS_LOGIN; ?><p class='help-block'><small><?php echo USERS_LOGIN_EX; ?></small></p></label>

                        <div class="col-lg-6">
                            <input class="form-control" type='text' name='username' value="<?php echo ((!empty($dvr_users->data['new'])) ? '' : $dvr_users->data[0]['username']); ?>"  />
                        </div>
                    </div>
                    
                    <div class="form-group">
                        <label class="col-lg-4 control-label"><?php echo USERS_PASSWORD; ?><p class='help-block'><small><?php echo USERS_PASSWORD_EX; ?></small></p></label>

                        <div class="col-lg-6">
                            <input class="form-control" type='password' name='password' value="<?php echo ((!empty($dvr_users->data['new'])) ? '' : '__default__'); ?>"  />
                        </div>
                    </div>
                    
                    <div class="form-group users-add-email-bl">
                        <label class="col-lg-4 control-label"><?php echo USERS_EMAIL; ?><p class='help-block'><small><?php echo USERS_EMAIL_EX; ?></small></p></label>

                        <div class="col-lg-6" id="users-add-email-inp-bl">


                            <?php if (empty($dvr_users->data[0]['emails'])) { ?>
                            <div class="users-add-email-inp">
                                <input class="form-control" type="text" class="email" name="email[]" value="" />
                                <span class="input-group-btn" style="display: none;">
                                    <a href="javascript:void(0);" class="btn btn-danger users-del-email"><i class="fa fa-times fa-fw"></i></a>
                                </span>
                            </div>
                            <?php } ?>

                            <?php foreach($dvr_users->data[0]['emails'] as $key => $email){ ?>
                            <div class=" <?php echo (($key == 0) ? '' : 'input-group'); ?> users-add-email-inp">
                                <input class="form-control" type="text" class="email" name="email[]" value="<?php echo $email['addrs']; ?>" />
                                <span class="input-group-btn" <?php echo (($key == 0) ? 'style="display: none;"' : ''); ?> >
                                    <a href="javascript:void(0);" class="btn btn-danger users-del-email"><i class="fa fa-times fa-fw"></i></a>
                                </span>
                            </div>
                            <?php } ?>


                        </div>
                    </div>
                    
                    <div class="form-group">
                        <div class="col-lg-6 col-lg-offset-4">
                            <a href="javascript:void(0);" class="btn btn-success pull-right users-add-email">Add email</a>
                            <div class="clearfix"></div>
                        </div>
                    </div>
                    
                    <div class="form-group">
                        <label class="col-lg-4 control-label"><?php echo USERS_PHONE; ?><p class='help-block'><small><?php echo USERS_PHONE_EX; ?></small></p></label>

                        <div class="col-lg-6">
                            <input class="form-control" type='text' name='phone' value="<?php echo ((!empty($dvr_users->data[0]['phone'])) ? $dvr_users->data[0]['phone'] : ''); ?>"  />
                        </div>
                    </div>
                    
                    <div class="form-group">
                        <label class="col-lg-4 control-label"><?php echo USERS_ACCESS_SETUP; ?><p class='help-block'><small><?php echo USERS_ACCESS_SETUP_EX; ?></small></p></label>

                        <div class="col-lg-6 form-control-static">
                            <input type="checkbox" name="access_setup" <?php echo ((!empty($dvr_users->data[0]['access_setup'])) ? 'checked' : ''); ?>  />
                        </div>
                    </div>
                    
                    <div class="form-group">
                        <label class="col-lg-4 control-label"><?php echo USERS_ACCESS_WEB; ?><p class='help-block'><small><?php echo USERS_ACCESS_WEB_EX; ?></small></p></label>

                        <div class="col-lg-6 form-control-static">
                            <input type="checkbox" name="access_web" <?php echo ((!empty($dvr_users->data[0]['access_web'])) ? 'checked' : ''); ?>  />
                        </div>
                    </div>
                    
                    <div class="form-group">
                        <label class="col-lg-4 control-label"><?php echo USERS_ACCESS_REMOTE; ?><p class='help-block'><small><?php echo USERS_ACCESS_REMOTE_EX; ?></small></p></label>

                        <div class="col-lg-6 form-control-static">
                            <input type="checkbox" name="access_remote" <?php echo ((!empty($dvr_users->data[0]['access_remote'])) ? 'checked' : ''); ?>  />
                        </div>
                    </div>
                    
                    <div class="form-group">
                        <label class="col-lg-4 control-label"><?php echo USERS_ACCESS_BACKUP; ?><p class='help-block'><small><?php echo USERS_ACCESS_BACKUP_EX; ?></small></p></label>

                        <div class="col-lg-6 form-control-static">
                            <input type="checkbox" name="access_backup" <?php echo ((!empty($dvr_users->data[0]['access_backup'])) ? 'checked' : ''); ?>  />
                        </div>
                    </div>
                    
                    <div class="form-group">
                        <label class="col-lg-4 control-label"><?php echo USERS_NOTES; ?><p class='help-block'><small><?php echo USERS_NOTES_EX; ?></small></p></label>

                        <div class="col-lg-6">
                            <textarea class="form-control" name="notes" class="notes"><?php echo ((!empty($dvr_users->data[0]['notes'])) ? $dvr_users->data[0]['notes'] : ''); ?></textarea>
                        </div>
                    </div>

                </div>
            </div>
        </form>
    </div>
</div>
    <?php } ?>


