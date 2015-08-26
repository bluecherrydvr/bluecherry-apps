<?php defined('INDVR') or exit(); ?>
<h1 class="header"><?php echo NEWS_HEADER; ?></h1><hr />
<div id="dvrNews">
<?php
if (!$news->news) {
	echo NO_NEWS;
} else {
	foreach($news->news as $entry){
 	echo "
	 <div class='news-entry' id=''>
 		<div class='news-title'><a href='{$entry['link']}'>{$entry['title']}</a></div>
		<div class='news-date'>".substr($entry['pubDate'], 0, 17)."</div>
		<div class='news-text'>".str_replace("[...]", "[<a href='{$entry['link']}'>...</a>]", $entry['description'])."</div>
	 </div >";
	};
};
?>
</div>
