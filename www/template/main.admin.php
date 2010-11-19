<?php defined('INDVR') or exit(); 

?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">

<html>
<head>
	<title><?php echo DVR_COMPANY_NAME.' '.DVR_DVR.PAGE_HEADER_SEPARATOR.PAGE_HEADER_MAIN; ?></title>
	<link rel="stylesheet" href="template/styles.css">
	<script type="text/javascript" src="./lib/mootools.js"></script>
	<script type="text/javascript" src="./lib/mootools-more.js"></script>
	<script type="text/javascript" src="./lib/js/main.js"></script>
	<script type="text/javascript" src="./lib/js/viewer.js"></script>
</head>
<body>
	<div id="leftColumn">
		<div id="logo"></div>
		<div id="mainMenu">
	  		<ul id="menuButtons">
				<li class='liveView'><?php echo MMENU_LIVEVIEW; ?></li>
				<hr>
				<li id="news" class="news"><? echo MMENU_NEWS; ?></li>
				<hr>
				<li id="general" class="settingsGeneral"><? echo MMENU_GS; ?></li>
				<li id="storage" class="settingsStorage"><? echo MMENU_STORAGE; ?></li>
				<li id="users" class="settingsUsers"><? echo MMENU_USERS; ?></li>
				<hr>
				<li id="devices" class="devices"><? echo MMENU_DEVICES; ?></li>
				<li id="deviceschedule" class="globalschedule"><? echo MMENU_SCHED; ?></li>
				<hr>
				<li id="log" class="viewLog"><? echo MMENU_LOG; ?></li>
			</ul>
		</div>
		<div id="serverStats">
			<div><?php echo STATS_HEARDER; ?></div><HR>
			<div class="label"><?php echo STATS_CPU; ?></div>
			<div class="progressBar" id='cpu'>
				<div class="text">0</div>
			</div>
			<div class="label"><?php echo STATS_MEM; ?></div>
			<div class="progressBar" id='mem'>
				<div class="text">0</div>
			</div>
			<div class="label" id="meminfo">&nbsp;</div>
			<div class="label"><?php echo STATS_UPT ?></div>
			<div id="serverUptime">&nbsp;</div>
		</div>
	</div>
	<div id="centerColumn">
		<div id="pageContainer"></div>
	</div>
	<div id="onLoadFailureContainer"><div id="onLoadFailure"><h1><?php echo COUND_NOT_OPEN_PAGE;  ?></h1><p><?php echo COUND_NOT_OPEN_PAGE_EX; ?></p></div></div>
</body>
</html>