<?php defined('INDVR') or exit(); 
require('../template/template.lib.php');

$mode = (!empty($_GET['m'])) ? $_GET['m'] : false;

if ($mode=='model'){
	if ($_GET['manufacturer'] == 'Generic'){
		echo "<INPUT type='hidden' name='models' id='models' value='Generic' readonly>";
		exit();
	}
	echo arrayToSelect(array_merge(array(AIP_CHOOSE_MODEL), Cameras::getList($_GET['manufacturer'])), '', 'models', 'change-event', false, 'data-function="cameraChooseModel"');
	exit;
};
if ($mode=='ops') {
    Cameras::getCamDetails($_GET['model']);
    exit;
};

?>
<div class="row">
    <div class="col-lg-12">
        <h1 class="page-header"><?php echo AIP_HEADER; ?>

        <?php if (AIP_SUBHEADER) { ?>
        <ol class="breadcrumb">
            <li class="active"><?php echo AIP_SUBHEADER; ?></li>
        </ol>
        <?php } ?>
        </h1>
    </div>
</div>

<div class="row">
    <div class="col-lg-12">
        <div class="alert alert-warning">
            <i class="fa fa-warning fa-fw"></i>
            <?php echo IPCAM_WIKI_LINK ?>
        </div>
    </div>
</div>

<div class="row">
    <div class="col-lg-12">
        <form action="/ajax/addip.php" method="POST" class="form-horizontal">
	        <input type="hidden" name="mode" value="addip" />

            <div class="panel panel-default">
                <div class="panel-body">
                    
                    <div class="form-group">
                        <label class="col-lg-4 control-label"><?php echo AIP_CHOOSE_MANUF; ?></label>

                        <div class="col-lg-6">
                            <?php echo arrayToSelect(array_merge(array(AIP_CHOOSE_MANUF), array("Generic"), Manufacturers::getList()), '', 'manufacturers', 'change-event', false, 'data-function="cameraChooseManuf"'); ?>
                        </div>
                    </div>
                    
                    <div class="form-group" style="display: none">
                        <label class="col-lg-4 control-label"><?php echo AIP_CHOOSE_MODEL; ?></label>

                        <div class="col-lg-6" id="addip-model-select"></div>
                    </div>
                    
                    <div class="form-group">
                        <label class="col-lg-4 control-label"><?php echo AIP_CAMERA_NAME; ?></label>

                        <div class="col-lg-6">
                            <input class="form-control" type="text" name="camName" disabled="disabled" />
                        </div>
                    </div>
                    
                    <div class="form-group">
                        <label class="col-lg-4 control-label"><?php echo AIP_IP_ADDR; ?></label>

                        <div class="col-lg-6">
                            <input class="form-control" type="text" name="ipAddr" disabled="disabled"  />
                        </div>
                    </div>
                    
                    <div class="form-group" style="display: none;">
                        <label class="col-lg-4 control-label"><?php echo AIP_IP_ADDR_MJPEG; ?></label>

                        <div class="col-lg-6">
                            <input class="form-control" type="text" name="ipAddrMjpeg" disabled="disabled"  />
                        </div>
                    </div>
                    
                    <div class="form-group">
                        <label class="col-lg-4 control-label"><?php echo AIP_USER; ?></label>

                        <div class="col-lg-6">
                            <input class="form-control" type="text" name="user" disabled="disabled" />
                        </div>
                    </div>
                    
                    <div class="form-group">
                        <label class="col-lg-4 control-label"><?php echo AIP_PASS; ?></label>

                        <div class="col-lg-6">
                            <input class="form-control" type="password" name="pass" disabled="disabled"  />
                        </div>
                    </div>

                    <div class="form-group">
                        <label class="col-lg-4 control-label">
                            <a href="javascript:void(0);" class="btn btn-default" id="edittip-advanced-settings"><?php echo AIP_ADVANCED_SETTINGS; ?></a>
                        </label>
                        <div class="col-lg-6">
                        </div>
                    </div>

                    <div id="edittip-advanced-settings-block" style="display: none;">
                    
                    <div class="form-group">
                        <label class="col-lg-4 control-label"><?php echo AIP_PROTOCOL; ?></label>

                        <div class="col-lg-6">
                			<select class="form-control change-event" name="protocol" id="devices-camera-protocol" data-function="protocolBlocks">
                				<option value="IP-RTSP">RTSP</option>
                				<option value="IP-MJPEG">MJPEG</option>
                			</select>
                        </div>
                    </div>
                    
                    <div class="devices-rtsp-setting">
                    <div class="form-group">
                        <label class="col-lg-4 control-label"><?php echo AIP_RTSP; ?></label>

                        <div class="col-lg-6">
                            <input class="form-control" type="text" name="rtsp" disabled="disabled" />
                        </div>
                    </div>
                    
                    <div class="form-group">
                        <label class="col-lg-4 control-label"><?php echo AIP_PORT; ?></label>

                        <div class="col-lg-6">
                            <input class="form-control" type="text" name="port" value="554" disabled="disabled"  />
                        </div>
                    </div>
                    
                    <div class="form-group">
                        <label class="col-lg-4 control-label"><?php echo IPCAM_TCPUDP_LEVEL; ?></label>

                        <div class="col-lg-6">
                            <select class="form-control" name="prefertcp">
                                <option value="1">TCP</option>
                                <option value="0">UDP</option>
                            </select>
                        </div>
                    </div>
                    </div>
                    
                    <div class="devices-mjpeg-setting" style="display: none;">
                    <div class="form-group">
                        <label class="col-lg-4 control-label"><?php echo AIP_MJPATH; ?></label>

                        <div class="col-lg-6">
                            <input class="form-control" type="text" name="mjpeg" disabled="disabled"  />
                        </div>
                    </div>
                    
                    <div class="form-group">
                        <label class="col-lg-4 control-label"><?php echo AIP_PORT_MJPEG; ?></label>

                        <div class="col-lg-6">
                            <input class="form-control" type="text" name="portMjpeg" disabled="disabled" />
                        </div>
                    </div>
                    </div>



                    </div>
                    
                    <div class="form-group">
                        <div class="col-lg-6 col-lg-offset-4">
                            <button class="btn btn-success send-req-form" type="submit" data-redirect-success="/ajax/devices.php"><i class="fa fa-check fa-fw"></i> <?php echo AIP_ADD; ?></button>
                        </div>
                    </div>

                </div>
            </div>

        </form>
    </div>
</div>


<a id="report-incorrect-data" href="javascript:;">
    Report incorrect information
</a>

<div id="wufoo-z1ws6wwb1p41rs9" style="display: none">
    Fill out my <a href="https://bluecherry.wufoo.com/forms/z1ws6wwb1p41rs9">online form</a>.
</div>
