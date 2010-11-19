<?php defined('INDVR') or exit(); ?>
<h1 class="header"><?php echo NEWS_HEADER; ?></h1><hr>
<div id="dvrNews">
<?php
if (!$news->news) {
	echo NO_NEWS;
} else {
	foreach($news->news as $entry){
 	echo '
	 <div id="newsEntry">
 		<div id="newsTitle"><a href="'.$entry['link'].'">'.$entry['title'].'</a></div>
		<div id="newsDate">'.substr($entry['pubDate'], 0, 17).'</div>
		<div id="newsText">'.$entry['description'].'<HR></div>
	 </div >';
	};
};
?>
</div>