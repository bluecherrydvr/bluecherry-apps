<div class="row">
    <div class="col-lg-12">
        <h1 class="page-header"><?php echo BACKUP_HEADER; ?>
    
        <ol class="breadcrumb">
            <li class="active"> <?php echo BACKUP_SUBHEADER; ?></li>
        </ol>
        </h1>
    </div>
</div>

<?php if (file_exists(VAR_MYSQLDUMP_TMP_LOCATION)) { ?>
<div class="row">
    <div class="col-lg-12">
        <a href="/ajax/backup.php?mode=download" class="btn btn-success"> <?php echo BACKUP_DOWNLOAD; ?></a>
    </div>
</div>
<br>
<?php } ?>

<div class="row">
    <div class="col-lg-12">
        <div class="panel panel-default">
            <div class="panel-heading"><?php echo BACKUP_B_TITLE; ?></div>
            <div class="panel-body">
                <form action="/backup?mode=prepare" method="post" id="backup-form">
                    <div class="form-group">
                        <label class=""><?php echo str_replace(array('%USERS%', '%DBNAME%'), array('bluecherry', $database_parameters['db']), BACKUP_PWD); ?></label>
                        <input class="form-control" type="password" name="pwd"  />
                    </div>
                    <div class="form-group">
                        <label class=""><?php echo BACKUP_B_OPT; ?></label>
                        <div class="checkbox">
                            <label>
                                <input type="checkbox" name="noevents" checked />
                                <?php echo BACKUP_B_OPT_NE; ?>
                            </label>
                        </div>
                        <div class="checkbox">
                            <label>
                                <input type="checkbox" name="nodevices" />
                                <?php echo BACKUP_B_OPT_ND; ?>
                            </label>
                        </div>
                        <div class="checkbox">
                            <label>
                                <input type="checkbox" name="nousers" />
                                <?php echo BACKUP_B_OPT_NU; ?>
                            </label>
                        </div>
                    </div>
                    <div class="form-group">
                        <button class="btn btn-success send-req-form" type="submit" data-redirect-success="/ajax/backup.php"> <?php echo U_BACKUP; ?></button>
                    </div>
                </form>
            </div>
        </div>
    </div>
</div>

<div class="row">
    <div class="col-lg-12">
        <div class="panel panel-default">
            <div class="panel-heading"><?php echo BACKUP_R_TITLE; ?></div>
            <div class="panel-body">
                <form action="/backup?mode=restore" method="post" id="backup-form-upload">
                    <div class="form-group" style="display: none;">
                        <input class="form-control" type="password" name="pwd"  />
                    </div>
                    <div class="form-group">
                        <label class=""><?php echo BACKUP_R_LABEL; ?></label>
		                <input type="file" name="restoreDataFile">	
                    </div>
                    <div class="form-group">
                        <button class="btn btn-success send-req-form" type="submit" data-func="backupCopyPass" data-class-success="backupRestore.showConfirm(1)"> <?php echo U_UPLOAD; ?></button>
                    </div>
                </form>

                <form action="/backup?mode=confirmRestore" method="post" id="backup-form-confirm" style="display: none;">
                    <div class="form-group">
                        <label class=""><?php echo str_replace(array('%USERS%', '%DBNAME%'), array('root', $database_parameters['db']), BACKUP_PWD); ?></label>
                        <input class="form-control" type="password" name="pwd"  />
                    </div>
                    <div class="form-group">
                        <button class="btn btn-success send-req-form" type="submit"  data-class-success="backupRestore.showConfirm(0).resetForms()"> <?php echo U_RESTORE; ?></button>
                        <button class="btn btn-default click-event" type="button"  data-class="backupRestore.showConfirm(0).resetForms()"> <?php echo U_CANCEL; ?></button>
                    </div>
                </form>
            </div>
        </div>
    </div>
</div>

