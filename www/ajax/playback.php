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
    }

    private function getCameras()
    {
        $res = data::query("SELECT dvs.* FROM Devices as dvs
            INNER JOIN EventsCam as m ON dvs.id=m.device_id
            WHERE m.type_id='motion' OR m.type_id='continuous'
            GROUP BY dvs.id
            ");

        return $res;
    }

    public function postData()
    {
        $res = '';

        $motion = Inp::post('motion_events');
        $continuous = Inp::post('continuous_events');
        $date_start = Inp::post('start');
        $date_end = Inp::post('end');

        if ($motion || $continuous) {
            $view = new View('ajax.playback.listEvents');

            $where_add = '';
            if ($date_start) $where_add .= " AND EventsCam.time >= ".dateToUnix($date_start);
            if ($date_end) $where_add .= " AND EventsCam.time <= ".dateToUnix($date_end);

            if ($motion && $continuous) {
                $where_add .= " AND (type_id='motion' OR type_id='continuous')";
            } else {
                if ($motion) $where_add .= " AND type_id='motion'";
                if ($continuous) $where_add .= " AND type_id='continuous'";
            }

            $query = "SELECT EventsCam.*, Devices.device_name, Media.id as media_id, Media.size AS media_size, Media.start, Media.end, ((Media.size>0 OR Media.end=0) AND Media.filepath!='') AS media_available
                FROM EventsCam
                LEFT JOIN Media ON (EventsCam.media_id=Media.id)
                LEFT JOIN Devices ON (EventsCam.device_id=Devices.id)
                WHERE EventsCam.device_id=".(int) Inp::post('device_id')."
                ".$where_add."
                ORDER BY EventsCam.time DESC
                ";

            $query_res = data::query($query);

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
            }

            $view->events = $query_res;

            $res = $view->render();
        }


        return $res;
    }

}
