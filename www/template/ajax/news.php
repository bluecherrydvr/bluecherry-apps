
<div class="row">
    <div class="col-lg-12">
        <h1 class="page-header"><?php echo NEWS_HEADER; ?></h1>
    </div>
</div>
<div class="row news-bl">
    <div class="col-lg-12">
<?php
if (!$news) {
	echo NO_NEWS;
} else {
    foreach($news as $entry){
?>

    <div class="panel panel-default">
        <div class="panel-heading">
            <h4><a href="<?php echo $entry['link']; ?>" class="pull-left"><?php echo $entry['title']; ?></a></h4>

            <span class="text-muted pull-right"><small><?php echo substr($entry['pubDate'], 0, 17); ?></small></span>

            <div class="clearfix"></div>
        </div>
        <div class="panel-body">
            <?php echo str_replace("[...]", "[<a href='{$entry['link']}'>...</a>]", $entry['description']); ?>
        </div>
        
    </div>

<?php    
	};
};
?>
    </div>
</div>
