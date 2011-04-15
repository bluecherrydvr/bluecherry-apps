<?php defined('INDVR') or exit();
 echo '<div id="version"><div id="ajaxMessage" class="INFO">'.NOT_UP_TO_DATE_LONG.'<BR>'.NOT_UP_TO_DATE_LINK.'<hr />'.INSTALLED.": <b>{$version->version['installed']}</b> <br />".CURR.": <b>{$version->version['current']}</b></div></div>";
?>