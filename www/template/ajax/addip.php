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
                			<select class="form-control" name="protocol">
                				<option value="IP-RTSP">RTSP</option>
                				<option value="IP-MJPEG">MJPEG</option>
                			</select>
                        </div>
                    </div>
                    
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
                    
                    <div class="form-group">
                        <div class="col-lg-6 col-lg-offset-4">
                            <button class="btn btn-success send-req-form" type="submit"><i class="fa fa-check fa-fw"></i> <?php echo AIP_ADD; ?></button>
                        </div>
                    </div>

                </div>
            </div>

        </form>
    </div>
</div>

<FORM id="settingsForm" action="/ajax/addip.php" method="post">
<div id="aip">
	<input type="hidden" name="mode" value="addip" />
        <div><label id="addipLabel"><?php echo AIP_CHOOSE_MANUF; ?>:</label><?php echo arrayToSelect(array_merge(array(AIP_CHOOSE_MANUF), array("Generic"), Manufacturers::getList()), '', 'manufacturers'); ?></div>
	<div id=model_selector" style="display: none"><label id="addipLabel"><?php echo AIP_CHOOSE_MODEL; ?>:</label><span id="modelSelector"><?php echo arrayToSelect(array(AIP_CH_MAN_FIRST), AIP_CH_MAN_FIRST, 'models', '', true); ?></span></div>
	<!--<div><label id="addipLabel"><?php echo AIP_CHOOSE_FPSRES; ?>:</label><span id="fpsresSelector"><?php echo arrayToSelect(array(AIP_CH_MOD_FIRST), AIP_CH_MOD_FIRST, 'fpsres', '', true); ?></span></div>!-->
	<div><label id="addipLabel"><?php echo AIP_CAMERA_NAME; ?></label><input type="Text" disabled="disabled" id="camName" name="camName" /></div>
	<div><label id="addipLabel"><?php echo AIP_IP_ADDR; ?></label><input type="Text" disabled="disabled" id="ipAddr" name="ipAddr" /></div>
	<div style="display:none;"><label id="addipLabel"><?php echo AIP_IP_ADDR_MJPEG; ?></label><input type="Text" disabled="disabled" id="ipAddrMjpeg" name="ipAddrMjpeg" /></div>
	<div><label id="addipLabel"><?php echo AIP_USER; ?></label><input disabled="disabled" id="user" type="Text" name="user" /></div>
	<div><label id="addipLabel"><?php echo AIP_PASS; ?></label><input disabled="disabled" id="pass" type="Password" name="pass" /></div>
	<div id="advancedSettingsSwitch">[<?php echo AIP_ADVANCED_SETTINGS; ?>]</div>
	<div id="advancedSettings">
		<div><label id="addipLabel"><?php echo AIP_PROTOCOL; ?></label>
			<select name="protocol" id="protocol">
				<option value="IP-RTSP">RTSP</option>
				<option value="IP-MJPEG">MJPEG</option>
			</select>
		</div>
		<div id='rtsp-settings'>
			<div><label id="addipLabel"><?php echo AIP_RTSP; ?></label><input disabled="disabled" id="rtsppath" type="Text" name="rtsp" /></div>
			<div><label id="addipLabel"><?php echo AIP_PORT; ?></label><input disabled="disabled" id="port" type="Text" name="port" value="554" /></div>
			<div><label id="addipLabel"><?php echo IPCAM_TCPUDP_LEVEL; ?></label><select name="prefertcp"><option value="1">TCP</option><option value="0">UDP</option></select></div>
		</div>
		<div><label id="addipLabel"><?php echo AIP_MJPATH; ?></label><input disabled="disabled" id="mjpeg" type="Text" name="mjpeg" /></div>
		<div><label id="addipLabel"><?php echo AIP_PORT_MJPEG; ?></label><input disabled="disabled" id="portMjpeg" type="Text" name="portMjpeg" value="80" /></div>
	</div>
	<div class='bClear'></div>
	<div><label id="addipLabel"></label><input disabled="disabled" id="saveButton" type="Submit" value="<?php echo AIP_ADD; ?>" /></div>
	
</div>
</FORM>

<a id="report-incorrect-data" href="javascript:;">
    Report incorrect information
</a>

<div id="wufoo-z1ws6wwb1p41rs9" style="display: none">
    Fill out my <a href="https://bluecherry.wufoo.com/forms/z1ws6wwb1p41rs9">online form</a>.
</div>
