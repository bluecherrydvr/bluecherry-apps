<?php defined('INDVR') or exit(); ?>

<div class="row">
    <div class="col-lg-12">
        <h1 class="page-header"><?php echo ACTIVE_USERS_HEADER; ?>

        <ol class="breadcrumb">
            <li class="active"><?php echo ACTIVE_USERS_SUBHEADER; ?></li>
        </ol>
        </h1>
    </div>
</div>

<div class="row">
    <div class="col-lg-12">
        <div class="table-responsive">
        <table class="table table-striped table-vert-align">
            <thead>
                <tr>
                    <th><?php echo USERS_TABLE_HEADER_NAME; ?></th>        
                    <th><?php echo AU_IP; ?></th>        
                    <th></th>        
                    <th></th>        
                </tr>
            </thead>
            <tbody>
            <?php 
    	    	$f = false;
        		foreach($this->users as $k => $user) {
            ?>
                <tr>
                    <td><?php echo $user->info['name']; ?></td>        
                    <td><?php echo $user->info['ip']; ?></td>        
                    <td><?php echo (($user->info['from_client']) ? RCLIENT : WEB); ?></td>        
                    <td>
                        <?php if ($user->info['kick']) { ?>
                            <a href="javascript:void(0);" class="btn btn-danger"><?php echo AU_KICKED; ?></a>
                        <?php } else { ?>
                        <form action="/ajax/update.php" method="POST">
                            <input type="hidden" name="mode" value="kick">
                            <input type="hidden" name="type" value="ActiveUsers">
                            <input type="hidden" name="kick" value="1">
                            <input type="hidden" name="id" value="<?php echo $user->info['ip']; ?>">

                            <button type="submit" class="btn btn-warning send-req-form" data-loading-text="<?php echo AU_KICK; ?>"><?php echo AU_KICK; ?></button>
                        </form>
                        <?php } ?>
                    </td>        
                </tr>
            <?php } ?>

            <tbody>
        </table>
        </div>

    </div>
</div>


