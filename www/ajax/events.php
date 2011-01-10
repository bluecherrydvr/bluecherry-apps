<?php DEFINE('INDVR', true);
#lib
include("../lib/lib.php");  #common functions

#auth check
$current_user = new DVRUser();
$current_user->CheckStatus();
$current_user->StatusAction('admin');
#/auth check


class DVREvents{
	public $events;
	public function __construct($limit = false){
		$this->GetEvents($limit);
	}
	
	private function GetEvents($limit){
		$db = DVRDatabase::getInstance();
		$this->events = $db->DBFetchAll($db->DBQuery("SELECT * FROM EventsCam ".((!$limit) ? "" : "LIMIT $limit")));
	}
	
	public function MakeXML(){
		
	}
}

$limit = intval($_GET['limit']);
$new = new DVREvents($limit);
var_dump($new->events);
?>
