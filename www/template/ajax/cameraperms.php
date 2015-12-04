<div class="row">
    <div class="col-lg-12">
        <h1 class="page-header"><?php echo CAMERA_PERMISSIONS_SUB; ?>

        <ol class="breadcrumb">
            <li><a href="/users" class="ajax-content"><?php echo USERS_LIST; ?></a></li> 
            <li><a href="/users?id=<?php echo $p->user->info['id'];?>" class="ajax-content"><?php echo $p->user->info['name']; ?></a></li> 
            <li class="active"><?php echo CAMERA_PERMISSIONS_SUB; ?></li>
        </ol>
        </h1>
    </div>
</div>

<div class="row">
    <div class="col-lg-12">
        <form action="/ajax/update.php" method="POST" id="users-form-save" class="form-horizontal">

            <input type="hidden" name="id" value="<?php echo $p->user->info['id'];?>" />
            <input type="hidden" name="mode" value="access_device_list" />
            <input type="hidden" name="type" value="Users" />

            <div class="form-group">
                <div class="col-lg-12">
                    <button class="btn btn-success pull-right send-req-form" type="submit"><i class="fa fa-check fa-fw"></i> <?php echo SAVE_CHANGES; ?></button>
                </div>
            </div>

            <div class="panel panel-default">
                <div class="panel-body">
                    <div class="col-lg-6">
                        <a href="javascript:void(0);" class="btn btn-danger btn-block" id="cameraperms-restrict-all"><i class="fa fa-times fa-fw"></i> <?php echo RESTRICT_ALL; ?></a>
                    </div>
                    <div class="col-lg-6">
                        <a href="javascript:void(0);" class="btn btn-success btn-block" id="cameraperms-allow-all"><i class="fa fa-check fa-fw"></i> <?php echo ALLOW_ALL; ?></a>
                    </div>
                </div>
            </div>


            <div class="panel panel-default">
                <div class="panel-heading">
                    <?php echo CAMERA_PERMISSIONS_LIST_TITLE; ?>
                </div>
                <div class="panel-body">
                    <?php foreach($p->camera_list as $key => $camera) { ?>
                    <div class="col-lg-4 col-md-4 col-margin-bottom">
                        <a href="javascript:void(0);" class="btn btn-block cameraperms-cams <?php echo (($camera['allowed']) ? 'btn-success' : 'btn-danger'); ?>" data-id="<?php echo $camera['id']; ?>">
                            <i class="fa <?php echo (($camera['allowed']) ? 'fa-check' : ' fa-times'); ?> fa-fw"></i><?php echo $camera['device_name']; ?>
                        </a>
                        <input type="checkbox" name="cams[<?php echo $camera['id']; ?>]" <?php echo (($camera['allowed']) ? '' : 'checked'); ?> style="display: none;">
                    </div>
                    <?php } ?>
                </div>
            </div>

        </form>
    </div>
</div>

