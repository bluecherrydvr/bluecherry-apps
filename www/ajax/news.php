<?php 

#reads news from rss/xml feed
class news extends Controller {
	public $news;
	
    public function __construct(){
        parent::__construct();
		$this->chAccess('admin');
    }

    public function getData()
    {
        $this->setView('ajax.news');

        $this->GetNews(VAR_NEWS_XML);
        $this->view->news = $this->news;
    }


	private function GetNews($path_to_xml){
		$xml = @simplexml_load_file($path_to_xml);
		$cnt = 0;
		if (!$xml || !$xml->channel->item) { return false; }
		foreach ($xml->channel->item as $posting){
			$cnt++;
			foreach($posting->children() as $child){
				$this->news[$cnt][$child->getName()] = $child;
			}
		}
	}
}

