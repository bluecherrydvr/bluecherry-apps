<?php 


class statistics extends Controller {
	public $type = '';
	public $event_type;
	public $results;
	public $result;
	public $total_records;
    private $query_params = array(
    	'year' => '%y',
    	'month' => '%m',
    	'day' => '%d',
    	'hour' => '%h'//,
    	//'minute' => '%i',
    	//'second' => '%s'
    );
    private $date_format = "Y-m-d H:i:s";

    public function __construct(){
        parent::__construct();
		$this->chAccess('admin');
    }

    public function getData()
    {
        $this->setView('ajax.statistics');

	    $this->getFirstLast($this->date_format);

        $statistics = $this->setStatVar();

        $this->view->statistics = $statistics;
        $this->view->query_params = $this->query_params;
        $this->view->date_format = $this->date_format;
    }

    private function setStatVar()
    {
        $statistics = new StdClass();
        $statistics->type = $this->type;
        $statistics->event_type = $this->event_type;
        $statistics->results = $this->results;
        $statistics->result = $this->result;
        $statistics->total_records = $this->total_records;

        return $statistics;
    }

    public function postData()
    {
	    #prepare data
    	$data['primary_grouping'] = (!empty($_POST['primary_grouping'])) ? $_POST['primary_grouping'] : false;
    	$data['secondary_grouping'] = (!empty($_POST['secondary_grouping'])) ? $_POST['secondary_grouping'] : false;
    	$data['start'] = @strtotime($_POST['start']);
    	$data['end'] = @strtotime($_POST['end']);
    	$data['event_filter']['all_events'] = (!empty($_POST['all_events']) && $_POST['all_events'] == 'on') ? true : false;
    	$data['event_filter']['motion_events'] = (!empty($_POST['motion_events']) && $_POST['motion_events'] == 'on') ? true : false;
    	$data['event_filter']['continuous_events'] = (!empty($_POST['continuous_events']) && $_POST['continuous_events'] == 'on') ? true : false;
    	#if type not set return html
    	$this->type = (!empty($_POST['type'])) ? $_POST['type'] : 'html';
    	#get data
    	$this->dbData($data, $this->query_params);


        $view = new View('ajax.statistics');
        $statistics = $this->setStatVar();

        $view->statistics = $statistics;
        $view->query_params = $this->query_params;
        $view->date_format = $this->date_format;
        $view->data = $data;
        echo $view->render();
        die();
    }


    public function dbData($data, $query_params)
    {
		$where = "WHERE type_id != 'not found' ";
		if (!empty($data['start']) && !empty($data['end'])) $where .= (($where != 'WHERE ') ? ' AND ' : '')." time>={$data['start']} AND time<={$data['end']} ";
		if (!$data['event_filter']['all_events'] && ($data['event_filter']['motion_events'] || $data['event_filter']['continuous_events']) && !($data['event_filter']['motion_events'] && $data['event_filter']['continuous_events'])) {
			$this->event_type = ($data['event_filter']['motion_events']) ? "motion" : "continuous";
			$where .= (($where != 'WHERE ') ? ' AND ' : '')."type_id = '".$this->event_type."'";		
		}
		$where = ($where != 'WHERE ') ? $where : '';
		$query = "SELECT count(*) as counter, DATE_FORMAT(FROM_UNIXTIME(time),'{$query_params[$data['primary_grouping']]}') as '{$data['primary_grouping']}', DATE_FORMAT(FROM_UNIXTIME(time),'{$query_params[$data['secondary_grouping']]}') as '{$data['secondary_grouping']}' FROM EventsCam {$where} GROUP BY {$data['primary_grouping']}, {$data['secondary_grouping']}";
		$this->results = data::query($query);
		$this->total_records = count(data::query("SELECT id FROM EventsCam {$where}"));
	}

    // default values for date fields are first and last events in db
    public function getFirstLast($date_format)
    {
		$result = data::query("SELECT id, time FROM EventsCam ORDER BY id ASC LIMIT 1");

		$this->result['first_event'] = date('Y-m-d H:i:s' ,$result[0]['time']);
		$result = data::query("SELECT id, time FROM EventsCam ORDER BY id DESC LIMIT 1");
		$this->result['last_event'] = date('Y-m-d H:i:s' ,$result[0]['time']); 
	}
	
}

