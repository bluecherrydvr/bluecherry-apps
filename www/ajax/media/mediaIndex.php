<?php

include_once lib_path . 'lib.php';

class mediaIndex extends Controller {
	
    public function __construct()
    {
        parent::__construct();
		$this->chAccess('backup');
    }

    public function getData()
    {
        print "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";

	$db = database::getInstance();

        $events = $db->fetchAll("SELECT * FROM Media ORDER BY start DESC");

        # Output one item for each media entry
        foreach ($events as $item) {
        	print "  <item id='". $item['id'] ."'>\n";

        	foreach (array_keys($item) as $key) {
        		if ($key == 'id')
        			continue;
        		print "    <$key>". $item[$key] ."</$key>\n";
        	}

        	print "  </item>\n";
        }


        die();
    }
}

