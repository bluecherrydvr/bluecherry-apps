<?php 

#reads news from rss/xml feed
class news extends Controller {
	
    public function __construct(){
        parent::__construct();
		$this->chAccess('admin');
    }

    public function getData()
    {
        $this->setView('ajax.news');
    }
}

