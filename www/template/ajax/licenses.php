<?php 

$mode = (empty($_GET['mode'])) ? false : $_GET['mode'];
?>


<div class="row">
    <div class="col-lg-12">
        <h1 class="page-header"></h1>
    </div>
</div>

<div class="row">
    <div class="col-lg-12">
    <div class="panel panel-default">
        <div class="panel-heading"><?php echo L_ADDCODE; ?></div>
        <div class="panel-body">
            <form action="/licenses?mode=add" method="POST">
                <div class="form-group">
                    <input class="form-control" type="text" class="license" name="licenseCode">
                </div>
                <div class="form-group">
                    <button class="btn btn-success send-req-form" type="submit" data-func="licensingBeforeAdd" data-func-after="licensingAdd"> <?php echo L_ADD; ?></button>
                </div>
            </form>
        </div>
    </div>
    </div>
</div>

<div class="row" id="licensing-conf" style="display: none;">
    <div class="col-lg-12">
    <div class="panel panel-default">
        <div class="panel-heading"><?php echo L_CONFIRMCODE; ?></div>
        <div class="panel-body">
		    <div class='alert alert-warning'><?php echo L_CONFIRMCODE_MSG; ?><span id="licensing-machine-id"></span></div>
            <form action="/licenses?mode=confirm" method="POST">
                <div class="form-group">
        			<input type="hidden" name="licenseCode">
        			<input class="form-control" type="text" name="confirmLicense">
                </div>
                <div class="form-group">
                    <button class="btn btn-success send-req-form" type="submit" data-redirect-success="/licenses"> <?php echo L_CONFIRM; ?></button>
                </div>
            </form>
        </div>
    </div>
    </div>
</div>


<div class="row">
    <div class="col-lg-12">
    <div class="panel panel-default">
        <div class="panel-heading"><?php echo L_CURRENT; ?></div>
        <div class="panel-body">
	    <?php if (empty($licenses)){ ?>
            <div class="alert alert-warning"><?php echo L_NO_LICENSES; ?></div>
        <?php } else { ?>
            <table class="table table-striped">
    		<?php foreach($licenses as $license){ ?>
                <tr>
                    <td><?php echo $license['license']; ?></td>
                    <td><?php echo bc_license_devices_allowed()." ".L_PORTS; ?></td>
                    <td class="col-lg-2 col-md-licensing">
                        <form action="/licenses?mode=delete&license=<?php echo $license['license']; ?>" method="POST" style="display: inline-block;">
                            <button type="submit" class="btn btn-danger send-req-form" data-confirm="<?php echo L_CONFIRM_DELETE; ?>" data-redirect-success="/licenses"><i class="fa fa-times fa-fw"></i> <?php echo L_DELETE; ?></button>
                        </form>
                    </td>
                </tr>
            <?php } ?>
            </table> 
        <?php } ?>
        </div>
    </div>
    </div>
</div>

