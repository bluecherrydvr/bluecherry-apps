<?php 

class eventsEvents extends Controller {
	private $events;
	
    public function __construct(){
        parent::__construct();
		$this->chAccess('admin');
    }

    public function getData()
    {
        $limit = intval(Inp::get('limit'));
		$this->GetEvents($limit);

        var_dump($this->events);
    }

    public function postData()
    {
        $limit = intval(Inp::get('limit'));
		$this->GetEvents($limit);

        var_dump($this->events);
    }

    private function GetEvents($limit)
    {
		$this->events = data::query("SELECT * FROM EventsCam ".((!$limit) ? "" : "LIMIT $limit"));
	}
	
    public function MakeXML()
    {
		
    }
}

