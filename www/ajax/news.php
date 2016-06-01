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
		$xml = @simplexml_load_string($this->loadNews($path_to_xml));
		$cnt = 0;
		if (!$xml || !$xml->channel->item) { return false; }
		foreach ($xml->channel->item as $posting){
			$cnt++;
			foreach($posting->children() as $child){
				$this->news[$cnt][$child->getName()] = $child;
			}
		}
	}

    private function loadNews($url)
    {
        $ch = curl_init();
        curl_setopt($ch, CURLOPT_URL, $url);
        curl_setopt($ch, CURLOPT_HEADER, false);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
        curl_setopt($ch, CURLOPT_FOLLOWLOCATION, true);
        curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false);
        curl_setopt($ch, CURLOPT_TIMEOUT, 3);
        $output = trim(curl_exec($ch));

        curl_close($ch);

        return $output;
    }
}

