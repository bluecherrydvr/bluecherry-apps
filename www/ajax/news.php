<?php DEFINE('INDVR', true);
#lib
include("../lib/lib.php");  #common functions

$current_user = new user('id', $_SESSION['id']);
$current_user->checkAccessPermissions('admin');

#reads news from rss/xml feed
class News{
	public $news;
	
	function __construct(){
		$this->GetNews(VAR_NEWS_XML);
	}
	private function GetNews($path_to_xml){
		$xml = simplexml_load_file($path_to_xml);
		$cnt = 0;
		if (!$xml->channel->item) { return false; }
		foreach ($xml->channel->item as $posting){
			$cnt++;
			foreach($posting->children() as $child){
				$this->news[$cnt][$child->getName()] = $child;
			}
		}
	}
}

$news = new News;
	
#require template to show data
require('../template/ajax/news.php');

?>
