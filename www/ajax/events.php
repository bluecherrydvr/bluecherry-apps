<?php 

class events extends Controller {
	
    public function __construct()
    {
        parent::__construct();
    }

    public function getAjax()
    {
        return $this->ctl('events.eventsEvents')->getData();
    }

    public function postAjax()
    {
        return $this->ctl('events.eventsEvents')->postData();
    }

    public function getData()
    {
        return $this->ctl('events.eventsIndex')->getData();
    }

    public function postData()
    {
        return $this->ctl('events.eventsIndex')->postData();
    }
}

