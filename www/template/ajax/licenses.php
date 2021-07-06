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
        <div class="panel-heading"><?php echo L_ADD_LICENSE_KEY_TITLE; ?></div>
        <div class="panel-body">
            <form action="/licenses?mode=add" method="POST">
                <div class="form-group">
                    <input class="form-control" type="text" class="license" name="licenseCode">
                </div>
                <div class="form-group">
                    <button class="btn btn-success send-req-form" type="submit" data-func="beforeAcitvatingLicense" data-func-after="afterAcitvatingLicense"> <?php echo L_ACTIVATE; ?></button>
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
                        <form action="/licenses?mode=deactivate&license=<?php echo $license['license']; ?>" method="POST" style="display: inline-block;">
                            <button type="submit" class="btn btn-danger send-req-form" data-confirm="<?php echo L_CONFIRM_DELETE; ?>" data-redirect-success="/licenses" data-func-after="afterDecitvatingLicense"><i class="fa fa-times fa-fw"></i> <?php echo L_DEACTIVATE; ?></button>
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

