<?php 


class log extends Controller {
    public $log;
    public $type;
	public $log_size;
    private $log_html = '';

    public function __construct()
    {
        parent::__construct();
		$this->chAccess('admin');
    }

    public function getData()
    {
        $this->setView('ajax.log');
        $this->preData();

        $this->view->log_size = $this->log_size;
        $this->view->log = $this->log;
        $this->view->type = $this->type;
        $this->view->log_html = $this->log_html;
    }

    public function postData()
    {
        $this->preData();
        return $this->log_html;
    }

    function preData() 
    {
        $log_type = (isset($_POST['type'])) ? $_POST['type'] : false;
        $lines = (isset($_POST['lines'])) ? $_POST['lines'] : false;

        $this->type = $log_type;
        
		$log_path = '';
		switch ($log_type){
			case 'www':	$log_path = VAR_WWW_LOG_PATH;
				break;
			default:		$log_path = VAR_LOG_PATH;
				break;
			
		}
		$this->log = $this->GetLog($log_path, $lines);
        $this->log_size = (file_exists($log_path)) ? intval(filesize($log_path)/1024) : LOG_FILE_DOES_NOT_EXIST;


        foreach($this->log as $value){
            $this->log_html .= '<tr><td>'.$value.'</td></tr>';
        }
    }

    private function GetLog($log_path, $lines)
    {
		if (!empty($lines) && $lines=='all'){
			$content = file($log_path);
		} else {
			$lines = empty($lines) ? 20 : intval($lines);
			# get VAR_LOG_PATH file {$lines} last lines, explode by
			# new line chars and filter out empty lines
			$content = array_filter(explode("\n" ,	nl2br(shell_exec("tail -n $lines ".$log_path))), 'strlen');
		}
		return (empty($content)) ? array(LOG_EMPTY) : $content;
	}
}

