<?php 

class auditlog extends Controller {
	public $auditlog;
	
    public function __construct(){
        parent::__construct();
		$this->chAccess('admin');
    }

    public function getData()
    {
        $this->setView('ajax.auditlog');

        $this->view->news = $this->news;
    }


}

