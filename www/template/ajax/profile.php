<div class="row">
    <div class="col-lg-12">
        <h1 class="page-header">Profile</h1>
    </div>
</div>


<div class="row">
    <div class="col-lg-12">
        <form action="/profile" method="post" class="form-horizontal">

            <div class="form-group">
                <div class="col-lg-12 col-md-12">
                    <button class="btn btn-success pull-right send-req-form" type="submit" data-func-success="profileSaved"><i class="fa fa-check fa-fw"></i> <?php echo SAVE_CHANGES; ?></button>
                    <div class="clearfix"></div>
                </div>
            </div>

            <div class="panel panel-default">
                <div class="panel-body">

                    <div class="form-group">
                        <label class="col-lg-4 col-md-4 control-label"><?php echo USERS_NAME; ?><p class='help-block'><small><?php echo USERS_NAME_EX; ?></small></p></label>

                        <div class="col-lg-6 col-md-6">
                            <input class="form-control" type='text' name='name' value='<?php echo $user_info['name']; ?>' disabled  />
                        </div>
                    </div>

                    <div class="form-group">
                        <label class="col-lg-4 col-md-4 control-label"><?php echo USERS_LOGIN; ?><p class='help-block'><small><?php echo USERS_LOGIN_EX; ?></small></p></label>

                        <div class="col-lg-6 col-md-6">
                            <input class="form-control" type='text' name='username' value="<?php echo $user_info['username']; ?>" disabled  />
                        </div>
                    </div>

                    <div class="form-group users-add-email-bl">
                        <label class="col-lg-4 col-md-4 control-label"><?php echo USERS_EMAIL; ?><p class='help-block'><small><?php echo USERS_EMAIL_EX; ?></small></p></label>
                        <div class="col-lg-6 col-md-6" id="users-add-email-inp-bl">
                            <?php if (empty($user_info['emails_arr'])) { ?>
                            <div class="users-add-email-inp">
                                <input class="form-control" type="text" class="email" name="email[]" value="" disabled />
                            </div>
                            <?php } ?>

                            <?php foreach($user_info['emails_arr'] as $key => $email){ ?>
                            <div class=" users-add-email-inp">
                                <input class="form-control" type="text" class="email" name="email[]" value="<?php echo $email; ?>" disabled />
                            </div>
                            <?php } ?>
                        </div>
                    </div>

                    <div class="form-group">
                        <label class="col-lg-4 col-md-4 control-label"><?php echo USERS_PHONE; ?><p class='help-block'><small><?php echo USERS_PHONE_EX; ?></small></p></label>
                        <div class="col-lg-6 col-md-6">
                            <input class="form-control" type='text' name='phone' value="<?php echo $user_info['phone']; ?>" disabled />
                        </div>
                    </div>

                    <?php if ($user_info['access_setup']) { ?>
                    <div class="form-group">
                        <label class="col-lg-4 col-md-4 control-label"><?php echo USERS_ACCESS_SETUP; ?><p class='help-block'><small><?php echo USERS_ACCESS_SETUP_EX; ?></small></p></label>

                        <div class="col-lg-6 col-md-6 form-control-static">
                            <input type="checkbox" name="access_setup" checked disabled />
                        </div>
                    </div>
                    <?php } ?>

                    <?php if ($user_info['access_web']) { ?>
                    <div class="form-group">
                        <label class="col-lg-4 col-md-4 control-label"><?php echo USERS_ACCESS_WEB; ?><p class='help-block'><small><?php echo USERS_ACCESS_WEB_EX; ?></small></p></label>

                        <div class="col-lg-6 col-md-6 form-control-static">
                            <input type="checkbox" name="access_web" checked disabled />
                        </div>
                    </div>
                    <?php } ?>

                    <?php if ($user_info['access_remote']) { ?>
                    <div class="form-group">
                        <label class="col-lg-4 col-md-4 control-label"><?php echo USERS_ACCESS_REMOTE; ?><p class='help-block'><small><?php echo USERS_ACCESS_REMOTE_EX; ?></small></p></label>

                        <div class="col-lg-6 col-md-6 form-control-static">
                            <input type="checkbox" name="access_remote" checked disabled />
                        </div>
                    </div>
                    <?php } ?>

                    <?php if ($user_info['access_backup']) { ?>
                    <div class="form-group">
                        <label class="col-lg-4 col-md-4 control-label"><?php echo USERS_ACCESS_BACKUP; ?><p class='help-block'><small><?php echo USERS_ACCESS_BACKUP_EX; ?></small></p></label>

                        <div class="col-lg-6 col-md-6 form-control-static">
                            <input type="checkbox" name="access_backup"  checked disabled />
                        </div>
                    </div>
                    <?php } ?>

                    <div class="form-group">
                        <label class="col-lg-4 col-md-4 control-label"><?php echo USERS_PASSWORD_CURRENT; ?></label>
                        <div class="col-lg-6 col-md-6">
                            <input class="form-control" type='password' name='password_current' value=""  />
                        </div>
                    </div>

                    <div class="form-group">
                        <label class="col-lg-4 col-md-4 control-label"><?php echo USERS_PASSWORD_NEW; ?></label>
                        <div class="col-lg-6 col-md-6">
                            <input class="form-control" type='password' name='password_new' value=""  />
                        </div>
                    </div>

                </div>
            </div>


            <div class="form-group">
                <div class="col-lg-12">
                    <button class="btn btn-success pull-right send-req-form" type="submit" data-func-success="profileSaved"><i class="fa fa-check fa-fw"></i> <?php echo SAVE_CHANGES; ?></button>
                    <div class="clearfix"></div>
                </div>
            </div>
        </form>
    </div>
</div>
