<?php

class eventsIndex extends Controller {

    public function __construct()
    {
        parent::__construct();
        $this->chAccess('backup');
    }

    public function getData()
    {
        $current_user = $this->user;
        //$number_of_recs =  data::query("SELECT COUNT(*) as n From EventsCam");
        //$memory_limit = intval(max($number_of_recs[0]['n']/20000,1)*256).'M';
        //ini_set('memory_limit', $memory_limit);

        $events_portion = 5000;

        $query = "SELECT EventsCam.*, Media.size AS media_size, Media.start, Media.end, Devices.device_name, ((Media.size>0 OR Media.end=0) AND Media.filepath!='') AS media_available, ".
            "IFNULL(TIMESTAMPDIFF(SECOND, Media.start, Media.end), 0) AS video_duration ".
            "FROM EventsCam ".
            "LEFT JOIN Media ON (EventsCam.media_id=Media.id) ".
            "LEFT JOIN Devices ON (EventsCam.device_id=Devices.id) ".
            "WHERE ";


        if (isset($_GET['startDate']))
            $query .= "EventsCam.time >= " . ((int)$_GET['startDate']) . " AND ";
        if (isset($_GET['endDate']))
            $query .= "EventsCam.time <= " . ((int)$_GET['endDate']) . " AND ";
        if (isset($_GET['beforeId']))
            $query .= "EventsCam.id < " . ((int)$_GET['beforeId']) . " AND ";
        if (isset($_GET['afterId']))
            $query .= "EventsCam.id > " . ((int)$_GET['afterId']) . " AND ";
        if (isset($_GET['id']))
            $query .= "EventsCam.id = " . ((int)$_GET['id']) . " AND ";

        if (empty($current_user->data[0]['access_device_list'])){
            $current_user->data[0]['access_device_list'] = '-1';
        }

        $query .= "EventsCam.device_id NOT IN ({$current_user->data[0]['access_device_list']}) ";
        $query .= "ORDER BY EventsCam.id DESC ";

        $requested_limit = (isset($_GET['limit']) ? (int)$_GET['limit'] : -1);
        // if ($requested_limit < 1)
        // 	$requested_limit = 15000; // #1015 - have a strict maximum to keep the server from running out of memory
        // if ($requested_limit > 0)
        // 	$query .= "LIMIT ".$requested_limit;




        # Output header for this feed
        header('Content-Type: application/json');

        $data = array();

        $data['title'] = 'Bluecherry Events for ' . $_SERVER['SERVER_NAME'];

        if (isset($events) && $events) {
            $curevent = current($events);
            $lastupdate = date(DATE_ATOM, $curevent['time']);
        } else {
            $lastupdate = date(DATE_ATOM);
        }

        $data['updated'] = $lastupdate;

        // XXX Need to generate this per-server
        $data['id'] = 'urn:uuid:60a76c80-d399-11d9-b93C-0003939e0af6';

        $data['link'] = array(
            'rel' => 'self',
            'type' => 'application/json',
            'href' => 'http://' . $_SERVER['SERVER_NAME'] . '/events/'
        );

        $data['generator'] = array(
            'uri' => 'http://www.bluecherrydvr.com/atom.html',
            'version' => '1.0',
            'content' => 'BluecherryDVR Events Atom Generator'
        );

        $entries = array();

        $offset = 0;
        while (true) {
            $limit = min($requested_limit, $events_portion);

            if ($limit < 0) {
                $limit = $events_portion;
            }

            $events = data::query($query . ' LIMIT ' . $limit . ' OFFSET ' . $offset);

            foreach ($events as $item) {
                if (!$current_user->camPermission($item['device_id'])) {
                    continue;
                }

                $entry = array(
                    'id' => 'http://' . $_SERVER['SERVER_NAME'] . '/events/?id=' . $item['id'],
                    'title' => $item['level_id'] . ': ' . $item['type_id'] . ' event on device ' . $item['device_name'],
                    'published' => date(DATE_ATOM, $item['time'])
                );

                if (!empty($item['length'])) {
                    if ($item['length'] > 0) {
                        $entry['updated'] = date(DATE_ATOM, $item['time'] + $item['length']);
                    }
                }

                $entry['category'] = array(
                    'scheme' => 'http://www.bluecherrydvr.com/atom.html',
                    'term' => $item['device_id'] . '/' . $item['level_id'] . '/' . $item['type_id']
                );

                if (!empty($item['media_id']) && $item['media_available']) {
                    $entry['content'] = array(
                        'media_id' => $item['media_id'],
                        'media_size' => $item['media_size'],
                        'media_duration' => $item['media_duration'],
                        'content' => (!empty($_SERVER['HTTPS']) ? 'https' : 'http') . '://' . $_SERVER['HTTP_HOST'] . '/media/request.php?id=' . $item['media_id']
                    );
                }

                $entries[] = $entry;
            }

            if (count($events) < $events_portion) {
                break;
            }

            $offset += $events_portion;
        }

        $data['entry'] = $entries;

        echo json_encode($data, JSON_PRETTY_PRINT);

    }

    public function postData()
    {
        echo '123';
    }
}

