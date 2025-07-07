<?php

class playback extends Controller {

    public function __construct()
    {
        parent::__construct();
        $this->left_menu = false;
        $this->chAccess('backup');
    }

    public function getData()
    {
        $this->setView('ajax.playback.index');

        $this->view->cameras = $this->getCameras();
        $this->view->event_types = $this->getEventTypes();
    }

    private function getCameras()
    {
        $res = data::query("SELECT dvs.* FROM Devices as dvs
            INNER JOIN EventsCam as m ON dvs.id=m.device_id
            GROUP BY dvs.id
            ");

        return $res;
    }

    private function getEventTypes()
    {
        $res = data::query("SELECT DISTINCT type_id FROM EventsCam ORDER BY type_id");
        $event_types = array();
        foreach ($res as $row) {
            $event_types[] = $row['type_id'];
        }
        return $event_types;
    }

    public function getDownloadMkv()
    {
        $id = (int) $this->uri->three;
        if ($id) {
            $info = data::query("SELECT * FROM Media WHERE id={$id}");
            if (!empty($info)) {
                $file_info = pathinfo($info[0]['filepath']);
                downloadFile($info[0]['filepath'], $file_info['basename'], false);
            }
        }

        return PLAYBACK_FILE_NOT_FOUND;
    }

    public function postData()
    {
        $res = '';

        $event_types = Inp::post('event_types');
        $date_start = Inp::post('start');
        $date_end = Inp::post('end');

        if (!empty($event_types)) {
            $view = new View('ajax.playback.listEvents');

            $where_add = '';
            if ($date_start) $where_add .= " AND EventsCam.time >= ".dateToUnix($date_start);
            if ($date_end) $where_add .= " AND EventsCam.time <= ".dateToUnix($date_end);

            // Build dynamic event type filter
            $event_type_conditions = array();
            foreach ($event_types as $type) {
                $event_type_conditions[] = "type_id='".addslashes($type)."'";
            }
            if (!empty($event_type_conditions)) {
                $where_add .= " AND (" . implode(" OR ", $event_type_conditions) . ")";
            }

            $query = "SELECT EventsCam.*, Devices.device_name, Media.id as media_id, Media.size AS media_size, Media.start, Media.end, (Media.id IS NOT NULL AND Media.filepath!='') AS media_available
                FROM EventsCam
                LEFT JOIN Media ON (EventsCam.media_id=Media.id)
                LEFT JOIN Devices ON (EventsCam.device_id=Devices.id)
                WHERE EventsCam.device_id=".(int) Inp::post('device_id')."
                ".$where_add."
                ORDER BY EventsCam.time DESC
                ";

            $query_res = data::query($query);

            // Debug: Log the query and results
            error_log("Playback query: " . $query);
            error_log("Query results count: " . count($query_res));

            $locale_en = localeEn();
            foreach ($query_res as $key => $val) {
                $query_res[$key]['media_size_converted'] = convertBytes($val['media_size']);
                $query_res[$key]['time_converted'] = dateFormat($val['time'], $locale_en)->time;

                $query_res[$key]['duration'] = gmdate('H:i:s', $val['end'] - $val['start']);

                if ($val['level_id'] == 'alrm') {
                    $color = 'info';
                } else if ($val['level_id'] == 'crit') {
                    $color = 'danger';
                } else if ($val['level_id'] == 'warn') {
                    $color = 'warning';
                } else {
                    // info
                    $color = '';
                }
                $query_res[$key]['color'] = $color;
                
                // Debug: Log each event
                error_log("Event: id=" . $val['id'] . ", type=" . $val['type_id'] . ", media_available=" . $val['media_available'] . ", media_size=" . $val['media_size'] . ", end=" . $val['end']);
            }

            $view->events = $query_res;

            $res = $view->render();
        }


        return $res;
    }

}
