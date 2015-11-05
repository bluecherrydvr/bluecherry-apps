<?php

class media extends Controller {

    public function getData()
    {
        $act = $this->uri->two;
        $act = ucfirst($act);

        return $this->ctl('media.media'.$act)->getData();
    }

    public function postData()
    {
        $act = $this->uri->two;
        $act = ucfirst($act);

        return $this->ctl('media.media'.$act)->postData();
    }
}
