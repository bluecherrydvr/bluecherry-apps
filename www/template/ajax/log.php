<?php //defined('INDVR') or exit(); ?>
<style>

</style>

<div id="dvrlog">
<?php
	$f = '';
	foreach($log->log as $value){
		echo "<div class='$f'>$value</div>";
		$f = empty($f) ? 'bg' : '';
	};
?>
</div>
<div id="logControls">
	<select id="logLoader"><option value="null"><?php echo LOG_HOW_MANY; ?></option><option value="20">20</option><option value="100">100</option><option value="500">500</option><option value="1000">1000</option><option value="all"><?php echo LOG_ALL.": {$log->log_size}".(is_int($log->log_size) ? 'kb' : ''); ?></option></select> | 
	<a href="#" id="scrollLogToBottom"><?php echo SCROLL_TO_BOTTOM; ?></a> | <a href="#" id="scrollLogToTop"><?php echo SCROLL_TO_TOP; ?></a>
</div>
